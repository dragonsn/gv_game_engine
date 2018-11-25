#include "gv_class_macro_pp.h"
//=====================================================>>
// utility
//=====================================================>>

GVM_REGISTER_CLASS(gv_importer_exporter)
GVM_REGISTER_CLASS(gv_impexp_obj)
GVM_REGISTER_CLASS(gv_impexp_3ds)
#if GV_WITH_FBX
GVM_REGISTER_CLASS(gv_impexp_fbx)
GVM_REGISTER_CLASS(gv_impexp_fbx_config)
#endif
GVM_REGISTER_CLASS(gv_impexp_rfx)
GVM_REGISTER_CLASS(gv_impexp_sn)
#if !GVM_NOT_SUPPORT_XIMAGE
GVM_REGISTER_CLASS(gv_impexp_cximage)
#endif
//=====================================================>>

#undef GVM_REGISTER_CLASS