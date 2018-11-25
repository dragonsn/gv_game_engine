#pragma once

namespace gv
{
class gv_com_x_mesh_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_com_x_mesh_renderer, gv_component_renderer);
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_x_mesh::static_class()))
			return true;
		return false;
	}
	virtual void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch);
	virtual void render_component(gv_component* com);
	virtual void precache_component(gv_com_graphic* com, gv_int pass);
};
}