namespace unit_test_render_simple_2d
{
int tick_count = 0;
class gv_unit_test_render_simple : public gv_unit_test_with_renderer
{
public:
	virtual gv_string name()
	{
		return "unit_test_render_simple_2d";
	}
	virtual void render()
	{
		gv_string_tmp s = "just a simple test of text draw ====>>";
		s << "tick_count:" << tick_count;
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120), gv_color::RED());
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120 + 10), gv_color::GREEN());
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120 + 20), gv_color::BLUE());
		gv_global::debug_draw.get()->draw_triangle(gv_vector3(100, 160, 0.5f), gv_vector3(140, 160, 0.5f), gv_vector3(100, 200, 0.5f), gv_color::RED());
		gv_global::debug_draw.get()->draw_triangle(gv_vector3(150, 160, 0.5f), gv_vector3(190, 160, 0.5f), gv_vector3(150, 200, 0.5f), gv_color::GREEN());
		gv_global::debug_draw.get()->draw_triangle(gv_vector3(200, 160, 0.5f), gv_vector3(240, 160, 0.5f), gv_vector3(200, 200, 0.5f), gv_color::BLUE());
		gv_global::debug_draw.get()->draw_triangle(gv_vector3(250, 160, 0.5f), gv_vector3(290, 160, 0.5f), gv_vector3(250, 200, 0.5f), gv_color::RED(), gv_color::GREEN(), gv_color::BLUE());

		gv_vector3 a = gv_vector3(100, 220, 0.5f);
		gv_vector3 ex = gv_vector3(50, 0, 0);
		gv_vector3 ey = gv_vector3(0, 50, 0);

		gv_global::debug_draw.get()->draw_quad(a, a + ex, a + ey, a + ex + ey, gv_color::RED());
		a += ex + ex / 2;
		gv_global::debug_draw.get()->draw_quad(a, a + ex, a + ey, a + ex + ey, gv_color::GREEN());
		a += ex + ex / 2;
		gv_global::debug_draw.get()->draw_quad(a, a + ex, a + ey, a + ex + ey, gv_color::BLUE());
		a += ex + ex / 2;
		gv_global::debug_draw.get()->draw_quad(a, a + ex, a + ey, a + ex + ey, gv_color::RED(), gv_color::GREEN(), gv_color::BLUE(), gv_color::WHITE());

		a = gv_vector3(100, 280, 0.5f);

		gv_global::debug_draw.get()->draw_line(a, a + ex, gv_color::RED(), gv_color::BLUE());
		a += ex + ex / 2;
		gv_global::debug_draw.get()->draw_line(a, a + ex, gv_color::GREEN(), gv_color::BLUE());

		tick_count++;
	};
	virtual bool is_finished()
	{
		if (tick_count < 1000)
			return false;
		return true;
	}
} test;
gv_unit_test_with_renderer* ptest = &test;
};
