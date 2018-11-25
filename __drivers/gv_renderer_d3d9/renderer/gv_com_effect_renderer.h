#pragma once

namespace gv
{
struct gv_effect_batch_element
{
	gvt_ptr< gv_effect > m_effect;
	gvt_ptr< gv_shader > m_pixel_shader;
	gvt_ptr< gv_shader > m_vertex_shader;
	gvt_ptr< gv_com_graphic > m_graphic_com;
	gv_int m_segment_index;
};

class gv_com_effect_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS(gv_com_effect_renderer, gv_component_renderer);
	gv_com_effect_renderer();
	~gv_com_effect_renderer();
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_static_mesh::static_class()))
			return true;
		if (p->is_a(gv_com_skeletal_mesh::static_class()))
			return true;
		return false;
	}
	virtual void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch);
	virtual void render_component(gv_component* com);
	virtual void precache_component(gv_com_graphic* com, gv_int pass);
	void add_component_to_batch(gv_component* com);
	virtual void post_render_pass(gv_int pass);

protected:
	bool m_batch_enabled;
};
}