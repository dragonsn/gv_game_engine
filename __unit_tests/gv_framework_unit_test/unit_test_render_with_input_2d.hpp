namespace unit_test_render_with_input_2d
{
int tick_count = 0;
class gv_unit_test_render_with_input : public gv_unit_test_with_renderer
{
public:
	gv_int m_max_frame;
	virtual gv_string name()
	{
		return "unit_test_render_with_input_2d";
	}
	virtual void initialize()
	{
		if (gv_global::command_line_options.size())
		{
			gv_global::command_line_options[0] >> m_max_frame;
		}
		else
			m_max_frame = 1000;
	};
	virtual void render()
	{
		gv_string_tmp s = "just a simple test with input ====>>";
		s << "tick_count:" << tick_count;
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120), gv_color::RED());

		gv_vector2i cu_pos(100, 120 + 10);

		s = "lbutton down is";
		s << gv_global::input->is_key_down(e_key_lbutton);
		gv_global::debug_draw.get()->draw_string(*s, cu_pos, gv_color::GREEN());
		cu_pos.y += 10;

		gv_global::input->get_mouse_pos(cu_pos);

		gv_vector3 a = gv_vector3(-25 + (float)cu_pos.x, -25 + (float)cu_pos.y, 0.5f);
		static float s_scale = 1.f;
		s_scale = s_scale + (float)gv_global::input->get_mouse_wheel_delta() / 1000.f;

		gv_vector3 ex = gv_vector3(50, 0, 0) * s_scale;
		gv_vector3 ey = gv_vector3(0, 50, 0) * s_scale;

		gv_global::input->set_mouse_wheel_delta(0);

		gv_global::debug_draw.get()->draw_quad(a, a + ex, a + ey, a + ex + ey, gv_color::RED());
		/*
			a+=ex+ex/2;
			gv_global::debug_draw.get()->draw_quad  (a,a+ex,a+ey,a+ex+ey,gv_color::GREEN());
			a+=ex+ex/2;
			gv_global::debug_draw.get()->draw_quad  (a,a+ex,a+ey,a+ex+ey,gv_color::BLUE());
			a+=ex+ex/2;
			gv_global::debug_draw.get()->draw_quad  (a,a+ex,a+ey,a+ex+ey,gv_color::RED(),gv_color::GREEN(),gv_color::BLUE(),gv_color::WHITE());

			a=gv_vector3(100,280,0.5f);

			gv_global::debug_draw.get()->draw_line(a,a+ex,gv_color::RED(),gv_color::BLUE());
			a+=ex+ex/2;
			gv_global::debug_draw.get()->draw_line  (a,a+ex,gv_color::GREEN(),gv_color::BLUE());
			*/

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
