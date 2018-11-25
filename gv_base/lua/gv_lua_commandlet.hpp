#include "gv_base.h"
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
#include "lua/ELuna.h"
namespace ELuna
{
Function_Vector CPPGarbage::m_CPPFunctions;
Method_Vector CPPGarbage::m_CPPMethods;
}

#if GV_WITH_OS_API

namespace gv
{

// sample class .
/*function Foo : speak()
        print("Hello, I am a Foo")
        end

        local foo = Foo.new("fred")
        local m = foo : add(3, 4)

        -- "fred: 3 + 4 = 7"
        print(m)

        -- "Hello, I am a Foo"
foo : speak()

          --Let's rig the original metatable
          Foo.add_ = Foo.add
          function Foo : add(a, b)
          return "here comes the magic: " ..self : add_(a, b)
          end
          m = foo : add(9, 8)
          */

class Foo
{
public:
	Foo(const std::string& name)
		: name(name)
	{
		std::cout << "Foo is born , name= " << name << std::endl;
	}

	std::string Add(int a, int b)
	{
		std::stringstream ss;
		ss << name << ": " << a << " + " << b << " = " << (a + b);
		return ss.str();
	}

	~Foo()
	{
		std::cout << "Foo is gone" << std::endl;
	}

private:
	std::string name;
};

// The general pattern to binding C++ class to Lua is to write a Lua
// thunk for every method for the class, so here we go:

int l_Foo_constructor(lua_State* l)
{
	const char* name = luaL_checkstring(l, 1);

	// We could actually allocate Foo itself as a user data but
	// since user data can be GC'ed and we gain unity by using CRT's heap
	// all along.
	Foo** udata = (Foo**)lua_newuserdata(l, sizeof(Foo*));
	*udata = new Foo(name);

	// Usually, we'll just use "Foo" as the second parameter, but I
	// say luaL_Foo here to distinguish the difference:
	//
	// This 2nd parameter here is an _internal label_ for luaL, it is
	// _not_ exposed to Lua by default.
	//
	// Effectively, this metatable is not accessible by Lua by default.
	luaL_getmetatable(l, "Foo");

	// The Lua stack at this point looks like this:
	//
	//     3| metatable "luaL_foo"   |-1
	//     2| userdata               |-2
	//     1| string parameter       |-3
	//
	// So the following line sets the metatable for the user data to the luaL_Foo
	// metatable
	//
	// We must set the metatable here because Lua prohibits setting
	// the metatable of a userdata in Lua. The only way to set a metatable
	// of a userdata is to do it in C.
	lua_setmetatable(l, -2);

	// The Lua stack at this point looks like this:
	//
	//     2| userdata               |-1
	//     1| string parameter       |-2
	//
	// We return 1 so Lua callsite will get the user data and
	// Lua will clean the stack after that.

	return 1;
}

Foo* l_CheckFoo(lua_State* l, int n)
{
	// This checks that the argument is a userdata
	// with the metatable "luaL_Foo"
	return *(Foo**)luaL_checkudata(l, n, "luaL_Foo");
}

int l_Foo_add(lua_State* l)
{
	Foo* foo = l_CheckFoo(l, 1);
	int a = (int)luaL_checknumber(l, 2);
	int b = (int)luaL_checknumber(l, 3);

	std::string s = foo->Add(a, b);
	lua_pushstring(l, s.c_str());

	// The Lua stack at this point looks like this:
	//
	//     4| result string          |-1
	//     3| metatable "luaL_foo"   |-2
	//     2| userdata               |-3
	//     1| string parameter       |-4
	//
	// Return 1 to return the result string to Lua callsite.

	return 1;
}

int l_Foo_destructor(lua_State* l)
{
	Foo* foo = l_CheckFoo(l, 1);
	delete foo;

	return 0;
}

void RegisterFoo(lua_State* l)
{
	luaL_Reg sFooRegs[] = {{"new", l_Foo_constructor},
						   {"add", l_Foo_add},
						   {"__gc", l_Foo_destructor},
						   {NULL, NULL}};

	// Create a luaL metatable. This metatable is not
	// exposed to Lua. The "luaL_Foo" label is used by luaL
	// internally to identity things.
	luaL_newmetatable(l, "Foo");

	// Register the C functions _into_ the metatable we just created.
	luaL_register(l, "Foo", sFooRegs);

	// The Lua stack at this point looks like this:
	//
	//     1| metatable "luaL_Foo"   |-1
	lua_pushvalue(l, -1);

	// The Lua stack at this point looks like this:
	//
	//     2| metatable "luaL_Foo"   |-1
	//     1| metatable "luaL_Foo"   |-2

	// Set the "__index" field of the metatable to point to itself
	// This pops the stack
	lua_setfield(l, -1, "__index");

	// The Lua stack at this point looks like this:
	//
	//     1| metatable "luaL_Foo"   |-1

	// The luaL_Foo metatable now has the following fields
	//     - __gc
	//     - __index
	//     - add
	//     - new

	// Now we use setglobal to officially expose the luaL_Foo metatable
	// to Lua. And we use the name "Foo".
	//
	// This allows Lua scripts to _override_ the metatable of Foo.
	// For high security code this may not be called for but
	// we'll do this to get greater flexibility.
	lua_setglobal(l, "Foo");

	const char* lua_function = "function Foo : speak() \r\n"
							   "print(\"Hello, I am a Foo\")\r\n"
							   "end\r\n";
	luaL_loadstring(l, lua_function);
}

int my_function(lua_State* L)
{
	int argc = lua_gettop(L);

	std::cerr << "-- my_function() called with " << argc
			  << " arguments:" << std::endl;

	for (int n = 1; n <= argc; ++n)
	{
		std::cerr << "-- argument " << n << ": " << lua_tostring(L, n) << std::endl;
	}

	lua_pushnumber(L, 123); // return value
	return 1;				// number of return values
}

int my_foo_add_function(lua_State* L)
{
	int argc = lua_gettop(L);

	std::cerr << "-- my_function() called with " << argc
			  << " arguments:" << std::endl;

	for (int n = 1; n <= argc; ++n)
	{
		std::cerr << "-- argument " << n << ": " << lua_tostring(L, n) << std::endl;
	}

	lua_pushnumber(L, 123); // return value
	return 1;				// number of return values
}

/*--new a class
        local cppClass = CPPClass("cppClass")
        --call cpp method
cppClass : cppPrint("Hello world!")
                   print("CPPClass's cppSum:", cppClass:cppSum(1, 2))

                   --call cpp function
                   cppPrint("Hello world!")
                   print("cppSum:", cppSum(1, 2))

                   --define lua function
                   function luaPrint(str)
                   print("luaPrint:", str)
                   end

                   function luaSum(a, b)
                   return a + b
                   end

                   --define table
                   luaTable = { "hello" }
        */

// define a class
class CPPClass
{
public:
	CPPClass(const char* name)
		: m_name(name)
	{
		printf("%s Constructor!\n", name);
	}
	~CPPClass()
	{
		printf("%s Destructor!\n", m_name);
	}

	// define method
	void cppPrint(const char* word)
	{
		printf("%s: %s\n", m_name, word);
	}

	int cppSum(int a, int b)
	{
		return a + b;
	}

private:
	const char* m_name;
};

// define function
void cppPrint(char* str)
{
	printf("cppPrint: %s\n", str);
}

int cppSum(int a, int b)
{
	return a + b;
}

int testCPP(lua_State* L)
{
	// register a class and it's constructor. indicate all constructor's param
	// type
	ELuna::registerClass< CPPClass >(L, "CPPClass",
									 ELuna::constructor< CPPClass, const char* >);
	// register a method
	ELuna::registerMethod< CPPClass >(L, "cppPrint", &CPPClass::cppPrint);
	ELuna::registerMethod< CPPClass >(L, "cppSum", &CPPClass::cppSum);

	// register a function
	ELuna::registerFunction(L, "cppPrint", &cppPrint);
	ELuna::registerFunction(L, "cppSum", &cppSum);

	const char* lua_function =
		"function lua_add(a,b) \r\n"
		"print(\"Hello, I am a lua Foo adding \", a ,b)\r\n"
		"return a+b\r\n"
		"end\r\n";
	int r = luaL_loadstring(L, lua_function);

	if (0 == r)
	{
		if (0 == lua_pcall(L, 0, LUA_MULTRET, 0))
		{
		}
	}

	ELuna::LuaFunction< int > f(L, "lua_add");
	r = f(3, 4);
	return r;
}

class gv_commandlet_lua_impl : public gv_refable
{
public:
	gv_commandlet_lua_impl()
	{
		L = luaL_newstate();
		luaL_openlibs(L);
		luaopen_math(L);
		lua_register(L, "my_function", my_function);
		// doesn't work , maybe just not compatible with current version....
		RegisterFoo(L);
	}
	~gv_commandlet_lua_impl()
	{
		lua_close(L);
	}

	void report_errors(lua_State* L, int status)
	{
		if (status != 0)
		{
			std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1); // remove error message
		}
	}

	bool execute(const gv_string_tmp& str)
	{
		if (str == "l")
		{
			L = ELuna::openLua();

			int r = testCPP(L);
			report_errors(L, r);
			// ELuna::doFile(L, fileName);

			// testLua(L);
			//	ELuna::closeLua(L);
			return true;
		}

		try
		{
			int s = luaL_loadstring(L, *str);

			if (s == 0)
			{
				// execute Lua program
				s = lua_pcall(L, 0, LUA_MULTRET, 0);
			}

			report_errors(L, s);
		}
		/*
    catch (const char * p)
    {
            GVM_ERROR("Error in exec " << str << "message is :" << p << "!!!" <<
    gv_endl);

    }*/
		catch (...)
		{
			GV_ASSERT(0);
		}
		return true;
	}

	lua_State* L;
};
gv_commandlet_lua::gv_commandlet_lua()
{
	m_pimpl = new gv_commandlet_lua_impl;
}
gv_commandlet_lua::~gv_commandlet_lua()
{
	m_pimpl = NULL;
}
bool gv_commandlet_lua::execute(const gv_string_tmp& str)
{
	return m_pimpl->execute(str);
}
};
#endif
