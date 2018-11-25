#include "gv_framework_private.h"
#include "gv_framework.h"
#include "gv_base/gesture/gv_gesture.h"

namespace gv
{
class gv_input_manager_imp : public gv_refable
{

	friend class gv_input_manager;

protected:
	static const int gvc_key_nb = 256;
	int gv_2_virtual_key[gvc_key_nb];
	int virtual_key_2_gv[gvc_key_nb];
	int mouse_delta;
	gv_vector2i mouse_position;
	bool key_input_state[e_key_max + 1];
	gv_double key_press_time[e_key_max + 1];
	bool key_just_pressed[e_key_max + 1];
	bool key_just_released[e_key_max + 1];
	bool key_double_clicked[e_key_max + 1];
	gv_vector2i mouse_delta_xy;
	bool first_time_mouse;
	gv_vector2i last_mouse_pos;
	gv_input_manager* m_mgr;

public:
	gv_input_manager_imp(gv_input_manager* owner)
	{
		memset(gv_2_virtual_key, -1, sizeof(gv_2_virtual_key));
		memset(virtual_key_2_gv, -1, sizeof(virtual_key_2_gv));
		memset(key_input_state, 0, sizeof(key_input_state));
		gvt_zero(key_press_time);
		gvt_zero(key_just_pressed);
		gvt_zero(key_just_released);
		gvt_zero(key_double_clicked);
		first_time_mouse = true;
		mouse_position.x = 0;
		mouse_position.y = 0;
		mouse_delta_xy = 0;
		mouse_delta = 0;
		// TO do read config file
		gv_create_device_key_mapping(gv_2_virtual_key, virtual_key_2_gv);
	}
	~gv_input_manager_imp(){};
	void reset()
	{
		gvt_zero(key_just_pressed);
		gvt_zero(key_just_released);
		gvt_zero(key_double_clicked);
	}

	gve_key map_key(int vk)
	{
		int ret = virtual_key_2_gv[vk];
		if (ret == -1)
			return e_key_max;
		return (gve_key)ret;
	};

	int map_key(gve_key my_key)
	{
		GV_ASSERT(my_key < e_key_max);
		int ret = gv_2_virtual_key[my_key];
		return ret;
	};

	bool is_key_down(gve_key my_key)
	{
		GV_ASSERT(my_key < e_key_max);
		return key_input_state[my_key];
	};

	bool set_key_down(gve_key key, bool down)
	{
		if (key >= e_key_max)
			return false;
		gv_bool last_key_down = key_input_state[key];
		key_just_pressed[key] = m_mgr->test_key_just_pressed(down, last_key_down);
		key_just_released[key] = m_mgr->test_key_just_released(down, last_key_down);
		if (key_just_pressed[key])
		{
			gv_double cu_time = gv_global::time->get_sec_from_start();
			key_double_clicked[key] =
				m_mgr->test_key_double_clicked(cu_time, key_press_time[key]);
			key_press_time[key] = cu_time;
		}
		else
		{
			key_double_clicked[key] = false;
		}
		key_input_state[key] = down;
		return true;
	};

	void set_mouse_pos(const gv_vector2i& pos)
	{
		mouse_position = pos;
		if (first_time_mouse)
		{
			first_time_mouse = false;
			last_mouse_pos = pos;
			mouse_delta_xy = 0;
		}
		else
		{
			mouse_delta_xy = mouse_position - last_mouse_pos;
			last_mouse_pos = mouse_position;
		}
	};

	void get_mouse_pos_delta(gv_vector2i& pos)
	{
		pos = mouse_delta_xy;
	}

	void get_mouse_pos(gv_vector2i& pos)
	{
		pos = mouse_position;
	};

	void set_mouse_wheel_delta(int d)
	{
		mouse_delta = d;
	};
	int get_mouse_wheel_delta()
	{
		return mouse_delta;
	};

	bool is_key_just_pressed(gve_key key)
	{
		return key_just_pressed[key];
	};
	bool is_key_just_released(gve_key key)
	{
		return key_just_pressed[key];
	};
	;
	bool is_key_double_clicked(gve_key key)
	{
		return key_double_clicked[key];
	};
	;
};

gv_input_manager::gv_input_manager()
{
	m_pimp = new gv_input_manager_imp(this);
};
gv_input_manager::~gv_input_manager(){

};
void gv_input_manager::tick()
{
	m_pimp->reset();
};
bool gv_input_manager::is_key_down(gve_key key)
{
	return m_pimp->is_key_down(key);
};
bool gv_input_manager::is_key_just_pressed(gve_key key)
{
	return m_pimp->is_key_just_pressed(key);
};
bool gv_input_manager::is_key_just_released(gve_key key)
{
	return m_pimp->is_key_just_released(key);
};
bool gv_input_manager::is_key_double_clicked(gve_key key)
{
	return m_pimp->is_key_double_clicked(key);
};
bool gv_input_manager::set_key_down(gve_key key, bool down)
{
	return m_pimp->set_key_down(key, down);
};
gve_key gv_input_manager::map_key(int device_key)
{
	return m_pimp->map_key(device_key);
};
int gv_input_manager::map_key(gve_key key)
{
	return m_pimp->map_key(key);
};
void gv_input_manager::set_mouse_pos(const gv_vector2i& pos)
{
	return m_pimp->set_mouse_pos(pos);
};
void gv_input_manager::get_mouse_pos(gv_vector2i& pos)
{
	return m_pimp->get_mouse_pos(pos);
};
void gv_input_manager::get_mouse_pos_delta(gv_vector2i& pos)
{
	return m_pimp->get_mouse_pos_delta(pos);
};
void gv_input_manager::set_mouse_wheel_delta(int d)
{
	return m_pimp->set_mouse_wheel_delta(d);
};
int gv_input_manager::get_mouse_wheel_delta()
{
	return m_pimp->get_mouse_wheel_delta();
};

bool gv_input_manager::simulate_one_point_touch_event_on_pc(
	class gv_touch_event_listener* listener, gv_gesture_event_result& result)
{
	gv_vector2i p;
	this->get_mouse_pos_delta(p);
	gve_touch_event e = e_touch_none;
	static gv_bool last_key_down = false;
	gv_bool key_down = is_key_down(e_key_lbutton);

	if (key_down && !last_key_down)
	{
		e = e_touch_press;
	}
	else if (last_key_down && !key_down)
	{
		e = e_touch_release;
	}
	else if (key_down && p != gv_vector2i::get_zero_vector())
	{
		e = e_touch_move;
	}
	if (e == e_touch_none)
		return false;

	get_mouse_pos(p);
	gv_bool b = listener->listen(e, p.x, p.y, 0, result);
	last_key_down = key_down;
	return b;
};
};
