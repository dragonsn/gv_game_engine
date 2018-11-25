#pragma once
namespace gv
{
class gv_effect;

enum gve_render_driver_type
{
	e_render_driver_d3d9 = 0,
	e_render_driver_d3d11,
	e_render_driver_d3d12,
	e_render_driver_ogl = 10,
	e_render_driver_ogles,
	e_render_driver_3nd_0 = 20,
	e_render_driver_3nd_1 = 21,
	e_render_driver_3nd_2 = 22,
};

class gv_render_option
{
public:
	gv_render_option();
	~gv_render_option();

	gv_vector2i get_screen_size();
	gv_bool is_windowed();

public:
	gv_int m_render_driver_type;
	gv_int m_screen_width;
	gv_int m_screen_height;
	gv_int m_screen_x;
	gv_int m_screen_y;
	gv_int m_max_debug_line;
	gv_int m_max_debug_tri;
	gv_bool m_is_windowed;
	gv_bool m_no_debug_draw;
	gv_bool m_is_wireframe_mode;
	//=render================================
	gv_int m_msaa_times;
	gv_bool m_no_post_effect;
	gv_bool m_no_shadow;
	gv_bool m_no_bloom;
	gv_bool m_no_color_adjust;
	gv_bool m_no_god_ray;
	gv_bool m_no_motion_blur;
	gv_bool m_enable_debug_texture;

	gv_float m_bloom_scale;
	gv_vector4 m_color_scale;
	gv_vector4 m_color_adjustment;
	gv_color m_clear_color;

	gvt_ref_ptr< gv_effect > m_engine_effect;
	gvt_ref_ptr< gv_effect > m_depth_only_effect;
	gvt_ref_ptr< gv_effect > m_shadow_pass_effect;
	gvt_ref_ptr< gv_effect > m_shadow_post_effect;
};

namespace gv_global
{
extern gv_render_option rnd_opt;
}
}