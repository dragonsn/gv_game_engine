//=================================================================================>
#if defined(GVM_DCL_PERSISTENT_TYPE_DEF_VAR)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type) gv_type_##type* s_type_##type;
#elif defined(GVM_DCL_PERSISTENT_TYPE_INIT)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type)   \
	s_type_##type = new gv_type_##type; \
	m_pimpl->m_native_types.add(s_type_##type);
#elif defined(GVM_DCL_PERSISTENT_TYPE_CASE_SELECT)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type) \
	case gv_id_space::TOKEN_##type:   \
	{                                 \
		return s_type_##type;         \
	}
#elif defined(GVM_DCL_PERSISTENT_TYPE_CASE_CREATE)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type) \
	case gve_data_type_##type:        \
	{                                 \
		return s_type_##type;         \
	}
#elif defined(GVM_DCL_PERSISTENT_TYPE_NEW_GV_TYPE)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type) GVM_NEW_TYPE(type)
#elif defined(GVM_DCL_PERSISTENT_TYPE_MAKE_ENUM)
#undef GVM_DCL_PERSISTENT_TYPE
#define GVM_DCL_PERSISTENT_TYPE(type) gve_data_type_##type,
#else
#error
#endif
//=================================================================================>

GVM_DCL_PERSISTENT_TYPE(byte)
GVM_DCL_PERSISTENT_TYPE(bool)
GVM_DCL_PERSISTENT_TYPE(char)
GVM_DCL_PERSISTENT_TYPE(short)
GVM_DCL_PERSISTENT_TYPE(ushort)
GVM_DCL_PERSISTENT_TYPE(int)
GVM_DCL_PERSISTENT_TYPE(uint)
GVM_DCL_PERSISTENT_TYPE(long)
GVM_DCL_PERSISTENT_TYPE(ulong)
GVM_DCL_PERSISTENT_TYPE(float)
GVM_DCL_PERSISTENT_TYPE(double)
GVM_DCL_PERSISTENT_TYPE(color)
GVM_DCL_PERSISTENT_TYPE(colorf)
GVM_DCL_PERSISTENT_TYPE(vector2)
GVM_DCL_PERSISTENT_TYPE(vector3)
GVM_DCL_PERSISTENT_TYPE(vector4)
GVM_DCL_PERSISTENT_TYPE(vector4i)
GVM_DCL_PERSISTENT_TYPE(vector2b)
GVM_DCL_PERSISTENT_TYPE(vector2w)
GVM_DCL_PERSISTENT_TYPE(vector3b)
GVM_DCL_PERSISTENT_TYPE(vector3i)
GVM_DCL_PERSISTENT_TYPE(matrix44)
GVM_DCL_PERSISTENT_TYPE(matrix43)
GVM_DCL_PERSISTENT_TYPE(euler)
GVM_DCL_PERSISTENT_TYPE(quat)
// complex.none-trivial struct here
GVM_DCL_PERSISTENT_TYPE(string)
GVM_DCL_PERSISTENT_TYPE(id)
GVM_DCL_PERSISTENT_TYPE(object_name)
GVM_DCL_PERSISTENT_TYPE(time_stamp)
GVM_DCL_PERSISTENT_TYPE(raw_data)

//=================================================================================>
#undef GVM_DCL_PERSISTENT_TYPE
#undef GVM_DCL_PERSISTENT_TYPE_DEF_VAR
#undef GVM_DCL_PERSISTENT_TYPE_INIT
#undef GVM_DCL_PERSISTENT_TYPE_CASE_SELECT
#undef GVM_DCL_PERSISTENT_TYPE_CASE_CREATE
#undef GVM_DCL_PERSISTENT_TYPE_NEW_GV_TYPE
#undef GVM_DCL_PERSISTENT_TYPE_MAKE_ENUM
//=================================================================================>