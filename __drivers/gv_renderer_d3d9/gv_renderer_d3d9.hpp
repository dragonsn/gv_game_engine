#pragma once

namespace gv
{
#include "gv_gpu_register_define.h"
D3DFORMAT gv_to_d3d_format(gve_pixel_format format)
{
	switch (format)
	{
#define GVM_DCL_PIXEL_FORMAT_CHANGE_PREFIX 1
#define GVM_PIXEL_PREFIX D3DFMT_
#include "gv_framework/renderer/gv_pixel_format.h"
	}
	GV_ASSERT(0);
	return D3DFMT_FORCE_DWORD;
};
//===========================================================================
const static int gvc_nb_shadow_map = 2;
const static int gvc_shadow_map_dimention = 1024;
class gv_renderer_d3d9_data : public gv_refable
{
public:
	gv_renderer_d3d9_data()
	{
		m_is_rendering_z_pass = false;
		m_current_pass = 0;
	}
	~gv_renderer_d3d9_data()
	{
	}
	gvt_ref_ptr< gv_texture > m_color_buffer_texture;
	gvt_ref_ptr< gv_texture > m_depth_buffer_texture;
	gvt_ref_ptr< gv_texture > m_shadow_map_textures[gvc_nb_shadow_map];
	gvt_ref_ptr< gv_texture > m_shadow_map_depth_buffer;
	gvt_ref_ptr< gv_texture > m_g_full_blur_buffer0;
	gvt_ref_ptr< gv_texture > m_g_full_blur_buffer1;
	gv_matrix44 m_view_light_matrix;
	gvt_array_static< gv_vector4, gvc_nb_shadow_map > m_shadow_scale_xyz;
	gv_vector4 m_scale_xyz_shadow_pass;
	gv_vector4 m_light_pos_world_shadow_pass;
	gvt_array_static< gv_vector4, gvc_nb_shadow_map > m_shadow_light_pos_world;
	gv_vector2i m_screen_size;
	gv_bool m_is_rendering_z_pass;
	gvt_ref_ptr< gv_component_renderer > m_effect_renderer;
	gvt_ref_ptr< gv_component_renderer > m_instancing_renderer;
	gvt_ref_ptr< gv_component_renderer > m_terrain_renderer;
	gvt_ref_ptr< gv_material > m_depth_only_pass_material;
	gvt_ref_ptr< gv_material > m_shadow_pass_material;
	gvt_ref_ptr< gv_component_renderer > m_shadow_map_renderer;
	gv_world_light_info m_light_info;
	gv_int m_current_pass;
};
//===========================================================================
gv_renderer_d3d9* s_gv_renderer_d3d9 = NULL;
gv_renderer_d3d9* gv_renderer_d3d9::static_get()
{
	return s_gv_renderer_d3d9;
}
gv_renderer_d3d9::gv_renderer_d3d9()
{
	m_current_render_frame = NULL;
	m_window_handle = NULL;
	GV_ASSERT(!s_gv_renderer_d3d9);
	s_gv_renderer_d3d9 = this;
	m_visible_test_tag = 0;
	GVM_SET_CLASS(gv_renderer_d3d9);
	m_private_data = new gv_renderer_d3d9_data;
	m_render_target_mgr = new gv_render_target_mgr_d3d;
	m_post_effect_mgr = new gv_post_effect_mgr_d3d;
	m_current_unified_life_time = 0;
};

gv_renderer_d3d9::~gv_renderer_d3d9()
{
	s_gv_renderer_d3d9 = NULL;
	if (gv_global::device_d3d9.try_get())
		destroy_d3d9();
};
/*old complicated way of doing things, keep as reference.
void gv_renderer_d3d9::register_events	()
{
        //gv_object_event_handler_c<gv_renderer_d3d9> * prec;
        //prec=new
gv_object_event_handler_c<gv_renderer_d3d9>(this,&gv_renderer_d3d9::on_event);
        //this->get_sandbox()->register_event(gv_object_event_id_render_init
,prec ,gve_event_channel_render );

        GVM_HOOK_EVENT(render_init,on_event,render);
        GVM_HOOK_EVENT(render_uninit,on_event,render);
        GVM_HOOK_EVENT(add_component ,on_event,render);
        GVM_HOOK_EVENT(remove_component,on_event,render);
        GVM_HOOK_EVENT(render_enable_pass, on_event,render);
        GVM_HOOK_EVENT(render_disable_pass, on_event,render);
        GVM_HOOK_EVENT(render_set_camera , on_event,render)
        GVM_HOOK_EVENT(render_set_ui_manager , on_event,render)
};

void gv_renderer_d3d9::unregister_events	()
{
        //this->get_sandbox()->unregister_event(gv_object_event_id_render_init,this
,gve_event_channel_render);
        GVM_UNHOOK_EVENT(render_init,render);
        GVM_UNHOOK_EVENT(render_uninit,render);
        GVM_UNHOOK_EVENT(add_component ,render);
        GVM_UNHOOK_EVENT(remove_component,render);
        GVM_UNHOOK_EVENT(render_enable_pass, render);
        GVM_UNHOOK_EVENT(render_disable_pass, render);
        GVM_UNHOOK_EVENT(render_set_camera ,render)
        GVM_UNHOOK_EVENT(render_set_ui_manager ,render)
};
*/

gv_int gv_renderer_d3d9::on_event(gv_object_event* pevent)
{
	switch (pevent->m_id)
	{
	case gv_object_event_id_render_init:
	{
		gv_object_event_render_init* pe =
			gvt_cast< gv_object_event_render_init >(pevent);
		GV_ASSERT(pe);
		this->m_window_handle = (HWND)pe->window_handle;
		this->init_d3d9();
	}
	break;
	case gv_object_event_id_render_uninit:
	{
		this->destroy_d3d9();
	}
	break;

	case gv_object_event_id_render_resize_window:
	{
		gv_object_event_render_resize_window* pe =
			gvt_cast< gv_object_event_render_resize_window >(pevent);
		pe->sx = pe->sx;

		D3DVIEWPORT9 port;
		port.X = 0;
		port.Y = 0;
		port.Width = pe->sx;
		port.Height = pe->sy;
		port.MinZ = 0;
		port.MaxZ = 1;
		GVM_VERIFY_D3D(get_device_d3d9()->SetViewport(&port));

		if (m_main_camera)
		{
			m_main_camera->set_ratio((float)pe->sx / (float)pe->sy);
			m_main_camera->update_projection_view_matrix();
		}

		get_device_d3d9()->SetViewport(&port);
	}
	break;
	case gv_object_event_id_add_component:
	{
		gv_object_event_add_component* pe =
			gvt_cast< gv_object_event_add_component >(pevent);
		GV_ASSERT(pe);
		this->add_a_renderable(gvt_cast< gv_com_graphic >(pe->component));
	}
	break;
	case gv_object_event_id_remove_component:
	{
		gv_object_event_remove_component* pe =
			gvt_cast< gv_object_event_remove_component >(pevent);
		GV_ASSERT(pe);
		this->remove_a_renderable(gvt_cast< gv_com_graphic >(pe->component));
	}
	break;

	case gv_object_event_id_render_enable_pass:
	{
		gv_object_event_render_enable_pass* pe =
			gvt_cast< gv_object_event_render_enable_pass >(pevent);
		GV_ASSERT(pe);
		this->enable_render_pass(pe->pass);
	}
	break;

	case gv_object_event_id_render_disable_pass:
	{
		gv_object_event_render_disable_pass* pe =
			gvt_cast< gv_object_event_render_disable_pass >(pevent);
		GV_ASSERT(pe);
		this->enable_render_pass(pe->pass, false);
	}
	break;

	case gv_object_event_id_render_set_camera:
	{
		gv_object_event_render_set_camera* pe =
			gvt_cast< gv_object_event_render_set_camera >(pevent);
		GV_ASSERT(pe);
		this->m_main_camera = gvt_cast< gv_com_camera >(pe->camera);
		GV_ASSERT(this->m_main_camera);
	}
	break;

	case gv_object_event_id_render_set_ui_manager:
	{
		gv_object_event_render_set_ui_manager* pe =
			gvt_cast< gv_object_event_render_set_ui_manager >(pevent);
		GV_ASSERT(pe);
		this->m_ui_mgr = pe->ui_mgr;
		this->m_ui_mgr->create_renderer();
	}
	break;
	}
	return 1;
}

bool gv_renderer_d3d9::tick(gv_float dt)
{
	GV_PROFILE_EVENT_PIX(gv_renderer_d3d9_tick, 0);
	super::tick(dt);
	gv_string_tmp s;
	bool ret = gv_debug_renderer_d3d::static_get()->do_render_3d_scene(dt, this);
	if (!ret)
		this->destroy_d3d9();
	m_mem_stack.free_all();
	return ret;
};

bool gv_renderer_d3d9::do_synchronization()
{
	gv_debug_renderer_d3d::static_get()->do_synchronization();
	if (m_camera_entity && !m_camera_entity->is_deleted())
	{
		gv_world* world = m_camera_entity->get_world();
		if (world)
			get_private_data()->m_light_info = world->get_light_info();
	}
	return super::do_synchronization();
};

void gv_renderer_d3d9::init_d3d9()
{
	if (!m_window_handle)
	{
		gv_global::debug_draw.get()->create_window(
			"gv", "gv", gv_vector2i(100, 100),
			gv_vector2i(gv_global::rnd_opt.m_screen_width,
						gv_global::rnd_opt.m_screen_height),
			true);
		gv_global::debug_draw.get()->init_vb(gv_global::rnd_opt.m_max_debug_tri,
											 gv_global::rnd_opt.m_max_debug_line);
	}
	else
	{
		gv_debug_renderer_d3d::static_get()->init_from_existed_window(
			(gv_int_ptr)m_window_handle);
		gv_global::debug_draw.get()->init_vb(gv_global::rnd_opt.m_max_debug_tri,
											 gv_global::rnd_opt.m_max_debug_line);
	}
	m_render_target_mgr->on_init();
	this->m_private_data->m_screen_size =
		m_render_target_mgr->get_default_color_buffer_size();
	this->m_private_data->m_color_buffer_texture =
		m_render_target_mgr->create_color_texture(gv_id_g_color_buffer,
												  m_private_data->m_screen_size,
												  gve_pixel_A8R8G8B8);
	this->m_private_data->m_depth_buffer_texture =
		m_render_target_mgr->create_color_texture(gv_id_g_depth_buffer,
												  m_private_data->m_screen_size,
												  gve_pixel_A8R8G8B8);
	if (!gv_global::rnd_opt.m_no_shadow)
	{
		for (int i = 0; i < gvc_nb_shadow_map; i++)
		{
			gv_string_tmp n;
			n = "g_shadow_map";
			n << i;
			this->get_private_data()->m_shadow_map_textures[i] =
				get_render_target_mgr()->create_color_texture(
					gv_id(*n),
					gv_vector2i(gvc_shadow_map_dimention, gvc_shadow_map_dimention),
					gve_pixel_R32F);
		}
		this->get_private_data()->m_shadow_map_depth_buffer =
			get_render_target_mgr()->create_depth_texture(
				gv_id("g_shadow_map_depth_buffer"),
				gv_vector2i(gvc_shadow_map_dimention, gvc_shadow_map_dimention),
				gve_pixel_D24S8);
	}
	this->m_private_data->m_g_full_blur_buffer0 =
		m_render_target_mgr->create_color_texture(gv_id("g_full_blur_buffer0"),
												  m_private_data->m_screen_size,
												  gve_pixel_A8R8G8B8);
	this->m_private_data->m_g_full_blur_buffer1 =
		m_render_target_mgr->create_color_texture(gv_id("g_full_blur_buffer1"),
												  m_private_data->m_screen_size,
												  gve_pixel_A8R8G8B8);

	m_post_effect_mgr->on_init();
	/*
  RECT rect;	GetClientRect(m_window_handle, & rect);
  gv_global::rnd_opt.m_screen_x=rect.left;
  gv_global::rnd_opt.m_screen_y=rect.top; */
};

gv_vector2i gv_renderer_d3d9::get_screen_size()
{
	return m_private_data->m_screen_size;
};

gv_texture* gv_renderer_d3d9::get_color_buffer_tex()
{
	return m_private_data->m_color_buffer_texture;
};

gv_texture* gv_renderer_d3d9::get_depth_buffer_tex()
{
	return m_private_data->m_depth_buffer_texture;
};

gv_texture* gv_renderer_d3d9::get_shadow_map_tex(int idx)
{
	return get_private_data()->m_shadow_map_textures[idx];
};

gv_texture* gv_renderer_d3d9::get_full_size_buffer(int idx)
{
	if (idx)
		return get_private_data()->m_g_full_blur_buffer1;
	return get_private_data()->m_g_full_blur_buffer0;
};

gv_texture* gv_renderer_d3d9::get_shadow_map_depth_buf()
{
	return get_private_data()->m_shadow_map_depth_buffer;
};

void gv_renderer_d3d9::destroy_d3d9()
{
#if GV_WITH_IMGUI
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	if (!gv_global::device_d3d9.try_get())
		return;
	{   // wait render complete.
		// get_device_d3d9()->Present( NULL, NULL, NULL, NULL );
	}
	this->m_components.for_each(
		boost::bind(&gv_renderer_d3d9::uncache_renderable, this, _1));
	if (gv_debug_renderer_d3d::static_get())
		gv_debug_renderer_d3d::static_get()->uninit_d3d();
	gv_global::device_d3d9.destroy();
};

bool gv_renderer_d3d9::do_render(gv_float dt)
{
	GV_PROFILE_EVENT_PIX(gv_renderer_d3d9_do_render, 0)


#if GV_WITH_IMGUI
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("SN_TEMP Hello, world!",NULL, ImGuiWindowFlags_AlwaysAutoResize);                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		if (ImGui::Button("file"))
		{
			//open a file 
			OPENFILENAMEA ofn;       // common dialog box structure
			char szFile[260];       // buffer for file name
			HWND hwnd=NULL;              // owner window
			//HANDLE hf;              // file handle

									// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			//
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			//
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			if (GetOpenFileNameA(&ofn) == TRUE)
			{
				GVM_DEBUG_OUT(ofn.lpstrFile);
			}
		}
		ImGui::SameLine();
		static bool b_open_file = false;
		if (ImGui::Button("file in imgui"))
		{
			b_open_file = !b_open_file;
		}
		if (ImGui::Button("Delete.."))
			ImGui::OpenPopup("Delete?");
		if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
			ImGui::Separator();

			//static int dummy_i = 0;
			//ImGui::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

			static bool dont_ask_me_next_time = false;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
			ImGui::PopStyleVar();

			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImVec2 window_pos = ImGui::GetIO().DisplaySize;
		window_pos.x *= 0.5;
		window_pos.y *= 0.5;

	/*	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
		if (!ImGui::Begin("files!", &b_open_file))
		{
			ImGui::End();
		}
		else
		{
			ImGui::Text("This is some useful text.");
			ImGui::End();
		}*/
	}
	ImGui::EndFrame();
#endif

	if (this->m_main_camera)
	{
		if (!m_main_camera_copy)
		{
			m_main_camera_copy =
				get_sandbox()->create_nameless_object< gv_com_camera >();
		}
		gv_com_camera::static_class()->copy_to((gv_byte*)m_main_camera.ptr(),
											   (gv_byte*)m_main_camera_copy.ptr());
		m_camera_entity = (gv_entity*)m_main_camera->get_entity();
		this->m_projection_matrix = m_main_camera->m_projection_matrix;
		this->m_view_matrix = m_main_camera->m_world_view_matrix;
		this->m_world_view_project_matrix =
			m_main_camera->m_world_view_project_matrix;
		this->m_near_clip = m_main_camera->m_near_z;
		this->m_far_clip = m_main_camera->m_far_z;
		this->m_inv_view_matrix = m_view_matrix.get_inverse();
		this->m_camera_pos = m_inv_view_matrix.get_trans();
		this->m_camera_dir = m_main_camera->get_world_dir();
		gv_vector3 v = this->m_view_matrix.mul_by(this->m_camera_pos);
		v = v;
	}
	build_main_visibility();
	// Begin the scene
	if (SUCCEEDED(get_device_d3d9()->BeginScene()))
	{
		GV_PROFILE_EVENT_PIX(render_scene, 0);

		if (!gv_global::rnd_opt.m_no_shadow)
		{
			GV_PROFILE_EVENT_PIX(render_shadow_map, 0);
			this->render_shadow_map();
		}

		{
			GV_PROFILE_EVENT_PIX(device_d3d9_clear, 0);
			get_device_d3d9()->Clear(
				0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
				gv_global::rnd_opt.m_clear_color.BGRA().fixed32, 1.0f, 0);
		}

		if (!gv_global::rnd_opt.m_no_shadow)
		{
			GV_PROFILE_EVENT_PIX(render_z_pass, 0);
			get_device_d3d9()->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
			this->render_z_pass();
			get_device_d3d9()->Clear(0, NULL, D3DCLEAR_TARGET,
									 gv_global::rnd_opt.m_clear_color.BGRA().fixed32,
									 1.0f, 0);
		}

		for (int i = 0; i < m_render_order.size(); i++)
		{
			gv_int pass = m_render_order[i];
			GV_PROFILE_EVENT_PIX(render_do_render_pass, pass);
			this->do_render_pass(dt, pass);
		}
		for (int i = 0; i < this->m_com_renderers.size(); i++)
		{
			this->m_com_renderers[i]->before_post_process();
		}
		if (!gv_global::rnd_opt.m_no_post_effect)
		{
			GV_PROFILE_EVENT_PIX(do_render_post_effect, 0)
			do_render_post_effect(dt);
		}
		for (int i = 0; i < this->m_com_renderers.size(); i++)
		{
			this->m_com_renderers[i]->after_post_process();
		}
		get_batch_renderer()->render_priority_group(
			0, gvc_render_priority_last); // clear the batch list;
		get_device_d3d9()->SetDefaultRenderStates();
		get_renderer_d3d9()->get_render_target_mgr()->set_default_render_target();
		if (m_ui_mgr)
		{
			m_ui_mgr->do_render(dt);
			get_device_d3d9()->SetDefaultRenderStates();
		}
#if GV_WITH_IMGUI
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif
		static bool render_debug_info = true;
		if (render_debug_info)
			gv_debug_renderer_d3d::static_get()->do_render_debug();
		get_device_d3d9()->EndScene();
	}

	// Present the backbuffer contents to the display
	{
		GV_PROFILE_EVENT_PIX(render_do_flip_buffer, 0);
		get_device_d3d9()->Present(NULL, NULL, NULL, NULL);
	}

	return true;
};

void gv_renderer_d3d9::do_render_pass(gv_float dt, gv_int pass)
{
	this->begin_render_pass(pass);
	this->m_visible_components.for_each(
		boost::bind(&gv_renderer_d3d9::render_a_renderable, this, _1, pass, dt));
	for (int i = 0; i < this->m_com_renderers.size(); i++)
	{
		this->m_com_renderers[i]->post_render_pass(pass);
		m_last_pass_effect = NULL;
	}
};

void gv_renderer_d3d9::render_a_renderable(gv_com_graphic* ptr, gv_int pass,
										   gv_float dt)
{
	gv_component_renderer* prender = ptr->get_renderer(pass);
	if (prender)
	{
		prender->render_component(ptr);
	}
}

void gv_renderer_d3d9::add_a_renderable(gv_com_graphic* ptr)
{
	precache_renderable(ptr);
	if (ptr->is_post_effect())
	{
		get_post_effect_mgr()->add_post_effect(ptr);
	}
	else
	{
		m_components.add(ptr);
	}
};

void gv_renderer_d3d9::remove_a_renderable(gv_com_graphic* ptr)
{
	if (ptr->is_post_effect())
	{
		get_post_effect_mgr()->remove_post_effect(ptr);
	}
	else
	{
		this->m_components.remove(ptr);
	}
};

gv_component_renderer* gv_renderer_d3d9::get_com_renderer(const gv_id& id)
{
	gvt_ref_ptr< gv_component_renderer > renderer;
	gvt_ref_ptr< gv_component_renderer >* pr = m_renderer_map.find(id);
	if (pr)
	{
		return *pr;
	}
	renderer = gvt_cast< gv_component_renderer >(
		get_sandbox()->create_nameless_object(id));
	if (!renderer)
	{
		GVM_WARNING("can't find the renderer class " << id << gv_endl);
		return NULL;
	}
	m_renderer_map[id] = renderer;
	m_com_renderers.push_back(renderer);
	return renderer;
}

gv_component_renderer* gv_renderer_d3d9::get_batch_renderer()
{
	if (!get_private_data()->m_instancing_renderer)
	{
		get_private_data()->m_instancing_renderer =
			get_com_renderer(gv_id("gv_com_instancing_renderer"));
	}
	return get_private_data()->m_instancing_renderer;
}

gv_component_renderer* gv_renderer_d3d9::get_effect_renderer()
{
	if (!get_private_data()->m_effect_renderer)
	{
		get_private_data()->m_effect_renderer =
			get_com_renderer(gv_id("gv_com_effect_renderer"));
	}
	return get_private_data()->m_effect_renderer;
};

gv_component_renderer* gv_renderer_d3d9::get_terrain_renderer()
{
	if (!get_private_data()->m_terrain_renderer)
	{
		get_private_data()->m_terrain_renderer =
			get_com_renderer(gv_id("gv_com_roam_renderer"));
	}
	return get_private_data()->m_terrain_renderer;
};

gv_texture* gv_renderer_d3d9::get_default_texture()
{
	if (!this->m_default_texture)
	{
		this->m_default_texture = get_sandbox()->create_object< gv_texture >();
		this->m_default_texture->set_file_name(GV_RESOURCE_ROOT
											   "/materials/Default.bmp");
		this->precache_texture(this->m_default_texture);
	}
	return this->m_default_texture;
}
static float get_projected_size(gv_com_camera* camera,
								const gv_box& world_aabb)
{
	float dist_sq =
		(world_aabb.get_center() - camera->get_world_position()).length_squared();
	gv_float box_size = (world_aabb.get_length() + world_aabb.get_height() +
						 world_aabb.get_width()) /
						3.0f;
	box_size *= box_size;
	if (dist_sq > 100.f)
	{
		return box_size / (dist_sq + 0.0001f);
	}
	return 1.f;
}

static float gv_visible_size_threshold = 0.0001f;

bool gv_renderer_d3d9::test_a_renderable_for_visibility(gv_com_graphic* ptr)
{
	bool is_visible = false;
	if (ptr->is_deleted() || ptr->get_ref() == 1)
	{
		ptr->set_visible(false);
		return false;
	}
	if (ptr->get_entity()->is_hided())
	{
		ptr->set_visible(false);
		return true;
	}
	const gv_world* pworld = ptr->get_entity()->get_world();
	if (!pworld)
	{
		ptr->set_visible(false);
		return true;
	}
	if (pworld && pworld->is_hided())
	{
		ptr->set_visible(false);
		return true;
	}
	if (pworld && m_camera_entity && pworld != m_camera_entity->get_world())
	{
		ptr->set_visible(false);
		return true;
	}
	if (ptr->is_always_visible())
	{
		if (ptr->is_visible_test_tag_enabled() &&
			!ptr->test_visible_test_tag(m_visible_test_tag))
		{
			is_visible = false;
		}
		else
			is_visible = true;
	}
	else if (!ptr->is_visible_test_tag_enabled())
	{
		const gv_box& world_aabb = ptr->get_entity()->get_world_aabb();
		if (m_main_camera &&
			m_main_camera->get_world_frustum().intersect(world_aabb))
		{
			if (get_projected_size(m_main_camera, world_aabb) >
				gv_visible_size_threshold)
			{
				is_visible = true;
			}
		}
	}
	else if (ptr->test_visible_test_tag(m_visible_test_tag))
	{
		const gv_box& world_aabb = ptr->get_entity()->get_world_aabb();
		if (m_main_camera &&
			m_main_camera->get_world_frustum().intersect(world_aabb))
		{
			if (get_projected_size(m_main_camera, world_aabb) >
				gv_visible_size_threshold)
			{
				is_visible = true;
			}
		}
	}
	if (is_visible)
	{
		if (ptr->is_background())
			m_visible_components.push_front(ptr);
		else
			m_visible_components.push_back(ptr);
		ptr->set_visible(true);
		if (ptr->has_visible_callback())
		{
			ptr->on_visible(m_main_camera, m_visible_test_tag);
		}
	}
	else
	{
		ptr->set_visible(false);
	}
	return true;
}

void gv_renderer_d3d9::build_main_visibility()
{
	GV_PROFILE_EVENT(build_main_visibility, 0);
	m_visible_test_tag++;
	this->m_visible_components.clear();
	this->m_components.for_each_delete(boost::bind(
		&gv_renderer_d3d9::test_a_renderable_for_visibility, this, _1));
};

void* gv_renderer_d3d9::get_param_from_semantric(gve_effect_semantic e)
{

	static gv_vector4 s_v;
	static gv_matrix44 matrix;
	gv_float t = (gv_float)get_sandbox()->get_time_in_seconds(); //m_current_life_time;					//(gv_float)get_sandbox()->get_time_in_seconds();//m_current_life_time;
												////(gv_float)get_sandbox()->get_time_in_seconds();
	gv_float t_1 = m_current_unified_life_time; // gvt_frac(t/120.f);
	gv_float t_2PI = t_1 * gv_float_trait::pi() * 2.0f;

	switch (e)
	{
	case gve_effect_semanticTime0_X:
	{
		s_v.set_x(t);
		return &s_v;
	}
	case gve_effect_semanticCosTime0_X:
	{
		s_v.set_x(cosf(t));
		return &s_v;
	}

	case gve_effect_semanticSinTime0_X:
	{
		s_v.set_x(sinf(t));
		return &s_v;
	}

	case gve_effect_semanticTanTime0_X:
	{
		s_v.set_x(tanf(t));
		return &s_v;
	}

	case gve_effect_semanticTime0_1:
	{
		s_v.set_x(t_1);
		return &s_v;
	}
	case gve_effect_semanticCosTime0_1:
	{
		s_v.set_x(cosf(t_1));
		return &s_v;
	}

	case gve_effect_semanticSinTime0_1:
	{
		s_v.set_x(sinf(t_1));
		return &s_v;
	}

	case gve_effect_semanticTanTime0_1:
	{
		s_v.set_x(tanf(t_1));
		return &s_v;
	}

	case gve_effect_semanticTime0_2PI:
	{
		s_v.set_x(t_2PI);
		return &s_v;
	}
	case gve_effect_semanticCosTime0_2PI:
	{
		s_v.set_x(cosf(t_2PI));
		return &s_v;
	}

	case gve_effect_semanticSinTime0_2PI:
	{
		s_v.set_x(sinf(t_2PI));
		return &s_v;
	}

	case gve_effect_semanticTanTime0_2PI:
	{
		s_v.set_x(tanf(t_2PI));
		return &s_v;
	}

	case gve_effect_semanticTimeCyclePeriod:
	case gve_effect_semanticTimeElapsed:
	{
		s_v.set_x(get_sandbox()->get_delta_time());
		return &s_v;
	}

	case gve_effect_semanticFPS:
	{
		s_v.set_x(1.0f / (get_sandbox()->get_delta_time() + 0.00001f));
		return &s_v;
	}

	case gve_effect_semanticViewportWidth:
	{
		s_v.set_x((gv_float)get_device_d3d9()->get_viewport_w());
		return &s_v;
	}

	case gve_effect_semanticViewportHeight:
	{
		s_v.set_x((gv_float)get_device_d3d9()->get_viewport_h());
		return &s_v;
	}

	case gve_effect_semanticViewportHeightInverse:
	{
		s_v.set_x(1.0f / (gv_float)get_device_d3d9()->get_viewport_h());
		return &s_v;
	}

	case gve_effect_semanticViewportWidthInverse:
	{
		s_v.set_x(1.0f / (gv_float)get_device_d3d9()->get_viewport_w());
		return &s_v;
	}

	case gve_effect_semanticViewportDimensionsInverse:
	{
		s_v.set_x(1.0f / (gv_float)get_device_d3d9()->get_viewport_w());
		s_v.set_y(1.0f / (gv_float)get_device_d3d9()->get_viewport_h());
		return &s_v;
	}

	case gve_effect_semanticNearClipPlane:
	{
		s_v.set_x(m_main_camera->m_near_z);
		return &s_v;
	}

	case gve_effect_semanticFarClipPlane:
	{
		s_v.set_x(m_main_camera->m_far_z);
		return &s_v;
	}

	case gve_effect_semanticRandomFraction1PerPass:
	case gve_effect_semanticRandomFraction2PerPass:
	case gve_effect_semanticRandomFraction3PerPass:
	case gve_effect_semanticRandomFraction4PerPass:
	case gve_effect_semanticRandomFraction1PerEffect:
	case gve_effect_semanticRandomFraction2PerEffect:
	case gve_effect_semanticRandomFraction3PerEffect:
	case gve_effect_semanticRandomFraction4PerEffect:
	{
		// TODO!
		s_v.set_x(t_1);
		return &s_v;
	}

	case gve_effect_semanticRightMouseButton:
	case gve_effect_semanticMiddleMouseButton:
	case gve_effect_semanticLeftMouseButton:
	case gve_effect_semanticMouseCoordinateX:
	case gve_effect_semanticMouseCoordinateY:
	case gve_effect_semanticModelBoundingSphereRadius:
	{
		s_v = 0;
		return &s_v;
	}

	//============================================================================================
	//								:
	//============================================================================================
	case gve_effect_semanticTime0_X_Packed:
	{
		s_v.set(t, cosf(t), sinf(t), tanf(t));
		return &s_v;
	}

	case gve_effect_semanticTime0_1_Packed:
	{
		s_v.set(t_1, cosf(t_1), sinf(t_1), tanf(t_1));
		return &s_v;
	}

	case gve_effect_semanticTime0_2PI_Packed:
	{
		s_v.set(t_2PI, cosf(t_2PI), sinf(t_2PI), tanf(t_2PI));
		return &s_v;
	}

	case gve_effect_semanticMouseButtonsPacked:
	case gve_effect_semanticMouseCoordsPacked:
	{
		s_v = 0;
		return &s_v;
	}

	case gve_effect_semanticViewDirection:
	{
		s_v = m_inv_view_matrix.axis_z;
		return &s_v;
	}

	case gve_effect_semanticViewPosition:
	{
		s_v = m_inv_view_matrix.get_trans();
		return &s_v;
	}

	case gve_effect_semanticViewSideDirection:
	{
		s_v = m_inv_view_matrix.axis_x;
		return &s_v;
	}

	case gve_effect_semanticViewUpDirection:
	{
		s_v = m_inv_view_matrix.axis_y;
		return &s_v;
	}

	//============================================================================================
	//								:
	//============================================================================================
	case gve_effect_semanticViewProjection:
	{
		matrix = this->get_view_projection();
		return &matrix;
	}

	case gve_effect_semanticViewProjectionInverse:
	{
		matrix = this->get_view_projection_inverse();
		return &matrix;
	}

	case gve_effect_semanticViewProjectionInverseTranspose:
	{
		matrix = this->get_view_projection_inverse_transpose();
		return &matrix;
	}

	case gve_effect_semanticViewProjectionTranspose:
	{
		matrix = this->get_view_projection_transpose();
		return &matrix;
	}

	case gve_effect_semanticView:
	{
		matrix = this->get_view();
		return &matrix;
	}

	case gve_effect_semanticViewInverse:
	{
		matrix = this->get_view_inverse();
		return &matrix;
	}

	case gve_effect_semanticViewInverseTranspose:
	{
		matrix = this->get_view_inverse_transpose();
		return &matrix;
	}

	case gve_effect_semanticViewTranspose:
	{
		matrix = this->get_view_transpose();
		return &matrix;
	}

	case gve_effect_semanticProjection:
	{
		matrix = this->get_projection_matrix();
		return &matrix;
	}

	case gve_effect_semanticProjectionInverse:
	{
		matrix = this->get_projection_inverse();
		return &matrix;
	}

	case gve_effect_semanticProjectionInverseTranspose:
	{
		matrix = this->get_projection_inverse_transpose();
		return &matrix;
	}
	case gve_effect_semanticProjectionTranspose:
	{
		matrix = this->get_projection_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorldViewProjection:
	{
		matrix = this->get_world_view_projection();
		return &matrix;
	}

	case gve_effect_semanticWorldViewProjectionInverse:
	{
		matrix = this->get_world_view_projection_inverse();
		return &matrix;
	}

	case gve_effect_semanticWorldViewProjectionInverseTranspose:
	{
		matrix = this->get_world_view_projection_inverse_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorldViewProjectionTranspose:
	{
		matrix = this->get_world_view_projection_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorld:
	{
		matrix = this->get_world();
		return &matrix;
	}

	case gve_effect_semanticWorldInverse:
	{
		matrix = this->get_world_inverse();
		return &matrix;
	}

	case gve_effect_semanticWorldInverseTranspose:
	{
		matrix = this->get_world_inverse_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorldTranspose:
	{
		matrix = this->get_world_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorldView:
	{
		matrix = this->get_world_view();
		return &matrix;
	}

	case gve_effect_semanticWorldViewInverse:
	{
		matrix = this->get_world_view_inverse();
		return &matrix;
	}

	case gve_effect_semanticWorldViewInverseTranspose:
	{
		matrix = this->get_world_view_inverse_transpose();
		return &matrix;
	}

	case gve_effect_semanticWorldViewTranspose:
	{
		matrix = this->get_world_view_transpose();
		return &matrix;
	}

	case gve_effect_semanticstatic_view_light_matrix:
	{
		return &get_private_data()->m_view_light_matrix;
	}

	case gve_effect_semanticstatic_light_pos_world:
	{
		return &get_private_data()->m_shadow_light_pos_world[0];
	}

	case gve_effect_semanticstatic_light_pos_world_1:
	{
		return &get_private_data()->m_shadow_light_pos_world[1];
	}

	case gve_effect_semanticstatic_light_pos_world_2:
	{
		return &get_private_data()->m_shadow_light_pos_world[2];
	}

	case gve_effect_semanticstatic_light_pos_world_3:
	{
		return &get_private_data()->m_shadow_light_pos_world[3];
	}

	case gve_effect_semanticstatic_scale_xyz_shadow_pass:
	{
		return &get_private_data()->m_scale_xyz_shadow_pass;
	}

	case gve_effect_semanticstatic_light_pos_world_shadow_pass:
	{
		return &get_private_data()->m_light_pos_world_shadow_pass;
	}

	case gve_effect_semanticstatic_scale_xyz:
	{
		return &get_private_data()->m_shadow_scale_xyz[0];
	}

	case gve_effect_semanticstatic_scale_xyz_1:
	{
		return &get_private_data()->m_shadow_scale_xyz[1];
	}

	case gve_effect_semanticstatic_scale_xyz_2:
	{
		return &get_private_data()->m_shadow_scale_xyz[2];
	}

	case gve_effect_semanticstatic_scale_xyz_3:
	{
		return &get_private_data()->m_shadow_scale_xyz[3];
	}

	} // switch;
	GV_ASSERT(0);
	return NULL;
};

void gv_renderer_d3d9::upload_skin_matrices()
{
	if (m_bone_matrices.size())
	{
		get_device_d3d9()->SetVertexShaderConstantF(
			GV_GPU_SKIN_BONE_REGISTER, (FLOAT*)m_bone_matrices.begin(),
			m_bone_matrices.size());
	}
};

void gv_renderer_d3d9::add_shader_macro(const char* macro)
{
	gv_string* ps = this->m_shader_precompile_macro.add_dummy();
	(*ps) = macro;
};

void gv_renderer_d3d9::clear_shader_macro()
{
	this->m_shader_precompile_macro.clear();
}

gv_uint gv_renderer_d3d9::set_pass_effect(gv_effect* pass_effect, bool do_skin,
										  gv_material* material, gv_int pass)
{
	get_private_data()->m_current_pass = pass;
	this->precache_effect(pass_effect);
	if (!pass_effect->get_pixel_shader())
		return NULL;
	if (!pass_effect->get_vertex_shader())
		return NULL;
	if (pass_effect->get_active_render_target().m_name.is_empty() ||
		pass_effect->get_active_render_target().m_name == gv_id_g_color_buffer)
	{
		get_renderer_d3d9()->get_render_target_mgr()->set_default_render_target();
	}
	else
	{
		gv_effect_renderable_texture* rt = pass_effect->get_renderable_texture(
			pass_effect->get_active_render_target().m_name);
		GV_ASSERT(rt);
		get_renderer_d3d9()->get_render_target_mgr()->begin_render_target(
			rt->m_texture, NULL,
			pass_effect->get_active_render_target().m_is_clear_color,
			pass_effect->get_active_render_target().m_clear_color_value);
	}
	bool effect_not_change = m_last_pass_effect == pass_effect;
	m_last_pass_effect = pass_effect;
	m_last_material = material;
	if (!effect_not_change)
	{
		// TODO
		get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		gv_int nb_state = pass_effect->get_nb_render_state();
		for (int j = 0; j < nb_state; j++)
		{
			gv_effect_render_state* pstate = pass_effect->get_render_state(j);
			if (pstate->m_state == D3DRS_POINTSIZE ||
				pstate->m_state == D3DRS_POINTSIZE_MIN ||
				pstate->m_state == D3DRS_MINTESSELLATIONLEVEL ||
				pstate->m_state == D3DRS_MAXTESSELLATIONLEVEL)
			{
				continue;
			}
			get_device_d3d9()->SetRenderState((D3DRENDERSTATETYPE)pstate->m_state,
											  (DWORD)pstate->m_value);
		}
	}
	gv_uint vertex_decl = pass_effect->get_current_stream_decl();
	if (do_skin)
	{
		vertex_decl |= e_vtx_with_blend_index | e_vtx_with_blend_weight;
	}
	gv_shader_d3d* vertex_shader =
		pass_effect->get_vertex_shader()->get_hardware_cache< gv_shader_d3d >();
	if (!vertex_shader)
		return NULL;
	GV_ASSERT(!vertex_shader->m_is_pixel_shader)
	if (!vertex_shader->set(do_skin))
		return NULL;

	gv_shader_d3d* pixel_shader =
		pass_effect->get_pixel_shader()->get_hardware_cache< gv_shader_d3d >();
	if (!pixel_shader)
		return NULL;
	GV_ASSERT(pixel_shader->m_is_pixel_shader)
	if (!pixel_shader->set())
		return NULL;

	return vertex_decl;
};

void* gv_renderer_d3d9::get_param_from_material(const gv_id& param_name,
												gve_shader_data_type type)
{
	gv_material* material = m_last_material;
	if (!material)
		return NULL;
	static gv_vector4 static_v4;
	static gv_colorf static_c;
	gv_string_tmp name = param_name.string();
	if (name.has_prefix("g_"))
	{
		if (param_name == gv_id_g_SunDir)
		{
			return &get_private_data()->m_light_info.m_sun_dir;
		}
		if (param_name == gv_id_g_Diffuse)
		{
			return &material->m_diffuse_color;
		}
		else if (param_name == gv_id_g_Ambient)
		{
			static_c = get_private_data()->m_light_info.m_ambient_color;
			return &static_c;
		}
		else if (param_name == gv_id_g_Specular)
		{
			static_c = get_private_data()->m_light_info.m_specular_color;
			return &static_c;
		}
	}
	else if (name == "pass_index")
	{
		//
		static_v4.x = (float)get_private_data()->m_current_pass;
		return &static_v4;
	}
	switch (type)
	{
	case e_shader_data_float:
	case e_shader_data_vector2:
	case e_shader_data_vector3:
	case e_shader_data_vector4:
	case e_shader_data_colorf:
	{
		gv_material_param_float4* param = material->get_param_float4(param_name);
		if (param)
		{
			return &param->m_data;
		}
	}
	break;

	case e_shader_data_bool:
		break;

	case e_shader_data_matrix4:
		break;

	case e_shader_data_matrix43:
		break;

	case e_shader_data_dynamic:
		break;

	default:
		GV_ASSERT(0);
	};
	return NULL;
};

gv_texture* gv_renderer_d3d9::get_texture_from_material(const gv_id& tex_name)
{
	gv_material* material = m_last_material;
	if (!material)
		return NULL;
	gv_material_tex* ptex = material->get_material_texture(tex_name);
	if (ptex)
	{
		return ptex->m_texture;
	}
	return NULL;
};

void gv_renderer_d3d9::do_render_post_effect(gv_float dt)
{
	get_render_target_mgr()->resolve_backbuffer(get_color_buffer_tex());
	this->m_post_effect_mgr->render(dt);
// debug
#if GV_DEBUG_VERSION
	if (gv_global::rnd_opt.m_enable_debug_texture)
	{
		GV_PROFILE_EVENT_PIX(DEBUG_POST_TEXTURE, 0)

		get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		get_device_d3d9()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		// this->kick_textured_quad(get_color_buffer_tex(), gv_recti ( 100,
		// 500,164,564) );
		// this->kick_textured_quad(get_depth_buffer_tex(), gv_recti ( 100,
		// 500,200,600) );
		int h = get_screen_size().get_y() - 200;
		this->kick_textured_quad(get_shadow_map_tex(),
								 gv_recti(0, h, 200, h + 200));
		this->kick_textured_quad(get_full_size_buffer(),
								 gv_recti(200, h, 400, h + 200));
		this->kick_textured_quad(get_depth_buffer_tex(),
								 gv_recti(400, h, 600, h + 200));
		this->kick_textured_quad(get_shadow_map_tex(1),
								 gv_recti(600, h, 800, h + 200));
	}
#endif
}

void gv_renderer_d3d9::kick_textured_quad(gv_texture* texture,
										  const gv_recti& rect)
{
	struct TLVertex
	{
		float x, y, z, rhw;
		float u, v;
	};
	TLVertex vertices[] = {
		{rect.left - 0.5f, rect.top - 0.5f, 0.1f, 1.0f, 0, 0},
		{rect.right - 0.5f, rect.top - 0.5f, 0.1f, 1.0f, 1, 0},
		{rect.left - 0.5f, rect.bottom - 0.5f, 0.1f, 1.0f, 0, 1},
		{rect.right - 0.5f, rect.bottom - 0.5f, 0.1f, 1.0f, 1, 1}};

	get_device_d3d9()->SetVertexShader(0);
	get_device_d3d9()->SetPixelShader(0);
	set_texture_to_d3d(texture, 0);
	get_device_d3d9()->SetRenderState(D3DRS_ZENABLE, false);
	get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	get_device_d3d9()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	get_device_d3d9()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	get_device_d3d9()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	get_device_d3d9()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	get_device_d3d9()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	get_device_d3d9()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	get_device_d3d9()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices,
									   sizeof(TLVertex));
	get_device_d3d9()->SetRenderState(D3DRS_ZENABLE, true);
};

void gv_renderer_d3d9::set_texture_to_d3d(gv_texture* texture, gv_int stage)
{
	if (!texture)
		texture = m_default_texture;
	gv_texture_d3d* tex_d3d = texture->get_hardware_cache< gv_texture_d3d >();
	if (!tex_d3d)
	{
		get_renderer_d3d9()->precache_resource(texture);
		tex_d3d = texture->get_hardware_cache< gv_texture_d3d >();
	}
	if (tex_d3d)
		tex_d3d->set(stage);
}
gv_vector3 gv_renderer_d3d9::get_sun_dir()
{
	return get_private_data()->m_light_info.m_sun_dir;
};
gv_vector3 gv_renderer_d3d9::get_main_camera_pos()
{
	return m_camera_pos;
};
gv_vector3 gv_renderer_d3d9::get_main_camera_dir()
{
	return m_camera_dir;
};
void gv_renderer_d3d9::set_static_shader_param_for_post_process(){

};
//=========================================================================================================
GVM_IMP_CLASS(gv_renderer_d3d9, gv_renderer)
GVM_END_CLASS
}
#include "gv_renderer_d3d9_shadow.hpp"