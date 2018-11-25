#pragma once
namespace gv
{
#define GV_DOM_FILE "../engine/gv_engine_struct.h"
#define GVM_DOM_DECL
#include "../inc/gv_data_model_ex.h"
class gv_entity;
class gv_com_game;
class gv_game_engine_data;
typedef void* gv_async_handle;
typedef gv_string128 gv_module_name;
//============================================================================================
//		async load ,may move to other place
//============================================================================================
enum e_module_loading_state
{
	e_async_not_loaded = 0,
	e_async_loading,
	e_async_loaded,
	e_async_load_failed
};

struct gv_async_load_info
{
	inline gv_async_load_info()
	{
		m_create_new_sandbox = false;
	};
	inline gv_async_load_info(const gv_async_load_info& info)
	{
		(*this) = info;
	}
	inline gv_async_load_info& operator=(const gv_async_load_info& info)
	{
		m_module_name = info.m_module_name;
		m_module_loaded = info.m_module_loaded;
		;
		m_create_new_sandbox = info.m_create_new_sandbox;
		m_sandbox = info.m_sandbox;
		m_out_sandbox = info.m_out_sandbox;
		m_loading_state = m_loading_state;
		return *this;
	}
	gv_int get_state()
	{
		return m_loading_state.get();
	}
	gv_bool completed()
	{
		return get_state() == e_async_load_failed || get_state() == e_async_loaded;
	}
	void set_state(gv_int state)
	{
		m_loading_state.set(state);
	}
	//============================================================================================
	//								:data
	//============================================================================================
	gv_module_name m_module_name;
	gvt_ref_ptr< gv_module > m_module_loaded;
	gv_bool m_create_new_sandbox;
	gvt_ptr< gv_sandbox > m_sandbox;
	gvt_ptr< gv_sandbox > m_out_sandbox;

protected:
	gv_atomic_count m_loading_state;
};
//============================================================================================
//		game engine class used by application
//============================================================================================
class gv_game_engine : public gv_object
{
public:
	GVM_DCL_CLASS(gv_game_engine, gv_object);
	gv_game_engine();
	~gv_game_engine();

public:
	static void static_init(int nb_arg = 0, char* args[] = NULL,
							char* extra_arg = NULL);
	static void static_destroy();
	static gv_game_engine* static_get();
	//============================================================================================
	//								:
	//============================================================================================
	virtual void run();
	virtual void init();
	virtual void uninit();
	virtual bool tick();
	virtual bool tick(gv_float dt);
	virtual bool do_synchronization();
	virtual void tick_stats(bool show_on_screen = true,
							const gv_recti& window = gv_recti(0, 0, 0, 0));
	//============================================================================================
	//								:get access to
	//manager
	//============================================================================================
	gvi_debug_renderer* get_debug_draw();
	gvi_ui_manager* get_ui_mgr();
	gv_entity* get_game_entity();

	//============================================================================================
	//								:async load
	//============================================================================================
	gv_bool async_load_module(gv_async_load_info*);
	//============================================================================================
	//								:load a new world , using async load by
	//default
	//============================================================================================
	virtual gv_world* get_loaded_world(const gv_module_name& world_name);
	virtual bool start_load_world(const gv_module_name& world_name);
	virtual bool is_world_ready(const gv_module_name& world_name);
	virtual bool set_current_active_world(const gv_module_name& world_name,
										  bool delete_old_world);
	virtual bool unload_world(gv_module_name world_name);
	virtual gv_world* get_current_active_world();
	virtual bool switch_world(gv_world* world, bool delete_old_world = true);
	virtual class gv_material* get_default_material();
	virtual void set_default_material(gv_material* p);
	//============================================================================================
	//								:
	//============================================================================================
	virtual bool spawn_post_effect(gv_effect* post_effect, gv_float time,
								   gv_world* world = NULL, gv_uint priority = 5);
	// the event channel can be extended, so use virtual function.
	virtual const gv_id& channel_enum_to_name(gv_int channel);
	virtual gv_int channel_name_to_enum(const gv_id& id);
	virtual const char* get_option_file_name();
	virtual void add_commandlet(gvi_commandlet* p);
	//============================================================================================
	// debug helpers
	//
	virtual void debug_draw_skeletal(gv_entity* entity);

protected:
	virtual void init(const gv_engine_option& option);
	virtual bool load_default_option(gv_engine_option& option);
	virtual bool do_async_load(gv_async_load_info* info);
	virtual void init_one_processor(const gv_event_processor_desc& desc);

	virtual bool execute(const gv_string_tmp& str);

protected:
	gvt_ref_ptr< gv_entity > m_game_entity;
	gvt_ref_ptr< gv_material > m_default_material;
	//============================================================================================
	//								:private data of game
	//engine
	//============================================================================================
	gvt_ref_ptr< gv_game_engine_data > m_engine_data;
	gv_game_engine_data* get_game_engine_data()
	{
		return m_engine_data;
	};
	gv_int m_fps;
};

// this function need to be implemented by user to register user drivers.
extern bool gv_register_user_drivers();
}