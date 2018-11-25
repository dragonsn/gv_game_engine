// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <iostream>
#if WIN32
#include <tchar.h>
#endif
#include "gv_base.h"
#include "gv_unit_test_utility.h"
namespace gv
{
	class sub_test_timer :public  gvt_scope_test_timer<gv_log> 
	{
	public:
		inline sub_test_timer(const char * _name ):gvt_scope_test_timer<gv_log >(*gv_global::log->get_log(),_name)
		{
			this->output_log;
		}
	};

	inline gv_file_manager  * test_get_fm()
	{
		return gv_global::fm.get();
	}
#if defined(WIN32)
	inline std::ostream & test_log() 
	{
		return std::cout;
	}
#else
	inline gv_log & test_log() 
	{
		return *gv_global::log->get_log();
	}
#endif
}
