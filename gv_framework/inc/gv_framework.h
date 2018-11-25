#pragma once
// object framework
#include "../../gv_base/inc/gv_base.h"
#include "gv_framework_defines.h"
#include "gv_object_struct.h"
#include "gv_driver.h"
#include "gv_type.h"
#include "gv_object_event.h"
#include "gv_sandbox.h"
#include "gv_object_job.h"
#include "gv_sandbox_manager.h"
#include "gv_object_macro.h"
#include "gv_object.h"
#include "gv_var_info.h"
#include "gv_func_info.h"
#include "gv_class_info.h"
#include "gv_object_builder.h"
#include "gv_module.h"
#include "gv_event_processor.h"
#include "gv_framework_config.h"

#include "gv_importer_exporter.h"
#include "gv_input_manager.h"

#include "../task/gv_task_manager.h"

#include "gv_resource.h"
#include "../renderer/gv_image.h"
#include "../renderer/gv_texture.h"
#include "../renderer/gv_material.h"
#include "../renderer/gv_index_buffer.h"
#include "../renderer/gv_vertex_buffer.h"
#include "../renderer/gv_static_mesh.h"
#include "../renderer/gv_skeletal_mesh.h"
#include "../renderer/gv_model.h"
#include "../renderer/gv_effect.h"

#include "../renderer/gv_render_option.h"
#include "../renderer/gv_renderer.h"
#include "../renderer/gv_component_renderer.h"
#include "../renderer/gv_debug_renderer.h"
#include "../renderer/gv_render_unit_test.h"
#include "gv_component.h"
#include "gv_entity.h"

#include "../component/gv_com_graphic.h"
#include "../component/gv_com_static_mesh.h"
#include "../component/gv_com_skeletal_mesh.h"
#if GV_WITH_OS_API
#include "../component/gv_com_x_mesh.h"
#include "../component/gv_com_terrain_roam.h"
#endif

#include "../component/gv_com_camera.h"
#include "../component/gv_com_tasklet.h"

#include "../animation/gv_ani_skeletal.h"
#include "../animation/gv_animation.h"

#include "../user_interface/gvi_ui_manager.h"
#include "../user_interface/gv_ui_module.h"

#include "../ai/gv_ai.h"

#include "../world/gv_world.h"
#include "../world/gv_world_2d.h"

#include "../world/gv_world_grid.h"
#include "../world/gv_physics_3d_fixed.h"
#include "../world/gv_collider_3d.h"
#include "../world/gv_actor_3d.h"
#include "../world/gv_world_3d.h"

#include "../engine/gv_game_engine.h"
#include "../engine/gv_com_game.h"
#include "gv_framework_events.h"

//=============================================================================>
// text is a new type used to hold long string , use XML CDATA to store to solve
// "><" and other token conflict with XML
// but struct and all the function is the same as string.




namespace gv
{
	class gv_class_info;

// some helper function;
extern void gv_load_framework_user_config(const char* user_config_file_name);

inline gv_world* get_world_in_sandbox(gv_sandbox* sandbox)
{
	return gvt_cast< gv_world >(
		sandbox->get_event_processor(gve_event_channel_world));
}
inline gv_renderer* get_renderer_in_sandbox(gv_sandbox* sandbox)
{
	return gvt_cast< gv_renderer >(
		sandbox->get_event_processor(gve_event_channel_render));
}

namespace gv_global
{
extern gvt_global< gvi_debug_renderer > debug_draw;
extern gv_framework_config framework_config;
extern bool framework_inited;
};
extern void gv_framework_init();
extern void gv_framework_destroy();
}