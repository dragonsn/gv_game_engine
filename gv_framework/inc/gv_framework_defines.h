#pragma once
namespace gv
{
enum gve_render_pass
{
	gve_render_pass_shadow_map = 0,
	gve_render_pass_pre_z_pass = 1,
	gve_render_pass_opaque = 2,
	gve_render_pass_tranparent = 3,
	gve_render_pass_outline = 4,
	gve_render_pass_max = 5,
};

enum gve_event_channel
{
	gve_event_channel_default = 0,
	gve_event_channel_ui = 1,
	gve_event_channel_render = 2,
	gve_event_channel_sound = 3,
	gve_event_channel_net = 4,
	gve_event_channel_database = 5,
	gve_event_channel_physics = 6,
	gve_event_channel_animation = 7,
	gve_event_channel_loading = 8,
	gve_event_channel_world = 9,
	gve_event_channel_editor = 10,
	gve_event_channel_task_queue = 11,
	gve_event_channel_platform = 12,
	gve_event_channel_game_logic = 13,
	gve_event_channel_game_timer = 14,
	gve_event_channel_lockstep = 15,
};

const gv_id& gv_framework_channel_enum_to_name(gv_int channel);
gv_int gv_framework_channel_name_to_enum(const gv_id& id);

#define GVM_MAKE_ID GVM_MAKE_ID_DCL
#include "../../gv_base/inc/gv_id_pp.h"
#include "gv_framework_ids.h"
#undef GVM_MAKE_ID

#define GV_RESOURCE_ROOT "resource_root:"

#define GV_DOM_ENUM_FUNC_HELPER_TO_STRING(name) \
	const char* GV_MACRO_CONCATE2(gv_get_string_from_, name)(name e)
#define GV_DOM_ENUM_FUNC_HELPER_FR_STRING(name) \
	const name GV_MACRO_CONCATE2(gv_get_enum_, name)(const char* s)
#define GV_DOM_ENUM_FUNC_HELPER_REGISTER(name) \
	void GV_MACRO_CONCATE2(gv_register_enum_, name)(gv_sandbox * sandbox)
#define GVM_ENUM_DO_REGISTER(name)             \
	GV_MACRO_CONCATE2(gv_register_enum_, name) \
	(sandbox);


}