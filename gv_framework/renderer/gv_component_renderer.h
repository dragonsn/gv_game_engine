#pragma once
namespace gv
{
class gv_component;
class gv_com_graphic;
class gv_component_renderer : public gv_object
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_component_renderer, gv_object)
	virtual gv_int get_priority()
	{
		return 0;
	}
	virtual bool can_render_component(gv_component*)
	{
		return false;
	}
	virtual void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch){};
	virtual void render_component(gv_component* com)
	{
	}
	virtual void precache_component(gv_com_graphic* com, gv_int pass)
	{
	}
	virtual void post_render_pass(gv_int pass){};
	virtual void render_priority_group(gv_uint start, gv_uint end){};
	virtual void add_a_instance(gv_static_mesh* mesh, gv_int seg,
								gv_com_graphic* com, gv_material* material,
								gv_effect* effect, gv_int pass = 0){};
	virtual void before_post_process()
	{
	}
	virtual void after_post_process()
	{
	}
};
};