#pragma  once 
using namespace gv; 
#include "gv_renderer_bgfx.h"

class gv_bgfx_app : public entry::AppI
{
	friend class gv::gv_renderer_bgfx;
public:
	gv_bgfx_app(const char* _name, const char* _description); 
	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override;
	virtual int shutdown() override;
	virtual void tick();
public:
	virtual gv_int	on_event(gv_object_event* pevent);
protected:
	bool update() override;
	void pre_tick();
	void post_tick();
	void add_renderable(gv_component* pcomponent);
	void remove_renderable(gv_component* pcomponent);

	bool precache(gv_com_static_mesh* mesh);
	bool precache(gv_com_skeletal_mesh* mesh);
	bool precache(class gv_com_terrain_roam* terrain);
	void set_camera();

	bool set_material(gv_com_graphic* com, gv_material * mat);
	//bool update_materials(gv_com_graphic* com);
	//bool update_material_parameter(gv_material* mat_gv);
	bool render_renderable(gv_component * pcomponent);

	entry::MouseState m_mouseState;
	gv_atomic_count m_inited; 
	gv_event m_end_tick_event;
	gv_event m_start_tick_event;
	gv_float m_delta_time; 
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	gvt_ptr< gv_com_camera> m_main_camera;
	gvt_array< gvt_ref_ptr< gv_component > > m_renderables;
};

