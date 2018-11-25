//============================================================================================
//								:
//============================================================================================

namespace unit_test_3d_lobby
{
#include "2d_bricks_maps.h"
static int lobby_max_player = 1;

void main(gvt_array< gv_string >& args)
{
	gv_bool is_server = args.find("server");
	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_3d_lobby");
		//============>>INIT=================================================>>
		m_sandbox->register_processor(gv_id("gv_world_rpg2d"), gve_event_channel_world);
		gv_global::rnd_opt.m_max_debug_line = 10000;
		gv_global::rnd_opt.m_max_debug_tri = 1000;
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));
		{
			gv_vector3 min_p = to_3d(gv_vector2(0, 0));
			min_p.y = -100;
			gv_vector3 max_p = to_3d(gv_vector2(8000, 3000));
			max_p.y = 100;
			my_world->init(1, gv_vector2i(map_width, map_height), gv_box(min_p, max_p), the_map);
		}
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
		create_ground(my_world);

		{ //load model & animation
			gv_string_tmp file_name = FILE_FBX_HERON;
			gv_model* model = m_sandbox->create_object< gv_model >(my_mod);
			if (!m_sandbox->import_external_format(model, file_name))
				return;
			my_animation = model->get_animation(0);
			my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
			my_skeletal_mesh = model->get_skeletal_mesh(0);
			if (!my_static_mesh->get_nb_normal())
				my_static_mesh->rebuild_normal();
			my_static_mesh->rebuild_binormal_tangent();
		}

		{ //load material
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_mod);
			gv_string_tmp file_name = FILE_RFX_BIRD;
			m_sandbox->import_external_format(effect, *file_name);
			my_effect = effect->get_base_effect();
			my_effect->query_renderable_effect(effect_table);
			my_effect = my_effect->get_renderable_effect(gv_id("TexturedPhong"));
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
		{ //
			my_entity->add_component(gv_id("gv_com_rpg2d_actor"));
		}
		my_entity->set_position(gv_vector3(30, 1, 30));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		my_world->set_main_actor(my_entity);

		while (1)
		{
			gv_vector3 pos;
			pos.x = m_random.get_uniform() * 1000.f;
			pos.y = 0;
			pos.z = m_random.get_uniform() * 1000.f;
			if (my_world->teleport_entity(my_entity, pos, gv_euler::get_zero_rotation()))
				break;
		};
		float r = my_static_mesh->get_bsphere().get_radius();
		gvt_array< gv_vector3 > original_pos;

		int loop = 200;
		bool quit = false;
		static bool test_batch = true;
		if (args.find("test_batch"))
			test_batch = true;
		if (test_batch)
		{
			gv_float f = 164.f;
			my_effect->set_param(gv_id("frequency"), f);
			static gv_int batch_w = 31;
			static gv_int batch_h = 31;
			for (int i = 1; i < batch_h; i++)
				for (int j = 1; j < batch_w; j++)
				{
					gv_entity* new_entity = my_entity->clone();
					new_entity->set_position(gv_vector3(j * r * 4, 1, i * r * 4));
					gv_com_rpg2d_actor* pactor = new_entity->get_component< gv_com_rpg2d_actor >();
					pactor->m_auto_move = true;
					my_world->add_entity(new_entity);
					new_entity->update_matrix();
					while (1)
					{
						gv_vector3 pos;
						pos.x = m_random.get_uniform() * 1000.f;
						pos.y = 0;
						pos.z = m_random.get_uniform() * 1000.f;
						if (my_world->teleport_entity(new_entity, pos, gv_euler::get_zero_rotation()))
							break;
					};
				}
		}

		gv_com_camera* my_camera;
		{ //set camera;
			my_entity->add_component(gv_id("gv_com_3d_lobby_controller"));
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
				my_world->delete_entity(my_world->get_entity(my_world->get_nb_entity() - 1));
			}

			quit = !m_sandbox->tick();
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
				gv_entity* entity = NULL;
				gv_vector3 hit_pos;
				//gv_global::debug_draw->draw_line_3d(line.start_p,line.end_p,gv_color::RED_B(), gv_color::RED_B());
				if (my_world->line_check(line, hit_pos, entity))
				{
					gv_box b(hit_pos);
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_D());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_B());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::WHITE());
					if (entity)
					{
						gv_global::debug_draw->draw_box_3d(entity->get_world_aabb(), gv_color::WHITE());
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
