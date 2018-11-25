#pragma once
namespace gv
{
enum gve_touch_event
{
	e_touch_none,
	e_touch_press,
	e_touch_release,
	e_touch_move,
	e_touch_last_event,
};

enum gve_gesture_event
{
	e_gesture_event_none,
	e_gesture_event_click,
	e_gesture_event_drag,
	e_gesture_event_double_click,
	e_gesture_event_circle_clock_wise,
	e_gesture_event_circle_anti_clock_wise,
	e_gesture_event_v,
	e_gesture_event_x,
	e_gesture_event_swipe_up,
	e_gesture_event_swipe_down,
	e_gesture_event_swipe_left,
	e_gesture_event_swipe_right,
	e_gesture_event_2_point_start = 100,
	e_gesture_event_click_2_point,
	e_gesture_event_double_click_2_point,
	e_gesture_event_zoom,
	e_gesture_event_rotate,
	e_gesture_event_swipe_2_point,
};

// this is a wrapper class of one dollar implementation
// http://depts.washington.edu/aimgroup/proj/dollar/
// ./gv_gesture_all_in_one.hpp
class gv_gesture_data;
class gv_gesture : public gv_refable
{
public:
	typedef gvt_array< gv_vector2 > path_2d;
	gv_gesture();
	~gv_gesture();
	void add_template(const path_2d& path, const gv_string& id);
	gv_bool recognize(const path_2d& path, gv_float& score, gv_string& id);
	static const char* gesture_name(gve_gesture_event);
	static gve_gesture_event gesture_from_name(const char* name);

private:
	gvt_ptr< gv_gesture_data > m_data;
	bool is_line(const path_2d& path);
};

//===============================================================
class gv_touch_event_listener_data;
struct touch_point_info;
static const int gvc_max_touch_contact_index = 20;
const char* gv_gesture_event_name(gve_gesture_event e);
struct gv_gesture_event_result
{
	gv_gesture_event_result()
	{
		GVM_ZERO_ME;
	}

	gv_gesture_event_result(const gv_gesture_event_result& r)
	{
		(*this) = r;
	}

	gv_gesture_event_result& operator=(const gv_gesture_event_result& r)
	{
		duration = r.duration;
		score = r.score;
		speed = r.speed;
		event = r.event;
		stroke_path = r.stroke_path;
		time = r.time;
		return *this;
	}

	gv_double time;
	gv_double duration;
	gv_float score;
	gv_vector2 speed;
	gve_gesture_event event;
	gvt_array< gv_vector2 > stroke_path;
};

class gv_touch_event_listener
{
public:
	gv_touch_event_listener();
	virtual ~gv_touch_event_listener();
	virtual gv_bool listen(gve_touch_event e, int x, int y,
						   unsigned int contact_index,
						   gv_gesture_event_result& result,
						   bool update_time_only = false);
	virtual gv_bool tick();
	virtual void reset();
	virtual void get_point_path(unsigned int contact_index,
								gvt_array< gv_vector2 >& path,
								gv_double time_duration_limit = 15.0,
								gv_int sample_limit = 500);
	virtual void enable_stroke_gesture_recognition(gv_bool b);
	virtual void set_recognizer(gv_gesture* r);
	virtual void set_start_drawing_time_threshold(gv_double);
	virtual void set_start_drawing_distance_threshold(gv_float);

public:
	gv_bool m_enable_multi_touch;

protected:
	virtual bool test_is_drawing_start(touch_point_info&, int x, int y);
	gvt_ref_ptr< gv_touch_event_listener_data > m_data;
	gv_double m_start_drawing_time_threshold;
	gv_double m_end_drawing_time_threshold;
	gv_float m_start_drawing_distance_threshold;
};

}
