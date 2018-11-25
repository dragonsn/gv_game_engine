
namespace unit_test_importer_exporter
{
void main(gvt_array< gv_string >& args)
{
	{
		gv_unit_test_context_guard context;
		//============>>INIT=================================================>>
		sub_test_timer timer("test_importer_");
		//init
		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("mesh_test"));
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), NULL);
		gv_com_static_mesh* pmesh0 = m_sandbox->create_object< gv_com_static_mesh >(my_entity);
		gv_static_mesh* my_static_mesh = m_sandbox->create_object< gv_static_mesh >(my_mod);
		gv_texture* my_texture = m_sandbox->create_object< gv_texture >(my_static_mesh);
		gv_string_tmp tex_file_name = FILE_TEX_SNOW_CUBEMAP;
		my_texture->set_file_name(tex_file_name);

		gv_string_tmp file_name = FILE_OBJ_NINJA_HEAD;
		if (args.size() >= 2)
			args[1] >> file_name;
		if (!m_sandbox->import_external_format(my_static_mesh, file_name))
		{
			gv_model* model = m_sandbox->create_object< gv_model >(my_mod);
			m_sandbox->import_external_format(model, file_name);
			if (model->get_nb_static_mesh())
				my_static_mesh = model->get_static_mesh(0);
			else if (model->get_nb_skeletal_mesh())
				my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
		}
		my_static_mesh->m_diffuse_texture = my_texture;
		pmesh0->set_resource(my_static_mesh);

		//set the renderer of the mesh, this is needed .
		pmesh0->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_simple_shader_renderer"));
		my_entity->add_component(pmesh0);
		my_world->add_entity(my_entity);

		//set camera!!
		gv_entity* my_camera = m_sandbox->create_object< gv_entity >(gv_id("my_camera"), NULL);
		my_camera->add_component(gv_id("gv_com_cam_fps_fly"));
		gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_camera);
		my_camera->add_component(camera);
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);

		camera->set_fov(60, 1.333f, 0.1f, 1000);
		camera->set_look_at(gv_vector3(0, 0, 300), gv_vector3(0, 0, 0));
		camera->update_projection_view_matrix();

		//============>>GO GO GO !!
		int loop = 1000;
		bool quit = false;
		if (args.size())
			args[0] >> loop;
		while (loop-- && !quit)
		{
			gv_string_tmp title = "do_ray_trace ,arrow to move , a,s to rotate ====>>";
			gv_global::debug_draw.get()->draw_string(*title, gv_vector2i(100, 120), gv_color::RED());
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			{
				gv_vector3 v[3] = {gv_vector3(0, 0, 0.5f), gv_vector3(100, 100, 0.5f), gv_vector3(100, 0, 0.5f)};
				gv_vector2 uv[3] = {gv_vector2(0, 0), gv_vector2(1, 1), gv_vector2(1, 0)};
				gv_color color[3] = {gv_color::WHITE(), gv_color::WHITE(), gv_color::WHITE()};
				get_debug_draw()->draw_tex_triangle(v, uv, color);
				get_debug_draw()->set_debug_texture(my_texture);
				gv_vector3 v2[3] = {gv_vector3(200, 0, 0.5f), gv_vector3(300, 100, 0.5f), gv_vector3(300, 0, 0.5f)};
				get_debug_draw()->draw_tex_triangle(v2, uv, color);
			}
			quit = !m_sandbox->tick();
		}

		//================================================
		m_sandbox->export_module(my_mod->get_name_id());
	}
}
}
