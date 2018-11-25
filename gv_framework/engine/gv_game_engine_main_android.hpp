#include <android_native_app_glue.h>
using namespace gv;
extern struct android_app* __state;

void android_main(struct android_app* state)
{
	// Android specific : Dummy function that needs to be called to
	// ensure that the native activity works properly behind the scenes.

	app_dummy();
	__state = state;
	gv_game_engine::static_init();
#if RETAIL
	try
#endif
	{
		gv_game_engine client;
		client.run();
	}
#if RETAIL
		case(const char *  msg)
	{
		GV_ERROR_OUT(msg);
	}
	case(...)
	{
	}
#endif

#if !RETAIL
	gv_global::profiler->dump_snap_shot("game_profile_");
#endif
	gv_game_engine::static_destroy();
#if !RETAIL
	gvp_memory_default::static_dump_tracking();
#endif
	
	exit(0);
}
