#include "gv_framework_private.h"
#include "gv_framework.h"

#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_framework_classes.h"
namespace gv
{
#define GVM_MAKE_ID GVM_MAKE_ID_IMP
#include "../../gv_base/inc/gv_id_pp.h"
#include "gv_framework_ids.h"
#undef GVM_MAKE_ID
#include "gv_driver.hpp"

namespace gv_global
{
gvt_global< gvi_debug_renderer > debug_draw;
gvt_global< gv_input_manager > input;
gv_framework_config framework_config;
bool framework_inited = false;
}

gv_framework_config::gv_framework_config()
{
	window_handle = 0;
	window_size = gv_vector2i(1280, 768);
	module_path_xml = "./";
	module_path_gvb = "./";
	data_path_root = "../../assets/";
	nb_debug_line = 1000;
	nb_debug_tri = 1000;
	max_sandbox_nb = 2;
	max_object_per_sandbox = 655360;
	texture_cache_path = "textures/";
};

void gv_framework_config::clear()
{
	module_path_xml.clear();
	module_path_gvb.clear();
	physics_asset_root_path.clear();
	driver_list.clear();
}
gv_string_tmp gv_framework_config::get_binary_module_path()
{
	gv_string_tmp s; 
	s = gv_global::framework_config.data_path_root;
	s += gv_global::framework_config.module_path_gvb;
	return s;
};
gv_string_tmp gv_framework_config::get_xml_module_path()
{
	gv_string_tmp s;
	s = gv_global::framework_config.data_path_root;
	s += gv_global::framework_config.module_path_xml;
	return s;
};
gv_string_tmp gv_framework_config::get_texture_cache_path()
{
	gv_string_tmp s;
	s = gv_global::framework_config.data_path_root;
	s += gv_global::framework_config.texture_cache_path;
	return s;
};

const gv_id& gv_framework_channel_enum_to_name(gv_int channel)
{
#undef GVM_DCL_CHANNEL
#define GVM_DCL_CHANNEL(x) \
	case gve_event_##x:    \
		return gv_id_##x;
	switch (channel)
	{
#include "gv_event_channels.h"
	};
	return gv_id_none;
};

gv_int gv_framework_channel_name_to_enum(const gv_id& id)
{
#undef GVM_DCL_CHANNEL
#define GVM_DCL_CHANNEL(x) \
	if (id == gv_id_##x)   \
		return gve_event_##x;
#include "gv_event_channels.h"
	return gvc_max_object_event_channel;
}

void gv_framework_init()
{
	gv_framework_config* pc = &gv_global::framework_config;
#define GVM_MAKE_ID GVM_MAKE_ID_INIT
#include "../../gv_base/inc/gv_id_pp.h"
#include "gv_framework_ids.h"
#undef GVM_MAKE_ID
	gv_global::input.get();
	gv_global::framework_inited = true;
	gv_global::stats.register_category(gv_id_framework, gv_color::BLUE_B(), true);
}

void gv_framework_destroy()
{
	gv_global::input.destroy();
#define GVM_MAKE_ID GVM_MAKE_ID_RELEASE
#include "../../gv_base/inc/gv_id_pp.h"
#include "gv_framework_ids.h"
#undef GVM_MAKE_ID
	gv_global::sandbox_mama.destroy();
	gv_global::input.destroy();
	gv_global::framework_config.clear();
	gv_global::framework_inited = false;
}

void gv_register_framework_classes()
{
	GV_PROFILE_EVENT(gv_register_framework_classes, 0)
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
// gv_native_module_guard g(sandbox, "gv_framework"); this will change the path
// of native classess, make old package failed to load..., need some other
// solution.
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_framework_classes.h"
	GVM_ENUM_DO_REGISTER(gve_effect_semantic);
	sandbox->init_classes();
};

void gv_unregister_framework_classes()
{
	GV_PROFILE_EVENT(gv_unregister_framework_classes, 0)
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_framework_classes.h"
};
}

#pragma GV_REMINDER("[PITFALL]don't use gvt_array<gv_string_tmp>  !!!")
#pragma GV_REMINDER( \
	"[PITFALL]no more gv_global!!, get all the interface from sandbox, or it can't be called sandbox!!!")
#pragma GV_REMINDER( \
	"[PITFALL]use explict in the constructor properly , don't use a general type  in object constructor !!!")
#pragma GV_REMINDER( \
	"[PITFALL]the weak pointer will delete object when ref count is 1, take care it in editor , the static mesh in property view ref count should be 4!!!")
#pragma GV_REMINDER( \
	"[PITFALL][animation]animation will be set to wrong skeletal if no sync to entity or sync_to_entity more than once!!!!!")
#pragma GV_REMINDER( \
	"[PITFALL][UI]can't use same name in different menu in CEGUI!!!")
#pragma GV_REMINDER( \
	"[PITFALL]don't call delete object directly, delete a object twice will be problem!!!!!")
#pragma GV_REMINDER( \
	"[NOTES][OPTIMIZATION][CPU]top->down profiler(my profiler) & down->top profiler(Code analysis , GPA) || cache miss || reduce lock || reduce in game memory allocation || thread & multicore || system call ||large scale computation(mmx)|| interlace|| async call|| compiler opt (inline & global optimizaton ) || memory leak & small memory allocation || lock free container|| visible trick & other active state trick.")
#pragma GV_REMINDER( \
	"[NOTES][OPTIMIZATION][GPU] PIX && GPA && Perfhud || shader || fillrate || texture size ,format & rendertarget format|| dynamic buffer managerment || hud & 2d performance ||reduce draw call || batch || z prepass || z cull || s (stencil) cull || alpha kill || occlusion queue || parallel with CPU ||AA ||shadow map technique && percision.|| shader compile options || reduce input & output attribute for shader. || dynamic branch in shader || reduce the shader switch|| LOD & imposter  & mipmap ||sort the render objects.. || filter (linear or other expansive)")
#pragma GV_REMINDER( \
	"[NOTES][CODING STANDARD] take google coding standard reference || boost standard (lower case + underline) || member var start with m_ || class and function start with gv || template start with gv_ ||")
#pragma GV_REMINDER( \
	"[MEMO]the global optimization is great and the link time optimization , it reduce the time for roam generation greatly .(7ms->1ms!!!), take care the assembly code as well...")
#pragma GV_REMINDER( \
	"[MEMO]the send_xxx functions not thread safe!!if you need thread safe ,use post_xxx instead!")
#pragma GV_REMINDER("[MEMO]the autoexp.dat goes here")
//"gflags -p /enable gv_framework_test.exe /full /unaligned"
/*

;------------------------------------------------------------------------------
;  gv
;------------------------------------------------------------------------------

gv::gvt_array<*>{
        children
        (
                #array
                (
                        expr :		($e.data_ptr)[$i],
                        size :		$e.array_size
                )
        )
        preview
        (
                #(
                        "[", $e.array_size , "](",
                        #array
                        (
                                expr :	($e.data_ptr)[$i],
                                size :	$e.array_size
                        ),
                        ")"
                )
        )
}

gv::gvt_string<char,*>{
        preview			([$e.data_ptr,s])
        stringview		([$e.data_ptr,sb])
}

gv::gv_id{
        preview			([$e.ppair->key.data_ptr,s])
        stringview		([$e.ppair->key.data_ptr,sb])
}

gv::gv_object_name{
        preview ( #( $e.m_id, " ", $e.m_postfix) )
}

gv::gv_object{
        preview ( #("name: ", $e.m_name, "class: ", $e.m_class->m_name,  owner",
$e.m_owner->Name ) )
}

;gv::gv_object{
;	preview			([$e.m_name])
;}

gv::gvt_vector4<*>{
        preview			([$e.x, $e.y, $e.z, $e.w])
}

gv::gvt_string<char,*>{
        preview			([$e.data_ptr,s])
        stringview		([$e.data_ptr,sb])
}


*/