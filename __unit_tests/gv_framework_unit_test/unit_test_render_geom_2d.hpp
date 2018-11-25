namespace unit_test_render_geom_2d
{
int tick_count = 0;
class gv_unit_test_render_geom_2d : public gv_unit_test_with_renderer
{
public:
	enum geom_test_type
	{
		geom_test_rect_clip_line,
		geom_test_ray_rect_intersect,
		geom_test_ray_rect_intersect_normal,
		geom_test_ray_trace,
		geom_test_ray_trace_dda,
		geom_test_ray_circle_intersect,
		geom_test_max
	};
	gv_unit_test_render_geom_2d()
	{
	}
	gv_int m_max_frame;
	gv_line m_line;
	gv_rect m_rect;
	gv_circle m_circle;
	gv_int m_current_test;
	float m_angle;
	gv_double m_last_tick_time;
	gv_double m_delta_time;

	virtual gv_string name()
	{
		return "unit_test_render_geom_2d";
	}
	virtual void initialize()
	{
		if (gv_global::command_line_options.size())
		{
			gv_global::command_line_options[0] >> m_max_frame;
		}
		else
			m_max_frame = 1000;
		m_current_test = geom_test_rect_clip_line;
		m_line.dir_v.set(0.7f, 0.7f, 0);
		m_line.dir_v.normalize();
		m_angle = 0;
		m_line.origin_p.set(400, 300, 0.1f);
		m_rect.set(200, 200, 400, 300);
		m_circle.set(gv_vector2(200, 200), 150.f);
		tick_count = 0;
		m_last_tick_time = gv_global::time->get_sec_from_start();
		m_delta_time = 0;
	};
	bool do_line_rect_test(float dt)
	{
		gv_string_tmp s = "do_line_rect_test,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120), gv_color::RED());

		float step = 20.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		gv_global::debug_draw.get()->draw_rect(m_rect, gv_color::BLUE(), 0.9f);

		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 100);
		gv_line_segment line_seg2;
		bool not_clip = gvt_geom< gv_float >::clip(line_seg, m_rect, line_seg2);
		if (not_clip)
		{

			gv_global::debug_draw.get()->draw_line(line_seg.start_p, line_seg.end_p, gv_color::GREY(), gv_color::GREY());
			gv_global::debug_draw.get()->draw_line(line_seg2.start_p, line_seg2.end_p, gv_color::RED(), gv_color::RED());
		}
		else
		{
			gv_global::debug_draw.get()->draw_line(line_seg.start_p, line_seg.end_p, gv_color::GREY(), gv_color::GREY());
		}
		return true;
	}

	bool do_ray_rect_test(float dt)
	{
		gv_string_tmp title = "do_line_rect_test,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(100, 120), gv_color::RED());

		float step = 20.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		gv_global::debug_draw.get()->draw_rect(m_rect, gv_color::BLUE(), 0.9f);
		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 300);
		gv_float s, t;
		bool hit = gvt_geom< gv_float >::intersect(m_line, m_rect, s, t);
		if (hit)
		{
			gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::RED());
			gv_rect rect;
			rect.move_to(m_line.get_point_on_line(s).v2);
			rect.extend(3);
			gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_B(), 0.3f);
			rect.set_size(0, 0);
			rect.move_to(m_line.get_point_on_line(t).v2);
			rect.extend(3);
			gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_D(), 0.3f);
		}
		else
		{
			gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::GREY());
			//gv_global::debug_draw.get()->draw_line (line_seg.start_p,line_seg.end_p,gv_color::GREY(), gv_color::GREY());
		}
		return true;
	}

	bool do_ray_rect_test_with_normal(float dt)
	{
		gv_string_tmp title = "do_line_rect_test,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(100, 120), gv_color::RED());

		float step = 20.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		gv_global::debug_draw.get()->draw_rect(m_rect, gv_color::BLUE(), 0.9f);
		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 300);
		gv_line_check_result result[2];
		bool hit = gvt_geom< gv_float >::intersect(m_line, m_rect, result);
		if (hit)
		{
			gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::RED());
			gv_rect rect;
			rect.move_to(result[0].pos.v2);
			rect.extend(3);
			gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_B(), 0.3f);
			rect.set_size(0, 0);
			rect.move_to(result[1].pos.v2);
			rect.extend(3);
			gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_D(), 0.3f);

			gv_line_segment normal0(result[0].pos, result[0].pos + result[0].normal * 50);
			gv_global::debug_draw.get()->draw_arrow(normal0, gv_color::PINK_B());

			gv_line_segment normal1(result[1].pos, result[1].pos + result[1].normal * 50);
			gv_global::debug_draw.get()->draw_arrow(normal1, gv_color::PINK_D());
		}
		else
		{
			gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::GREY());
			//gv_global::debug_draw.get()->draw_line (line_seg.start_p,line_seg.end_p,gv_color::GREY(), gv_color::GREY());
		}
		return true;
	}

	gv_bool plot(int x, int y) const
	{
		gv_rect rect;
		rect.move_to(gv_vector2((float)x * 10, (float)y * 10));
		rect.set_size(10, 10);
		gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_B(), 0.3f);
		rect.set_size(9, 9);
		gv_global::debug_draw.get()->draw_rect(rect, gv_color::GREEN_D(), 0.3f);
		return true;
	}

	gv_bool plot2(const gvt_vector_generic< gv_int, 2 >& v) const
	{
		return plot(v[0], v[1]);
	}

	bool do_ray_trace(float dt)
	{
		gv_string_tmp title = "do_ray_trace ,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(100, 120), gv_color::RED());
		float step = 2.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.1f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.1f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		//gv_global::debug_draw.get()->draw_rect  (m_rect,gv_color::BLUE(), 0.9f);
		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 300);
		gv_vector2 v0 = line_seg.start_p.v2 / 10.f;
		gv_vector2 v1 = line_seg.end_p.v2 / 10.f;
		int nb_traced = gvt_trace_line2d(v0, v1, boost::bind(&gv_unit_test_render_geom_2d::plot, this, _1, _2));
		gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::RED());

		title = "nb point traced ";
		title << nb_traced;
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(20, 50), gv_color::RED());
		return true;
	}

	bool do_ray_trace_dda(float dt)
	{
		gv_string_tmp title = "do_ray_trace DDA ,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(100, 120), gv_color::RED());
		float step = 2.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.1f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.1f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		//gv_global::debug_draw.get()->draw_rect  (m_rect,gv_color::BLUE(), 0.9f);
		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 300);
		gv_vector2 v0 = line_seg.start_p.v2 / 10.f;
		gv_vector2 v1 = line_seg.end_p.v2 / 10.f;
		int nb_traced = gvt_trace_line_dda(v0.v, v1.v, boost::bind(&gv_unit_test_render_geom_2d::plot2, this, _1));
		gv_global::debug_draw.get()->draw_arrow(line_seg, gv_color::RED());

		title = "nb point traced ";
		title << nb_traced;
		gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(20, 50), gv_color::RED());
		return true;
	}

	bool do_line_circle_test(float dt)
	{
		gv_string_tmp s = "do_line_circle_test,arrow to move , a,s to rotate ====>>";
		gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(100, 120), gv_color::RED());

		float step = 20.0f * dt;
		if (gv_global::input->is_key_down(e_key_up))
		{
			m_line.origin_p.y -= step;
		}
		if (gv_global::input->is_key_down(e_key_down))
		{
			m_line.origin_p.y += step;
		}
		if (gv_global::input->is_key_down(e_key_left))
		{
			m_line.origin_p.x -= step;
		}
		if (gv_global::input->is_key_down(e_key_right))
		{
			m_line.origin_p.x += step;
		}
		if (gv_global::input->is_key_down(e_key_a))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(-0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		if (gv_global::input->is_key_down(e_key_s))
		{
			gv_matrix44 mat;
			mat = gv_matrix44::get_rotation_by_z(0.3f * dt);
			m_line.dir_v = mat.mul_by(m_line.dir_v);
		}
		gv_global::debug_draw.get()->draw_circle(m_circle, gv_color::BLUE(), 0.9f);

		gv_line_segment line_seg(m_line.origin_p, m_line.origin_p + m_line.dir_v * 100);
		float ss, tt;
		bool hit = gvt_geom< gv_float >::intersect_ray_circle(m_line, m_circle, ss, tt);
		if (!hit)
		{
			gv_global::debug_draw.get()->draw_line(line_seg.start_p, line_seg.end_p, gv_color::GREY(), gv_color::GREY());
		}
		else
		{
			gv_global::debug_draw.get()->draw_line(line_seg.start_p, line_seg.end_p, gv_color::GREY(), gv_color::GREY());
			gv_vector3 v = m_line.get_point_on_line(ss);
			gv_rect r;
			r.set_size(5, 5);
			r.move_center_to(v.v2);
			gv_global::debug_draw.get()->draw_rect(r, gv_color::RED(), 0.1f);
			v = m_line.get_point_on_line(tt);
			r.move_center_to(v.v2);
			gv_global::debug_draw.get()->draw_rect(r, gv_color::RED(), 0.1f);
		}
		return true;
	}

	virtual void render()
	{
		gv_double cu = gv_global::time->get_sec_from_start();
		m_delta_time = gvt_clamp(cu - m_last_tick_time, 0.01, 0.1);
		m_last_tick_time = cu;
		static bool last_key_down = false;
		switch (m_current_test)
		{
		case geom_test_rect_clip_line:
			do_line_rect_test((gv_float)m_delta_time);
			break;
		case geom_test_ray_rect_intersect:
			do_ray_rect_test((gv_float)m_delta_time);
			break;
		case geom_test_ray_rect_intersect_normal:
			do_ray_rect_test_with_normal((gv_float)m_delta_time);
			break;
		case geom_test_ray_trace:
			do_ray_trace((gv_float)m_delta_time);
			break;
		case geom_test_ray_trace_dda:
			do_ray_trace_dda((gv_float)m_delta_time);
			break;
		case geom_test_ray_circle_intersect:
			do_line_circle_test((gv_float)m_delta_time);
			break;
		}
		if (!last_key_down && gv_global::input->is_key_down(e_key_space))
		{
			m_current_test = m_current_test + 1;
		}
		last_key_down = gv_global::input->is_key_down(e_key_space);
		tick_count++;
		if (gv_global::input->is_key_down(e_key_return) || m_current_test == geom_test_max)
		{
			tick_count = m_max_frame;
		}
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