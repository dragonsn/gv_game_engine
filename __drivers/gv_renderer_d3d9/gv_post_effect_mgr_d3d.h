namespace gv
{

class gv_post_effect_mgr_d3d_imp;
class gv_post_effect_mgr_d3d : public gv_refable
{
public:
	gv_post_effect_mgr_d3d();
	~gv_post_effect_mgr_d3d();
	//===========================================================================================
	void on_init();
	void on_destroy();
	void on_device_lost();
	void on_device_reset();
	//===========================================================================================
	void render(gv_float dt, gv_recti* rect = NULL);
	void add_post_effect(gv_com_graphic* effect);
	void remove_post_effect(gv_com_graphic* effect);
	void enable_post_effect(const gv_id& effect_name, gv_bool e);
	void set_post_effect_param(const gv_id& effect_name, const gv_id& id,
							   const gv_vector4& param);
	gv_com_graphic* get_post_effect(const gv_id& name);

protected:
	gvt_ref_ptr< gv_post_effect_mgr_d3d_imp > m_impl;
};
}