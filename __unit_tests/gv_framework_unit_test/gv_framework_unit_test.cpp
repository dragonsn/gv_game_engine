// gv_framework_unit_test.cpp : Defines the entry point for the console
// application.
//

#include "stdafx.h"
using namespace gv;

#include "gv_framework_unit_test_helper.hpp"

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                      \
	namespace x                               \
	{                                         \
	extern gv_unit_test_with_renderer* ptest; \
	};
#include "gv_framework_tests.h"

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                            \
	namespace x                                     \
	{                                               \
	extern void main(gvt_array< gv_string >& args); \
	};
#include "gv_framework_console_tests.h"
#undef REG_UNIT_TEST

namespace gv
{

bool gv_register_user_drivers()
{
	if (1)
	{
		switch (gv_global::rnd_opt.m_render_driver_type)
		{
		case e_render_driver_ogl:
#if GV_WITH_OGL
			GVM_REG_DRIVER(ogl);
#endif
			break;

		case e_render_driver_3nd_0:
#if GV_WITH_GP
			GVM_REG_DRIVER(gameplay3d);
#endif
			break;

		case e_render_driver_3nd_1:
#if GV_WITH_BGFX
			GVM_REG_DRIVER(renderer_bgfx);
#endif
			break;

		default:
			GVM_REG_DRIVER(d3d9);
		}
	};

#if GV_WITH_IMPEXP
	GVM_REG_DRIVER(impexp);
#endif

#if WITH_CEGUI
	GVM_REG_DRIVER(cegui);
#endif
	return true;
}
}

#include "gv_framework_test_browser.hpp"
int main(int argc, char* argv[])
{
	///=============init system===========================
	// override the parameter
	// console  unit_test_display_animation -100000 tool
	// console unit_test_importer_exporter_fbx tool
	// -"resource_root:/animations/fbx/Heron@Run.fbx
	// unit_test_importer_exporter_fbx console
	// unit_test_importer_exporter_fbx console
	// console unit_test_importer_exporter		tool -100000
	// -"resource_root:/animations/dae/astroBoy_walk.dae"
	// console unit_test_importer_exporter_fbx	tool
	// -"resource_root:/animations/fbx/humanoid.fbx"
	// console unit_test_importer_exporter tool -100000		-FILE_FBX_HERON
	// argc=4 ;_TCHAR* argv2[]={"gv_framework_tests.exe","unit_test_load_x_file",
	// "console","tool"}; argv=argv2;
	// argc=6; _TCHAR* argv2[]={"gv_framework_tests.exe","console",
	// "unit_test_importer_exporter", "-100000",
	// "-resource_root:/materials/Media/Models/Torus2.obj", "tool"}; argv=argv2;
	// GV_DEBUG_BREAK;

	bool run_tool = gv_find_in_command_line("tool", argc, argv);
	bool use_mem_track = gv_find_in_command_line("mem_track", argc, argv);
	bool no_profile = gv_find_in_command_line("no_profile", argc, argv);
	bool no_debug_draw = gv_find_in_command_line("no_debug_draw", argc, argv);
	bool with_browser = gv_find_in_command_line("browser", argc, argv);
	gv_vector2i screen_size(1024, 768);
	if (gv_find_in_command_line("1080p", argc, argv))
	{
		screen_size = gv_vector2i(1920, 1080);
	};
#if GV_WITH_GP
	if (with_browser)
	{
		return browser_main();
	}
#endif
	if (gv_find_in_command_line("ogl", argc, argv))
	{
		gv_global::rnd_opt.m_render_driver_type = e_render_driver_ogl;
	}
	if (gv_find_in_command_line("gp", argc, argv))
	{
		gv_global::rnd_opt.m_render_driver_type = e_render_driver_3nd_0;
		gv_global::framework_config.data_path_root = RES_ROOT;
		gv_global::framework_config.data_path_root += "/android/assets/";
	}
	if (gv_find_in_command_line("bgfx", argc, argv))
	{
		gv_global::rnd_opt.m_render_driver_type = e_render_driver_3nd_1;
	};
	;
	gv_base_config& boost_config = gv_global::config;
	boost_config.enable_mem_tracking = use_mem_track;
	boost_config.enable_profiler = !run_tool && !no_profile;
	boost_config.main_log_file_name = "gv_framework_test";
	boost_config.lock_free_zone_size = 128 * 1024 * 1024;
	gv_global::rnd_opt.m_no_debug_draw = no_debug_draw;
	gv_global::rnd_opt.m_screen_width = 1024;
	gv_global::rnd_opt.m_screen_height = 720;

#if GV_DEBUG_VERSION
	boost_config.no_boost_pool = true;
#endif
	bool no_log = gv_find_in_command_line("no_log", argc, argv);
	;
	boost_config.no_log_all = no_log;
	gv_base_init();
	gv_framework_init();

	///=============test start ===========================
	{
		bool run_all_test = gv_find_in_command_line("all", argc, argv);
		bool need_help = gv_find_in_command_line("help", argc, argv);
		bool log_to_console = gv_find_in_command_line("console", argc, argv);

		gvt_array< gv_string > arg;
		gv_fill_command_line_arg(arg, argc, argv);
		int render_mode = 0;
		if (log_to_console)
		{
			render_mode = 2;
		}
		if (gv_find_in_command_line("debug", argc, argv))
		{
			render_mode = 0;
		}
		if (render_mode == 0)
		{
			// 2d render,default;
			gv_global::debug_draw.get();
			// gv_unit_test_with_renderer *first_test=NULL;
			gvt_array< gv_unit_test_with_renderer* > test_array;

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                                                          \
	{                                                                             \
		gv_string_tmp name = x::ptest->name();                                    \
		bool is_cmd = gv_find_in_command_line(*name, argc, argv) || run_all_test; \
		if (is_cmd)                                                               \
		{                                                                         \
			test_array.add(x::ptest);                                             \
		}                                                                         \
	}
#include "gv_framework_tests.h"

			for (int i = 0; i < test_array.size() - 1; i++)
			{
				test_array[i]->set_next(test_array[i + 1]);
			}

			if (test_array.size())
			{
				gv_unit_test_context_guard context;
				gvt_scope_test_timer< std::ostream > timer(std::cout, "2d_render");
				gv_global::debug_draw.get()->create_window(
					"framework_unit_test", "framework_unit_test_2D",
					gv_vector2i(100, 100), screen_size, true);
				gv_global::debug_draw.get()->init_vb(10000, 10000);
				gv_global::debug_draw.get()->start_test(test_array[0]);
			}

			test_array.clear();
		}

		else
		{
			gvt_array< gv_string > g_args;
			gv_fill_command_line_arg(g_args, argc, argv);
#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                                                    \
	{                                                                       \
		const char* name = #x;                                              \
		if (need_help)                                                      \
		{                                                                   \
			std::cout << "|[" << name << "]" << std::endl;                  \
		}                                                                   \
		else if (run_all_test || gv_find_in_command_line(name, argc, argv)) \
		{                                                                   \
			gvt_scope_test_timer< std::ostream > timer(std::cout, name);    \
			x::main(g_args);                                                \
		}                                                                   \
	}
#include "gv_framework_console_tests.h"
			// no render  test;
		}
	}
	///=============deinit system===========================
	std::cout << "all test passed!!" << std::endl;
	gv_framework_destroy();
	gv_global::profiler->dump_snap_shot("frame_work_test_profile_");
	gv_base_destroy();
	gvp_memory_default::static_dump_tracking();
	getchar();
	return 0;
}
