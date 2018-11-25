#include "gv_class_macro_pp.h"

#if GV_WITH_BGFX 
GVM_REGISTER_CLASS(gv_renderer_bgfx)
GVM_REGISTER_CLASS(gv_index_buffer_bgfx)
GVM_REGISTER_CLASS(gv_texture_bgfx)
GVM_REGISTER_CLASS(gv_vertex_buffer_bgfx)
GVM_REGISTER_CLASS(gv_shader_bgfx)
GVM_REGISTER_CLASS(gv_effect_bgfx)

#endif
#undef GVM_REGISTER_CLASS