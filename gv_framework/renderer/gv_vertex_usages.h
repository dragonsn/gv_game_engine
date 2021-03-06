#if defined(GVM_DCL_VERTEX_USAGE_ENUM)
#undef GVM_DCL_VERTEX_USAGE
#define GVM_DCL_VERTEX_USAGE(base_type) gve_vertex_usage_##base_type,
#elif defined(GVM_DCL_VERTEX_USAGE_GET_STRING)
#undef GVM_DCL_VERTEX_USAGE
#define GVM_DCL_VERTEX_USAGE(base_type) \
	case gve_vertex_usage_##base_type:  \
		return #base_type;
#elif defined(GVM_DCL_VERTEX_USAGE_FROM_STRING)
#undef GVM_DCL_VERTEX_USAGE
#define GVM_DCL_VERTEX_USAGE(base_type) \
	if (s == #base_type)                \
		return gve_vertex_usage_##base_type;
#else
#error
#endif

GVM_DCL_VERTEX_USAGE(POSITION)
GVM_DCL_VERTEX_USAGE(BLENDWEIGHT)
GVM_DCL_VERTEX_USAGE(BLENDINDICE)
GVM_DCL_VERTEX_USAGE(NORMAL)
GVM_DCL_VERTEX_USAGE(PSIZE)
GVM_DCL_VERTEX_USAGE(TEXCOORD)
GVM_DCL_VERTEX_USAGE(TANGENT)
GVM_DCL_VERTEX_USAGE(BINORMAL)
GVM_DCL_VERTEX_USAGE(TESSFACTOR)
GVM_DCL_VERTEX_USAGE(POSITIONT)
GVM_DCL_VERTEX_USAGE(COLOR)
GVM_DCL_VERTEX_USAGE(FOG)
GVM_DCL_VERTEX_USAGE(DEPTH)
GVM_DCL_VERTEX_USAGE(SAMPLE)
GVM_DCL_VERTEX_USAGE(MAX)

#undef GVM_DCL_VERTEX_USAGE
#undef GVM_DCL_VERTEX_USAGE_ENUM
#undef GVM_DCL_VERTEX_USAGE_GET_STRING
#undef GVM_DCL_VERTEX_USAGE_FROM_STRING