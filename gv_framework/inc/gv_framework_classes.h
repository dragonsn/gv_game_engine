#include "gv_class_macro_pp.h"
//=====================================================>>
// structure goes first
//=====================================================>>
GVM_REGISTER_STRUCT(gv_vector2i)
GVM_REGISTER_STRUCT(gv_sphere)
GVM_REGISTER_STRUCT(gv_box)
GVM_REGISTER_STRUCT(gv_boxi)
GVM_REGISTER_STRUCT(gv_euleri)
GVM_REGISTER_STRUCT(gv_rect)
GVM_REGISTER_STRUCT(gv_recti)
GVM_REGISTER_STRUCT(gv_plane)
GVM_REGISTER_STRUCT(gv_vector4b)
GVM_REGISTER_STRUCT(gv_int_string_pair)

GVM_REGISTER_STRUCT(gv_framework_config)
GVM_REGISTER_STRUCT(gv_render_option)
GVM_REGISTER_STRUCT(gv_frustum)
GVM_REGISTER_STRUCT(gv_mesh_segment)
GVM_REGISTER_STRUCT(gv_bone)
GVM_REGISTER_STRUCT(gv_material_tex)
GVM_REGISTER_STRUCT(gv_shader_const)

GVM_REGISTER_STRUCT(gv_ani_pos_key)
GVM_REGISTER_STRUCT(gv_ani_rot_key)
GVM_REGISTER_STRUCT(gv_ani_scale_key)
GVM_REGISTER_STRUCT(gv_ani_pos_track)
GVM_REGISTER_STRUCT(gv_ani_rot_track)
GVM_REGISTER_STRUCT(gv_ani_scale_track)

GVM_REGISTER_STRUCT(gv_module_type_info)
GVM_REGISTER_STRUCT(gv_object_location_info)
GVM_REGISTER_STRUCT(gv_material_param_float4)
GVM_REGISTER_STRUCT(gv_map_3d_init_text)

#if GV_WITH_OS_API
GVM_REGISTER_STRUCT(gv_ip_address_ipv4)
GVM_REGISTER_STRUCT(gv_socket_address)
GVM_REGISTER_STRUCT(gv_packet)

GVM_REGISTER_STRUCT(gv_roam_patch)
GVM_REGISTER_STRUCT(gv_roam_node)
GVM_REGISTER_STRUCT(gv_terrain_grass_layer_info)
GVM_REGISTER_STRUCT(gv_terrain_mesh_layer_info)
GVM_REGISTER_STRUCT(gv_terrain_fur_layer_info)

#if GV_WITH_OS_API
#define GV_DOM_FILE "../component/gv_com_terrain_structs.h"
#define GVM_DOM_REGISTER
#include "gv_data_model_ex.h"
#endif
#if WIN32 && GV_WITH_DATABASE
GVM_REGISTER_STRUCT(gv_unit_test_table)
#endif
#endif 

GVM_REGISTER_STRUCT(gv_world_light_info)
GVM_REGISTER_STRUCT(gv_cell_2d)

#define GV_DOM_FILE "../renderer/gv_effect_struct.h"
#define GVM_DOM_REGISTER
#include "gv_data_model_ex.h"

#define GV_DOM_FILE "../animation/gv_ani_struct.h"
#define GVM_DOM_REGISTER
#include "gv_data_model_ex.h"

#define GV_DOM_FILE "gv_func_param_structs.h"
#define GVM_DOM_REGISTER
#include "gv_data_model_ex.h"
//=====================================================>>
// objects!!!
//=====================================================>>
GVM_REGISTER_CLASS(gv_object)
GVM_REGISTER_CLASS(gv_var_info)
GVM_REGISTER_CLASS(gv_func_info)
GVM_REGISTER_CLASS(gv_class_info)
GVM_REGISTER_CLASS(gv_module)
GVM_REGISTER_CLASS(gv_event_processor)
GVM_REGISTER_CLASS(gv_object_builder)

//=====================================================>>
// entity & components
//=====================================================>>
GVM_REGISTER_CLASS(gv_entity)
GVM_REGISTER_CLASS(gv_component)
GVM_REGISTER_CLASS(gv_com_graphic)
GVM_REGISTER_CLASS(gv_com_static_mesh)
GVM_REGISTER_CLASS(gv_com_skeletal_mesh)
GVM_REGISTER_CLASS(gv_com_camera)
GVM_REGISTER_CLASS(gv_com_animation)
GVM_REGISTER_CLASS(gv_com_tasklet)
GVM_REGISTER_CLASS(gv_com_attach)

// com for test, don't use in the final release
GVM_REGISTER_CLASS(gv_com_x_mesh)
GVM_REGISTER_CLASS(gv_com_cam_fps_fly)
GVM_REGISTER_CLASS(gv_com_3d_lobby_controller)
GVM_REGISTER_CLASS(gv_com_ai_player_controller_rpg)
GVM_REGISTER_CLASS(gv_com_observer_camera_controller)

//=====================================================>>
// render
//=====================================================>>
GVM_REGISTER_CLASS(gv_renderer)
GVM_REGISTER_CLASS(gv_component_renderer)
GVM_REGISTER_CLASS(gv_shader)
GVM_REGISTER_CLASS(gv_effect)
GVM_REGISTER_CLASS(gv_effect_project)

//=====================================================>>
// UI
//=====================================================>>
GVM_REGISTER_CLASS(gvi_ui_manager)
GVM_REGISTER_CLASS(gv_ui_module)
//=====================================================>>
// resource
//=====================================================>>
GVM_REGISTER_CLASS(gv_resource)
GVM_REGISTER_CLASS(gv_resource_cache)
GVM_REGISTER_CLASS(gv_index_buffer)
GVM_REGISTER_CLASS(gv_vertex_buffer)
GVM_REGISTER_CLASS(gv_static_mesh)
GVM_REGISTER_CLASS(gv_skeletal_mesh)
GVM_REGISTER_CLASS(gv_model_node)
GVM_REGISTER_CLASS(gv_model)

GVM_REGISTER_CLASS(gv_material)
GVM_REGISTER_CLASS(gv_texture)
GVM_REGISTER_CLASS(gv_image_2d)

//=====================================================>>
// animation
//=====================================================>>
GVM_REGISTER_CLASS(gv_skeletal)
GVM_REGISTER_CLASS(gv_ani_sequence)
GVM_REGISTER_CLASS(gv_ani_track)
GVM_REGISTER_CLASS(gv_ani_track_raw)
GVM_REGISTER_CLASS(gv_ani_set)
GVM_REGISTER_CLASS(gv_ani_track_with_key)

//=====================================================>>
// world
//=====================================================>>
GVM_REGISTER_CLASS(gv_world)
GVM_REGISTER_CLASS(gv_world_2d)
GVM_REGISTER_CLASS(gv_actor_2d)

GVM_REGISTER_CLASS(gv_world_3d)
GVM_REGISTER_CLASS(gv_actor_3d)
GVM_REGISTER_CLASS(gv_physics_fixed_setting)
GVM_REGISTER_CLASS(gv_collider)

#if GV_WITH_OS_API
//=====================================================>>
// terrain
//=====================================================>>
GVM_REGISTER_CLASS(gv_com_terrain_roam)
GVM_REGISTER_CLASS(gv_roam_chunk)
//=====================================================>>
// network
//=====================================================>>
GVM_REGISTER_CLASS(gv_com_net_host)
GVM_REGISTER_CLASS(gv_com_net_replica)
GVM_REGISTER_CLASS(gv_network_mgr)
#if WIN32 && GV_WITH_DATABASE
GVM_REGISTER_CLASS(gv_database_manager)
#endif

#define GVM_NET_REPLICA_REG_CLS
#define GV_NET_REPLICA_INC_FILE "../network/gv_net_replica_basic.h"
#include "../network/gv_net_replica_maker.h"
#endif

//=====================================================>>
// engine
//=====================================================>>
#define GV_DOM_FILE "../engine/gv_engine_struct.h"
#define GVM_DOM_REGISTER
#include "gv_data_model_ex.h"

GVM_REGISTER_CLASS(gv_com_game)
GVM_REGISTER_CLASS(gv_game_engine)
GVM_REGISTER_CLASS(gv_game_event_processor)
GVM_REGISTER_CLASS(gv_task_manager)

#undef GVM_REGISTER_CLASS