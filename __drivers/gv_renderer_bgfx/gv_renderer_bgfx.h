#pragma once

class gv_bgfx_app;
namespace gv
{

	// gv_renderer_bgfx static functions interface
	class gv_renderer_bgfx_data;
	class gv_renderer_bgfx : public gv_renderer
	{
	public:
		GVM_DCL_CLASS(gv_renderer_bgfx, gv_renderer);
		gv_renderer_bgfx();
		~gv_renderer_bgfx();
		static gv_renderer_bgfx * static_get();
		bool tick(gv_float dt);
		gv_int on_event(gv_object_event* pevent);
		virtual bool precache_resource(class gv_resource* resource);
		virtual bool precache_static_mesh(class gv_static_mesh*);
		virtual bool precache_skeletal_mesh(class gv_skeletal_mesh*);
		virtual bool precache_texture(class gv_texture*);
		virtual bool precache_index_buffer(class gv_index_buffer*);
		virtual bool precache_vertex_buffer(class gv_vertex_buffer*);
		virtual bool precache_effect(class gv_effect*);
		virtual bool precache_shader(class gv_shader*);
		virtual bool precache_material(class gv_material*);
	protected:
		gvt_ptr< gv_renderer_bgfx_data > m_impl;
		gvt_ptr<gv_bgfx_app > m_current_app;
	};

	

}