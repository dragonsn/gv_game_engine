
#if GVM_REGISTER_CLASS == GVM_REGISTER_CLASS_DCL
#undef GVM_REGISTER_CLASS
#define GVM_REGISTER_CLASS(cls)                                      \
	namespace gv                                                     \
	{                                                                \
	extern void _gv_initialize_class_##cls(gv::gv_sandbox* sandbox); \
	extern gv_class_info* _##cls##_s_ptr;                            \
	}
#elif GVM_REGISTER_CLASS == GVM_REGISTER_CLASS_IMP
#undef GVM_REGISTER_CLASS
#define GVM_REGISTER_CLASS(cls) _gv_initialize_class_##cls(sandbox);
#elif GVM_REGISTER_CLASS == GVM_REGISTER_CLASS_DEL
#undef GVM_REGISTER_CLASS
#define GVM_REGISTER_CLASS(cls)             \
	sandbox->unregister_class(gv_id(#cls)); \
	_##cls##_s_ptr = NULL;
#else
#error
#endif

#define GVM_REGISTER_STRUCT(x) GVM_REGISTER_CLASS(x)