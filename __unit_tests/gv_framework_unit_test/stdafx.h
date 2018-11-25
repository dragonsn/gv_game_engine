// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <iostream>
#include <tchar.h>
#define  WITH_CEGUI 0
#include "gv_framework.h"
#include "gv_framework_events.h"
#include "gv_framework/renderer/gv_debug_renderer.h"
#include "gv_world_rpg2d.h"
#include "gv_com_rpg2d_actor_online.h"
#include "gv_unit_test_utility.h"
#include "gv_framework_test_res.h"


namespace gv
{
	extern gv_sandbox * m_sandbox;
	extern gv_sandbox * get_sandbox();
	class sub_test_timer :public  gvt_scope_test_timer<std::ostream > 
	{
	public:
		inline sub_test_timer(const char * _name ):gvt_scope_test_timer<std::ostream >(std::cout,_name)
		{
			this->output_log;
		}
	};
	inline gvi_debug_renderer * get_debug_draw()
	{
		return gv_global::debug_draw.get();
	}

	void gv_register_test_classes();
	void gv_unregister_test_classes();

	class gv_unit_test_context_guard 
	{
	public:
		gv_unit_test_context_guard(bool with_render=true);
		~gv_unit_test_context_guard();
	}; 

	inline gv_log & test_log() 
	{
		return *gv_global::log->get_log();
	}

	
}



// TODO: reference additional headers your program requires here
