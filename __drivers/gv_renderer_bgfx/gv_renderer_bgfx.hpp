
#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"

int bgfx_main(int _argc, const char* const* _argv);
void start_bgfx()
{
	char* def_arg[] = {"DFDF"};
	bgfx_main(1, {def_arg});
}

namespace gv
{

	class gv_renderer_bgfx_data
	{
	public:
		gv_renderer_bgfx_data();
		~gv_renderer_bgfx_data();
		gv_thread m_bgfx_thread;
	private:
	};
	gv_renderer_bgfx * s_gv_renderer_bgfx = nullptr; 

	gv_renderer_bgfx_data::gv_renderer_bgfx_data(){
		
	}

	gv_renderer_bgfx_data::~gv_renderer_bgfx_data(){
	}

	gv_renderer_bgfx::gv_renderer_bgfx()
	{
		s_gv_renderer_bgfx = this;
		GVM_SET_CLASS(gv_renderer_bgfx);
		m_impl = new gv_renderer_bgfx_data;
		m_impl->m_bgfx_thread.start(start_bgfx);
	}

	gv_renderer_bgfx::~gv_renderer_bgfx()
	{
	}
	gv_renderer_bgfx * gv_renderer_bgfx::static_get()
	{
		return s_gv_renderer_bgfx;
	}
	//===========================================================================================
	bool gv_renderer_bgfx::tick(gv_float dt)
	{
		uint32_t ii = 0;
		int32_t current = 0;
		bool ret = false; 
		if (!m_current_app)
		{
			for (entry::AppI* app = entry::getFirstApp(); NULL != app; app = app->getNext())
			{
				gv_string s = app->getName();
				gv_bgfx_app *pp = (gv_bgfx_app *)app;
				if (pp->m_inited.current()) {
					m_current_app = pp;
				}
			}
		}
		ret=super::tick(dt);
		if (m_current_app) {
			m_current_app->m_start_tick_event.set();
			m_current_app->m_end_tick_event.wait();
		}
		return ret;
	};

	gv_int gv_renderer_bgfx::on_event(gv_object_event* pevent)
	{
		switch (pevent->m_id)
		{
			case gv_object_event_id_render_init:
			{
				gv_object_event_render_init* pe =
					gvt_cast<gv_object_event_render_init>(pevent);
			}
			break;

			case gv_object_event_id_render_uninit:
			{
			}
			break;
		}
		if (m_current_app)
		{
			m_current_app->on_event(pevent);
		}
		return 1;
	}

	GVM_IMP_CLASS(gv_renderer_bgfx, gv_renderer)
	GVM_END_CLASS
}
