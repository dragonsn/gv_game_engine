//============================================================================================
//								:
//============================================================================================
namespace unit_test_rfx_view
{

void main(gvt_array< gv_string >& args)
{
	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_rfx_render");

		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));

		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("rfx_test_scene"));
		my_world->set_owner(my_mod);
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), NULL);
		gv_com_static_mesh* com_mesh = m_sandbox->create_object< gv_com_static_mesh >(my_entity);
		gv_com_skeletal_mesh* com_sk_mesh = NULL;
		gv_skeletal* my_skeletal = NULL;
		gv_static_mesh* my_static_mesh = m_sandbox->create_object< gv_static_mesh >(my_mod);
		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gv_texture* my_texture = m_sandbox->create_object< gv_texture >(my_static_mesh);
		gv_string_tmp tex_file_name = FILE_TEX_SNOW_CUBEMAP;
		my_texture->set_file_name(tex_file_name);
		gv_ani_set* my_animation = NULL;
		bool no_ani = args.find("no_ani");
		bool do_compress = args.find("compress");
		gv_int index_seq = 0;
		gv_int idx;
		static bool test_batch = false;
		if (args.find("test_batch"))
			test_batch = true;

		gv_string mod_file_name;
		if (args.find("mod", idx))
		{
			mod_file_name = args[idx + 1];
			m_sandbox->import_module(gv_id(*mod_file_name));
			gvt_object_iterator< gv_ani_set > it_ani(m_sandbox);
			if (!it_ani.is_empty())
			{
				my_animation = it_ani;
			}
			gvt_object_iterator< gv_skeletal_mesh > it_sk(m_sandbox);
			if (!it_sk.is_empty())
			{
				my_skeletal_mesh = it_sk;
				my_skeletal = my_skeletal_mesh->get_skeletal();
				my_static_mesh = my_skeletal_mesh->m_t_pose_mesh;
			}
		}

		if (args.find("bin", idx))
		{
			mod_file_name = args[idx + 1];
			m_sandbox->load_module(gv_id(*mod_file_name));
			gvt_object_iterator< gv_ani_set > it_ani(m_sandbox);
			if (!it_ani.is_empty())
			{
				my_animation = it_ani;
			}
			gvt_object_iterator< gv_skeletal_mesh > it_sk(m_sandbox);
			if (!it_sk.is_empty())
			{
				my_skeletal_mesh = it_sk;
				my_skeletal = my_skeletal_mesh->get_skeletal();
				my_static_mesh = my_skeletal_mesh->m_t_pose_mesh;
			}
		}

		gv_string effect_name;
		if (args.find("effect", idx))
		{
			args[idx + 1] >> effect_name;
		}

		gv_string_tmp file_name = FILE_3DS_TEAPOT;
		if (args.size() >= 2)
			args[1] >> file_name;
		if (!m_sandbox->import_external_format(my_static_mesh, file_name))
		{
			gv_model* model = m_sandbox->create_object< gv_model >(my_mod);
			if (!m_sandbox->import_external_format(model, file_name))
			{
				return;
			}
			if (!no_ani && model->get_nb_animation() && !my_animation)
			{
				my_animation = model->get_animation(0);
				if (do_compress)
				{
					my_animation->compress_all();
				}
			}
			if (!no_ani && model->get_nb_skeletal_mesh() && !my_skeletal_mesh)
			{
				my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
				if (model->get_skeletal_mesh(0)->get_skeletal()->get_nb_bone())
				{
					my_skeletal_mesh = model->get_skeletal_mesh(0);
					my_skeletal = my_skeletal_mesh->get_skeletal();
				}
			}
			else if (model->get_nb_static_mesh())
			{
				my_static_mesh = model->get_static_mesh(0);
			}

			if (my_skeletal_mesh)
			{
				com_sk_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >(my_entity);
			}

			if (no_ani && model->get_nb_skeletal_mesh())
			{
				my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
			}
			//if (!my_static_mesh->get_nb_normal()) my_static_mesh->rebuild_normal();
			my_static_mesh->rebuild_normal();
			my_static_mesh->rebuild_binormal_tangent();
			my_static_mesh->rebuild_bounding_volumn();
		}
		//m_sandbox->export_module(my_mod->get_name_id() );
		//my_mod		=m_sandbox->create_object<gv_module>(gv_id("rfx_test") );

		gv_effect* my_effect;
		gvt_array_cached< gv_effect*, 256 > effect_table;
		int current_idx = 0;
		{
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_mod);
			gv_string_tmp file_name = FILE_RFX_WOOD;
			if (args.size() >= 3)
				args[2] >> file_name;
			m_sandbox->import_external_format(effect, *file_name);
			my_effect = effect->get_base_effect();
			my_effect->query_renderable_effect(effect_table);
			if (!effect_name.size())
				my_effect = my_effect->get_renderable_effect(gv_id_any);
			else
				my_effect = my_effect->get_renderable_effect(gv_id(*effect_name));
			//gv_float f=164.f;
			//my_effect->set_param(gv_id("frequency"),f);
		}
		gv_material* my_material = m_sandbox->create_object< gv_material >(gv_id("material"));
		my_material->m_effect = my_effect;
		my_static_mesh->m_diffuse_texture = my_texture;
		if (com_sk_mesh)
		{
			com_sk_mesh->set_material(my_material);
			com_sk_mesh->set_resource(my_skeletal_mesh);
			com_sk_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_effect_renderer"));
			//com_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_simple_shader_renderer") );
			my_entity->add_component(com_sk_mesh);
			if (my_animation && my_animation->get_sequence(0))
			{
				gv_com_animation* com_ani = m_sandbox->create_object< gv_com_animation >(my_entity);
				com_ani->set_resource(my_animation);
				my_entity->add_component(com_ani);
				com_ani->play_animation(my_animation->get_sequence(0)->get_name_id(), 0.f, true);
			}
			else
				my_animation = NULL;
		}
		else
		{
			com_mesh->set_material(my_material);
			com_mesh->set_resource(my_static_mesh);
			if (test_batch)
				com_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_instancing_renderer"));
			else
				com_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_effect_renderer"));
			//com_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_simple_shader_renderer") );
			my_entity->add_component(com_mesh);
		}
		//my_entity->set_scale(gv_vector3(10,10,10));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		//set camera!!
		gv_entity* my_camera = m_sandbox->create_object< gv_entity >(gv_id("my_camera"), NULL);
		my_camera->add_component(gv_id("gv_com_cam_fps_fly"));
		gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_camera);
		float r = my_static_mesh->get_bsphere().get_radius();
		//camera->set_fov(60.f,1.333f,0.1f*r, 100*r );
		//camera->set_look_at(gv_vector3 ( 1, 0,5.f)*r , gv_vector3(0,0,0) );
		camera->set_fov(60, 1.333f, 0.1f * r, 100 * r);
		camera->set_look_at(gv_vector3(1, 0, 5.f) * r, gv_vector3(0, 0, 0));
		camera->update_projection_view_matrix();
		my_camera->add_component(camera);
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);
		gvt_array< gv_vector3 > original_pos;
		//============>>GO GO GO !!
		int loop = 1000;
		bool quit = false;
		if (args.size())
			args[0] >> loop;

		if (test_batch)
		{
			//gv_float f=164.f;
			//my_effect->set_param(gv_id("frequency"),f);
			static gv_int batch_w = 11;
			static gv_int batch_h = 11;
			for (int i = 1; i < batch_h; i++)
				for (int j = 1; j < batch_w; j++)
				{
					gv_entity* new_entity = my_entity->clone();
					new_entity->set_position(gv_vector3(j * r * 4, 0, i * r * 4));
					my_world->add_entity(new_entity);
					new_entity->update_matrix();
				}
		}
		while (loop-- && !quit)
		{
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}

			quit = !m_sandbox->tick();
			{
				static bool last_key_down = false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
				{
					current_idx++;
					if (current_idx >= effect_table.size())
						current_idx = 0;
					my_material->m_effect = effect_table[current_idx];
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_space);
			}
			{
				static bool last_key_down = false;
				if (my_animation && m_sandbox->get_input_manager()->is_key_down(e_key_p) && !last_key_down)
				{
					index_seq++;
					if (index_seq >= my_animation->get_nb_sequence())
					{
						index_seq = 0;
					}
					my_entity->get_component< gv_com_animation >()->play_animation(my_animation->get_sequence(index_seq)->get_name_id(), 0.5f, true, true);
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_p);
			}

			gv_string_tmp info;
			if (my_animation)
			{
				info << " current animation ";
				if (index_seq >= my_animation->get_nb_sequence())
				{
					index_seq = 0;
				}
				gv_ani_sequence* pseq = my_animation->get_sequence(index_seq);
				info << pseq->get_name_id() << "(" << index_seq << "/" << my_animation->get_nb_sequence();
				gv_global::debug_draw->draw_string(*info, gv_vector2i(100, 20), gv_color::BLUE_D());
			}
			//============================================================================================
			//								:
			//============================================================================================
			//ANIMATE THE MESH
			if (com_sk_mesh)
			{
				my_skeletal = com_sk_mesh->get_writable_skeletal();
			}
			//my_entity->debug_draw_aabb();
			//my_entity->debug_draw_axis();
		}

		//================================================
		m_sandbox->export_module(my_mod->get_name_id());
	}
}
}
