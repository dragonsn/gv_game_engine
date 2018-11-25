using namespace gv;

int main(int argc, char* argv[])
{

	gv_game_engine::static_init(argc, argv);
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
	return 0;
}