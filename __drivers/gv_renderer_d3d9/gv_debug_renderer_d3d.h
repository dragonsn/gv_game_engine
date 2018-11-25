#pragma once

namespace gv
{
class gv_debug_renderer_d3d : public gvi_debug_renderer
{
public:
	gv_debug_renderer_d3d();
	virtual ~gv_debug_renderer_d3d();
	virtual void init_vb(const int max_triangle = 100000,
						 const int max_line = 100000);
	virtual void create_window(const gv_string& name, const gv_string& title,
							   const gv_vector2i& pos, const gv_vector2i& size,
							   bool init_device);
	virtual void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
							   const gv_vector3& v2, const gv_color& color0,
							   const gv_color& color1, const gv_color& color2);
	virtual void draw_line(const gv_vector3& v0, const gv_vector3& v1,
						   const gv_color& color0, const gv_color& color1);
	virtual void draw_line_3d(const gv_vector3& v0, const gv_vector3& v1,
							  const gv_color& color0, const gv_color& color1);
	virtual void draw_tex_triangle(const gv_vector3 v[3], const gv_vector2 uv[3],
								   const gv_color color[3]);
	virtual void draw_tex_triangle_3d(const gv_vector3 v[3],
									  const gv_vector2 uv[3],
									  const gv_color color[3]);
	virtual void draw_string(const char* string, const gv_vector2i& screen_pos,
							 const gv_color& color);
	virtual void start_test(gv_unit_test_with_renderer* test);
	virtual void render();
	virtual gv_vector2i get_window_size();
	virtual void capture_mouse();
	virtual void release_mouse();
	virtual void do_synchronization();
	virtual void set_debug_texture(class gv_texture*,
								   bool use_alpha_blending = false,
								   bool for_3d = false);
	//=====================================================================================
	bool init_d3d(gv_int_ptr handle);
	void uninit_d3d();
	void init_from_existed_window(gv_int_ptr handle);
	bool do_render_3d_scene(float, class gv_renderer*);
	bool do_render_debug();
	//=====================================================================================
	static gv_debug_renderer_d3d* static_get();

protected:
	class gv_debug_renderer_d3d_imp* m_impl;
};
}