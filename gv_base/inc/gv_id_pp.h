#if GVM_MAKE_ID == GVM_MAKE_ID_DCL
#undef GVM_MAKE_ID
#define GVM_MAKE_ID(name) extern gv_id gv_id_##name;
#elif GVM_MAKE_ID == GVM_MAKE_ID_IMP
#undef GVM_MAKE_ID
#define GVM_MAKE_ID(name) gv_id gv_id_##name;
#elif GVM_MAKE_ID == GVM_MAKE_ID_INIT
#undef GVM_MAKE_ID
#define GVM_MAKE_ID(name) gv_id_##name = #name;
#elif GVM_MAKE_ID == GVM_MAKE_ID_RELEASE
#undef GVM_MAKE_ID
#define GVM_MAKE_ID(name) gv_id_##name.release();
#else
#error
#endif
