#pragma once
namespace gv
{
class gv_lua_vm : public gv_refable
{
public:
	gv_lua_vm();
	virtual ~gv_lua_vm();

public:
	template < class type_of_return, class type_of_p1 p1 >
	type_of_return call_lua_function(type_of_p1)
	{
	}

	// template < class type_of_return , class type_of_p1 p1
};
}