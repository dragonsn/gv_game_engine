#pragma once
namespace gv
{

class gv_input_manager_imp;

class gv_input_manager
{
public:
	gv_input_manager();
	virtual ~gv_input_manager();
	virtual void tick();
	virtual bool is_key_down(gve_key key);
	virtual bool is_key_just_pressed(gve_key key);
	virtual bool is_key_just_released(gve_key key);
	virtual bool is_key_double_clicked(gve_key key);
	virtual bool set_key_down(gve_key key, bool down);
	virtual gve_key map_key(int device_key);
	virtual int map_key(gve_key);
	virtual void set_mouse_pos(const gv_vector2i& pos);
	virtual void get_mouse_pos(gv_vector2i& pos);
	virtual void get_mouse_pos_delta(gv_vector2i& pos);
	virtual void set_mouse_wheel_delta(int d);
	virtual int get_mouse_wheel_delta();
	virtual bool
	simulate_one_point_touch_event_on_pc(class gv_touch_event_listener* listener,
										 struct gv_gesture_event_result& result);
	inline bool test_key_just_pressed(gv_bool down, gv_bool last_key_down)
	{
		gv_bool pressed = false;
		if (down && !last_key_down)
		{
			pressed = true;
		}
		return pressed;
	}
	inline bool test_key_just_released(gv_bool down, gv_bool last_key_down)
	{
		gv_bool released = false;
		if (!down && last_key_down)
		{
			released = true;
		}
		return released;
	}
	inline bool test_key_double_clicked(gv_double cu_time,
										gv_double last_click_time,
										gv_double threshold = 0.3)
	{
		gv_bool ret = false;
		if (cu_time - last_click_time < threshold)
		{
			ret = true;
		}
		return ret;
	}

private:
	gvt_ref_ptr< gv_input_manager_imp > m_pimp;
};

#define GVM_BGN_TEST_JUST_PRESSED(key)                                           \
	{                                                                            \
		static bool bool_is_##key##_down = false;                                \
		if (gv_global::input->is_key_down(e_key_##key) && !bool_is_##key##_down) \
		{

#define GVM_END_TEST_JUST_PRESSED(key)                                 \
	}                                                                  \
	bool_is_##key##_down = gv_global::input->is_key_down(e_key_##key); \
	}

namespace gv_global
{
extern gvt_global< gv_input_manager > input;
};
};