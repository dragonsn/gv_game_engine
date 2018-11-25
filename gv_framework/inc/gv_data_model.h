#if !defined(GV_DOM_FILE)
#error
#endif

#if !defined(GV_DOM_NAMESPACE)
#error
#endif

//============================================================================================
//								:decl
//============================================================================================
#if !GVM_DOM_IMP_ONLY
#define GVM_DOM_ELEMENT(T) \
	class T                \
	{                      \
	public:                \
		T(){};             \
		~T(){};            \
		T(const T& src);   \
		T& operator=(const T& src);
#define GVM_DOM_END_E \
	}                 \
	;
#define GVM_DOM_ATTRIB(type, name) type name;
namespace GV_DOM_NAMESPACE
{
#include GV_DOM_FILE
}
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#endif
//============================================================================================
//								:rtti
//============================================================================================
#if !GVM_DOM_DCL_ONLY
#define GVM_DOM_ELEMENT GVM_IMP_STRUCT
#define GVM_DOM_END_E GVM_END_STRUCT
#define GVM_DOM_ATTRIB GVM_VAR
namespace GV_DOM_NAMESPACE
{
#include GV_DOM_FILE
}
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
//============================================================================================
//								:register , and
//unregister
//============================================================================================
#if !GVM_DOM_NO_REGISTER
#define GVM_DOM_ELEMENT GVM_REGISTER_STRUCT
#define GVM_DOM_END_E
#define GVM_DOM_ATTRIB(type, name)

namespace GV_DOM_NAMESPACE
{

void register_classes(gv_sandbox* sandbox)
{
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_class_macro_pp.h"
#include GV_DOM_FILE
	sandbox->init_classes();
#undef GVM_REGISTER_CLASS
};
void unregister_classes(gv_sandbox* sandbox)
{
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_class_macro_pp.h"
#include GV_DOM_FILE
#undef GVM_REGISTER_CLASS
}

struct scope_register
{
	scope_register(gv_sandbox* psandbox)
	{
		register_classes(psandbox);
		m_sandbox = psandbox;
	}
	~scope_register()
	{
		unregister_classes(m_sandbox);
	}
	gv_sandbox* m_sandbox;
};
};
#undef GVM_DOM_ELEMENT
#undef GVM_DOM_END_E
#undef GVM_DOM_ATTRIB
#endif // GVM_DOM_NO_REGISTER

#endif // GVM_DOM_DCL_ONLY

#undef GV_DOM_FILE
#undef GVM_DOM_DCL_ONLY
#undef GVM_DOM_IMP_ONLY
#undef GV_DOM_NAMESPACE
#undef GVM_DOM_NO_REGISTER