#ifndef GV_NET_CMD_INC_FILE
#error
#endif
//============================================================================================
//								:id implement
//============================================================================================
#if defined GVM_NET_CMD_IMP_ID
#define GVM_DCL_NET_CMD(name, num) GVM_REGISTER_NET_CMD_ID(name, num)
#define GVM_END_DCL_NET_CMD
#define GVM_NET_CMD_VAR(...)
#include GV_NET_CMD_INC_FILE
#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR
#endif
//============================================================================================
//								:struct
//implement
//============================================================================================
#if defined GVM_NET_CMD_IMP_STRUCT
#define GVM_DCL_NET_CMD(name, id) GVM_DOM_ELEMENT(gv_net_cmd_##name)
#define GVM_END_DCL_NET_CMD GVM_DOM_END_E
#define GVM_NET_CMD_VAR GVM_DOM_ATTRIB

#define GV_DOM_FILE GV_NET_CMD_INC_FILE
#define GV_DOM_CONSTRUCT(name)            \
	{};                                   \
	static gv_ushort static_net_cmd_id(); \
	bool check_is_valid();                \
	static const char* static_name()      \
	{                                     \
		return #name;                     \
	};

#define GVM_DOM_DECL
#define GVM_DOM_IMP_IS_EQUAL
#define GVM_DOM_IMP_IS_LESS
#define GVM_DOM_IMP_XML
#define GVM_DOM_IMP_SERIALIZE
#define GVM_DOM_IMP_COPY
#include "..\inc\gv_data_model_ex.h"

#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR

#define GVM_END_DCL_NET_CMD
#define GVM_NET_CMD_VAR(...)
#define GVM_DCL_NET_CMD(name, id)                                \
	inline gv_ushort gv_net_cmd_##name::static_net_cmd_id()      \
	{                                                            \
		return gv_net_cmd_id_##name;                             \
	}                                                            \
	inline bool gv_net_cmd_##name::check_is_valid()              \
	{                                                            \
		return gvt_is_valid_net_cmd< gv_net_cmd_##name >(*this); \
	}

#include GV_NET_CMD_INC_FILE
#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR
#endif

//============================================================================================
//								:enum generator
//implement
//============================================================================================
#if defined GVM_NET_CMD_IMP_ENUM
#define GVM_DCL_NET_CMD(name, id) gve_net_cmd_##name = id,
#define GVM_END_DCL_NET_CMD
#define GVM_NET_CMD_VAR(...)
#include GV_NET_CMD_INC_FILE
#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR
#endif

//============================================================================================
//								:enum generator
//implement
//============================================================================================
#if defined GVM_NET_CMD_IMP_RECV
#define GVM_DCL_NET_CMD(name, id)                                                \
	case gv_net_cmd_id_##name:                                                   \
	{                                                                            \
		gv_net_cmd_##name cmd;                                                   \
		packet >> cmd;                                                           \
		if (packet.eof())                                                        \
		{                                                                        \
			error.m_has_error = true;                                            \
			error.m_error_string = "unexpected end of packet!!";                 \
			break;                                                               \
		}                                                                        \
		if (!cmd.check_is_valid())                                               \
		{                                                                        \
			error.m_has_error = true;                                            \
			error.m_error_string = "invalid net command !!";                     \
			break;                                                               \
		}                                                                        \
		ret = recieve_net_cmd(cmd);                                              \
		GVM_DEBUG_LOG(net, "[recieve_net_cmd]" << cmd.static_name() << "  from:" \
											   << host_name() << gv_endl);       \
		break;                                                                   \
	}
#define GVM_END_DCL_NET_CMD
#define GVM_NET_CMD_VAR(...)
#include GV_NET_CMD_INC_FILE
#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR
#endif

//============================================================================================
#if defined GVM_NET_CMD_IMP_HANDLE
#define GVM_DCL_NET_CMD(name, id)                                                \
	case gv_net_cmd_id_##name:                                                   \
	{                                                                            \
		gv_net_cmd_##name* cmd = (gv_net_cmd_##name*)pc;                         \
		GV_NET_CMD_HANDLER(cmd);                                                 \
		GVM_DEBUG_LOG(net, "[" << GVM_MACRO_TO_STRING(GV_NET_CMD_HANDLER) << "]" \
							   << cmd.static_name() << "  from:" << host_name()  \
							   << gv_endl);                                      \
		break;                                                                   \
	}
#define GVM_END_DCL_NET_CMD
#define GVM_NET_CMD_VAR(...)
#include GV_NET_CMD_INC_FILE
#undef GVM_DCL_NET_CMD
#undef GVM_END_DCL_NET_CMD
#undef GVM_NET_CMD_VAR
#endif
//============================================================================================
#undef GV_NET_CMD_INC_FILE
#undef GVM_NET_CMD_IMP_STRUCT
#undef GVM_NET_CMD_IMP_ID
#undef GVM_NET_CMD_IMP_ENUM
#undef GVM_NET_CMD_IMP_RECV
#undef GVM_NET_CMD_IMP_HANDLE
#undef GV_NET_CMD_HANDLER
