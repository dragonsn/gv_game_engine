namespace gv
{
class gv_render_target_mgr_d3d : public gv_refable
{
public:
	friend class gv_renderer_d3d9;
	;
	friend class gv_render_target_mgr_d3d_imp;
	gv_render_target_mgr_d3d();
	~gv_render_target_mgr_d3d();
	//===========================================================================================
	void on_init();
	void on_destroy();
	void on_device_lost();
	void on_device_reset();
	//===========================================================================================
	void begin_render_target(gv_texture* color_buffer, gv_texture* depth_buffer,
							 bool clear = true,
							 gv_color clear_color = gv_color::BLACK());
	void end_render_target(void);
	void lock_render_target(gv_bool lock);
	void resolve_backbuffer(gv_texture*);
	void set_default_render_target();
	//===========================================================================================
	gv_vector2i get_default_color_buffer_size();
	gv_vector2i get_default_depth_buffer_size();
	void precache(gv_effect_renderable_texture* texture);
	gv_texture* create_color_texture(const gv_id& id, gv_vector2i size,
									 gve_pixel_format format,
									 gv_bool clear = true);
	gv_texture* create_depth_texture(const gv_id& id, gv_vector2i size,
									 gve_pixel_format format,
									 gv_bool clear = true);
	//===========================================================================================
	gv_vector2i get_surface_size();

protected:
	gvt_ref_ptr< gv_render_target_mgr_d3d_imp > m_impl;
};
}