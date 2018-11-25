namespace gv
{

void gvi_debug_renderer::draw_string(const char* string, gv_float x, gv_float y,
									 const gv_color& color)
{
	draw_string(string, gv_vector2i((int)x, (int)y), color);
};

void gvi_debug_renderer::draw_shadowed_string(const char* string, gv_float x,
											  gv_float y, const gv_color& color,
											  gv_int height)
{
	gv_color shadow = color;
	shadow.v /= 2;
	for (int i = 0; i < height; i++)
	{
		draw_string(string, gv_vector2i((int)x + 1 + i, (int)y + 1 + i), shadow);
	}
	draw_string(string, gv_vector2i((int)x, (int)y), color);
}

void gvi_debug_renderer::draw_quad(const gv_vector3& v0, const gv_vector3& v1,
								   const gv_vector3& v2, const gv_vector3& v3,
								   const gv_color& color0,
								   const gv_color& color1,
								   const gv_color& color2,
								   const gv_color& color3)
{
	this->draw_triangle(v0, v1, v2, color0, color1, color2);
	this->draw_triangle(v2, v1, v3, color2, color1, color3);
};

void gvi_debug_renderer::draw_quad(const gv_vector3& v0, const gv_vector3& v1,
								   const gv_vector3& v2, const gv_vector3& v3,
								   const gv_color& color)
{
	this->draw_quad(v0, v1, v2, v3, color, color, color, color);
};

void gvi_debug_renderer::draw_triangle(const gv_vector3& v0,
									   const gv_vector3& v1,
									   const gv_vector3& v2,
									   const gv_color& color)
{
	this->draw_triangle(v0, v1, v2, color, color, color);
};

void gvi_debug_renderer::draw_circle(const gv_circle& circle,
									 const gv_color& color, float depth)
{
	static int nb_seg = 64;
	float angle = 0;
	float delta_angle = gv_float_trait::pi() * 2.0f / (float)nb_seg;
	for (int i = 0; i < nb_seg; i++)
	{
		gv_vector2 v0 = gv_math::sincos(angle);
		v0 *= circle.r;
		v0 += circle.get_center();
		gv_vector2 v1 = gv_math::sincos(angle + delta_angle);
		v1 *= circle.r;
		v1 += circle.get_center();
		this->draw_triangle(gv_vector3(circle.get_center(), depth),
							gv_vector3(v0, depth), gv_vector3(v1, depth), color,
							color, color);
		angle += delta_angle;
	}
}

void gvi_debug_renderer::draw_rect(const gv_rect& rect, const gv_color& color,
								   gv_float depth)
{
	gv_vector3 a = gv_vector3(rect.left, rect.top, depth);
	gv_vector3 ex = gv_vector3(rect.width(), 0, 0);
	gv_vector3 ey = gv_vector3(0, rect.height(), 0);
	draw_quad(a, a + ex, a + ey, a + ex + ey, color);
}

void gvi_debug_renderer::draw_frame(const gv_rect& rect,
									const gv_color& background_color,
									const gv_color& frame_color,
									const gv_color& frame_edge_color,
									gv_int width, gv_float depth,
									bool fill_back_ground)
{
	gv_rect r = rect;
	if (fill_back_ground)
		draw_rect(rect, background_color, depth);
	gv_colorf c1 = frame_edge_color;
	gv_colorf c0 = frame_color;
	r.extend(gv_float(width));
	for (int i = 0; i <= width * 2; i++)
	{
		gv_colorf c;
		c.v = gvt_lerp(c0.v, c1.v, (float)gvt_abs(i - width) / (float)width);
		gv_color cc = c;
		draw_rect(r, cc, depth);
		r.extend(-1.f);
	}
}

void gvi_debug_renderer::draw_arrow(const gv_line_segment& line,
									const gv_color& color)
{
	this->draw_line(line.start_p, line.end_p, color, color);
	gv_vector3 v = line.start_p - line.end_p;
	v.normalize();
	gv_matrix44 mat = gv_matrix44::get_rotation_by_z(0.4f);
	gv_vector3 v1 = mat.mul_by(v);
	mat = gv_matrix44::get_rotation_by_z(-0.4f);
	gv_vector3 v2 = mat.mul_by(v);
	v1 *= 8.f;
	v2 *= 8.f;
	this->draw_line(line.end_p, line.end_p + v1, color, color);
	this->draw_line(line.end_p, line.end_p + v2, color, color);
}

void gvi_debug_renderer::draw_heart(const gv_vector3 pos, gv_float size,
									const gv_color& color)
{
	gv_vector3 v[8] = {
		gv_vector3(0.f, 0.5f, 0.f), gv_vector3(-0.5f, -0.2f, 0.f),
		gv_vector3(-0.35f, -0.5f, 0.f), gv_vector3(-0.15f, -0.5f, 0.f),
		gv_vector3(0.f, -0.25f, 0.f), gv_vector3(0.15f, -0.5f, 0.f),
		gv_vector3(0.35f, -0.5f, 0.f), gv_vector3(0.5f, -0.2f, 0.f),
	};
	for (int i = 0; i < gvt_array_length(v); i++)
		v[i] = v[i] * size + pos;
	this->draw_triangle(v[0], v[1], v[2], color, color, color);
	this->draw_triangle(v[0], v[2], v[3], color, color, color);
	this->draw_triangle(v[0], v[3], v[4], color, color, color);
	this->draw_triangle(v[0], v[4], v[5], color, color, color);
	this->draw_triangle(v[0], v[5], v[6], color, color, color);
	this->draw_triangle(v[0], v[6], v[7], color, color, color);
}

void gvi_debug_renderer::draw_jiong(const gv_vector3 pos, gv_float size,
									const gv_color& color0,
									const gv_color& color1)
{
	gv_rect r(-0.5f, -0.5f, 0.5f, 0.5f);
	r.scale(size);
	r.move(pos.v2);
	this->draw_rect(r, color0, pos.z);

	gv_rect r_mouth(-0.3f, 0.1f, 0.3f, 0.5f);
	r_mouth.scale(size);
	r_mouth.move(pos.v2);
	this->draw_rect(r_mouth, color1, pos.z - 0.1f);

	gv_vector3 v[8] = {
		gv_vector3(-0.1f, -0.4f, 0.f), gv_vector3(-0.45f, -0.1f, 0.f),
		gv_vector3(-0.35f, -0.1f, 0.f), gv_vector3(0.1f, -0.4f, 0.f),
		gv_vector3(0.45f, -0.1f, 0.f), gv_vector3(0.35f, -0.1f, 0.f),
	};

	for (int i = 0; i < gvt_array_length(v); i++)
		v[i] = v[i] * size + pos;
	this->draw_triangle(v[0], v[2], v[1], color1, color1, color1);
	this->draw_triangle(v[3], v[4], v[5], color1, color1, color1);
}

void gvi_debug_renderer::draw_box_3d(const gv_box& box, const gv_color c)
{

	gv_vector3 xdir = gv_vector3(box.get_xdelta(), 0, 0);
	gv_vector3 ydir = gv_vector3(0, box.get_ydelta(), 0);
	gv_vector3 zdir = gv_vector3(0, 0, box.get_zdelta());

	gv_vector3 v[8] = {
		box.min_p,
		box.min_p + xdir,
		box.min_p + xdir + zdir,
		box.min_p + zdir,

		box.min_p + ydir,
		box.min_p + xdir + ydir,
		box.min_p + xdir + ydir + zdir,
		box.min_p + ydir + zdir,
	};
	this->draw_line_3d(v[0], v[1], c, c);
	this->draw_line_3d(v[1], v[2], c, c);
	this->draw_line_3d(v[2], v[3], c, c);
	this->draw_line_3d(v[3], v[0], c, c);

	this->draw_line_3d(v[4], v[5], c, c);
	this->draw_line_3d(v[5], v[6], c, c);
	this->draw_line_3d(v[6], v[7], c, c);
	this->draw_line_3d(v[7], v[4], c, c);

	this->draw_line_3d(v[0], v[4], c, c);
	this->draw_line_3d(v[1], v[5], c, c);
	this->draw_line_3d(v[2], v[6], c, c);
	this->draw_line_3d(v[3], v[7], c, c);
}

void gvi_debug_renderer::draw_grid_3d(const gv_vector3& o, float w, float h,
									  float dx, float dy)
{
	float x, y;
	int count = 0; // avoid too big grid.
	for (y = 0; y < h + dy; y += dy)
	{
		this->draw_line_3d(gv_vector3(0, 0, y) + o, gv_vector3(w, 0, y) + o,
						   gv_color::RED_B(), gv_color::RED());
		if (count++ > 1000)
			break;
	}
	count = 0;
	for (x = 0; x < w + dx; x += dx)
	{
		this->draw_line_3d(gv_vector3(x, 0, 0) + o, gv_vector3(x, 0, h) + o,
						   gv_color::BLUE_B(), gv_color::BLUE());
		if (count++ > 1000)
			break;
	}
	return;
}

void gvi_debug_renderer::draw_axis(const gv_matrix44& tm, float l)
{
	draw_line_3d(tm.get_trans(), tm.get_trans() + tm.axis_x * 1, gv_color::RED(),
				 gv_color::RED_B());
	draw_line_3d(tm.get_trans(), tm.get_trans() + tm.axis_y * 1,
				 gv_color::GREEN(), gv_color::GREEN());
	draw_line_3d(tm.get_trans(), tm.get_trans() + tm.axis_z * 1, gv_color::BLUE(),
				 gv_color::BLUE());
};

void gvi_debug_renderer::draw_axis_with_ani(const gv_vector3& o, float l,
											float w)
{
	// float pos= sinf(gv_global::time->get_sec_from_start() *0.1f)*2*l-l;
	float pos = ((float)gv_global::time->get_sec_from_start() * 0.1f);
	pos = pos - floorf(pos);
	pos = pos * 2 * l - l;
	gv_vector3 offset_x(w, 0, 0);
	gv_vector3 offset_y(0, w, 0);
	gv_vector3 offset_z(0, 0, w);

	this->draw_line_3d(gv_vector3(-l, 0, 0) + o,
					   gv_vector3(pos - w * 4, 0, 0) + o, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos - w * 4, 0, 0) + o,
					   gv_vector3(pos, 0, 0) + o, gv_color::RED(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos + w * 4, 0, 0) + o,
					   gv_vector3(pos, 0, 0) + o + offset_y, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos + w * 4, 0, 0) + o,
					   gv_vector3(pos, 0, 0) + o - offset_y, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos + w * 4, 0, 0) + o,
					   gv_vector3(pos, 0, 0) + o + offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos + w * 4, 0, 0) + o,
					   gv_vector3(pos, 0, 0) + o - offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos, 0, 0) + o + offset_y,
					   gv_vector3(pos, 0, 0) + o + offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos, 0, 0) + o - offset_y,
					   gv_vector3(pos, 0, 0) + o + offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos, 0, 0) + o + offset_y,
					   gv_vector3(pos, 0, 0) + o - offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(pos, 0, 0) + o - offset_y,
					   gv_vector3(pos, 0, 0) + o - offset_z, gv_color::RED_D(),
					   gv_color::RED());
	this->draw_line_3d(gv_vector3(l, 0, 0) + o, gv_vector3(pos - w * 4, 0, 0) + o,
					   gv_color::RED_D(), gv_color::RED());

	this->draw_line_3d(gv_vector3(0, 0, -l) + o,
					   gv_vector3(0, 0, pos - w * 4) + o, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos - w * 4) + o,
					   gv_vector3(0, 0, pos) + o, gv_color::BLUE(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos + w * 4) + o,
					   gv_vector3(0, 0, pos) + o + offset_y, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos + w * 4) + o,
					   gv_vector3(0, 0, pos) + o - offset_y, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos + w * 4) + o,
					   gv_vector3(0, 0, pos) + o + offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos + w * 4) + o,
					   gv_vector3(0, 0, pos) + o - offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos) + o + offset_y,
					   gv_vector3(0, 0, pos) + o + offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos) + o - offset_y,
					   gv_vector3(0, 0, pos) + o + offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos) + o + offset_y,
					   gv_vector3(0, 0, pos) + o - offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos) + o - offset_y,
					   gv_vector3(0, 0, pos) + o - offset_x, gv_color::BLUE_D(),
					   gv_color::BLUE());
	this->draw_line_3d(gv_vector3(0, 0, pos + w * 4) + o, gv_vector3(0, 0, l) + o,
					   gv_color::BLUE_D(), gv_color::BLUE());

	this->draw_line_3d(gv_vector3(0, -l, 0) + o,
					   gv_vector3(0, pos - w * 4, 0) + o, gv_color::GREEN_D(),
					   gv_color::GREEN_B());
	this->draw_line_3d(gv_vector3(0, pos - w * 4, 0) + o,
					   gv_vector3(0, pos, 0) + o, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos + w * 4, 0) + o,
					   gv_vector3(0, pos, 0) + o + offset_z, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos + w * 4, 0) + o,
					   gv_vector3(0, pos, 0) + o - offset_z, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos + w * 4, 0) + o,
					   gv_vector3(0, pos, 0) + o + offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos + w * 4, 0) + o,
					   gv_vector3(0, pos, 0) + o - offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos, 0) + o + offset_z,
					   gv_vector3(0, pos, 0) + o + offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos, 0) + o - offset_z,
					   gv_vector3(0, pos, 0) + o + offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos, 0) + o + offset_z,
					   gv_vector3(0, pos, 0) + o - offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos, 0) + o - offset_z,
					   gv_vector3(0, pos, 0) + o - offset_x, gv_color::GREEN_D(),
					   gv_color::GREEN());
	this->draw_line_3d(gv_vector3(0, pos + w * 4, 0) + o, gv_vector3(0, l, 0) + o,
					   gv_color::GREEN_D(), gv_color::GREEN_B());
}

void gvi_debug_renderer::draw_sphere(const gv_sphere& sphere, const gv_color& c,
									 int segment)
{
	gv_float angle_a, angle_b;
	gv_float delta = gv_float_trait::pi() * 2.0f / (gv_float)segment;
	gv_float r = sphere.r;
	// to do: opt
	for (int i = 0; i < segment; i++)
		for (int j = 0; j < segment; j++)
		{
			angle_a = i * delta;
			angle_b = j * delta;
			gv_vector3 a(cosf(angle_a) * cosf(angle_b), cosf(angle_a) * sinf(angle_b),
						 sinf(angle_a));
			angle_a = i * delta;
			angle_b = (j + 1) * delta;
			gv_vector3 b(cosf(angle_a) * cosf(angle_b), cosf(angle_a) * sinf(angle_b),
						 sinf(angle_a));
			this->draw_line_3d(a * r + sphere.pos, b * r + sphere.pos, c, c);
		}
	//
	for (int i = 0; i < segment; i++)
		for (int j = 0; j < segment; j++)
		{
			angle_b = i * delta;
			angle_a = j * delta;
			gv_vector3 a(cosf(angle_a) * cosf(angle_b), cosf(angle_a) * sinf(angle_b),
						 sinf(angle_a));
			angle_b = i * delta;
			angle_a = (j + 1) * delta;
			gv_vector3 b(cosf(angle_a) * cosf(angle_b), cosf(angle_a) * sinf(angle_b),
						 sinf(angle_a));
			this->draw_line_3d(a * r + sphere.pos, b * r + sphere.pos, c, c);
		}
}

void gvi_debug_renderer::draw_rect_ring(const gv_rect& r, const gv_color& color,
										gv_float width, gv_float depth)
{
	gv_rect rr = r;
	rr.set_width(width);
	draw_rect(rr, color, depth);
	rr.move(r.width() - width, 0);
	draw_rect(rr, color, depth);
	rr = r;
	rr.set_height(width);
	draw_rect(rr, color, depth);
	rr.move(0, r.height() - width);
	draw_rect(rr, color, depth);
};
}
