#pragma once
using namespace gameplay;
using namespace gv;

namespace gv
{
class gv_hook_gameplay3d;
class gv_renderer_gameplay3d : public gv_renderer
{
public:
	GVM_DCL_CLASS(gv_renderer_gameplay3d, gv_event_processor);
	gv_renderer_gameplay3d();
	~gv_renderer_gameplay3d();

public:
	virtual bool tick(gv_float dt);
	virtual bool do_synchronization();
	virtual gv_int on_event(gv_object_event* pevent);

protected:
	non_virtual void init_gameplay3d();
	non_virtual void destroy_gameplay3d();

protected: // object in  gameplay
	gvt_ptr< Platform > m_platform;
	gvt_ptr< gv_hook_gameplay3d > m_hook;
	gv_world_light_info m_light_info;
	void* m_window_handle;

protected:
	gvt_object_type_list< gv_com_graphic > m_components;
	gvt_array_cached< gv_com_graphic*, 2048 > m_visible_components;
	gvt_ref_ptr< gv_com_camera > m_main_camera;
	gvt_ref_ptr< gv_texture > m_default_texture;
	gv_int m_current_render_frame;
	gvt_ref_ptr< gv_com_camera > m_main_camera_copy;
	gv_ulong m_visible_test_tag;
	gvt_memory_stack< 65536 > m_mem_stack;
	gvt_ref_ptr< gv_entity > m_camera_entity;
	bool m_game_already_inited;
};
}
