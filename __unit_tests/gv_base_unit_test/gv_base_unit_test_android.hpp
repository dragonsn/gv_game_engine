

#include <android_native_app_glue.h>
#include "gameplay.h"

using namespace gv;
using namespace gameplay;

extern struct android_app* __state;

/**
 * Main entry point.
 */
#include "gv_base_unit_test_android_game.hpp"

#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                            \
	namespace x                                     \
	{                                               \
	extern void main(gvt_array< gv_string >& args); \
	};
#include "gv_base_unit_tests.h"
extern struct android_app* __state;

TestsGame mygame;
void android_main(struct android_app* state)
{
	// Android specific : Dummy function that needs to be called to
	// ensure that the native activity works properly behind the scenes.

	app_dummy();
	__state = state;
	/*for ( int i=0; i< 1000000000; i++)
  {
          GVM_CONSOLE_OUT("I am superman! \n");
          gv::gv_thread::sleep(100);
  } */
	gv_base_config& boost_config = gv_global::config;
	boost_config.enable_mem_tracking = false;
	boost_config.enable_profiler = true;
	boost_config.profiler_buffer_size = 256 * 1024; // for test
	GVM_CONSOLE_OUT("I am "
					"superman----------------------------------------------------"
					"-----! \n");
	gv_base_init();
	GVM_WARNING("gv_base_inited! \n")
	GVM_DEBUG_OUT("I am "
				  "debugman######################################################"
				  "####! \n");
	(*gv_global::log->get_log())
		<< "I am logman $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$! \n";
#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                            \
	{                                               \
		TestsGame::addTest("base", #x, x::main, 0); \
		GVM_DEBUG_OUT(#x " added to test\n");       \
	}
	TestsGame::addTest("system", "all", 0, 0);
	TestsGame::addTest("system", "quit", 0, 0);

#include "gv_base_unit_tests.h"

	{
		__state = state;
		Game* game = Game::getInstance();
		Platform* platform = Platform::create(game);
		GP_ASSERT(platform);
		platform->enterMessagePump();
		delete platform;
	}
	(*gv_global::log->get_log()) << " @@@@@@@@@@@@@@@I am deadman "
									"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
									"@@@@@! \n";
	gv_global::profiler->dump_snap_shot("boost_unit_test_profile_");
	gv_base_destroy();
	gvp_memory_default::static_dump_tracking();

	// Android specific : the process needs to exit to trigger
	// cleanup of global and static resources (such as the game).
	exit(0);
}
