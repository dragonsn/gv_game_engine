//============================================================================================
//								:
//============================================================================================

namespace unit_test_roam_terrain
{

void main(gvt_array< gv_string >& args)
{
	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_roam_terrain");
		//============>>INIT=================================================>>
		m_sandbox->register_processor(gv_id("gv_world"), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));
		{
			//my_world->init(1, gv_vector2i(map_width, map_height),gv_box(gv_vector3(-2000,-200, -2000),gv_vector3(2000,200,2000) ),the_map);
		}
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("test_terrain"));
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), my_world);
		gv_com_skeletal_mesh* com_sk_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >(my_entity);
		gv_material* my_material = m_sandbox->create_object< gv_material >();
		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gv_static_mesh* my_static_mesh = NULL;
		gv_ani_set* my_animation = NULL;
		gv_effect* my_effect = NULL;

		gv_entity* my_terrain_entity = m_sandbox->create_object< gv_entity >(gv_id("terrain_entity"), my_world);
		gv_com_terrain_roam* my_terrain = m_sandbox->create_object< gv_com_terrain_roam >(my_terrain_entity);
		bool new_sky = args.find("new_sky");
		bool use_parallax = args.find("parallax");
		bool is_island = args.find("island");
		bool with_trees = args.find("tree");
		bool with_trees_layer = args.find("tree_layer");
		bool with_grass = args.find("grass");
		bool with_flower = args.find("flower");
		bool simple = args.find("simple");

		bool need_ocean = is_island;
		bool no_lod = args.find("no_lod");
		gv_string effect_name = "simple_texture";
		int idx;
		if (args.find("effect", idx))
		{
			args[idx + 1] >> effect_name;
		}
		int tree_number = 1000;
		if (args.find("tree_nb", idx))
		{
			args[idx + 1] >> tree_number;
		}

		float grass_nb = 10000;
		if (args.find("grass_nb", idx))
		{
			args[idx + 1] >> grass_nb;
		}
		gv_string height_map_name = FILE_TEX_HEIGHTMAP;
		if (simple)
		{
			height_map_name = FILE_TEX_HEIGHTMAP_S;
		}
		if (args.find("height_map", idx))
		{
			args[idx + 1] >> height_map_name;
		}
		//============================================================================================
		//								:
		//============================================================================================
		gv_effect* atmosphere_effect = NULL;
		create_skybox(my_world, !new_sky, &atmosphere_effect);

		//============================================================================================
		//								:
		//============================================================================================

		if (need_ocean)
		{
			gv_string_tmp file_name = FILE_3DS_SCREEN_QUAD;
			gv_model* model = m_sandbox->create_object< gv_model >(my_world);
			m_sandbox->import_external_format(model, file_name);
			gv_static_mesh* pmesh = model->get_static_mesh(0);
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_world);
			gv_effect* my_effect = atmosphere_effect->get_renderable_effect(gv_id("OceanWater"));
			gv_entity* ocean_entity = m_sandbox->create_object< gv_entity >(gv_id("ocean_entity"), my_world);
			gv_material* ocean_material = m_sandbox->create_object< gv_material >();
			ocean_material->m_effect = my_effect;
			gv_com_static_mesh* com_static_mesh = get_sandbox()->create_object< gv_com_static_mesh >(my_entity);
			com_static_mesh->set_material(ocean_material);
			com_static_mesh->set_resource(pmesh);
			com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_effect_renderer"));
			com_static_mesh->set_always_visible(true);
			ocean_entity->add_component(com_static_mesh);
			ocean_entity->set_position(gv_vector3(0, 0, 0));
			ocean_entity->update_matrix();
			my_world->add_entity(ocean_entity);
		}

		//============================================================================================
		//								:
		//============================================================================================
		{
			my_terrain_entity->add_component(my_terrain);
			my_terrain_entity->set_scale(gv_vector3(16.f, 4.f, 16.f));
			my_terrain_entity->update_matrix();
			my_world->add_entity(my_terrain_entity);
		}

		gvt_random< gv_float > m_random;
		gvt_array_cached< gv_effect*, 256 > effect_table;
		int current_idx = 0;
		{ //load model & animation
			gv_string_tmp file_name = FILE_FBX_HERON;
			gv_model* model = m_sandbox->create_object< gv_model >(my_world);
			m_sandbox->import_external_format(model, file_name);
			if (!model->get_nb_animation())
				return;
			my_animation = model->get_animation(0);
			my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
			my_skeletal_mesh = model->get_skeletal_mesh(0);
			if (!my_static_mesh->get_nb_normal())
				my_static_mesh->rebuild_normal();
			my_static_mesh->rebuild_binormal_tangent();
		}
		gv_effect* phong_effect = NULL;
		{ //load material for bird
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_world);
			gv_string_tmp file_name = FILE_RFX_BIRD;
			m_sandbox->import_external_format(effect, *file_name);
			my_effect = effect->get_base_effect();
			my_effect->query_renderable_effect(effect_table);
			phong_effect = my_effect = my_effect->get_renderable_effect(gv_id("TexturedPhong"));
			my_material->m_effect = my_effect;
		}

		{ //link animation & material
			com_sk_mesh->set_material(my_material);
			com_sk_mesh->set_resource(my_skeletal_mesh);
			com_sk_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_effect_renderer"));
			my_entity->add_component(com_sk_mesh);
			gv_com_animation* com_ani = m_sandbox->create_object< gv_com_animation >(my_entity);
			com_ani->set_resource(my_animation);
			my_entity->add_component(com_ani);
			com_ani->play_animation(my_animation->get_sequence(0)->get_name_id(), 0.f, true);
		}
		//============================================================================================
		//								:
		//============================================================================================
		gv_effect* terrain_base_effect = NULL;

		{
			//create terrain
			//gv_entity * terrain_entity	=m_sandbox->create_object<gv_entity> (my_mod);
			//terrain_entity->add_component(my_terrain);
			gv_effect* pterrain_effect = NULL;
			my_terrain->enable_export_normal_map(true);
			if (is_island)
				my_terrain->enable_island_generation(gv_vector2i(512, 512), 2.f, 5.f);
			my_terrain->init_from_image_file(*height_map_name);
			if (no_lod)
				my_terrain->enable_dynamic_LOD(false);
			gv_material* terrain_material = get_sandbox()->create_object< gv_material >(my_terrain_entity);
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_world);
			gv_string_tmp file_name = FILE_RFX_TERRAIN;
			m_sandbox->import_external_format(effect, *file_name);
			terrain_base_effect = effect->get_base_effect();

			if (use_parallax)
			{
				pterrain_effect = effect->get_base_effect()->get_renderable_effect(gv_id("bump_2_layer"));
			}
			else
			{
				pterrain_effect = effect->get_base_effect()->get_renderable_effect(gv_id(*effect_name));
			}

			{
				gv_vector4 offset(100, 200, 200, 0);
				pterrain_effect->set_param(gv_id("f4Offset"), offset);
			}
			terrain_material->m_effect = pterrain_effect;
			my_terrain->set_material(terrain_material);
			gv_vector4 scale(1.f, 1, 1, 1);
			pterrain_effect->set_param(gv_id("g_TexCoordScale"), scale);
		}

		//============================================================================================
		//								:
		//============================================================================================
		if (with_grass)
		{
			gv_material* grass_material = get_sandbox()->create_object< gv_material >(my_terrain_entity);
			gv_effect* grass_effect = terrain_base_effect->get_renderable_effect(gv_id("grass_normal"));
			grass_material->m_effect = grass_effect;
			if (is_island)
				my_terrain->add_grass_layer(grass_material, grass_nb, 0.5f, gv_vector2(250, 1500), 1, 0.3f);
			else
				my_terrain->add_grass_layer(grass_material, 1000, 0.5f, gv_vector2(0, 1500), 1, 0.3f);
			//add_grass_layer
		}

		//============================================================================================
		//								:
		//============================================================================================
		if (with_flower)
		{
			gv_material* grass_material = get_sandbox()->create_object< gv_material >(my_terrain_entity);
			gv_effect* grass_effect = terrain_base_effect->get_renderable_effect(gv_id("flower_normal"));
			grass_material->m_effect = grass_effect;
			my_terrain->add_grass_layer(grass_material, grass_nb, 0.5f, gv_vector2(250, 1500), 1, 0.0f);
			//add_grass_layer
		}
		//============================================================================================
		//								:
		//============================================================================================
		if (with_trees)
		{
			gv_string_tmp file_name = FILE_FBX_TREE;
			gv_model* model = m_sandbox->create_object< gv_model >(my_world);
			m_sandbox->import_external_format(model, file_name);
			gv_static_mesh* pmesh = model->get_static_mesh(0);
			pmesh->rebuild_normal();
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_world);
			gv_effect* my_effect = phong_effect;
			gv_entity* tree_entity = m_sandbox->create_object< gv_entity >(gv_id("tree_entity"), my_world);
			gv_material* tree_material = m_sandbox->create_object< gv_material >();
			tree_material->m_effect = my_effect;
			gv_com_static_mesh* com_static_mesh = get_sandbox()->create_object< gv_com_static_mesh >(my_entity);
			com_static_mesh->set_material(tree_material);
			com_static_mesh->set_resource(pmesh);
			//com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_effect_renderer") );
			com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_instancing_renderer"));
			tree_entity->add_component(com_static_mesh);
			tree_entity->set_position(gv_vector3(0, 0, 0));
			tree_entity->update_matrix();
			tree_entity->set_world(my_world);
			for (int j = 0; j < tree_number; j++)
			{
				gv_entity* new_entity = tree_entity->clone();

				new_entity->update_matrix();
				while (1)
				{
					gv_vector3 pos;
					pos.x = m_random.get_uniform() * 1024 * 16.f;
					pos.y = 0;
					pos.z = m_random.get_uniform() * 1024 * 16.f;

					gv_float h = my_terrain->get_height_world(pos);
					if (h < 300.f)
						continue;
					pos.y = h;
					gv_vector3 normal = my_terrain->get_normal_world(pos);
					if (normal.get_y() < 0.5)
						continue;
					new_entity->set_position(pos);
					new_entity->update_matrix();
					new_entity->set_world(my_world);
					my_terrain->attach_a_renderable(new_entity->get_component< gv_com_static_mesh >());
					break;
				};
			}
		}

		if (with_trees_layer)
		{
			gv_string_tmp file_name = FILE_FBX_TREE;
			gv_model* model = m_sandbox->create_object< gv_model >(my_world);
			m_sandbox->import_external_format(model, file_name);
			gv_static_mesh* pmesh = model->get_static_mesh(0);
			pmesh->rebuild_normal();
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_world);
			gv_effect* my_effect = phong_effect;
			gv_material* tree_material = m_sandbox->create_object< gv_material >();
			tree_material->m_effect = my_effect;
			gv_com_static_mesh* com_static_mesh = get_sandbox()->create_object< gv_com_static_mesh >(my_entity);
			com_static_mesh->set_material(tree_material);
			com_static_mesh->set_resource(pmesh);
			com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_instancing_renderer"));
			gv_terrain_mesh_layer_info info;
			info.is_colidable = false;
			info.mesh = com_static_mesh;
			my_terrain->add_mesh_layer(info);
			for (int j = 0; j < tree_number; j++)
			{
				while (1)
				{
					gv_vector2 pos;
					pos.x = m_random.get_uniform() * my_terrain->get_size_in_point().x;
					pos.y = m_random.get_uniform() * my_terrain->get_size_in_point().y;
					gv_rect rect;
					rect.move_to(pos);
					gvt_array< gv_float > p;
					my_terrain->update_mesh_layer(0, rect, p, 1);
					break;
				};
			}
		}

		//============================================================================================
		//								:
		//============================================================================================
		if (is_island)
			my_entity->set_position(gv_vector3(1024 * 8, 1, 1024 * 8));
		else
			my_entity->set_position(gv_vector3(0, 1, 0));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		my_world->set_main_actor(my_entity);
		m_sandbox->export_module(my_mod->get_name_id());

		float r = my_static_mesh->get_bsphere().get_radius();
		gvt_array< gv_vector3 > original_pos;
		int loop = 1000;
		bool quit = false;

		gv_com_camera* camera = NULL;
		{ //set camera;
			my_entity->add_component(gv_id("gv_com_cam_fps_fly"));
			camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_entity);
			camera->set_fov(60.f, 1.333f, 0.3f, 5000);
			camera->set_look_at(gv_vector3(0, 4, 4.f) * r, gv_vector3(0, 0, 0));
			camera->update_projection_view_matrix();
			my_entity->add_component(camera);
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		}
		//============================================================================================
		//								:
		//============================================================================================
		if (args.size())
			args[0] >> loop;
		bool use_fly_mode = false;
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
					my_terrain->enable_wireframe_mode(!my_terrain->is_wireframe_mode());
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_space);
			}
			{
				static bool last_key_down = false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_f) && !last_key_down)
				{
					use_fly_mode = !use_fly_mode;
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_f);
			}
			static float threshold = 500;
			if (m_sandbox->get_input_manager()->is_key_down(e_key_f1))
			{
				threshold -= 1;
				threshold = gvt_max(threshold, 0.1f);
			}
			if (m_sandbox->get_input_manager()->is_key_down(e_key_f2))
			{
				threshold += 1;
				threshold = gvt_min(threshold, 100000.f);
			}
			gv_vector3 pos = my_entity->get_position();
			gv_float h = my_terrain->get_height_world(pos);
			if (!use_fly_mode)
				pos.set_y(h + 3.5f);
			my_entity->set_position(pos);
			my_entity->update_matrix();
			camera->sync_to_entity(my_entity);
			//get_debug_draw()->draw_box_3d(my_entity->cget_world_aabb(),gv_color::RED_B());
			gv_string_tmp s;
			s << "nb visible patch: " << my_terrain->get_nb_visible_patch();
			s << "--------nb trangles : " << my_terrain->get_nb_tri() << "----current threshold :" << threshold << "use f1 f2 to change threthod, use f change fly mode";
			get_debug_draw()->draw_string(*s, gv_vector2i(60, 20), gv_color::GREEN_D());
			my_terrain->set_lod_threshold(threshold);

			gvi_debug_renderer* pdebug = gv_global::debug_draw.get();
			{
				static bool last_key_down = false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_l) && !last_key_down)
				{
					no_lod = !no_lod;
					my_terrain->enable_dynamic_LOD(!no_lod);
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_l);
			}
			//pdebug->draw_line_3d(pos , pos+gv_vector3(10, 0, 0 ), gv_color::RED(), gv_color::RED_B());
			//pdebug->draw_line_3d( pos ,pos+ gv_vector3(0, 10, 0 ), gv_color::GREEN(), gv_color::GREEN());
			//pdebug->draw_line_3d( pos ,pos+ gv_vector3(0, 0, 10 ), gv_color::BLUE(), gv_color::BLUE());
		}
	}
}
}
