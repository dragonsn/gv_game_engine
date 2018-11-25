namespace gv
{
gv_renderer_gameplay3d::gv_renderer_gameplay3d()
{
	GVM_SET_CLASS(gv_renderer_gameplay3d);
	m_game_already_inited = false;
	m_window_handle = NULL;
};

gv_renderer_gameplay3d::~gv_renderer_gameplay3d()
{
	this->destroy_gameplay3d();
};

void gv_renderer_gameplay3d::init_gameplay3d()
{
	GV_PROFILE_EVENT(gv_renderer_gameplay3d__init_gameplay3d, 0);
	GV_ASSERT(!m_hook);
	if (Game::getInstance())
	{
		m_game_already_inited = true;
		m_hook = static_cast< gv_hook_gameplay3d* >(Game::getInstance());
	}
	else
	{
		m_game_already_inited = false;
		m_hook = new gv_hook_gameplay3d;
		;
	}
	FileSystem::setResourcePath(*gv_global::framework_config.data_path_root);
	m_platform = Platform::create(m_hook, (void*)m_window_handle);
	GV_ASSERT(m_platform);
	m_platform->startupMessagePump();
	m_platform->tickMessagePump(); // tick it once to initialize gameplay game.

	gv_global::rnd_opt.m_screen_height =
		gv_global::framework_config.window_size.y = Platform::getDisplayHeight();
	gv_global::rnd_opt.m_screen_width =
		gv_global::framework_config.window_size.x = Platform::getDisplayWidth();
}

void gv_renderer_gameplay3d::destroy_gameplay3d()
{
	GV_PROFILE_EVENT(gv_renderer_gameplay3d__destroy_gameplay3d, 0);
	if (m_hook)
	{
		m_hook->finalize();
		GVM_SAFE_DELETE(m_platform);
		if (!m_game_already_inited)
		{
			GVM_SAFE_DELETE(m_hook);
		}
	}
};

gv_int gv_renderer_gameplay3d::on_event(gv_object_event* pevent)
{
	switch (pevent->m_id)
	{
	case gv_object_event_id_render_init:
	{
		gv_object_event_render_init* pe =
			gvt_cast< gv_object_event_render_init >(pevent);
		GV_ASSERT(pe);
		this->m_window_handle = (void*)pe->window_handle;
		this->init_gameplay3d();
	}
	break;
	case gv_object_event_id_render_uninit:
	{
		this->destroy_gameplay3d();
	}
	break;
	case gv_object_event_id_add_component:
	{
		gv_object_event_add_component* pe =
			gvt_cast< gv_object_event_add_component >(pevent);
		GV_ASSERT(pe);
		this->m_hook->addRenderable(gvt_cast< gv_component >(pe->component));
	}
	break;
	case gv_object_event_id_remove_component:
	{
		gv_object_event_remove_component* pe =
			gvt_cast< gv_object_event_remove_component >(pevent);
		GV_ASSERT(pe);
		this->m_hook->removeRenderable(pe->component);
	}
	break;

	case gv_object_event_id_render_enable_pass:
	{
		gv_object_event_render_enable_pass* pe =
			gvt_cast< gv_object_event_render_enable_pass >(pevent);
		GV_ASSERT(pe);
		// todo this->enable_render_pass( pe->pass);
	}
	break;

	case gv_object_event_id_render_disable_pass:
	{
		gv_object_event_render_disable_pass* pe =
			gvt_cast< gv_object_event_render_disable_pass >(pevent);
		GV_ASSERT(pe);
		// todo this->enable_render_pass( pe->pass,false);
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
		// todo this->m_ui_mgr=pe->ui_mgr;
	}
	break;

	case gv_object_event_id_render_reload_shader_cache:
	{
		this->m_hook->reloadShaderCache();
	}
	break;
	}
	return 1;
}

bool gv_renderer_gameplay3d::tick(gv_float dt)
{
	GV_PROFILE_EVENT(gv_processor_gameplay3d_tick, 0);
	super::tick(dt);
	if (!m_hook)
		return false;
	if (this->m_main_camera)
	{
		m_hook->updateCamera(this->m_main_camera);
	}
	if (m_platform)
	{
		if (!m_platform->tickMessagePump())
			return false;
	}
	return true;
};

bool gv_renderer_gameplay3d::do_synchronization()
{
	GV_PROFILE_EVENT(gv_processor_gameplay3d_sync, 0);
	if (m_camera_entity && !m_camera_entity->is_deleted())
	{
		gv_world* world = m_camera_entity->get_world();
		m_light_info = world->get_light_info();
	}

	gv_global::debug_draw->do_synchronization();
	gv_global::input->tick();
	return super::do_synchronization();
};

GVM_IMP_CLASS(gv_renderer_gameplay3d, gv_event_processor)
GVM_END_CLASS

GVM_IMP_CLASS(gv_cache_mesh_gameplay, gv_resource_cache);
GVM_END_CLASS

GVM_IMP_CLASS(gv_cache_texture_gameplay, gv_resource_cache);
GVM_END_CLASS

GVM_IMP_CLASS(gv_cache_effect_gameplay, gv_resource_cache);
GVM_END_CLASS

GVM_IMP_CLASS(gv_cache_material_gameplay, gv_resource_cache);
GVM_END_CLASS
}
