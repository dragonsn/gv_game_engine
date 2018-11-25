#include "gv_base/gesture/gv_gesture.h"
namespace unit_test_gesture_2d
{
int tick_count = 0;
class gv_unit_test_gesture_2d : public gv_unit_test_with_renderer
{
public:
	gv_int m_max_frame;
	gv_bool m_drag_started;
	gvt_array< gv_vector2 > path;
	gv_gesture gesture;
	gv_string last_gesture;
	gv_float last_score;
	virtual gv_string name()
	{
		return "unit_test_gesture_2d";
	}
	virtual void initialize()
	{
		if (gv_global::command_line_options.size())
		{
			gv_global::command_line_options[0] >> m_max_frame;
		}
		else
			m_max_frame = 1000;
		m_drag_started = false;
		last_score = 0;
	};
	virtual void render()
	{
		//gv_string_tmp s=" a GESTURE test ====>>" ;
		//s<<"last==>:"<<last_gesture <<"last score==>:"<<last_score;
		//gv_global::debug_draw.get()->draw_string(*s,gv_vector2i(100,120),gv_color::GREEN());

		gv_vector2i cu_pos;
		gv_global::input->get_mouse_pos(cu_pos);
		gv_vector2 pos2;
		pos2 = cu_pos;
		gv_bool lb_down = gv_global::input->is_key_down(e_key_lbutton);
		if (!m_drag_started && lb_down)
		{
			m_drag_started = true;
			path.clear();
		}
		if (m_drag_started)
		{
			path.push_back(pos2);
			for (int i = 0; i < path.size() - 1; i++)
			{
				gv_vector3 v = path[i];
				v.set_z(0.5);
				gv_vector3 v2 = path[i + 1];
				v2.set_z(0.5);
				gv_global::debug_draw.get()->draw_line(v, v2, gv_color::RED(), gv_color::BLUE());
			}
		}
		if (m_drag_started && !lb_down)
		{
			m_drag_started = false;
			gesture.recognize(path, last_score, last_gesture);
		}
		tick_count++;
	};
	virtual bool is_finished()
	{
		if (tick_count < m_max_frame)
			return false;
		return true;
	}
} test;
gv_unit_test_with_renderer* ptest = &test;
};
