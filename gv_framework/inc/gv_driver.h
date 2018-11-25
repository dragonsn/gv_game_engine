#pragma once
namespace gv
{

//====================================================================================
//
//	this is a pattern , can be used to init plug-ins at the beginning
//without
//	the info is actual factory . the factories registered in a list.
//
//====================================================================================
class gv_driver;
class gv_driver_info_imp;
class gv_sandbox;
typedef gv_driver*(gv_func_driver_ctor)(void);
class gv_driver_info
{

public:
	gv_driver_info();
	gv_driver_info(const char* name, gv_func_driver_ctor func);
	~gv_driver_info();
	const char* get_name();
	gv_driver* create_driver();
	gv_driver* get_driver();
	gv_driver_info* get_next_driver_info();

public:
	static void static_register_driver_info(gv_driver_info* info);
	static void static_unregister_all();
	static gv_driver_info* static_get_first_driver_info();
	static gv_driver_info* static_get_driver_info(const char*);
	static void static_create_drivers();
	static void static_create_drivers(gvt_array< gv_string >& names);
	static void static_destroy_drivers();
	static bool static_init_all_drivers(gv_sandbox*);
	static bool static_uninit_all_drivers(gv_sandbox*);
	static bool static_create_event_processors(gv_sandbox*);

private:
	gv_driver_info_imp* m_imp;
};

//====================================================================================
// driver can be in DLL module or static linked class.
//
//====================================================================================
class gv_driver
{
	friend class gv_driver_info;

public:
	gv_driver();
	virtual ~gv_driver();
	virtual void init(gv_sandbox*);
	virtual void uninit(gv_sandbox*);
	virtual bool get_processor_descs(gvt_array< gv_event_processor_desc >& descs);

	gv_driver_info* get_driver_info();

private:
	gv_driver_info* m_info;
};
};

#define GVM_IMP_DRIVER(name)                   \
	static gv_driver* s_create_driver_##name() \
	{                                          \
		return new gv_driver_##name;           \
	};                                         \
	gv_driver_info driver_info_##name(#name, s_create_driver_##name);

#define GVM_REG_DRIVER(name) \
	gv_driver_info::static_register_driver_info(&driver_info_##name);

#define GVM_REG_FIRST_DRIVER(name)           \
	gv_driver_info::static_unregister_all(); \
	GVM_REG_DRIVER(name);

#define GVM_DCL_DRIVER(name)                  \
	namespace gv                              \
	{                                         \
	extern gv_driver_info driver_info_##name; \
	}

//#define QUOTEME(M)       #M
//#define INCLUDE_FILE(M)  QUOTEME(M##_impl_win.hpp)
//#include INCLUDE_FILE(module)
// C and C++ languages explicitly prohibit forming preprocessor directives as
// the result of macro expansion.
// This means that you can't include a preprocessor directive into a macro
// replacement list.
// And if you try to trick the preprocessor by "building" a new preprocessor
// directive through concatenation (and tricks like that), the behavior is
// undefined.
/* [CODE TEMPLATE] : replace name %%name%% driver name
class gv_driver_%%name%% :public gv_driver
{
        public:
                        gv_driver_%%name%%()	{	};
                        ~gv_driver_%%name%%()	{	};
                        void init(gv_sandbox * sandbox)	{
                                gv_driver::init(sandbox);
                                gv_native_module_guard g(sandbox, "%%name%%");
                                #undef  GVM_REGISTER_CLASS
                                #define GVM_REGISTER_CLASS
GVM_REGISTER_CLASS_IMP
                                #include "my_classes.h"
                        }
                        void uninit(gv_sandbox * sandbox)
                        {
                                #undef  GVM_REGISTER_CLASS
                                #define GVM_REGISTER_CLASS
GVM_REGISTER_CLASS_DEL
                                #include "my_classes.h"
                                gv_driver::uninit(sandbox);
                        }
                        bool get_processor_descs(gvt_array <
gv_event_processor_desc> & descs)
                        {
                                return true;
                        };
};
GVM_IMP_DRIVER(%%name%%);
*/
