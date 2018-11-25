namespace unit_test_3d_init
{
void main(gvt_array< gv_string >& args)
{

	{
		gv_unit_test_context_guard context;

		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));
		gv_entity* my_camera = m_sandbox->create_object< gv_entity >(gv_id("my_camera"), NULL);
		gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_camera);
		my_camera->add_component(camera);
		camera->set_fov(60, 1.333f, 0.1f, 100);
		camera->set_look_at(gv_vector3(0, 1, 5.f), gv_vector3(0, 0, 0));
		camera->update_projection_view_matrix();
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);

		sub_test_timer timer("unit_test_3d_init");
		int loop = 1000;
		if (args.size())
			args[0] >> loop;
		bool quit = false;
		while (loop-- && !quit)
		{
			if (loop == 1)
			{
				GVM_POST_EVENT(render_uninit, render, );
			}
			quit = !m_sandbox->tick();
			gv_global::debug_draw->draw_string("superman is flying", 100, 100, gv_color::RED_B());
			gvi_debug_renderer* pdebug = gv_global::debug_draw.get();

			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(10, 0, 0), gv_color::RED(), gv_color::RED_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 10, 0), gv_color::GREEN(), gv_color::GREEN_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 0, 10), gv_color::BLUE(), gv_color::BLUE_B());

			pdebug->draw_line(gv_vector3(100, 100, 0.0), gv_vector3(100, 200, 0.0), gv_color::RED(), gv_color::RED_B());
			pdebug->draw_line(gv_vector3(200, 100, 0.0), gv_vector3(100, 100, 0.0), gv_color::GREEN(), gv_color::GREEN_B());
			pdebug->draw_line(gv_vector3(100, 200, 0.0), gv_vector3(200, 200, 0.0), gv_color::BLACK(), gv_color::GREEN());
			pdebug->draw_line(gv_vector3(200, 200, 0.0), gv_vector3(200, 100, 0.0), gv_color::BLUE(), gv_color::WHITE());
		}
		gv_id::static_purge();
	}
}
}