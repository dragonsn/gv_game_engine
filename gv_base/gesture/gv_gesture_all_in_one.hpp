#include "one_dollar/GeometricRecognizer.h"
#include "one_dollar/GeometricRecognizer.hpp"

//a gesture recognition based on 1 $ implementation
//http://depts.washington.edu/madlab/proj/dollar/index.html
namespace gv
{
class gv_gesture_data : public gv_refable
{
public:
	gv_gesture_data(){};
	~gv_gesture_data()
	{
	}
	static void to_one_dollar(const gv_gesture::path_2d& path,
							  DollarRecognizer::Path2D& path_1_d)
	{
		path_1_d.resize(path.size());
		for (int i = 0; i < path.size(); i++)
		{
			path_1_d[i].x = path[i].get_x();
			path_1_d[i].y = path[i].get_y();
		}
	}
	DollarRecognizer::GeometricRecognizer m_recognizer;
};

gv_gesture::gv_gesture()
{
	m_data = new gv_gesture_data;
	m_data->m_recognizer.loadTemplates();

	// m_data->m_recognizer.setRotationInvariance(false);//don't ignore rotation
};
gv_gesture::~gv_gesture(){

};

void gv_gesture::add_template(const path_2d& path, const gv_string& id)
{
	DollarRecognizer::Path2D p;
	gv_gesture_data::to_one_dollar(path, p);
	m_data->m_recognizer.addTemplate(*id, p);
}

bool gv_gesture::is_line(const path_2d& path)
{
	if (path.size() < 3)
		return false;
	gv_vector2 dir = *path.last() - *path.first();
	if (dir.length() < 10.f)
		return false;
	dir.normalize();
	for (int i = 0; i < path.size() - 1; i++)
	{
		gv_vector2 dir2 = path[i + 1] - path[0];
		if (dir2.length() < 3.f)
			continue;
		dir2.normalize();
		if (dir.dot(dir2) < 0)
		{
			return false;
		}

		dir2 = path[i + 1] - path[i];
		if (dir2.length() < 3.f)
			continue;
		dir2.normalize();
		if (dir.dot(dir2) < 0)
		{
			return false;
		}
	}
	return true;
};
;
gv_bool gv_gesture::recognize(const path_2d& path, gv_float& score,
							  gv_string& id)
{
	// we use bounding rect to test direction up , down , left ,right
	// test bounding if the width/length too large or too small, not fit for one
	// dollar
	gv_rect rect;
	score = 1.0f;
	for (int i = 0; i < path.size(); i++)
	{
		rect.add(path[i]);
	}
	if (rect.is_empty())
		return false;
	gv_float ratio = rect.width() / (rect.height() + 0.001f);
	bool b = is_line(path);
	if (ratio > 2.f && b)
	{
		if (path[0].x < path[path.size() - 1].x)
		{
			id = "right";
			return true;
		}
		else
		{
			id = "left";
			return true;
		}
	}
	else if (ratio < 0.5 && b)
	{
		if (path[0].y < path[path.size() - 1].y)
		{
			id = "down";
			return true;
		}
		else
		{
			id = "up";
			return true;
		}
	}
	DollarRecognizer::RecognitionResult r;
	DollarRecognizer::Path2D p;
	gv_gesture_data::to_one_dollar(path, p);
	r = m_data->m_recognizer.recognize(p);
	score = (gv_float)r.score;
	id = r.name.c_str();
	return true;
};

const char* gv_gesture::gesture_name(gve_gesture_event e)
{
	switch (e)
	{
	case e_gesture_event_none:
		return "none";
	case e_gesture_event_click:
		return "click";
	case e_gesture_event_double_click:
		return "double_click";
	case e_gesture_event_drag:
		return "drag";
	case e_gesture_event_circle_clock_wise:
		return "circle_clock_wise";
	case e_gesture_event_circle_anti_clock_wise:
		return "circle_anti_clock_wise";
	case e_gesture_event_swipe_up:
		return "up";
	case e_gesture_event_swipe_down:
		return "down";
	case e_gesture_event_swipe_left:
		return "left";
	case e_gesture_event_swipe_right:
		return "right";
	case e_gesture_event_2_point_start:
		return "2_point_start";
	case e_gesture_event_click_2_point:
		return "click_2_point";
	case e_gesture_event_double_click_2_point:
		return "double_click_2_point";
	case e_gesture_event_zoom:
		return "zoom";
	case e_gesture_event_rotate:
		return "rotate";
	case e_gesture_event_swipe_2_point:
		return "swipe_2_point";
	case e_gesture_event_v:
		return "v";
	case e_gesture_event_x:
		return "x";
	};
	return "none";
};

gve_gesture_event gv_gesture::gesture_from_name(const char* name)
{
	gv_string_tmp s(name);
	if (s == "none")
		return e_gesture_event_none;
	if (s == "click")
		return e_gesture_event_click;
	if (s == "drag")
		return e_gesture_event_drag;
	if (s == "double_click")
		return e_gesture_event_double_click;
	if (s == "circle_clock_wise")
		return e_gesture_event_circle_clock_wise;
	if (s == "circle_anti_clock_wise")
		return e_gesture_event_circle_anti_clock_wise;
	if (s == "up")
		return e_gesture_event_swipe_up;
	if (s == "down")
		return e_gesture_event_swipe_down;
	if (s == "left")
		return e_gesture_event_swipe_left;
	if (s == "right")
		return e_gesture_event_swipe_right;
	if (s == "2_point_start")
		return e_gesture_event_2_point_start;
	if (s == "click_2_point")
		return e_gesture_event_click_2_point;
	if (s == "double_click_2_point")
		return e_gesture_event_double_click_2_point;
	if (s == "zoom")
		return e_gesture_event_zoom;
	if (s == "rotate")
		return e_gesture_event_rotate;
	if (s == "swipe_2_point")
		return e_gesture_event_swipe_2_point;
	if (s == "v")
		return e_gesture_event_v;
	if (s == "x")
		return e_gesture_event_x;
	return e_gesture_event_none;
};

//=========================================================================================
struct touch_history
{
	touch_history()
	{
		GVM_ZERO_ME;
	}
	gve_touch_event event;
	gv_double time;
	gv_vector2 pos;
};
struct touch_point_info
{

	touch_point_info()
	{
		// GVM_ZERO_ME;
		reset();
	}

	void reset()
	{
		/*velocity=gv_vector2::get_zero_vector();
    avg_speed=gv_vector2::get_zero_vector();
    avg_velocity=gv_vector2::get_zero_vector();
    total_displacement=gv_vector2::get_zero_vector();
    total_length_of_path=gv_vector2::get_zero_vector();
    is_pressed = false;*/
		GVM_ZERO_ME;
		brother_index = -1;
	}
	void on_press(const gv_vector2& in_pos, gv_double cu_time)
	{
		reset();
		double_click_start_time = press_start_time;
		last_updated_time = press_start_time = cu_time;
		press_start_pos = pos = in_pos;
		press_duration = 0;
		is_pressed = true;
	}
	void on_release(const gv_vector2& in_pos, gv_double cu_time)
	{
		update_pos(in_pos, cu_time);
		last_release_elapse = cu_time - last_release_time;
		last_release_time = cu_time;
		is_pressed = false;
	}
	void update_pos(const gv_vector2& in_pos, gv_double cu_time)
	{
		gv_float duration = (gv_float)(cu_time - press_start_time);
		gv_vector2 moved = in_pos - pos;
		gv_float dt = (gv_float)(cu_time - last_updated_time);
		if (dt < 0.001f)
			dt = 0.001f;
		gv_vector2 cu_v = moved / (dt);
		total_displacement += moved;
		moved.abs();
		total_length_of_path += moved;
		avg_speed = total_length_of_path / duration;
		avg_velocity = total_displacement / duration;
		velocity = cu_v;
		last_updated_time = cu_time;
		press_duration = duration;
		pos = in_pos;
		// GVM_DEBUG_OUT("[ui] point v=" <<velocity);
	}
	gv_float relative_speed(const touch_point_info& info)
	{
		gv_float dist = pos.distance_to(info.pos);
		if (dist < 10.f)
		{
			// too close
			return 0;
		}
		gv_vector2 v0 = velocity;
		gv_vector2 v1 = info.velocity;
		float s0 = v0.normalize();
		float s1 = v1.normalize();
		// assume the delta time ==0.01;
		s0 *= 0.01f;
		s1 *= 0.01f;
		gv_float half_dist = dist * 0.5f;
		// avoid over shooting!
		if (s0 > half_dist)
			s0 = half_dist;
		if (s1 > half_dist)
			s1 = half_dist;
		gv_float dist2 = (pos + v0 * s0).distance_to(info.pos + v1 * s1);
		return dist - dist2;
	} //

	gv_bool is_pressed;
	gv_bool is_drawing;
	gv_bool is_dragging;
	gv_int brother_index;
	gv_double press_start_time;
	gv_double last_release_time;
	gv_double last_updated_time;
	gv_double last_release_elapse;
	gv_double double_click_start_time;
	gv_double press_duration;
	gv_vector2 pos;
	gv_vector2 press_start_pos;
	// motion info of this stroke , will be reset after the point is released.
	gv_vector2 avg_speed;
	gv_vector2 avg_velocity;
	gv_vector2 velocity;
	gv_vector2 total_displacement;
	gv_vector2 total_length_of_path;
};

class gv_touch_event_listener_data : public gv_refable
{
public:
	gv_touch_event_listener_data()
	{
		nb_pressed_point = 0;
		max_double_click_duration = 0.5f;
		max_click_duration = 0.2f;
		threshold_gesture_score = 0.6f;
		threshold_stroke_distance_sqared = 256;
		recognizer = new gv_gesture;
		reset();
	};
	// some threshold
	gv_double max_double_click_duration;
	gv_double max_click_duration;
	gv_float threshold_gesture_score;
	gv_float threshold_stroke_distance_sqared;
	//
	gvt_ref_ptr< gv_gesture > recognizer;
	gvt_array_static< gvt_ring_buf< touch_history >, gvc_max_touch_contact_index >
		contact_point_history;
	gvt_array_static< touch_point_info, gvc_max_touch_contact_index >
		contact_point_state;
	gv_int nb_pressed_point;
	//
	int get_pressed_points(gv_int index[2])
	{
		int cu_index = 0;
		for (int i = 0; i < gvc_max_touch_contact_index; i++)
		{
			touch_point_info& info = contact_point_state[i];
			if (info.is_pressed)
			{
				index[cu_index++] = i;
			}
			if (cu_index > 1)
				break;
		}
		if (nb_pressed_point != cu_index)
		{
			GVM_WARNING("[ui] pressed info not consistent !!!" << nb_pressed_point
															   << "vs " << cu_index);
		}
		return cu_index;
	};
	void reset()
	{
		for (int i = 0; i < gvc_max_touch_contact_index; i++)
		{
			touch_point_info& info = contact_point_state[i];
			info.reset();
			contact_point_history[i].clear();
			contact_point_history[i].set_capacity((size_t) 1000);
		}
	}
};

gv_touch_event_listener::gv_touch_event_listener()
{
	m_data = new gv_touch_event_listener_data;
	m_enable_multi_touch = false;

	m_start_drawing_distance_threshold = 50;
	m_start_drawing_time_threshold = 0.3f;
	m_end_drawing_time_threshold = 1.0f;
}

gv_touch_event_listener::~gv_touch_event_listener(){};

void gv_touch_event_listener::set_start_drawing_time_threshold(gv_double d)
{
	m_start_drawing_time_threshold = d;
};

void gv_touch_event_listener::set_start_drawing_distance_threshold(gv_float f)
{
	m_start_drawing_distance_threshold = f;
};

bool gv_touch_event_listener::test_is_drawing_start(touch_point_info& info,
													int x, int y)
{
	if (info.press_duration > m_start_drawing_time_threshold &&
		!info.is_drawing)
	{
		info.is_dragging = true;
		info.is_drawing = false;
		return false;
	}
	else if (info.press_duration > m_end_drawing_time_threshold &&
			 !info.is_dragging)
	{
		info.is_dragging = true;
		info.is_drawing = false;
		return false;
	}
	else if (info.total_displacement.length() >
			 m_start_drawing_distance_threshold)
	{
		info.is_drawing = true;
		info.is_dragging = false;
	}
	return false;
};

gv_bool gv_touch_event_listener::listen(gve_touch_event e, int x, int y,
										unsigned int contact_index,
										gv_gesture_event_result& result,
										bool update_time_only)
{

	// GVM_DEBUG_OUT( "[ui]  touch event "<< e << " for index "<<contact_index);
	if (contact_index >= gvc_max_touch_contact_index)
		return e_gesture_event_none;
	gv_double cu_time = gv_global::time->get_sec_from_start();
	result.time = cu_time;
	touch_point_info& cu_state = m_data->contact_point_state[contact_index];
	if (update_time_only)
	{ // update state when not move
		if (!cu_state.is_pressed)
			return false;
		x = cu_state.pos.x;
		y = cu_state.pos.y;
	}
	gvt_ring_buf< touch_history >& history =
		m_data->contact_point_history[contact_index];
	touch_history h;
	gv_vector2 pos((gv_float)x, (gv_float)y);
	h.event = e;
	h.pos = pos;
	h.time = cu_time;
	history.push_back(h);

	gv_int pressed_point[2];
	gv_int cu_pressed_point = m_data->get_pressed_points(pressed_point);

	if (e == e_touch_press)
	{
		if (cu_state.is_pressed == false)
		{
			m_data->nb_pressed_point++;
			cu_state.on_press(pos, cu_time);
			if (cu_pressed_point && pressed_point[0] != (int)contact_index)
			{
				// 2 point hold at the same time are brothers., update the brother
				// index.
				cu_state.brother_index = pressed_point[0];
				touch_point_info& brother =
					m_data->contact_point_state[pressed_point[0]];
				brother.brother_index = contact_index;
			}
		}
		cu_state.is_pressed = true;
	}
	else if (e == e_touch_release)
	{
		if (cu_state.is_pressed == true)
		{
			cu_state.on_release(pos, cu_time);
			m_data->nb_pressed_point--;
			GV_ASSERT(m_data->nb_pressed_point >= 0);
			gv_double duration = cu_state.press_duration;
			// one click or double click
			if (duration < m_data->max_click_duration ||
				(cu_state.total_length_of_path.length_squared() <
					 m_data->threshold_stroke_distance_sqared &&
				 duration < m_data->max_click_duration * 2))
			{
				if (cu_time - cu_state.double_click_start_time <
					m_data->max_double_click_duration)
				{
					// double click event;
					result.duration = cu_state.last_release_elapse;
					if (cu_state.brother_index == -1)
					{
						result.event = e_gesture_event_double_click;
						return true;
					}
					else
					{
						result.event = e_gesture_event_double_click_2_point;
						return true;
					}
				}
				else
				{
					result.duration = cu_state.press_duration;
					if (cu_state.brother_index == -1)
					{
						result.event = e_gesture_event_click;
					}
					else
					{
						result.event = e_gesture_event_click_2_point;
					}
					return true;
				}
			}
			// stroke recognition., this is only for one point.
			else if (cu_state.is_drawing)
			{
				if (cu_state.brother_index == -1)
				{
					gvt_array< gv_vector2 > path;
					this->get_point_path(contact_index, path);
					gv_float score;
					gv_string id;
					m_data->recognizer->recognize(path, score, id);
					if (score > m_data->threshold_gesture_score)
					{
						gve_gesture_event e = m_data->recognizer->gesture_from_name(*id);
						result.score = score;
						result.event = e;
						return true;
					}
				}
				return false;
			}
		}
		else // for some reason , the press event not recorded;
			cu_state.is_pressed = false;
	}
	else if (e == e_touch_move && cu_state.is_pressed)
	{
		cu_state.update_pos(pos, cu_time);
		if (!test_is_drawing_start(cu_state, x, y))
		{
			if (cu_state.is_dragging)
			{
				result.event = e_gesture_event_drag;
				result.speed = cu_state.pos - cu_state.press_start_pos;
				// result.speed.normalize();
				// result.speed *;
				return true;
			}
		}

		if (cu_state.brother_index != -1 && m_enable_multi_touch)
		{
			// test 2point pinch!
			touch_point_info& brother =
				m_data->contact_point_state[cu_state.brother_index];
			if (brother.is_pressed == false)
			{
				return false;
			}
			gv_vector2 v0 = cu_state.velocity;
			gv_vector2 v1 = brother.velocity;
			gv_float s0 = v0.length_squared();
			gv_float s1 = v1.length_squared();
			gv_vector2 v0to1 = brother.pos - cu_state.pos;
			gv_vector2 v1to0 = -v0to1;
			v0.normalize();
			v1.normalize();
			if (s0 > m_data->threshold_stroke_distance_sqared &&
				s1 > m_data->threshold_stroke_distance_sqared)
			{
				gv_float f = v0.dot(v1);
				if (f > 0.7f)
				{
					// the same direction.
					result.event = e_gesture_event_swipe_2_point;
					result.speed = (cu_state.velocity + brother.velocity) / 2.f;
					return true;
				}
				else if (f < -0.7f)
				{
					// to the opporsite direction.
					gv_float s = cu_state.relative_speed(brother);
					result.event = e_gesture_event_zoom;
					result.speed.x = s;
					result.speed.y = 0;
					return true;
				}
			}
			else if (s0 < m_data->threshold_stroke_distance_sqared * 0.25f &&
					 s1 > m_data->threshold_stroke_distance_sqared)
			{
				// rotate around 0
				gv_vector2 normal = v0to1.ortho();
				normal.normalize();
				gv_float ndotv = normal.dot(v0);
				if (gvt_abs(ndotv) > 0.5f)
				{
					result.event = e_gesture_event_rotate;
					result.speed.x = ndotv;
					return true;
				}
			}
			else if (s1 < m_data->threshold_stroke_distance_sqared * 0.25f &&
					 s0 > m_data->threshold_stroke_distance_sqared)
			{
				// rotate around 1
				gv_vector2 normal = v1to0.ortho();
				normal.normalize();
				gv_float ndotv = normal.dot(v1);
				if (gvt_abs(ndotv) > 0.5f)
				{
					result.event = e_gesture_event_rotate;
					result.speed.x = ndotv;
					return true;
				}
			}
		} // 2 point
	}	 // move
	return false;
};

void gv_touch_event_listener::get_point_path(unsigned int contact_index,
											 gvt_array< gv_vector2 >& path,
											 gv_double time_duration_limit,
											 gv_int sample_limit)
{
	gvt_ring_buf< touch_history >& history =
		m_data->contact_point_history[contact_index];
	gv_double cu_time = gv_global::time->get_sec_from_start();
	path.reserve(sample_limit);
	for (int i = (gv_int)history.size() - 1; i >= 0; i--)
	{
		touch_history& h = history[i];
		if (cu_time - h.time > time_duration_limit)
			break;
		path.push_back(h.pos);
		if (path.size() == sample_limit)
			break;
		if (h.event == e_touch_press)
			break;
	}
	std::reverse(path.begin(), path.end());
};

void gv_touch_event_listener::reset()
{
	m_data->reset();
}

void gv_touch_event_listener::set_recognizer(gv_gesture* r)
{
	m_data->recognizer = r;
};

gv_bool gv_touch_event_listener::tick()
{
	return true;
};
void gv_touch_event_listener::enable_stroke_gesture_recognition(gv_bool b)
{
}
};
