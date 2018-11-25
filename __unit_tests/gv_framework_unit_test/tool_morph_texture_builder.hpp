namespace tool_morph_texture_builder 
{
	// mod ib_grunting mesh 
	void display(gv_model * pmodel, int loop, gvt_array<gv_image_2d>& images, gvt_array<gv_vector3 >&  parray, int max_frame = 1000,float diff_factor=1);
	void proecess_fbx_batch(gv_string directory)
	{
		//const 
		gv_file_manager * fm = gv_global::fm.get(); 
		gvt_array<gv_string> files; 
		fm->find_file_with_pattern_with_boost(".fbx", files, directory); 
		fm->find_file_with_pattern_with_boost(".FBX", files, directory);
		//gvt_for_each()
		int count = 0; 
		files.sort();
		//lambda 
		gvt_for_each(files,
				[&count](const gv_string& str) { std::cout << "file: "<< count<<": " << str << std::endl; }
		);
		if (files.size())
		{
			gv_unit_test_context_guard context;
			sub_test_timer timer("proecess_fbx_batch");
			gv_string mesh_name = fm->get_main_name(files[0]); 
			gv_module * my_mod = m_sandbox->create_object<gv_module>(gv_id("temp"));
			gv_model * model = m_sandbox->create_object<gv_model>(my_mod);
			{
				
				bool ret=m_sandbox->import_external_format(model, *files[0]);
				if (!ret) { std::cout << "failed to import, abort!!" << std::endl; return; }
				gv_skeletal_mesh * pskeletal_mesh = model->get_skeletal_mesh(0);
				if (!pskeletal_mesh) return;
				//export_the_OBJ
				gv_string obj_name = files[0]; 
				obj_name.replace_all(".FBX", ".OBJ"); 
				obj_name.replace_all(".fbx", ".OBJ");
				m_sandbox->export_external_format(pskeletal_mesh->get_t_mesh(), *obj_name);
			}
			//import animation 

			gv_ani_set *ani = model->get_animation(0);
			if (!ani)
			{
				m_sandbox->create_object<gv_ani_set>(model);
				model->add_animation(ani);
			}

			for (int i = 1; i < files.size(); i++)
			{
				gv_string ani_name = fm->get_main_name(files[i]); 
				ani_name.replace_all(mesh_name, ""); 
				gv_model * model_temp = m_sandbox->create_object<gv_model>(my_mod);
				bool ret = m_sandbox->import_external_format(model_temp, *files[i]);
				if (!ret) { std::cout << "failed to import, abort!!" << std::endl; return; }
				gv_ani_set * cu_ani = model_temp->get_animation(0);
				if (!cu_ani) continue;;
				gv_ani_sequence *ani_sq = cu_ani->get_sequence(0);
				if (!ani_sq) continue; 
				ani_sq->rename(ani_name);
				ani->add_sequence(ani_sq); 
				m_sandbox->delete_object_tree(model_temp); 
			}
			m_sandbox->export_module(my_mod->get_name_id()); 

			gvt_array<gv_image_2d> images;
			gvt_array<gv_string> names;
			gvt_array<gv_int >length;
			gvt_array<gv_vector3 > parray;
			gvt_array<gv_vector3 > narray;
			gv_string_tmp output;
			model->build_morph_texture(0, 0, images, length, names, 1024, parray, narray, true, output, 0,0,true);
			
			files[0].replace_all(".fbx", ".FBX");
			gv_string tex_name = files[0];
			tex_name.replace_all(".FBX", "_XY.TGA");
			images[0].export_file(*tex_name);

			tex_name = files[0];
			tex_name.replace_all(".FBX", "_ZW.TGA");
			images[1].export_file(*tex_name);

			tex_name = files[0];
			tex_name.replace_all(".FBX", "_NXY.TGA");
			images[2].export_file(*tex_name);

			tex_name = files[0];
			tex_name.replace_all(".FBX", "_NZW.TGA");
			images[3].export_file(*tex_name);
			
			tex_name = files[0];
			tex_name.replace_all(".FBX", "_OUTPUT.LOG");
			gv_save_string_to_file(*tex_name, output);

			display(model, 100000, images, parray, 500,0);
		}

		//gv_string file_main_name=fm->get_main_name(file)
		//fm->find_file_with_pattern_with_boost()
		
	}

	//=======================================================================
	void display(gv_model * pmodel, int loop , gvt_array<gv_image_2d>& images, gvt_array<gv_vector3 >&  parray, int max_frame,float diff_factor)
	{
		int cu_frame = 0;

		//do display result
		m_sandbox->register_processor(gv_world::static_class(), gve_event_channel_world);
		gv_world * my_world = gvt_cast<gv_world> (m_sandbox->get_event_processor(gve_event_channel_world));
		gv_entity * my_camera = m_sandbox->create_object<gv_entity>(gv_id("my_camera"), NULL);
		my_camera->add_component(gv_id("gv_com_cam_fps_fly"));
		gv_com_camera  * camera = m_sandbox->create_object<gv_com_camera>(gv_id("main_camera"), my_camera);
		my_camera->add_component(camera);
		GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		my_world->add_entity(my_camera);

		//GV_ASSERT(pmod);
		gv_skeletal_mesh * my_skeletal_mesh = pmodel->get_skeletal_mesh(0);
		if (!my_skeletal_mesh) return;
		gv_skeletal * my_skeletal = my_skeletal_mesh->get_skeletal();
		gv_entity * my_entity = m_sandbox->create_object<gv_entity>(gv_id("entity"), NULL);
		gv_static_mesh *	  my_static_mesh = my_skeletal_mesh->m_t_pose_mesh;
		my_static_mesh->get_vb()->m_raw_pos = parray;
		m_sandbox->export_external_format(my_static_mesh, "E:\\mag.obj");
		gv_vertex_buffer *vb = my_static_mesh->get_vb();
		gv_string_tmp inline_file;
		for (int i = 0; i < my_static_mesh->get_nb_vertex(); i++)
		{
			gv_vector3 v = vb->m_raw_pos[i];
			inline_file << "FVector(" << v.x << "," << v.y << "," << v.z << ")," << gv_endl;
		}
		gv_save_string_to_file("e:\\inline.txt", inline_file);

		gv_com_static_mesh  * pmesh0 = m_sandbox->create_object<gv_com_static_mesh>(my_entity);
		pmesh0->set_resource(my_static_mesh);
		pmesh0->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_wire_frame_renderer"));
		my_entity->add_component(pmesh0);

		my_world->add_entity(my_entity);
		float r = my_static_mesh->get_bsphere().get_radius();
		camera->set_fov(60, 1.333f, 0.1f*r, 100 * r);
		camera->set_look_at(gv_vector3(0, 5, 5.f)*r, gv_vector3(0, 0, 0));
		camera->update_projection_view_matrix();
		bool quit = false;

		gv_float scale_factor = 1.0f;
		gvt_array<gv_vector3> original_pos;
		bool pause = false;
		cu_frame = 0;
		while (loop-- && !quit)
		{

			static int index_seq = 0;
			if (loop == 1)
			{
				gv_object_event_render_uninit * pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			quit = !m_sandbox->tick();
			gvi_debug_renderer * pdebug = gv_global::debug_draw.get();
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(100, 0, 0), gv_color::RED(), gv_color::RED_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 100, 0), gv_color::GREEN(), gv_color::GREEN_B());
			pdebug->draw_line_3d(gv_vector3(0, 0, 0), gv_vector3(0, 0, 100), gv_color::BLUE(), gv_color::BLUE_B());
			gv_string_tmp info;

			//======================================================
			//ANIMATE THE MESH
			my_skeletal->m_root_tm.set_identity();
			//======================================================
			//DRAW THE SKELETAL
			static bool draw_skin = true;
			if (my_skeletal_mesh&&draw_skin)
			{
				gv_static_mesh * pmesh = my_skeletal_mesh->m_t_pose_mesh;
				if (pmesh && pmesh->m_vertex_buffer)
				{
					gv_vertex_buffer *vb = pmesh->m_vertex_buffer;
					if (!original_pos.size()) original_pos = vb->m_raw_pos;
					for (int i = 0; i < vb->m_raw_pos.size(); i++)
					{
						gv_color cxy = images[0].get_pixel(gv_vector2i(i, cu_frame));
						gv_color czw = images[1].get_pixel(gv_vector2i(i, cu_frame));
						gv_byte c[2];
						gv_vector3 offset;
						float f;

						c[0] = cxy.r; c[1] = cxy.g;
						gvt_decode_byte_to_zero_to_one(f, c);
						offset.x = gvt_restore_normalized_value<gv_float>(f, -512, 512);

						c[0] = cxy.b; c[1] = cxy.a;
						gvt_decode_byte_to_zero_to_one(f, c);
						offset.y = gvt_restore_normalized_value<gv_float>(f, -512, 512);

						c[0] = czw.r; c[1] = czw.g;
						gvt_decode_byte_to_zero_to_one(f, c);
						offset.z = gvt_restore_normalized_value<gv_float>(f, -512, 512);

						gv_vector3 &v3 = vb->m_raw_pos[i];
						v3 = offset+parray[i]* diff_factor;

						gv_color nxy = images[2].get_pixel(gv_vector2i(i, cu_frame));
						gv_color nzw = images[3].get_pixel(gv_vector2i(i, cu_frame));

						gv_vector3 n3;
						c[0] = nxy.r; c[1] = nxy.g;
						gvt_decode_byte_to_zero_to_one(f, c);
						f = gvt_restore_normalized_value<gv_float>(f, -1, 1);
						n3.x = f;

						c[0] = nxy.b; c[1] = nxy.a;
						gvt_decode_byte_to_zero_to_one(f, c);
						f = gvt_restore_normalized_value<gv_float>(f, -1, 1);
						n3.y = f;

						c[0] = nxy.r; c[1] = nxy.g;
						gvt_decode_byte_to_zero_to_one(f, c);
						f = gvt_restore_normalized_value<gv_float>(f, -1, 1);
						n3.z = f;

						//pdebug->draw_line_3d(v3, v3 + n3*100.f, gv_color::YELLOW(), gv_color::GREY());
					}
					vb->set_dirty();
				}
			}//draw t-pose

			cu_frame = (gv_int)(m_sandbox->get_time_in_seconds() * 30);
			cu_frame %= (max_frame + 50);

		}//loop 



	}
	void main(gvt_array<gv_string > &args)
	{
		//console tool tool_morph_texture_builder -fbx -"E:/raw_data/infinity_blade/ExoGame_Gruntling"
		//gv_string_tmp mesh_name = "(3)wolf:0/magmonk_model:0/SK_MESH_MOD_007_Magmonk_001:0";
		//gv_string_tmp ani_name = "(3)wolf:0/magmonk_model:0/SK_MESH_MOD_007_Magmonk_001:0";
		gv_string mod_file_name = "magmonk";
		bool do_compress = false;
		gv_int idx = 0;
		int loop = 1000;
		if (args.size()) args[0] >> loop;
		if (args.find("fbx",idx))
		{
			return proecess_fbx_batch(args[idx + 1]);
		}
		if (args.find("compress"))
		{
			do_compress = true;
		}
		
		bool do_software_skinning = true;
		gv_string file_name;
		if (args.find("mod", idx))
		{
			mod_file_name = args[idx + 1];
		}
		/*if (args.find("mesh", idx))
		{
			mesh_name = args[idx + 1];
		}*/
		{
			gv_unit_test_context_guard context;
			sub_test_timer timer("tool_morph_texture_builder");
			gv_module * pmod = NULL;
			if (mod_file_name.size())
				pmod = m_sandbox->load_module(gv_id(*mod_file_name));
			if (!pmod) return;
			gv_model * pmodel = NULL;
			gvt_object_iterator<gv_model> it_model(m_sandbox);
			if (!it_model.is_empty())
			{
				pmodel = it_model;
			}
			if (!pmodel) return; 
			gvt_array<gv_image_2d> images;
			gvt_array<gv_string> names; 
			gvt_array<gv_int >length;
			gvt_array<gv_vector3 > parray;
			gvt_array<gv_vector3 > narray;
			gv_string_tmp output;
			pmodel->build_morph_texture(0, 0, images, length, names, 1024, parray,narray,true,output);
			gv_text error;
			gv_save_string_to_file("morph.txt", output);
			images[0].export_file("xy.tga");
			images[1].export_file("zw.tga");
			images[2].export_file("nxy.tga");
			images[3].export_file("nzw.tga");
			
			
			int max_frame = *length.last(); 
		}//context

	}//main
	
}


