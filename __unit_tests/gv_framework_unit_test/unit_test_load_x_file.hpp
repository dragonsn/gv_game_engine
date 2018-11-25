namespace unit_test_load_x_file
{

void main(gvt_array< gv_string >& args)
{
	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_load_x_file");

		//============>>INIT
		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));

		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), NULL);
		gv_com_x_mesh* pmesh0 = m_sandbox->create_object< gv_com_x_mesh >(gv_id("x_mesh"), my_entity);
		gv_resource* resource = m_sandbox->create_object< gv_resource >(gv_id("x_file"), pmesh0);
		resource->set_file_name(FILE_X_TIGER);
		pmesh0->set_resource(resource);
		my_entity->add_component(pmesh0);
		my_world->add_entity(my_entity);

		gv_entity* my_camera = m_sandbox->create_object< gv_entity >(gv_id("my_camera"), NULL);
		my_camera->add_component(gv_id("gv_com_cam_fps_fly"));
		gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_camera);
		my_camera->add_component(camera);
		pmesh0 = m_sandbox->create_object< gv_com_x_mesh >(gv_id("x_mesh"), my_entity);
		pmesh0->set_resource(resource);
		my_camera->add_component(pmesh0);
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);

		//============>>GO GO GO !!
		int loop = 1000;
		if (args.size())
			args[0] >> loop;
		bool quit = false;

		while (loop-- && !quit)
		{
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			quit = !m_sandbox->tick();
			gvi_debug_renderer* pdebug = gv_global::debug_draw.get();
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(10, 0, 0), gv_color::RED(), gv_color::RED_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 10, 0), gv_color::GREEN(), gv_color::GREEN());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 0, 10), gv_color::BLUE(), gv_color::BLUE());
		}

		//============>>QUITING!
	}
}
}
