
GVM_DCL_DRIVER(dummy)

namespace unit_test_game_engine_async_load
{
const char* module_name[] =
	{
		MOD_GIRL};

void main(gvt_array< gv_string >& args)
{
	gv_driver_info::static_unregister_all();
	GVM_REG_DRIVER(dummy);
	static const int size = GV_ARRAY_LENGTH(module_name);
	gv_async_load_info infos[size];
	for (int n = 0; n < size; n++)
	{
		gv_async_load_info& info = infos[n];
		info.m_module_name = module_name[n];
	}
	gv_game_engine::static_init(-1, 0, "");
	{
		gv_game_engine game;
		game.init();
		for (int n = 0; n < size; n++)
		{
			gv_async_load_info& info = infos[n];
			game.async_load_module(&info);
		}
		for (int i = 0; i < 100; i++)
		{
			gv_thread::sleep(10);
			game.tick();
			gv_string_tmp s;
			s << "hello world !!" << i << gv_endl;
			game.get_debug_draw()->draw_string(*s, gv_vector2i(100, 100), gv_color::BLUE_B());
			;
			gv_vector2i pos(100, 130);
			for (int n = 0; n < size; n++)
			{
				gv_async_load_info& info = infos[n];
				gv_string_tmp s;
				s << info.m_module_name << " ";
				gv_color c;
				switch (info.get_state())
				{
				case e_async_loading:
					s << "still loading";
					for (int k = 0; k < i % 10; k++)
						s << ".";
					c = gv_color::YELLOW();
					break;

				case e_async_load_failed:
					s << "load failed";
					c = gv_color::RED_B();
					break;

				case e_async_loaded:
					s << "load done";
					c = gv_color::GREEN();
					break;
				}
				game.get_debug_draw()->draw_string(*s, pos, c);
				pos.y += 16;
			}
		}
	}
	gv_game_engine::static_destroy();
	return;
}
}
