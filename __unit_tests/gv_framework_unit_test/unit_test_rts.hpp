//============================================================================================
//								:
//============================================================================================

#include "unit_test_rts.h"
#include "unit_test_rts_imp.hpp"

#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "unit_test_rts_classes.h"



namespace gv
{

	void register_rts_test_classes()
	{
		gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "unit_test_rts_classes.h"
		sandbox->init_classes();
	};
	void unregister_rts_test_classes()
	{
		gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "unit_test_rts_classes.h"
	}
}
namespace unit_test_rts
{

static int lobby_max_player = 1;
static int char_count = 0;
static gv_vector3 target_pos = gv_vector3(300, 1, 300);
void spawn_army(gv_world_3d* my_world, gv_vector3 pos = gv_vector3(10, 1, 10),
				gv_vector2i army_size = gv_vector2i(16, 16), float space = 2,
				int faction = 0, int team = 0 	)
{
	for (int y = 0; y < army_size.y; y++)
		for (int x = 0; x < army_size.x; x++)
		{
			
			gv_string_tmp s; 
			s << "U" << char_count++;
			gv_entity* my_entity =
				m_sandbox->create_object< gv_entity >( gv_id(*s),my_world);
			float r = 2;
			my_entity->set_position(gv_vector3(space * x, 1, space * y) + pos);
			my_entity->set_local_aabb(gv_box::static_unit_box());
			my_entity->update_matrix();
			my_world->add_entity(my_entity);

			my_entity->add_component(gv_rts_unit::static_class());
			my_entity->set_static(false);
			gv_rts_unit* pa = my_entity->get_component< gv_rts_unit >();
			pa->sync_from_entity(my_entity);
			pa->create_base_collider();
			gv_uint sx = my_world->get_random_for_gameplay().get_ranged(0, 3000);
			gv_uint sz = my_world->get_random_for_gameplay().get_ranged(0, 3000);
			my_entity->get_component< gv_rts_unit >()->set_velocity(gv_vector3i(0, 0, 0));
			pa->set_target_pos( static_invalid_location());

			gv_rts_unit_ai * pai=my_entity->add_component<gv_rts_unit_ai>();
			pai->set_faction(faction);
			pai->set_team(team);
		}
}

void spawn_obstacle(gv_world_3d* my_world, gv_vector3 pos = target_pos)
{
	
	gv_string_tmp s;
	s << "O" << char_count++;
	gv_entity* my_entity =
	m_sandbox->create_object< gv_entity >(gv_id(*s), my_world);
	float r = 2;
	my_entity->set_position(  pos);
	gv_box box = gv_box::static_unit_box();
	box.extend(gv_vector3(1, 5, 1));
	my_entity->set_local_aabb(box);
	my_entity->update_matrix();
	my_world->add_entity(my_entity);

	my_entity->add_component(gv_rts_unit::static_class());
	my_entity->set_static(true);
	gv_rts_unit* pa = my_entity->get_component< gv_rts_unit >();
	pa->sync_from_entity(my_entity);
	pa->create_base_collider();
	
}

void main(gvt_array< gv_string >& args)
{

	gv_bool is_server = args.find("server");
	gv_bool no_jump = args.find("no_jump");
	int loop = 200;
	bool quit = false;
	if (args.size())
		args[0] >> loop;
	

	if (loop)
	{
		gv_unit_test_context_guard context;
		register_rts_test_classes();
		sub_test_timer timer("unit_test_rts");
		gv_global::rnd_opt.m_max_debug_line = 60000;
		gv_global::rnd_opt.m_max_debug_tri = 30000;
		gv_global::rnd_opt.m_clear_color = gv_color::BLACK();

		gv_math_fixed math;
		math.rebuild_sin_table();

		// create world
		gv_world_3d* my_world = m_sandbox->register_processor<gv_world_3d>();
		my_world->set_autonomous(false);
		my_world->set_synchronization(true);
		gv_boxi world_box(gv_vector3i(0, 0, 0),
						  gv_vector3i(1024 * 1000, 20 * 1000, 1024 * 1000));
		my_world->init_map_size(world_box, gv_vector2i(2000, 2000));
		my_world->m_axis_freedom = gv_vector3i(1, 0, 1);
		if (no_jump)
			my_world->m_axis_freedom = gv_vector3i(1, 0, 1);
		gv_box b = gv_box::static_unit_box();
		b.scale_box(gv_vector3(1000, 1, 1000));
		my_world->get_entity(0)->set_local_aabb(b);
		my_world->get_entity(0)->update_matrix();
		//create AI system
		gv_rts_ai_system* my_ai = m_sandbox->register_processor<gv_rts_ai_system>();
		my_ai->set_autonomous(true);
		my_ai->set_synchronization(true);

		// create camera
		gv_entity* my_entity =
			m_sandbox->create_object< gv_entity >(gv_id("entity"), my_world);
		float r = 2;
		my_entity->set_position(gv_vector3(30, 1, 30));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		my_entity->set_static(false);
		gv_com_camera* my_camera = NULL;
		my_entity->add_component(gv_id("gv_com_rts_controller"));
		my_camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_entity);
		my_camera->set_fov(60.f, 1.333f, 0.3f, 5000);
		my_camera->set_look_at(gv_vector3(0, 40, 0) * r, gv_vector3(0, 0, 40));
		my_camera->update_projection_view_matrix();
		my_entity->add_component(my_camera);
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = my_camera));
		my_world->set_main_actor(my_entity);

		// create army
		spawn_army(my_world, gv_vector3(10	,1,10)  ,gv_vector2i(8,8),2, 0,0 );
		spawn_army(my_world, gv_vector3(10	, 1, 50), gv_vector2i(8, 8), 2, 0,1);
		spawn_army(my_world, gv_vector3(50, 1, 50), gv_vector2i(8, 8), 2, 0,2);
		spawn_army(my_world, gv_vector3(50, 1, 10), gv_vector2i(8, 8), 2, 0,3);

		spawn_army(my_world,gv_vector3(80,1,80),gv_vector2i(8,8),2,1 ,0);
		spawn_army(my_world,gv_vector3(80, 1, 120), gv_vector2i(8, 8), 2, 1,1);

		spawn_obstacle(my_world);
		gv_bool first_frame = true;
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
					my_world->delete_entity(
						my_world->get_entity(my_world->get_nb_entity() - 1));
			}
			if (!first_frame)
			{
				// synchronization;
				m_sandbox->post_tick();
				int nb_visible = 0;
				// draw names and bounding box
				GV_PROFILE_EVENT(render_all_actors, 0);
				for (int i = 0; i < my_world->get_nb_entity(); i++)
				{
					gv_entity* pentity = my_world->get_entity(i);
					if (my_camera->get_world_frustum().intersect(pentity->get_world_aabb()))
					{
						
						gv_vector2i string_pos =
							my_camera->world_to_window(pentity->get_position());
						gv_vector3 v;
						v = my_camera->window_to_world(string_pos);
						gv_string_tmp s;
						s << pentity->get_name();
						gv_rts_unit_ai * pai = pentity->get_component<gv_rts_unit_ai>();
						gv_color  color = gv_color::RED(); 
						
						if (pai)
						{
							color = pai->get_faction() ? gv_color::RED() : gv_color::BLUE_B();
							if (pai->is_controlled_by_player())
							{
								gv_color c; 
								switch (pai->get_state())
								{
									case gv_rts_unit_ai::e_unit_ai_idle:  c = color;  break;
									case gv_rts_unit_ai::e_unit_ai_move:  c = gv_color::GREEN_B();  break;
									case gv_rts_unit_ai::e_unit_ai_attack:  c = gv_color::RED();  break;
									case gv_rts_unit_ai::e_unit_ai_dead:  c = gv_color::GREY_D();  break;
									default: c = color;
								}
								gv_global::debug_draw->draw_string(*s, string_pos, c);
							}
						}
						
						gv_global::debug_draw->draw_box_3d(pentity->get_world_aabb(),color);
						gv_rts_unit * punit = pentity->get_component<gv_rts_unit>();
						if (punit)
						{
							gv_global::debug_draw->draw_line_3d(pentity->get_position(),
								pentity->get_position() + gv_math_fixed::to_float(punit->get_velocity()),
								gv_color::BLACK(), gv_color::RED());
						}
						gv_global::debug_draw->draw_axis(pentity->get_tm());
						nb_visible++;
					}
				}

				gv_string_tmp s;
				s << "visible:" << nb_visible << " frametime=:" << (int)m_sandbox->get_delta_time() * 1000;
				gv_global::debug_draw->draw_string(*s, gv_vector2i(100, 100),
												   gv_color::YELLOW());
				if (get_sandbox()->get_input_manager()->is_key_down(e_key_q))
				{
					quit = true;
				}
			}
			gv_global::debug_draw->draw_grid_3d(gv_vector3(0, 0, 0), 500, 500, 5, 5);
			first_frame = false;
			//test selection
			{
				gv_vector2i cursor;
				gv_global::input->get_mouse_pos(cursor);
				gv_vector3 v = my_camera->window_to_world(cursor);
				gv_vector3 o= my_camera->get_world_position();
				gv_line_segment line(o, o + (v - o) * 1000.f);
				gv_entity* entity = NULL;
				gv_vector3 hit_pos;
				if (my_world->line_check(line, hit_pos, entity))
				{
					gv_box b(hit_pos);
					b.extend(gv_vector3(0.3f));	gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_D());
					b.extend(gv_vector3(0.3f));	gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_B());
					b.extend(gv_vector3(0.3f));	gv_global::debug_draw->draw_box_3d(b, gv_color::WHITE());
					if (entity)
					{
						gv_global::debug_draw->draw_box_3d(entity->get_world_aabb(), gv_color::WHITE());
					}
					gv_global::debug_draw->draw_line_3d(o /*-my_entity->get_tm().get_axis_z()*/ + my_camera->get_world_dir() * 2, hit_pos, gv_color::BLUE_B(), gv_color::BLUE_B());
					gv_global::debug_draw->draw_line_3d(my_entity->get_position() + gv_vector3(0, 5, 0), my_entity->get_position() + gv_vector3(0, 5, 0) + my_camera->get_world_dir() * 50, gv_color::GREEN(), gv_color::YELLOW());
					if (gv_global::input->is_key_just_pressed(gve_key::e_key_rbutton))
					{
						my_ai->exec_player_move_to(gv_math_fixed::to_fixed(hit_pos));
					}
				}
				if (gv_global::input->is_key_just_pressed(gve_key::e_key_space))
				{
					my_ai->exec_player_select_next_team();
				}
				gv_string_tmp s;
				s << cursor << "world " << v << " camera pos " << o;
				gv_global::debug_draw->draw_string(*s, gv_vector2i(0, 0), gv_color::WHITE());
			}
			
			
			my_world->set_logic_frame_time((int)(m_sandbox->get_delta_time() * 1000)+1);
			m_sandbox->do_tick();
			m_sandbox->pre_tick();
			
		}
		if (!first_frame)
		{
			m_sandbox->post_tick();
		}
		//================================================
		unregister_rts_test_classes();
	}

	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}