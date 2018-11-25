#ifndef GV_NET_REPLICA_INC_FILE
#error
#endif

#define GVM_DO_REPLICA_IF(x)
#define GVM_DO_REPLICA_CONTINUE(x)
#define GVM_DO_REPLICA_END_IF
#define GVM_NET_REPLICA_PRIORITY(x)
//============================================================================================
//								:id implement
//============================================================================================
#if defined GVM_NET_REPLICA_IMP_ID
#define GVM_DCL_NET_REPLICA GVM_REGISTER_NET_REPLICA_ID
#define GVM_END_DCL_NET_REPLICA
#define GVM_NET_REPLICA_VAR(...)
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#endif
//============================================================================================
//								:register class
//============================================================================================
#if defined GVM_NET_REPLICA_REG_CLS
#define GVM_DCL_NET_REPLICA(name, id) GVM_REGISTER_STRUCT(gv_net_replica_##name)
#define GVM_END_DCL_NET_REPLICA
#define GVM_NET_REPLICA_VAR(...)
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#endif
//============================================================================================
//								:struct  decl
//============================================================================================
#if defined GVM_NET_REPLICA_DCL_STRUCT
#define GVM_DCL_NET_REPLICA(name, id)                                       \
	class gv_net_replica_##name : public gv_net_replica                     \
	{                                                                       \
	public:                                                                 \
		typedef gv_net_replica super;                                       \
		gv_net_replica_##name()                                             \
		{                                                                   \
			GVM_ZERO_ME_EXCLD_SUPER;                                        \
			m_class = static_class();                                       \
			m_replica_id = static_id();                                     \
		}                                                                   \
		gv_net_replica_##name(const gv_net_replica_##name& src)             \
		{                                                                   \
			(*this) = src;                                                  \
		}                                                                   \
		~gv_net_replica_##name()                                            \
		{                                                                   \
		}                                                                   \
		static gv_ushort static_id()                                        \
		{                                                                   \
			return id;                                                      \
		}                                                                   \
		gv_net_replica_##name& operator=(const gv_net_replica_##name& src); \
		virtual gv_bool update_replica(gv_replica_update_result& result,    \
									   gv_int array_index);                 \
		static gv_class_info* static_class();
#define GVM_NET_REPLICA_VAR(type, name) \
	type name;                          \
	gv_byte name##_is_dirty;            \
	type name##_sent;                   \
	gv_uint name##_sq_no;

#define GVM_END_DCL_NET_REPLICA \
	}                           \
	;
//============================================================================================
//								:
//============================================================================================
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR

#define GVM_DCL_NET_REPLICA(name, id)                               \
	inline gv_net_replica_##name& gv_net_replica_##name::operator=( \
		const gv_net_replica_##name& src)                           \
	{
#define GVM_NET_REPLICA_VAR(type, name) name = src.name;

#define GVM_END_DCL_NET_REPLICA \
	return *this;               \
	}                           \
	;
//============================================================================================
//								:
//============================================================================================
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#endif

//============================================================================================
//								:struct
//implement
//============================================================================================
#if defined GVM_NET_REPLICA_IMP_STRUCT
#define GVM_DCL_NET_REPLICA(name, id) GVM_DOM_ELEMENT(gv_net_replica_##name)
#define GVM_END_DCL_NET_REPLICA GVM_DOM_END_E
#define GVM_NET_REPLICA_VAR(type, name)       \
	GVM_DOM_FLAG(need_replicate);             \
	GVM_DOM_ATTRIB(type, name);               \
	GVM_DOM_CLEAR_FLAG(need_replicate);       \
	GVM_DOM_ATTRIB(gv_byte, name##_is_dirty); \
	GVM_DOM_ATTRIB(type, name##_sent);        \
	GVM_DOM_ATTRIB(gv_uint, name##_sq_no);
//============================================================================================
//								:
//============================================================================================
#define GV_DOM_FILE GV_NET_REPLICA_INC_FILE
#define GVM_DOM_RTTI
#include "..\inc\gv_data_model_ex.h"

#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR

//============================================================================================
//								:
//============================================================================================
#define GVM_DCL_NET_REPLICA(name, id)                    \
	gv_class_info* gv_net_replica_##name::static_class() \
	{                                                    \
		return _gv_net_replica_##name##_s_ptr;           \
	}
#define GVM_END_DCL_NET_REPLICA
#define GVM_NET_REPLICA_VAR(...)
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#endif

//============================================================================================
//								:enum generator
//implement
//============================================================================================
#if defined GVM_NET_REPLICA_IMP_REPLICA
#undef GVM_DO_REPLICA_IF
#undef GVM_DO_REPLICA_CONTINUE
#undef GVM_DO_REPLICA_END_IF
#undef GVM_NET_REPLICA_PRIORITY
#define GVM_DCL_NET_REPLICA(name, id)                                            \
	bool gv_net_replica_##name::update_replica(gv_replica_update_result& result, \
											   gv_int array_index)               \
	{                                                                            \
		gv_int current_priority = 0;                                             \
		gv_int current_index = 0;                                                \
		gv_var_info* pvar = NULL;                                                \
		bool condition = true;

#define GVM_DO_REPLICA_IF(x)            \
	{                                   \
		bool condition_bak = condition; \
		{                               \
			bool condition = x;
#define GVM_DO_REPLICA_CONTINUE(x) \
	if (!x)                        \
		return true;
#define GVM_DO_REPLICA_END_IF  \
	}                          \
	condition = condition_bak; \
	}
#define GVM_NET_REPLICA_PRIORITY(x) current_priority = x;
#define GVM_NET_REPLICA_VAR(type, name)                                           \
	if (condition)                                                                \
		update_one_var(current_index, name##_is_dirty,                            \
					   name##_is_dirty ? name##_is_dirty : (name != name##_sent), \
					   result, current_priority, array_index, (gv_byte*) & name,  \
					   (gv_byte*)&name##_sent);                                   \
	current_index += 4;

#define GVM_END_DCL_NET_REPLICA \
	return true;                \
	}
//============================================================================================
//								:
//============================================================================================
#include GV_NET_REPLICA_INC_FILE
#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#undef GVM_DO_REPLICA_IF
#undef GVM_DO_REPLICA_CONTINUE
#undef GVM_DO_REPLICA_END_IF
#undef GVM_NET_REPLICA_PRIORITY
#endif

#undef GVM_DCL_NET_REPLICA
#undef GVM_END_DCL_NET_REPLICA
#undef GVM_NET_REPLICA_VAR
#undef GVM_DO_REPLICA_IF
#undef GVM_DO_REPLICA_CONTINUE
#undef GVM_DO_REPLICA_END_IF
#undef GVM_NET_REPLICA_PRIORITY
//============================================================================================
#undef GV_NET_REPLICA_INC_FILE
#undef GVM_NET_REPLICA_IMP_STRUCT
#undef GVM_NET_REPLICA_IMP_ID
#undef GVM_NET_REPLICA_IMP_ENUM
#undef GVM_NET_REPLICA_IMP_REPLICA
#undef GVM_NET_REPLICA_REG_CLS
#undef GVM_NET_REPLICA_DCL_STRUCT