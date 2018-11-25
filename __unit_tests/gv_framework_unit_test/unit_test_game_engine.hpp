
GVM_DCL_DRIVER(dummy)

namespace unit_test_game_engine
{
void main(gvt_array< gv_string >& args)
{
	gv_driver_info::static_unregister_all();
	GVM_REG_DRIVER(dummy);

	gv_game_engine::static_init();
	{
		gv_game_engine game;
		game.init();
		for (int i = 0; i < 100; i++)
		{
			gv_thread::sleep(10);
			game.tick();
			gv_string_tmp s;
			s << "hello world !!" << i << gv_endl;
			game.get_debug_draw()->draw_string(*s, gv_vector2i(100, 100), gv_color::BLUE_B());
		}
	}
	gv_game_engine::static_destroy();
	return;
}
}
