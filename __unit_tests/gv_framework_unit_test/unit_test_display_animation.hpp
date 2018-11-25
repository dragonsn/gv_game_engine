namespace unit_test_display_animation
{
void main(gvt_array< gv_string >& args)
{

	bool do_compress = false;
	if (args.find("compress"))
	{
		do_compress = true;
	}
	gv_int idx;
	bool do_file = false;
	bool do_software_skinning = true;
	gv_string file_name;
	if (args.find("file", idx))
	{
		do_file = true;
		file_name = args[idx + 1];
	}
	bool t_pose_only = false;
	if (args.find("t_pose", idx))
	{
		t_pose_only = true;
	}

	gv_string mod_file_name;
	if (args.find("mod", idx))
	{
		mod_file_name = args[idx + 1];
	}

	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_importer_exporter_fbx");
		double start_time = m_sandbox->get_time_in_seconds();

		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));

		gv_entity* my_camera = m_sandbox->create_object< gv_entity >(gv_id("my_camera"), NULL);
		my_camera->add_component(gv_id("gv_com_cam_fps_fly"));
		gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_camera);
		my_camera->add_component(camera);

		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);

		gv_module* pmod = NULL;
		if (!do_file)
		{
			if (mod_file_name.size())
				pmod = m_sandbox->load_module(gv_id(*mod_file_name));
			else
				pmod = m_sandbox->try_load_module(gv_id("wolf"));
		}
		else
		{
			pmod = m_sandbox->create_object< gv_module >(gv_id(MOD_WOLF));
			gv_model* model = m_sandbox->create_object< gv_model >(pmod);
			m_sandbox->import_external_format(model, *file_name);
			m_sandbox->export_module(pmod->get_name_id());
		}

		gvt_object_iterator< gv_skeletal > it(m_sandbox);
		gv_skeletal* my_skeletal = NULL;
		if (!it.is_empty())
		{
			my_skeletal = it;
		};
		if (!my_skeletal)
		{
			return;
		}
		GV_ASSERT(my_skeletal);
		gv_ani_set* my_animation = NULL;
		gvt_object_iterator< gv_ani_set > it_ani(m_sandbox);
		if (!it_ani.is_empty())
		{
			my_animation = it_ani;
			if (do_compress)
			{
			}
		}

		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gvt_object_iterator< gv_skeletal_mesh > it_skeletal_mesh(m_sandbox);
		if (!it_skeletal_mesh.is_empty())
		{
			my_skeletal_mesh = it_skeletal_mesh;
			my_skeletal_mesh->optimize_bones();
		}
		if (my_skeletal_mesh)
		{
			gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), NULL);
			gv_static_mesh* my_static_mesh = my_skeletal_mesh->m_t_pose_mesh;
			if (do_software_skinning)
			{
				gv_com_static_mesh* pmesh0 = m_sandbox->create_object< gv_com_static_mesh >(my_entity);
				//gv_texture		*	  my_texture	=m_sandbox->create_object<gv_texture>(my_static_mesh);
				//gv_string_tmp		tex_file_name=FILE_TEX_SNOW_CUBEMAP;
				//my_texture->set_file_name(tex_file_name);
				//my_static_mesh->m_diffuse_texture=my_texture;
				pmesh0->set_resource(my_static_mesh);
				pmesh0->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_wire_frame_renderer"));
				//pmesh0->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_wire_frame_renderer") );
				my_entity->add_component(pmesh0);
			}
			else
			{
				gv_com_skeletal_mesh* pmesh0 = m_sandbox->create_object< gv_com_skeletal_mesh >(my_entity);
				pmesh0->set_resource(my_skeletal_mesh);
				my_entity->add_component(pmesh0);
			}

			my_world->add_entity(my_entity);

			float r = my_static_mesh->get_bsphere().get_radius();
			camera->set_fov(60, 1.333f, 0.1f * r, 100 * r);
			camera->set_look_at(gv_vector3(0, 1, 5.f) * r, gv_vector3(0, 0, 0));
			camera->update_projection_view_matrix();
		}
		//============>>GO GO GO !!
		int loop = 1000;
		if (args.size())
			args[0] >> loop;
		bool quit = false;

		gv_float scale_factor = 1.0f;
		//if (args.size()>1 ) args[1]>>scale_factor;
		gvt_array< gv_vector3 > original_pos;
		bool pause = false;
		while (loop-- && !quit)
		{
			static int index_seq = 0;
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			quit = !m_sandbox->tick();
			gvi_debug_renderer* pdebug = gv_global::debug_draw.get();
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(10, 0, 0), gv_color::RED(), gv_color::RED_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 10, 0), gv_color::GREEN(), gv_color::GREEN_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 0, 10), gv_color::BLUE(), gv_color::BLUE_B());
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
				pdebug->draw_string(*info, gv_vector2i(100, 20), gv_color::BLUE_D());
			}

			{
				static bool last_key_down = false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
				{
					index_seq++;
					start_time = m_sandbox->get_time_in_seconds();
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_space);
			}

			{
				static bool last_key_down = false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_p) && !last_key_down)
				{
					pause = !pause;
				}
				last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_p);
			}
			//======================================================
			//ANIMATE THE MESH
			my_skeletal->m_root_tm.set_identity();
			if (!t_pose_only && my_animation && my_skeletal && !pause)
			{
				gv_double time = m_sandbox->get_time_in_seconds() - start_time;
				gv_ulong frame = (gv_ulong)(time * 30.0);
				if (index_seq >= my_animation->get_nb_sequence())
				{
					index_seq = 0;
				}
				gv_ani_sequence* pseq = my_animation->get_sequence(index_seq);
				if (pseq)
				{
					int nb_track = pseq->get_track_number();
					if (!nb_track)
						continue;
					gv_float d = pseq->get_duration();
					gv_vector3 pos, scale;
					gv_quat q;
					for (int i = 0; i < nb_track; i++)
					{
						gv_ani_track* ptrack = pseq->get_track(i);
						ptrack->get_trans_rot((float)time, pos, q, scale, true);
						pos *= scale_factor;
						my_skeletal->set_bone_local_rotation_trans(ptrack->get_name_id(), q, pos, scale);
					}
				}
				my_skeletal->update_world_matrix();
			}
			//======================================================
			//DRAW THE SKELETAL
			if (my_skeletal)
			{
				for (int i = 0; i < my_skeletal->m_bones.size(); i++)
				{
					gv_bone& bone = my_skeletal->m_bones[i];
					if (bone.m_hierachy_depth)
					{
						gv_bone& father = my_skeletal->m_bones[bone.m_parent_idx];
						pdebug->draw_line_3d(bone.m_tm.get_trans(), father.m_tm.get_trans(), gv_color::WHITE(), gv_color::BLACK());
					}
				}
			}
			//======================================================
			//MORPH THE VERTEX!!
			//SIN WAVE MORPH..
			/*
				if (my_skeletal_mesh)
				{
					gv_static_mesh * pmesh=my_skeletal_mesh->m_t_pose_mesh;
					if (pmesh && pmesh->m_vertex_buffer)
					{
						gv_vertex_buffer *vb=pmesh->m_vertex_buffer; 
						if (!original_pos.size()) original_pos=vb->m_raw_pos;
						for ( int i=0; i< vb->m_raw_pos.size(); i++)
						{
							gv_vector3 &v3=vb->m_raw_pos[i];
							v3.x=original_pos[i].x+sinf(v3.y+m_sandbox->get_time_in_seconds()*10.f)*0.4f;
						}
						vb->set_dirty();
					}
				}
				*/
			static bool draw_skin = true;
			if (!t_pose_only && my_skeletal_mesh && draw_skin)
			{
				gv_static_mesh* pmesh = my_skeletal_mesh->m_t_pose_mesh;
				if (pmesh && pmesh->m_vertex_buffer)
				{
					gv_vertex_buffer* vb = pmesh->m_vertex_buffer;
					if (!original_pos.size())
						original_pos = vb->m_raw_pos;
					for (int i = 0; i < vb->m_raw_pos.size(); i++)
					{
						gv_vector3& v3 = vb->m_raw_pos[i];
						v3 = 0;
						for (int j = 0; j < 4; j++)
						{
							gv_short bone_index = (gv_short)vb->m_raw_blend_index[i][j];
							if (bone_index == -1)
								continue;
							gv_bone* pbone = NULL;
							if (!my_skeletal_mesh->m_bone_mapping.size())
							{
								pbone = &my_skeletal_mesh->m_skeletal->m_bones[bone_index];
							}
							else
							{
								pbone = &my_skeletal_mesh->m_skeletal->m_bones[my_skeletal_mesh->m_bone_inv_mapping[bone_index]];
							}
							gv_bone& bone = *pbone;
							v3 += original_pos[i] * bone.m_matrix_model_to_bone * bone.m_tm * vb->m_raw_blend_weight[i][j];
						}
						if (v3.is_almost_zero())
						{
							int c = 1;
						}
					}
					vb->set_dirty();
				}
			} //draw t-pose

		} //loop

	} //context

} //main
}
