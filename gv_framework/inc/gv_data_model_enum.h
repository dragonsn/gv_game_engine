

#if !defined(GV_DOM_ENUM_NAME)
#error
#endif

#if !defined(GV_DOM_ENUM_FILE)
#define GV_DOM_ENUM_FILE \
	GVM_MACRO_TO_STRING(GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, .h))
#endif
//============================================================================================
//								:
//============================================================================================
#ifdef GV_DOM_ENUM_DCL
#define GV_DOM_MAKE_ENUM(x) GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x) \
							,
#define GV_DOM_MAKE_ENUM_2(x, val) GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x) = val,

enum GV_DOM_ENUM_NAME
{
#include GV_DOM_ENUM_FILE
	GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, _last)
};
GV_DOM_ENUM_FUNC_HELPER_TO_STRING(GV_DOM_ENUM_NAME);
GV_DOM_ENUM_FUNC_HELPER_FR_STRING(GV_DOM_ENUM_NAME);
GV_DOM_ENUM_FUNC_HELPER_REGISTER(GV_DOM_ENUM_NAME);
#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2
#endif

//============================================================================================
//								:
//============================================================================================

#ifdef GV_DOM_ENUM_DCL_ID
#define GV_DOM_MAKE_ENUM(x) GVM_MAKE_ID(x)
#define GV_DOM_MAKE_ENUM_2(x, val) GVM_MAKE_ID(x)

#define GVM_MAKE_ID GVM_MAKE_ID_DCL
#include "gv_id_pp.h"
#include GV_DOM_ENUM_FILE
#undef GVM_MAKE_ID

#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2
#endif //=GV_DOM_ENUM_DCL_ID

//============================================================================================
//								:
//============================================================================================
#ifdef GV_DOM_ENUM_IMP
#define GV_DOM_MAKE_ENUM(x)                      \
	case GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x): \
		return #x;
#define GV_DOM_MAKE_ENUM_2(x, val)               \
	case GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x): \
		return #x;

GV_DOM_ENUM_FUNC_HELPER_TO_STRING(GV_DOM_ENUM_NAME)
{
	switch (e)
	{
#include GV_DOM_ENUM_FILE
	}
	return "Unknown";
}
#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2

#define GV_DOM_MAKE_ENUM(x) \
	if (ss == #x)           \
		return GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x);
#define GV_DOM_MAKE_ENUM_2(x, val) \
	if (ss == #x)                  \
		return GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x);
GV_DOM_ENUM_FUNC_HELPER_FR_STRING(GV_DOM_ENUM_NAME)
{
	gv_string_tmp ss = s;
#include GV_DOM_ENUM_FILE
	return GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, _last);
}
#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2

#define GV_DOM_MAKE_ENUM(x) \
	info->add_pair(GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x), gv_id(#x));
#define GV_DOM_MAKE_ENUM_2(x, val) \
	info->add_pair(GV_MACRO_CONCATE2(GV_DOM_ENUM_NAME, x), gv_id(#x));
GV_DOM_ENUM_FUNC_HELPER_REGISTER(GV_DOM_ENUM_NAME)
{
	gv_string_tmp s = GV_REMINDER_STR2(GV_DOM_ENUM_NAME);
	gv_enum_info* info = sandbox->register_enum(gv_id(*s));
#include GV_DOM_ENUM_FILE
}
#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2

#endif // GV_DOM_ENUM_IMP

#ifdef GV_DOM_ENUM_IMP_ID
#define GV_DOM_MAKE_ENUM(x) GVM_MAKE_ID(x)
#define GV_DOM_MAKE_ENUM_2(x, val) GVM_MAKE_ID(x)

#define GVM_MAKE_ID GVM_MAKE_ID_IMP
#include "gv_id_pp.h"
#include GV_DOM_ENUM_FILE
#undef GVM_MAKE_ID

#undef GV_DOM_MAKE_ENUM
#undef GV_DOM_MAKE_ENUM_2
#endif //=GV_DOM_ENUM_DCL_ID

#undef GV_DOM_ENUM_FILE
#undef GV_DOM_ENUM_NAME
#undef GV_DOM_ENUM_DCL
#undef GV_DOM_ENUM_DCL_ID
#undef GV_DOM_ENUM_IMP