// gv_base_unit_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "inc/vld.h"
#include "string.h"

#include "gv_base.h"
#include "gv_time.h"
using namespace gv;

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                            \
	namespace x                                     \
	{                                               \
	extern void main(gvt_array< gv_string >& args); \
	};
#include "gv_base_unit_tests.h"

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                                                    \
	{                                                                       \
		const char* name = #x;                                              \
		if (need_help)                                                      \
		{                                                                   \
			test_log() << "|[" << name << "]" << gv_endl;                   \
		}                                                                   \
		else if (run_all_test || gv_find_in_command_line(name, argc, argv)) \
		{                                                                   \
			gvt_scope_test_timer< std::ostream > timer(test_log(), name);   \
			x::main(g_args);                                                \
		}                                                                   \
	}

int main(int argc, char* argv[])
{
	// test start!!
	bool run_tool = gv_find_in_command_line("tool", argc, argv);
	bool no_stop = gv_find_in_command_line("no_stop", argc, argv);
	bool use_memtrack = gv_find_in_command_line("mem_track", argc, argv);
	;
	gv_base_config& boost_config = gv_global::config;
	boost_config.enable_mem_tracking = use_memtrack;
	boost_config.enable_profiler = !run_tool;
	bool no_log = gv_find_in_command_line("no_log", argc, argv);
	;
	boost_config.no_log_all = no_log;
#if GV_DEBUG_VERSION
	boost_config.no_boost_pool = true;
#endif
	boost_config.profiler_buffer_size = 256 * 1024; // for test
	boost_config.lock_free_zone_size = 128 * 1024 * 1024;
	gv_base_init();
	{
		using namespace std;
		bool run_all_test = gv_find_in_command_line("all", argc, argv);
		bool need_help = gv_find_in_command_line("help", argc, argv);
		bool log_to_console = gv_find_in_command_line("console", argc, argv);
		gvt_array< gv_string > g_args;
		gv_fill_command_line_arg(g_args, argc, argv);

		streambuf* sbuf = NULL;
		ofstream file;
		gv_string cout_file_name;

		if (!log_to_console)
		{
			cout_file_name = gv_global::fm->get_work_path();
			gv_string gs;
			gs += gv_global::time->get_local_time_string();
			gs.replace_all(":", "_");
#if GV_DEBUG_VERSION
			gs += "_debug";
#endif
			gs += ".xml";
			file.open(*gs);
			sbuf = test_log().rdbuf();
			test_log().rdbuf(file.rdbuf());
			cout_file_name += gs;
		}

		if (need_help)
		{
			GVM_CONSOLE_OUT(gv_endl << "USAGE:  gv_base_unit_tests  [all]"
									<< gv_endl);
		}
		else
		{
			test_log() << " ";
			test_log() << gv_endl << "<"
					   << "root"
					   << ">" << gv_endl;
		}
//
#include "gv_base_unit_tests.h"
		if (!need_help)
		{
			test_log() << gv_endl << "<"
					   << "/root"
					   << ">" << gv_endl;
		}

		if (!log_to_console)
		{
			test_log().rdbuf(sbuf);
		}
		if (!need_help)
			GVM_CONSOLE_OUT(gv_endl << "!!!!!###########all tests  END ,press any "
									   "key to continue  !!!!!>>>>"
									<< gv_endl << gv_endl << gv_endl);
		GVM_DEBUG_OUT("\r\n" << cout_file_name << "(0):");
		GVM_DEBUG_OUT("\r\n" << gv_global::fm->get_exe_path()
							 << gv_global::log->get_log()->get_file_name()
							 << "(0):");
	}
	gv_global::profiler->dump_snap_shot("boost_unit_test_profile_");
	gv_base_destroy();
	gvp_memory_default::static_dump_tracking();
	if (!no_stop)
		getchar();
	return 0;
}
