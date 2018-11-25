#ifndef GV_EVENT_INC_FILE
#error
#endif

//============================================================================================
//								:id implement
//============================================================================================
#if defined GVM_EVENT_IMP_ID
#define GVM_DCL_EVENT GV_REGISTER_EVENT_ID
#define GVM_DCL_EVENT_MUTE GVM_DCL_EVENT
#define GVM_END_DCL_EVENT
#define GVM_EVENT_VAR(...)
#include GV_EVENT_INC_FILE
#undef GVM_DCL_EVENT
#undef GVM_END_DCL_EVENT
#undef GVM_EVENT_VAR
#undef GVM_DCL_EVENT_MUTE
#endif
//============================================================================================
//								:is_less
//implement
//============================================================================================
#if defined GVM_EVENT_IMP_STRUCT
#define GVM_DCL_EVENT(name, id) GVM_DOM_ELEMENT(gv_object_event_##name)
#define GVM_END_DCL_EVENT GVM_DOM_END_E
#define GVM_EVENT_VAR GVM_DOM_ATTRIB
#define GVM_DCL_EVENT_MUTE GVM_DCL_EVENT

#define GV_DOM_FILE GV_EVENT_INC_FILE
#ifndef GV_DOM_SUPER_CLASS
#define GV_DOM_SUPER_CLASS gv_object_event
#endif
#define GV_DOM_CONSTRUCT(name)                   \
	;                                            \
	static gv_object_event_id static_event_id(); \
	const char* get_id_string();                 \
	bool do_check();                             \
	bool is_mute();

#define GVM_DOM_DECL
#define GVM_DOM_IMP_IS_EQUAL
#define GVM_DOM_IMP_IS_LESS
#define GVM_DOM_IMP_XML
#define GVM_DOM_IMP_SERIALIZE
#define GVM_DOM_IMP_COPY
#include "gv_data_model_ex.h"

#undef GVM_DCL_EVENT
#undef GVM_END_DCL_EVENT
#undef GVM_EVENT_VAR
#undef GVM_DCL_EVENT_MUTE

#define GVM_END_DCL_EVENT
#define GVM_EVENT_VAR(...)
#define GVM_DCL_EVENT_BASE(name, id)                                    \
	inline gv_object_event_##name::gv_object_event_##name()             \
	{                                                                   \
		init();                                                         \
		this->m_id = gv_object_event_id_##name;                         \
	}                                                                   \
	inline gv_object_event_id gv_object_event_##name::static_event_id() \
	{                                                                   \
		return gv_object_event_id_##name;                               \
	}                                                                   \
	inline const char* gv_object_event_##name::get_id_string()          \
	{                                                                   \
		return "gv_object_event_id_" #name;                             \
	}                                                                   \
	inline bool gv_object_event_##name::do_check()                      \
	{                                                                   \
		return (m_id == gv_object_event_id_##name);                     \
	}

#define GVM_DCL_EVENT(name, id)                   \
	GVM_DCL_EVENT_BASE(name, id)                  \
	inline bool gv_object_event_##name::is_mute() \
	{                                             \
		return false;                             \
	}

#define GVM_DCL_EVENT_MUTE(name, id)              \
	GVM_DCL_EVENT_BASE(name, id)                  \
	inline bool gv_object_event_##name::is_mute() \
	{                                             \
		return true;                              \
	}
#include GV_EVENT_INC_FILE

#undef GVM_DCL_EVENT
#undef GVM_END_DCL_EVENT
#undef GVM_EVENT_VAR
#undef GVM_DCL_EVENT_MUTE
#endif

//============================================================================================
//								:is_less
//implement
//============================================================================================
#if defined GVM_EVENT_IMP_ENUM
#define GVM_DCL_EVENT(name, id) gve_event_##name = id,
#define GVM_END_DCL_EVENT
#define GVM_EVENT_VAR(...)
#define GVM_DCL_EVENT_MUTE GVM_DCL_EVENT
#include GV_EVENT_INC_FILE
#undef GVM_DCL_EVENT
#undef GVM_END_DCL_EVENT
#undef GVM_EVENT_VAR
#undef GVM_DCL_EVENT_MUTE
#endif
//============================================================================================
#undef GV_EVENT_INC_FILE
#undef GVM_EVENT_IMP_STRUCT
#undef GVM_EVENT_IMP_ID
#undef GVM_EVENT_IMP_ENUM
