//============================================================================================
//								:
//============================================================================================

namespace unit_test_world_3d
{

static int lobby_max_player = 1;
bool m_is_drag;
gv_vector2i m_cu_mouse_pos;
gv_vector2i m_last_mouse_pos;
gv_euler rot;

void spawn_a_ball(gv_world_3d* my_world, gv_vector3i pos, gv_vector3i v)
{
	gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), my_world);
	my_world->add_entity(my_entity);
	my_entity->set_static(false);
	{ //
		my_entity->add_component(gv_actor_3d::static_class());
		gv_actor_3d* pa = my_entity->get_component< gv_actor_3d >();
		pa->set_local_aabb(gv_boxi(gv_vector3i(-300, -300, -300), gv_vector3i(300, 300, 300)));
		pa->set_position(pos);
		pa->create_base_collider();
		pa->set_velocity(v);
		pa->set_bouncingness(900);
		pa->update_grids();
		pa->set_mass(200);
	}
}

bool tick_control(gv_entity* entity)
{

	gv_vector3 dir;
	gv_entity* my_camera = entity;
	static gv_float step = 2.f;
	float dt = 1.f;
	float ddt = entity->get_sandbox()->get_delta_time();
	gv_actor_3d* a = entity->get_component< gv_actor_3d >();
	GV_ASSERT(a);
	gv_vector3i v_old = a->get_velocity();
	gv_vector3 v;
	gv_math_fixed::to_float(a->get_velocity(), v);
	v.y = 0;
	dir = -my_camera->get_tm().axis_z;
	dir.y = 0;
	rot = my_camera->get_rotation();
	if (get_sandbox()->get_input_manager()->is_key_down(e_key_shift))
	{
		dt *= 10;
	}
	if (get_sandbox()->get_input_manager()->is_key_down(e_key_w))
	{
		v = dir * step * dt;
	}
	if (get_sandbox()->get_input_manager()->is_key_down(e_key_s))
	{
		v = -dir * step * dt;
	}
	if (get_sandbox()->get_input_manager()->is_key_just_pressed(e_key_lbutton))
	{
		v = dir * step * dt;
		gv_vector3i di;
		gv_math_fixed::to_fixed(dir, di);
		spawn_a_ball(a->get_world(), a->get_position() + di + gv_vector3i(0, 600, 0), (di + gv_vector3i(0, 400, 0)) * 10);
	}
	gv_matrix43 mat;
	gv_math::convert(mat, rot);
	if (get_sandbox()->get_input_manager()->is_key_down(e_key_left))
	{
		v = mat.axis_x * dt;
	}
	if (get_sandbox()->get_input_manager()->is_key_down(e_key_right))
	{
		v = -mat.axis_x * dt;
	}
	{
		bool down = get_sandbox()->get_input_manager()->is_key_down(e_key_lbutton);
		if (m_is_drag && !down)
		{
			m_is_drag = false;
		}
		m_is_drag = down;
		m_last_mouse_pos = m_cu_mouse_pos;
		get_sandbox()->get_input_manager()->get_mouse_pos(m_cu_mouse_pos);
		if (m_is_drag)
		{
			gv_vector2i delta = m_cu_mouse_pos - m_last_mouse_pos;
			rot.yaw -= gvt_clamp(delta.x, -100, +100) * ddt;
			//rot.roll-=gvt_clamp(delta.y,-100,+100)*dt;
		}
	}
	{
		gv_float step = get_sandbox()->get_input_manager()->get_mouse_wheel_delta() * (dt);
		v += dir * step;
		get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
	}
	if (get_sandbox()->get_input_manager()->is_key_just_pressed(e_key_rbutton) && a->get_position().y < 500)
	{
		v.y = 30.f;
	}
	gv_euleri e;
	gv_math_fixed::to_fixed(rot, e);
	a->get_world()->rotate_actor(a, e);
	gv_vector3i vi;
	gv_math_fixed::to_fixed(v, vi);
	vi.y += v_old.y;
	a->add_impulse_to_control_velocity(vi, gv_vector3i(3000, 30000, 3000));

	rot = entity->get_rotation();
	//entity->set_rotation(rot);
	//entity->set_position(pos);
	//entity->update_matrix();
	get_sandbox()->get_input_manager()->tick();
	return true;
}
void test_info()
{
	gv_map_3d_init_text info;
	info.m_size = gv_vector3i(128, 128, 16);
	info.m_type_pairs.add(gv_int_string_pair(1, "Zombie"));
	info.m_type_pairs.add(gv_int_string_pair(2, "Spide"));
	for (int y = 0; y < 128; y++)
	{
		info.m_map += "\\";
		info.m_map += gv_endl;
		for (int x = 0; x < 128; x++)
		{
			if (y < 80)
				info.m_map += "1";
			else if (y > 90)
				info.m_map += "2";
			else
				info.m_map += " ";
		}
	}
	gv_string_tmp text;
	info.static_class()->export_to_xml(gvt_byte_ptr(info), text, 0);
	gv_save_string_to_file("map.xml", text);
	gv_map_3d_init_text info2;
	/*gv_xml_parser ps; 
		ps.load_string(text);*/
	gvt_load_option_file(info2, "map.xml");
	if (info.m_size == info2.m_size)
	{
	}
	//info2.static_class()->import_from_xml(gvt_byte_ptr(this), &ps, false);
}

void main(gvt_array< gv_string >& args)
{
	m_is_drag = false;
	m_cu_mouse_pos.set(0, 0);
	m_last_mouse_pos.set(0, 0);
	gv_bool is_server = args.find("server");
	gv_bool no_jump = args.find("no_jump");
	{
		gv_unit_test_context_guard context;
		test_info();
		sub_test_timer timer("unit_test_world_3d");
		gv_math_fixed math;
		math.rebuild_sin_table();
		gv_global::rnd_opt.m_clear_color = gv_color::BLUE_D();
		//============>>INIT=================================================>>
		m_sandbox->register_processor(gv_world_3d::static_class(), gve_event_channel_world);
		gv_global::rnd_opt.m_max_debug_line = 10000;
		gv_global::rnd_opt.m_max_debug_tri = 1000;
		gv_world_3d* my_world = gvt_cast< gv_world_3d >(m_sandbox->get_event_processor(gve_event_channel_world));
		gv_boxi world_box(gv_vector3i(0, 0, 0), gv_vector3i(20 * 1000, 20 * 1000, 20 * 1000));
		my_world->init_map_size(world_box, gv_vector2i(1000, 1000));

		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("test_world"));
		my_world->set_owner(my_mod);
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), my_world);
		gv_com_skeletal_mesh* com_sk_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >(my_entity);
		gv_material* my_material = m_sandbox->create_object< gv_material >();
		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gv_static_mesh* my_static_mesh = NULL;
		gv_ani_set* my_animation = NULL;
		gv_effect* my_effect = NULL;
		gvt_random< gv_float > m_random;

		gvt_array_cached< gv_effect*, 256 > effect_table;
		int current_idx = 0;
		create_skybox(my_world);
		create_ground(my_world, gv_vector3(0.1f, -5, 0.1f), gv_vector3(99.9f, -0.1f, 49.9f));
		//my_world->set_fixed_delta_time(true);
		m_sandbox->get_event_processor(gve_event_channel_render)->set_fixed_delta_time(true, 0.033f);

		/*{//load model & animation
				gv_string_tmp	file_name= FILE_FBX_HERON;
				gv_model * model= m_sandbox->create_object<gv_model> (my_mod);
				m_sandbox->import_external_format( model,file_name);
				if (!model->get_nb_animation()) return;
				my_animation=model->get_animation(0);
				my_static_mesh=model->get_skeletal_mesh(0)->m_t_pose_mesh;
				my_skeletal_mesh=model->get_skeletal_mesh(0);
				if (!my_static_mesh->get_nb_normal()) my_static_mesh->rebuild_normal();
				my_static_mesh->rebuild_binormal_tangent();
			}

			{//load material
				gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> (my_mod);
				gv_string_tmp  file_name= FILE_RFX_BIRD;
				m_sandbox->import_external_format( effect,*file_name);
				my_effect=effect->get_base_effect(); 
				my_effect->query_renderable_effect(effect_table);
				my_effect=my_effect->get_renderable_effect(gv_id("TexturedPhong"));
				my_material->m_effect=my_effect;

			}

			{//link animation & material
				com_sk_mesh->set_material(my_material);
				com_sk_mesh->set_resource(my_skeletal_mesh);
				com_sk_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_effect_renderer") );
				my_entity->add_component(com_sk_mesh); 
				gv_com_animation * com_ani=m_sandbox->create_object<gv_com_animation> ( my_entity);
				com_ani->set_resource(my_animation); 
				my_entity->add_component(com_ani);
				com_ani->play_animation(my_animation->get_sequence(0)->get_name_id(),0.f,true); 
			}*/

		my_entity->set_position(gv_vector3(30, 1, 30));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		my_entity->set_static(false);
		{ //
			my_entity->add_component(gv_actor_3d::static_class());
			my_entity->get_component< gv_actor_3d >()->set_local_aabb(gv_boxi(gv_vector3i(-500, 0, -500), gv_vector3i(500, 2000, 500)));
			gv_actor_3d* pa = my_entity->get_component< gv_actor_3d >();
			pa->create_base_collider();
			my_entity->get_component< gv_actor_3d >()->set_velocity(gv_vector3i(1000, 20, 1000));
		}

		my_world->set_main_actor(my_entity);
		if (no_jump)
			my_world->m_axis_freedom = gv_vector3i(1, 0, 1);
		while (1)
		{
			gv_vector3 pos;
			pos.x = m_random.get_uniform() * 20.f;
			pos.y = 1.f;
			pos.z = m_random.get_uniform() * 20.f;
			if (my_world->teleport_entity(my_entity, pos, gv_euler(0, 0, 0)))
			{
				gv_vector3 pos;
				pos.x = m_random.get_uniform() * 0.01f;
				pos.y = 0.1f;
				pos.z = m_random.get_uniform() * 0.01f;
				gv_vector3i pi;
				gv_math_fixed::to_fixed(pos, pi);
				my_entity->get_component< gv_actor_3d >()->set_velocity(pi);
				break;
			}
		};
		float r = 2; // my_static_mesh->get_bsphere().get_radius();
		gvt_array< gv_vector3 > original_pos;
		gv_entity* selected_entity = NULL;
		int loop = 200;
		bool quit = false;
		/*
			static bool test_batch=true;
			if (args.find("test_batch")  ) test_batch=true;
			if (test_batch)
			{
				gv_float f=164.f;
				my_effect->set_param(gv_id("frequency"),f);
				static gv_int batch_w=5; 
				static gv_int batch_h=5; 
				for ( int i=1; i< batch_h; i++)
					for (int j=1; j< batch_w; j++ )
					{
						gv_entity * new_entity=my_entity->clone(); 
						new_entity->set_position(gv_vector3 ( j*r*4, 1 ,i*r*4 ) );
						my_world->add_entity(new_entity);
						new_entity->update_matrix();
						while(1)
						{
							gv_vector3 pos;
							pos.x =m_random.get_uniform()*100.f;
							pos.y =0; 
							pos.z =m_random.get_uniform()*100.f;
							gv_vector3i pi;
							gv_math_fixed::to_fixed(pos, pi);
							if (my_world->teleport_actor(my_entity->get_component<gv_actor_3d>(), pi))
								break;
						};
					}
			}
			*/
		gv_com_camera* my_camera;
		{ //set camera;
			//my_entity->add_component(gv_id("gv_com_3d_lobby_controller") );
			gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_entity);
			my_camera = camera;
			camera->set_fov(60.f, 1.333f, 0.1f * r, 100 * r);
			camera->set_look_at(gv_vector3(0, 2, 5.f) * r, gv_vector3(0, 0, 0));
			camera->update_projection_view_matrix();
			my_entity->add_component(camera);
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		}
		if (args.size())
			args[0] >> loop;
		while (loop-- && !quit)
		{
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			if (loop < 100)
			{
				if (my_world->get_nb_entity())
					my_world->delete_entity(my_world->get_entity(my_world->get_nb_entity() - 1));
			}

			quit = !m_sandbox->tick();
			if (!my_entity->is_deleted())
				tick_control(my_entity);
			static bool last_key_down = false;
			if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
			{
				current_idx++;
				if (current_idx >= effect_table.size())
					current_idx = 0;
				my_material->m_effect = effect_table[current_idx];
			}
			last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_space);
			//draw names
			for (int i = 0; i < my_world->get_nb_entity(); i++)
			{
				gv_entity* pentity = my_world->get_entity(i);
				gv_com_skeletal_mesh* pcom = pentity->get_component< gv_com_skeletal_mesh >();
				if (pcom && pcom->is_visible_last_frame())
				{
					gv_vector2i string_pos = my_camera->world_to_window(pentity->get_position());
					gv_vector3 v;
					v = my_camera->window_to_world(string_pos);
					gv_string_tmp s;
					s << pentity->get_name();
					gv_global::debug_draw->draw_string(*s, string_pos, gv_color::WHITE());
				}
			}
			//test selection
			{
				gv_vector2i cursor;
				gv_global::input->get_mouse_pos(cursor);
				gv_vector3 v;
				v = my_camera->window_to_world(cursor);
				gv_vector3 o;
				o = my_camera->get_world_position();
				gv_line_segment line(o, o + (v - o) * 1000.f);
				if (selected_entity)
					selected_entity->set_selected(false);
				gv_vector3 hit_pos;
				//gv_global::debug_draw->draw_line_3d(line.start_p,line.end_p,gv_color::RED_B(), gv_color::RED_B());
				if (my_world->line_check(line, hit_pos, selected_entity))
				{
					gv_box b(hit_pos);
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_D());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_B());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::WHITE());
					if (selected_entity)
					{
						selected_entity->set_selected(true);
						gv_global::debug_draw->draw_box_3d(selected_entity->get_world_aabb(), gv_color::WHITE());
					}
					gv_global::debug_draw->draw_line_3d(o /*-my_entity->get_tm().get_axis_z()*/ + my_camera->get_world_dir() * 2, hit_pos, gv_color::BLUE_B(), gv_color::BLUE_B());
					//gv_global::debug_draw->draw_line_3d(my_entity->get_position(),hit_pos,gv_color::RED_B(), gv_color::RED_B());
					gv_global::debug_draw->draw_line_3d(my_entity->get_position() + gv_vector3(0, 5, 0), my_entity->get_position() + gv_vector3(0, 5, 0) + my_camera->get_world_dir() * 50, gv_color::GREEN(), gv_color::YELLOW());
				}
				gv_string_tmp s;
				s << cursor << "world " << v << " camera pos " << o;
				gv_global::debug_draw->draw_string(*s, gv_vector2i(0, 0), gv_color::WHITE());
			}
		}
		//================================================
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}
