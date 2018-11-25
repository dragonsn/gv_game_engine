#include "gv_framework_private.h"
#include "gv_framework.h"
#include "gv_com_game.h"
#include "gv_framework_events.h"
namespace gv
{
static bool m_no_render = false;
static bool m_no_sound = false;
static bool m_no_profiler = false;
static bool m_no_player = false;
static bool m_track_memory = false;
static bool m_with_console = false;
static bool m_no_log = false;
static bool m_no_debug_draw = false;
static bool m_framework_inited_outside = false;
static gvt_ptr< gv_game_engine > s_static_game_engine;
//============================================================================================
//								:async loading
//info
//============================================================================================

//============================================================================================
//								:
//============================================================================================
class gv_game_engine_data : public gv_refable
{
public:
	friend class gv_game_engine;
	gv_game_engine_data(){};
	~gv_game_engine_data()
	{
	}

protected:
	gvt_ref_ptr< gv_entity > m_post_effect_template;
	gv_atomic_count m_nb_module_to_load;
	gv_command_console m_console;
	gvt_hash_map< gv_module_name, gv_async_load_info, 256 > m_map_load_list;
};

gv_game_engine* gv_game_engine::static_get()
{
	return s_static_game_engine;
};
gv_game_engine::gv_game_engine()
{
	// a game engine create it's own sandbox.
	// so don't use sandbox to create the game engine.
	GVM_SET_CLASS(gv_game_engine);
	gv_sandbox* sandbox = gv_global::sandbox_mama->create_sandbox();
	this->m_name = gv_id_engine;
	gv_object::inc_ref();
	sandbox->add_object(this);
	sandbox->set_game_engine(this);
	m_engine_data = new gv_game_engine_data;
	s_static_game_engine = this;
};

gv_game_engine::~gv_game_engine()
{
#pragma GV_REMINDER( \
	"[MEMO] here is a hack, the game engine is still in the sandbox ,but engine is already partly destoryed here, so only set it as base class to be destroyed by sandbox")
	GVM_SET_CLASS(gv_game_engine);
	this->inc_ref();
	while (get_game_engine_data()->m_nb_module_to_load.get())
	{
		GVM_DEBUG_CONSOLE_OUT("wait for async load to finish" << gv_endl);
		gv_thread::sleep(300);
	}
	m_game_entity = NULL;
	m_engine_data = NULL;
	gv_global::sandbox_mama->delete_sandbox(m_sandbox);
	this->m_sandbox = NULL;
	gv_id::static_purge();
	gv_object::m_ref_count = 0;
};

void gv_game_engine::static_init(int argc, char* argv[], char* extra_arg)
{
	gv_base_config& boost_config = gv_global::config;
	gv_framework_config& framework_config = gv_global::framework_config;
	gvt_array< gv_string > args;
	gv_fill_command_line_arg(args, argc, argv);
	if (gv_global::framework_inited)
	{
		m_framework_inited_outside = true;
	}
	else
	{
		m_framework_inited_outside = false;
		m_no_render = gv_find_in_command_line("no_render", argc, argv, extra_arg);
		m_no_sound = gv_find_in_command_line("no_sound", argc, argv, extra_arg);
		m_no_profiler =
			gv_find_in_command_line("no_profile", argc, argv, extra_arg);
		m_no_player = gv_find_in_command_line("no_player", argc, argv, extra_arg);
		m_track_memory =
			gv_find_in_command_line("mem_track", argc, argv, extra_arg);
		m_with_console = gv_find_in_command_line("console", argc, argv, extra_arg);
		m_no_log = gv_find_in_command_line("no_log", argc, argv, extra_arg);
		m_no_debug_draw =
			gv_find_in_command_line("no_debug_draw", argc, argv, extra_arg);
		boost_config.enable_mem_tracking = m_track_memory;
		boost_config.enable_profiler = !m_no_profiler;
		boost_config.no_log_all = m_no_log;
		gv_base_init();
		gv_framework_init();
	}

	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init((gv_ushort)framework_config.max_sandbox_nb,
								  framework_config.max_object_per_sandbox);
	if (!m_no_render)
	{
		gv_global::rnd_opt.m_max_debug_line = framework_config.nb_debug_line;
		gv_global::rnd_opt.m_max_debug_tri = framework_config.nb_debug_tri;
		gv_global::rnd_opt.m_screen_width = framework_config.window_size.get_x();
		gv_global::rnd_opt.m_screen_height = framework_config.window_size.get_y();
		gv_global::rnd_opt.m_no_debug_draw = m_no_debug_draw;
	}
};

void gv_game_engine::static_destroy()
{

	gv_global::rnd_opt.m_engine_effect = NULL;
	gv_global::rnd_opt.m_depth_only_effect = NULL;
	gv_global::rnd_opt.m_shadow_pass_effect = NULL;
	gv_global::rnd_opt.m_shadow_post_effect = NULL;
	gv_id::static_purge();
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
	if (!m_framework_inited_outside)
	{
		gv_framework_destroy();
		gv_base_destroy();
	}
};

void gv_game_engine::init()
{
	gv_engine_option option;
	this->load_default_option(option);
	gv_string_tmp text;
	bool find_option = gv_load_file_to_string(this->get_option_file_name(), text);
	if (find_option)
	{
		gv_xml_parser parser;
		parser.load_string(*text);
		gv_class_info* pcls = get_sandbox()->find_class(gv_id("gv_engine_option"));
		GV_ASSERT(pcls);
		pcls->import_from_xml(gvt_byte_ptr(option), &parser);
	}
	else
	{
		// create new ini file
		gv_class_info* pcls = get_sandbox()->find_class(gv_id("gv_engine_option"));
		GV_ASSERT(pcls);
		pcls->export_to_xml(gvt_byte_ptr(option), text);
		gv_save_string_to_file(get_option_file_name(), text);
	}
	get_sandbox()->init_classes(); // for safe.
	this->init(option);
	m_fps = 0;
	gv_global::stats.register_stats("fps", gv_id_framework, m_fps);
}

void gv_game_engine::init_one_processor(const gv_event_processor_desc& desc)
{
	gv_int channel = channel_name_to_enum(desc.m_channel_id);
	GV_ASSERT(channel != gvc_max_object_event_channel &&
			  "delete the game_engine.ini to have a try!");
	if (m_no_render && channel == gve_event_channel_render)
		return;
	if (m_no_sound && channel == gve_event_channel_sound)
		return;
	m_sandbox->register_processor(desc.m_processor_cls, channel);
	if (m_sandbox->get_event_processor(
			channel)) // possible NULL when the class module not loaded!!
	{
		m_sandbox->get_event_processor(channel)
			->set_synchronization(desc.m_is_synchronization);
		m_sandbox->get_event_processor(channel)
			->set_autonomous(desc.m_is_autonomous);
	}
};

void gv_game_engine::init(const gv_engine_option& option)
{
	{
		gv_global::rnd_opt.m_screen_height =
			gv_global::framework_config.window_size.get_y();
		gv_global::rnd_opt.m_screen_width =
			gv_global::framework_config.window_size.get_x();

		gv_global::rnd_opt.m_msaa_times = option.m_msaa_times;
		gv_global::rnd_opt.m_no_shadow = option.m_no_shadow;
		gv_global::rnd_opt.m_no_bloom = option.m_no_bloom;
		gv_global::rnd_opt.m_no_color_adjust = option.m_no_bloom;
		gv_global::rnd_opt.m_no_god_ray = option.m_no_god_ray;
		gv_global::rnd_opt.m_no_motion_blur = option.m_no_motion_blur;
		gv_global::rnd_opt.m_bloom_scale = option.m_bloom_scale;
		gv_global::rnd_opt.m_color_scale = option.m_color_scale;
		gv_global::rnd_opt.m_color_adjustment = option.m_color_adjustment;
		gv_global::rnd_opt.m_clear_color = option.m_clear_color;
	}
	get_sandbox()->set_resource_root_path(*option.m_root_path);
	// create processors from driver;
	gv_driver_info::static_create_event_processors(get_sandbox());
	{ // create the async loading thread.
		get_sandbox()->register_processor(gv_event_processor::static_class(),
										  gve_event_channel_loading);
		get_sandbox()
			->get_event_processor(gve_event_channel_loading)
			->set_fixed_delta_time(true, 0.02f);
		get_sandbox()
			->get_event_processor(gve_event_channel_loading)
			->set_synchronization(false);
		get_sandbox()
			->get_event_processor(gve_event_channel_loading)
			->set_autonomous(true);
	}
	{ // create game logic event processor
		get_sandbox()->register_processor(gv_game_event_processor::static_class(),
										  gve_event_channel_game_timer);
		get_sandbox()
			->get_event_processor(gve_event_channel_game_timer)
			->set_synchronization(true);
		get_sandbox()
			->get_event_processor(gve_event_channel_game_timer)
			->set_autonomous(true);
		get_sandbox()
			->get_event_processor(gve_event_channel_game_timer)
			->set_fixed_delta_time(true, 0.033f);
	}
	for (int i = 0; i < option.m_processors.size(); i++)
	{
		const gv_event_processor_desc& desc = option.m_processors[i];
		init_one_processor((desc));
	}

	//===============================================================================================
	GVM_POST_EVENT(
		render_init, render,
		(pe->window_handle = gv_global::framework_config.window_handle));
	GVM_POST_EVENT(render_enable_pass, render,
				   (pe->pass = gve_render_pass_opaque));
	this->m_game_entity = get_sandbox()->create_object< gv_entity >(this);
	this->m_game_entity->add_component(option.m_game_cls);
	this->set_owner(get_sandbox()->get_native_module());
#if WIN32
	if (!m_no_render)
	{
		gv_module* mod = get_sandbox()->try_load_module(gv_id("mat_engine"));
#pragma GV_REMINDER( \
	"[TOP PRIORITY!!] [hack][data dependency here!!	: TODO put the data link in the ini file..")
		if (mod)
		{
			gv_global::rnd_opt.m_engine_effect =
				get_sandbox()->find_object< gv_effect >(
					"(3)mat_engine:0/default:0/base:0");
			gv_global::rnd_opt.m_depth_only_effect =
				get_sandbox()->find_object< gv_effect >(
					"(4)mat_engine:0/depth_only:0/base:0/z_pass:0", false);
			this->get_game_engine_data()->m_post_effect_template =
				get_sandbox()->find_object< gv_entity >(
					"(2)mat_engine:0/actor_post_effect_template:0", false);
			gv_global::rnd_opt.m_shadow_pass_effect =
				get_sandbox()->find_object< gv_effect >(
					"(3)mat_engine:0/shadow_map:0/base:0", false);
			gv_global::rnd_opt
				.m_shadow_post_effect = get_sandbox()->find_object< gv_effect >(
				"(5)mat_engine:0/post_shadow:0/base:0/postprocess:0/defer_shading:0",
				false);
		}
	}

#endif
	// get_game_engine_data()->m_console.add(this);
	get_game_engine_data()->m_console.start();
};

void gv_game_engine::uninit()
{
	get_game_engine_data()->m_console.stop();
}
bool gv_game_engine::execute(const gv_string_tmp& str)
{
	// do
	bool ret = false;
	GVM_CONSOLE_OUT("[EXEC] " << str << gv_endl);
	if (get_current_active_world())
	{
		ret = get_current_active_world()->execute(*str);
	}
	if (ret)
		return ret;
	return false;
};
void gv_game_engine::run()
{
	this->init();
	while (1)
	{
		if (!tick())
			break;
	}
	this->uninit();
};

bool gv_game_engine::tick()
{
	{ // exec command
		gvt_array< gv_string > s;
		get_game_engine_data()->m_console.get_command_array(s);
		for (int i = 0; i < s.size(); i++)
		{
			execute(*s[i]);
		}
	}
	m_fps = (gv_int)gvt_floor(1.0 / (m_sandbox->get_delta_time() + 0.000001));
	return m_sandbox->tick();
};

void gv_game_engine::tick_stats(bool show_on_screen, const gv_recti& window)
{
	gvt_array< gv_string > lines;
	gv_string s = *gv_global::stats.to_string();
	s.split(lines, "\n");
	gv_vector2i pos = window.min_p + gv_vector2i(32, 32);
	// gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(60, 60),
	// gv_color::GREEN_B());
	for (int i = 0; i < lines.size(); i++)
	{
		gv_global::debug_draw.get()->draw_string(*lines[i], pos,
												 gv_color::GREEN_B());
		pos.y += 16;
	}
	gv_global::stats.reset_frame();
};

bool gv_game_engine::do_synchronization()
{
	if (this->m_game_entity)
	{
		this->m_game_entity->synchronize_from_components();
	}
	tick_stats();
	return true;
};

bool gv_game_engine::tick(gv_float dt)
{
	if (this->m_game_entity && !this->m_game_entity->is_hided())
	{
		return this->m_game_entity->tick(dt);
	}
	return true;
};

gvi_ui_manager* gv_game_engine::get_ui_mgr()
{
	return gvt_cast< gvi_ui_manager >(
		get_sandbox()->get_event_processor(gve_event_channel_ui));
}

// the event channel can be extended, so use virtual function.
const gv_id& gv_game_engine::channel_enum_to_name(gv_int channel)
{
	return gv_framework_channel_enum_to_name(channel);
};

gv_int gv_game_engine::channel_name_to_enum(const gv_id& id)
{
	return gv_framework_channel_name_to_enum(id);
}

bool gv_game_engine::load_default_option(gv_engine_option& option)
{
	option.m_game_cls = gv_id("gv_com_game");
	option.m_root_path = "../../data";
	option.m_color_scale.set(1, 1, 1, 1);
	option.m_bloom_scale = 1.0f;
	option.m_clear_color.set(1, 1, 1, 255);
	return true;
};
const char* gv_game_engine::get_option_file_name()
{
	return "game_engine.ini";
};

gv_entity* gv_game_engine::get_game_entity()
{
	return m_game_entity;
};

gvi_debug_renderer* gv_game_engine::get_debug_draw()
{
	return gv_global::debug_draw.get();
}

//============================================================================================
//								:
//============================================================================================
bool gv_game_engine::spawn_post_effect(gv_effect* post_effect, gv_float time,
									   gv_world* world, gv_uint priority)
{
	gv_entity* e = get_game_engine_data()->m_post_effect_template;
	if (!e)
		return false;
	if (!world)
		world = get_current_active_world();
	if (!world)
		return false;
	e = e->clone(world->get_sandbox());
	gv_com_static_mesh* com_mesh = e->get_component< gv_com_static_mesh >();
	e->set_total_life_time(time);
	if (com_mesh)
	{
		com_mesh->get_material()->set_effect(post_effect);
	}
	com_mesh->set_render_priority(priority);
	world->add_entity(e);
	return true;
};

//============================================================================================
//								:
//============================================================================================
gv_bool gv_game_engine::start_load_world(
	const gv_module_name& world_name)
{ //  async loading
	gv_async_load_info& info =
		get_game_engine_data()->m_map_load_list[world_name];
	if (info.get_state() != e_async_not_loaded)
		return false;
	info.m_create_new_sandbox = true;
	info.m_module_name = world_name;
	return async_load_module(&info);
};
bool gv_game_engine::is_world_ready(const gv_module_name& world_name)
{
	gv_async_load_info& info =
		get_game_engine_data()->m_map_load_list[world_name];
	return info.get_state() == e_async_loaded;
};
bool gv_game_engine::switch_world(gv_world* new_world, bool delete_old_world)
{
	if (!new_world)
		return false;
	gv_world* pworld = get_current_active_world();
	if (new_world == pworld)
		return false;
	get_sandbox()->set_processor(new_world, gve_event_channel_world);
	if (pworld)
	{
		if (delete_old_world)
		{
			unload_world(*pworld->get_module()->get_name_id());
		}
	}
	return true;
};
gv_world* gv_game_engine::get_loaded_world(const gv_module_name& world_name)
{
	gv_async_load_info& info =
		get_game_engine_data()->m_map_load_list[world_name];
	if (info.get_state() == e_async_loaded && info.m_module_loaded)
		return info.m_module_loaded->get_world();
	return NULL;
}
bool gv_game_engine::unload_world(gv_module_name world_name)
{
	GV_PROFILE_EVENT(gv_game_engine___unload_world, 0);
	gv_async_load_info& info =
		get_game_engine_data()->m_map_load_list[world_name];
	if (info.get_state() == e_async_not_loaded)
		return false;
	if (info.get_state() == e_async_loading)
	{ // not finished, try again...
		GVM_POST_JOB(loading, (&gv_game_engine::unload_world, this, world_name));
		return false;
	}
	if (!info.m_module_loaded)
		return false;
	info.set_state(e_async_not_loaded);
	if (info.m_create_new_sandbox)
	{
		gv_global::sandbox_mama->delete_sandbox(info.m_sandbox);
	}
	else
	{
		info.m_sandbox->delete_object_tree(info.m_module_loaded);
	}
	return true;
};

bool gv_game_engine::set_current_active_world(const gv_module_name& world_name,
											  bool delete_old_world)
{
	GV_PROFILE_EVENT(gv_game_engine___set_current_active_world, 0);
	gv_async_load_info& info =
		get_game_engine_data()->m_map_load_list[world_name];
	if (info.get_state() != e_async_loaded)
		return false;
	this->switch_world(info.m_module_loaded->get_world(), delete_old_world);
	return true;
};

gv_world* gv_game_engine::get_current_active_world()
{
	return gvt_cast< gv_world >(
		get_sandbox()->get_event_processor(gve_event_channel_world));
};
//============================================================================================
//								:
//============================================================================================
gv_bool gv_game_engine::async_load_module(gv_async_load_info* info)
{
	GV_PROFILE_EVENT(gv_game_engine___async_load_module, 0);
	GV_ASSERT(info->get_state() == e_async_not_loaded);
	GV_ASSERT(get_sandbox()->get_event_processor(
		gve_event_channel_loading)); // make sure there is event processor in
									 // loading
	info->set_state(e_async_loading);
	get_game_engine_data()->m_nb_module_to_load++;
	GVM_POST_JOB(loading, (&gv_game_engine::do_async_load, this, info));
	return true;
};
bool gv_game_engine::do_async_load(gv_async_load_info* info)
{
	GV_PROFILE_EVENT(gv_game_engine___do_async_load, 0);
	GV_ASSERT(info->get_state() == e_async_loading);
	gv_sandbox* my_sandbox = get_sandbox();
	if (info->m_create_new_sandbox)
	{
		gv_sandbox* out_sandbox =
			info->m_out_sandbox ? info->m_out_sandbox.ptr() : get_sandbox();
		my_sandbox =
			gv_global::sandbox_mama->create_sandbox(out_sandbox); // more memory ..
		get_sandbox()->share_event_processors(
			my_sandbox); // catch the event during loading..maybe we can delay the
						 // post load call ....but what will happen if we load
						 // several dependent module together?
	}
	gv_module* pmod = my_sandbox->try_load_module(gv_id(*info->m_module_name));
	get_game_engine_data()->m_nb_module_to_load--;
	gv_int result = pmod ? e_async_loaded : e_async_load_failed;
	info->m_module_loaded = pmod;
	info->set_state(result);
	info->m_sandbox = my_sandbox;
	return true;
};
void gv_game_engine::debug_draw_skeletal(gv_entity* entity)
{
	if (!entity)
		return;
	gv_com_skeletal_mesh* com_mesh =
		entity->get_component< gv_com_skeletal_mesh >();
	if (com_mesh)
	{
		gv_skeletal_mesh* my_skeletal = com_mesh->get_skeletal_mesh();
		const gv_matrix44& world = entity->get_tm();
		if (my_skeletal)
		{
			for (int i = 0; i < com_mesh->get_writable_skeletal()->m_bones.size();
				 i++)
			{
				gv_bone& bone = com_mesh->get_writable_skeletal()->m_bones[i];
				if (bone.m_hierachy_depth)
				{
					gv_bone& father =
						com_mesh->get_writable_skeletal()->m_bones[bone.m_parent_idx];
					this->get_debug_draw()->draw_line_3d(
						(bone.m_tm * world).get_trans(),
						(father.m_tm * world).get_trans(), gv_color::WHITE(),
						gv_color::BLACK());
				}
				// gv_vector2i pos=g->world_to_window((bone.m_tm*world).get_trans());
				// get_debug_draw()->draw_string(*bone.m_name,(float)pos.get_x(),(float)pos.get_y(),
				// gv_color::YELLOW());
			}
		}
	}
}
gv_material* gv_game_engine::get_default_material()
{
	return m_default_material.ptr();
};
void gv_game_engine::set_default_material(gv_material* p)
{
	m_default_material = p;
}
void gv_game_engine::add_commandlet(gvi_commandlet* p)
{
	m_engine_data->m_console.add(p);
};
//============================================================================================
//								:
//============================================================================================

GVM_IMP_CLASS(gv_game_engine, gv_object)
GVM_VAR(gvt_ref_ptr< gv_entity >, m_game_entity)
GVM_END_CLASS

#define GV_DOM_FILE "../engine/gv_engine_struct.h"
#define GVM_DOM_RTTI
#define GVM_DOM_IMP_COPY
#include "gv_data_model_ex.h"
};
