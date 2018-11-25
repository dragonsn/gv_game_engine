#if defined(GVM_DCL_VERTEX_FORMAT_ENUM)
#undef GVM_DCL_VERTEX_FORMAT
#define GVM_DCL_VERTEX_FORMAT(base_type, size, is_normal) \
	gve_vertex_format_##base_type##size##is_normal,
#elif defined(GVM_DCL_VERTEX_FORMAT_TYPEDEF)
#undef GVM_DCL_VERTEX_FORMAT
#define GVM_DCL_VERTEX_FORMAT(base_type, size, is_normal) \
	typedef base_type base_type##size##is_normal[size];
#elif defined(GVM_DCL_VERTEX_FORMAT_GET_STRING)
#undef GVM_DCL_VERTEX_FORMAT
#define GVM_DCL_VERTEX_FORMAT(base_type, size, is_normal) \
	case gve_vertex_format_##base_type##size##is_normal:  \
		return #base_type #size #is_normal;
#elif defined(GVM_DCL_VERTEX_FORMAT_FROM_STRING)
#undef GVM_DCL_VERTEX_FORMAT
#define GVM_DCL_VERTEX_FORMAT(base_type, size, is_normal) \
	if (s == #base_type #size #is_normal)                 \
		return gve_vertex_format_##base_type##size##is_normal;
#else
#error
#endif

GVM_DCL_VERTEX_FORMAT(FLOAT, 1, )
GVM_DCL_VERTEX_FORMAT(FLOAT, 2, )
GVM_DCL_VERTEX_FORMAT(FLOAT, 3, )
GVM_DCL_VERTEX_FORMAT(FLOAT, 4, )
GVM_DCL_VERTEX_FORMAT(COLOR, , )
GVM_DCL_VERTEX_FORMAT(UBYTE, 4, )
GVM_DCL_VERTEX_FORMAT(SHORT, 2, )
GVM_DCL_VERTEX_FORMAT(SHORT, 4, )
GVM_DCL_VERTEX_FORMAT(UBYTE, 4, N)
GVM_DCL_VERTEX_FORMAT(SHORT, 2, N)
GVM_DCL_VERTEX_FORMAT(SHORT, 4, N)
GVM_DCL_VERTEX_FORMAT(USHORT, 2, N)
GVM_DCL_VERTEX_FORMAT(USHORT, 4, N)
// GVM_DCL_VERTEX_FORMAT(UDEC3,,)
// GVM_DCL_VERTEX_FORMAT(DEC3,,N)
GVM_DCL_VERTEX_FORMAT(FLOAT16_, 2, )
GVM_DCL_VERTEX_FORMAT(FLOAT16_, 4, )
GVM_DCL_VERTEX_FORMAT(MAX, , )

//=====================================================================>
#undef GVM_DCL_VERTEX_FORMAT
#undef GVM_DCL_VERTEX_FORMAT_ENUM
#undef GVM_DCL_VERTEX_FORMAT_GET_STRING
#undef GVM_DCL_VERTEX_FORMAT_FROM_STRING
#undef GVM_DCL_VERTEX_FORMAT_TYPEDEF