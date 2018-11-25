#pragma once
namespace gv
{
class gv_debug_renderer_gp_imp;
class gv_debug_renderer_gameplay3d : public gvi_debug_renderer
{

public:
	gv_debug_renderer_gameplay3d();
	~gv_debug_renderer_gameplay3d();

public:
	virtual void init_vb(const int max_triangle = 100000,
						 const int max_line = 100000);
	virtual void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
							   const gv_vector3& v2, const gv_color& color0,
							   const gv_color& color1, const gv_color& color2);
	virtual void draw_triangle_3d(const gv_vector3& v0, const gv_vector3& v1,
								  const gv_vector3& v2, const gv_color& color0,
								  const gv_color& color1, const gv_color& color2);
	virtual void draw_line(const gv_vector3& v0, const gv_vector3& v1,
						   const gv_color& color0, const gv_color& color1);
	virtual void draw_line_3d(const gv_vector3& v0, const gv_vector3& v1,
							  const gv_color& color0, const gv_color& color1);
	virtual void set_debug_texture(class gv_texture*,
								   bool use_alpha_blending = false,
								   bool for_3d = false);
	virtual void draw_tex_triangle(const gv_vector3 v[3], const gv_vector2 uv[3],
								   const gv_color color[3]);
	virtual void draw_tex_triangle_3d(const gv_vector3 v[3],
									  const gv_vector2 uv[3],
									  const gv_color color[3]);
	virtual void draw_string(const char* string, const gv_vector2i& screen_pos,
							 const gv_color& color);
	virtual void render();
	virtual void capture_mouse();
	virtual void release_mouse();
	virtual gv_vector2i get_window_size();
	virtual void do_synchronization();
	//=====================================================================================
	//	gameplay engine
	bool initialize_gameplay3d();
	void finalize_gameplay3d();
	void set_view_projection_matrix_3d(const Matrix& matrix);
	const Matrix& get_view_projection_matrix_3d();

public:
	static gv_debug_renderer_gameplay3d* static_get();
	Font* get_debug_font();
	MeshBatch* get_line_batch();
	int get_line_vertex(void*& pv);

private:
	gvt_ptr< gv_debug_renderer_gp_imp > m_impl;
	static gv_debug_renderer_gameplay3d* s_debug_draw_gameplay3d;
};
}