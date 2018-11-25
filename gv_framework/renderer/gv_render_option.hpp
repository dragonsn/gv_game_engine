namespace gv
{
gv_render_option::gv_render_option()
{
	//(gv_render_option::static_class() );
	gvt_zero(*this);
	m_max_debug_line = 1000;
	m_max_debug_tri = 1000;
	m_screen_width = 800;
	m_screen_height = 600;
	m_is_windowed = true;
	m_msaa_times = 0;
	m_clear_color = gv_color(128, 140, 240, 1);
	m_no_debug_draw = false;
	m_screen_x = m_screen_y = 0;
}

gv_render_option::~gv_render_option(){};

gv_vector2i gv_render_option::get_screen_size()
{
	return gv_vector2i(this->m_screen_width, this->m_screen_height);
};

gv_bool gv_render_option::is_windowed()
{
	return m_is_windowed;
};

GVM_IMP_STRUCT(gv_render_option)
GVM_VAR(gv_int, m_screen_width)
GVM_VAR(gv_int, m_screen_height)
GVM_VAR(gv_bool, m_is_windowed)
GVM_VAR(gv_int, m_msaa_times)
GVM_END_STRUCT
}