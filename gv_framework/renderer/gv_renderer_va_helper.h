namespace gv
{
namespace va_helper
{
struct gv_effect_renderable_texture
{
	gv_id m_name;
	gv_vector2i m_size;
	gv_colorf m_clear_color;
	gv_text m_file_name;
	gvt_ref_ptr< gv_texture > m_texture;
	gv_int m_format;
	gv_bool m_use_mipmap;
	gv_bool m_use_window_size;
	gv_float m_width_ratio;
	gv_float m_height_ratio;
};

struct gv_effect_render_target
{
	gv_id m_name;
	gv_int m_mip_index;
	gv_bool m_render_to_screen;
	gv_bool m_is_clear_color;
	gv_bool m_is_clear_depth;
	gv_colorf m_clear_color_value;
	gv_float m_clear_depth_value;
};

struct gv_effect_pass
{
	gvt_ptr< gv_effect > m_owner;
	gvt_ref_ptr< gv_effect > m_pass_effect;
	gv_id m_name;
	gv_bool m_enabled;
};
}
}