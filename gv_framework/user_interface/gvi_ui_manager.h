#pragma once
namespace gv
{

class gvi_ui_manager : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gvi_ui_manager, gv_event_processor);
	gvi_ui_manager();
	virtual ~gvi_ui_manager();
	virtual void register_events();
	virtual void unregister_events();
	virtual bool tick(gv_float dt);
	virtual bool do_synchronization();
	virtual gv_uint msg_proc(gv_uint h_wnd, gv_uint msg, gv_uint wParam,
							 gv_uint lParam)
	{
		return 0;
	};

	virtual bool load_ui_package(const gv_string_tmp& file_name,
								 const gv_id& name)
	{
		return false;
	};
	virtual bool create_window(const gv_string_tmp& type, const gv_id& name)
	{
		return false;
	};
	virtual bool create_message_box(const gv_string_tmp& message,
									const gv_string_tmp& title,
									bool need_OK = true,
									bool need_cancel = true)
	{
		return false;
	};
	virtual bool close_message_box()
	{
		return false;
	};
	virtual bool message_box_return(gv_bool& is_ok)
	{
		return false;
	};
	virtual bool destroy_window()
	{
		return false;
	};
	virtual bool before_render()
	{
		return false;
	};
	virtual bool after_render()
	{
		return false;
	};
	virtual bool on_init_render()
	{
		return false;
	};
	virtual gv_string_tmp get_ui_data_path()
	{
		return gv_string_tmp();
	};
	virtual void set_scheme_name(){};
	virtual void set_renderer(gv_object* renderer)
	{
	}
	virtual void create_renderer(){};
	virtual gv_entity* get_ui_entity()
	{
		return NULL;
	}
	virtual bool is_ready()
	{
		return false;
	}
	virtual bool do_render(gv_float dt)
	{
		return false;
	}
};
}