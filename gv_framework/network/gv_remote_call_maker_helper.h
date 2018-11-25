
//============================================================================================
//								:id implement
//============================================================================================
#if defined GVM_REMOTE_CALL_IMP_ID
#define GVM_DCL_REMOTE_CALL_2(name, num) GVM_REGISTER_REMOTE_CALL_ID(name, num)
#define GVM_END_DCL_REMOTE_CALL
#define GVM_REMOTE_CALL_VAR(...)
#include GV_REMOTE_CALL_INC_FILE
#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR
#endif
//============================================================================================
//								:check id
//implement
//============================================================================================
#if defined(GVM_REMOTE_CALL_IMP_CHECK_ID) && defined(GVM_REMOTE_CALL_IMP_ID)
#define GVM_DCL_REMOTE_CALL_2(name, num) GVM_CHECK_REMOTE_CALL_ID(num)
#define GVM_END_DCL_REMOTE_CALL
#define GVM_REMOTE_CALL_VAR(...)
#include GV_REMOTE_CALL_INC_FILE
#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR
#endif
//============================================================================================
//								:struct
//implement
//============================================================================================
#if defined GVM_REMOTE_CALL_DCL_STRUCT
#define GVM_DCL_REMOTE_CALL_2(name, id) GVM_DOM_ELEMENT(gv_remote_call_##name)
#define GVM_END_DCL_REMOTE_CALL GVM_DOM_END_E
#define GVM_REMOTE_CALL_VAR GVM_DOM_ATTRIB

#define GV_DOM_FILE GV_REMOTE_CALL_INC_FILE
#define GV_DOM_CONSTRUCT(name)                                                \
	{                                                                         \
		m_header =                                                            \
			gv_net_cmd(true, GV_REMOTE_CALL_IS_ACK, static_remote_call_id()); \
	};                                                                        \
	static gv_ushort static_remote_call_id();                                 \
	virtual const char* get_name();
#define GV_DOM_STREAM gv_packet

#define GVM_DOM_DECL
#define GVM_DOM_IMP_IS_EQUAL
#define GVM_DOM_IMP_XML
#define GV_DOM_SUPER_CLASS gv_remote_call
#include "..\inc\gv_data_model_ex.h"

#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR

#endif
//============================================================================================
//								:struct
//implement
//============================================================================================
#if defined GVM_REMOTE_CALL_IMP_STRUCT
#define GVM_DCL_REMOTE_CALL_2(name, id) GVM_DOM_ELEMENT(gv_remote_call_##name)
#define GVM_END_DCL_REMOTE_CALL GVM_DOM_END_E
#define GVM_REMOTE_CALL_VAR GVM_DOM_ATTRIB

#define GV_DOM_FILE GV_REMOTE_CALL_INC_FILE
#define GV_DOM_STREAM gv_packet
#define GVM_DOM_IMP_SERIALIZE
#include "..\inc\gv_data_model_ex.h"

#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR

#define GVM_DCL_REMOTE_CALL_2(name, id)                      \
	gv_ushort gv_remote_call_##name::static_remote_call_id() \
	{                                                        \
		return gv_remote_call_id_##name;                     \
	}                                                        \
	const char* gv_remote_call_##name::get_name()            \
	{                                                        \
		return #name;                                        \
	}
#define GVM_END_DCL_REMOTE_CALL
#define GVM_REMOTE_CALL_VAR(...)
#include GV_REMOTE_CALL_INC_FILE
#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR
#endif

//============================================================================================
//								:enum generator
//implement
//============================================================================================
#if defined(GVM_REMOTE_CALL_IMP_RECV_IMP)
#define GVM_DCL_REMOTE_CALL_2(name, id)                                            \
	case gv_remote_call_id_##name:                                                 \
	{                                                                              \
		gv_remote_call_##name call;                                                \
		call << packet;                                                            \
		if (packet.eof())                                                          \
		{                                                                          \
			error.m_has_error = true;                                              \
			error.m_error_string = "unexpected end of packet!!";                   \
			break;                                                                 \
		}                                                                          \
		GVM_DEBUG_LOG(net, "[recieve_remote_call]" << call.get_name() << "  from:" \
												   << host_name() << gv_endl);     \
		ret = exec_remote_call(call);                                              \
		break;                                                                     \
	}
#define GVM_END_DCL_REMOTE_CALL
#define GVM_REMOTE_CALL_VAR(...)
#include GV_REMOTE_CALL_INC_FILE
#undef GVM_DCL_REMOTE_CALL_2
#undef GVM_END_DCL_REMOTE_CALL
#undef GVM_REMOTE_CALL_VAR
#endif
//============================================================================================
