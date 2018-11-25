#pragma once

namespace gv
{

class gvi_debug_renderer
{
public:
	gvi_debug_renderer(){};
	virtual ~gvi_debug_renderer(){};
	virtual void init_vb(const int max_triangle = 100000,
						 const int max_line = 100000){};
	virtual void create_window(const gv_string& name, const gv_string& title,
							   const gv_vector2i& pos, const gv_vector2i& size,
							   bool init_device){};
	virtual void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
							   const gv_vector3& v2, const gv_color& color0,
							   const gv_color& color1, const gv_color& color2){};
	virtual void draw_triangle_3d(const gv_vector3& v0, const gv_vector3& v1,
								  const gv_vector3& v2, const gv_color& color0,
								  const gv_color& color1,
								  const gv_color& color2){};
	virtual void draw_line(const gv_vector3& v0, const gv_vector3& v1,
						   const gv_color& color0, const gv_color& color1){};
	virtual void draw_line_3d(const gv_vector3& v0, const gv_vector3& v1,
							  const gv_color& color0, const gv_color& color1){};
	virtual void set_debug_texture(class gv_texture*,
								   bool use_alpha_blending = false,
								   bool for_3d = false){};
	virtual void draw_tex_triangle(const gv_vector3 v[3], const gv_vector2 uv[3],
								   const gv_color color[3]){};
	virtual void draw_tex_triangle_3d(const gv_vector3 v[3],
									  const gv_vector2 uv[3],
									  const gv_color color[3]){};
	virtual void draw_string(const char* string, const gv_vector2i& screen_pos,
							 const gv_color& color){};
	virtual void start_test(class gv_unit_test_with_renderer* test){};
	virtual void render(){};
	virtual void capture_mouse(){};
	virtual void release_mouse(){};
	virtual gv_vector2i get_window_size()
	{
		return gv_vector2i::get_zero_vector();
	};
	virtual float get_window_ratio()
	{
		gv_vector2i s = get_window_size();
		return (gv_float)(s.get_x()) / (gv_float)(s.get_y());
	}
	virtual void do_synchronization()
	{
	}

public:
	// none virtual utility
	inline gv_rect get_window_rect()
	{
		gv_vector2i w_size = get_window_size();
		return gv_rect(0, 0, (gv_float)w_size.x, (gv_float)w_size.y);
	}
	void draw_string(const char* string, gv_float x, gv_float y,
					 const gv_color& color);
	void draw_shadowed_string(const char* string, gv_float x, gv_float y,
							  const gv_color& color, gv_int height = 1);
	void draw_quad(const gv_vector3& v0, const gv_vector3& v1,
				   const gv_vector3& v2, const gv_vector3& v3,
				   const gv_color& color0, const gv_color& color1,
				   const gv_color& color2, const gv_color& color3);
	void draw_quad(const gv_vector3& v0, const gv_vector3& v1,
				   const gv_vector3& v2, const gv_vector3& v3,
				   const gv_color& color);
	void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
					   const gv_vector3& v2, const gv_color& color);
	void draw_circle(const gv_circle& circle, const gv_color& color, float depth);
	void draw_rect(const gv_rect& rect, const gv_color& color, gv_float depth);
	void draw_frame(const gv_rect& rect, const gv_color& background_color,
					const gv_color& frame_color, const gv_color& frame_edge_color,
					gv_int width, gv_float depth = 0.1f,
					bool fill_back_ground = true);
	void draw_arrow(const gv_line_segment& line, const gv_color& color);
	void draw_heart(const gv_vector3 pos, gv_float size,
					const gv_color& color = gv_color::RED());
	void draw_jiong(const gv_vector3 pos, gv_float size,
					const gv_color& color0 = gv_color::GREY_B(),
					const gv_color& color1 = gv_color::BLACK());
	void draw_box_3d(const gv_box& box, const gv_color c);
	void draw_grid_3d(const gv_vector3& o, float w, float h, float dx, float dy);
	void draw_axis_with_ani(const gv_vector3& o, float l, float w);
	void draw_axis(const gv_matrix44& tm, float l = 2.f);
	void draw_sphere(const gv_sphere& sphere, const gv_color& c,
					 int segment = 16);
	void draw_rect_ring(const gv_rect& rect, const gv_color& color,
						gv_float width, gv_float depth = 0.1f);
};

namespace gv_global
{
extern gvt_global< gvi_debug_renderer > debug_draw;
};
}