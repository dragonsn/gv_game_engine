namespace gv
{
namespace va_helper
{
struct gv_event_processor_desc
{
	gv_bool m_is_synchronization;
	gv_bool m_is_autonomous;
	gv_id m_channel_id;
	gv_id m_processor_cls;
};

struct gv_engine_option
{
	gvt_array< gv_event_processor_desc > m_processors;
	gv_text m_root_path;
	gv_id m_game_cls;

	gv_int m_msaa_times;
	gv_bool m_no_shadow;
	gv_bool m_no_post_effect;
	gv_bool m_no_bloom;
	gv_bool m_no_color_adjust;
	gv_bool m_no_god_ray;
	gv_bool m_no_motion_blur;

	gv_float m_bloom_scale;
	gv_vector4 m_color_scale;
	gv_vector4 m_color_adjustment;
	gv_color m_clear_color;
};
}
}