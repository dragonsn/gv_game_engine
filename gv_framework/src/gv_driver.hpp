//====================================================================================
class gv_driver_info_imp
{
public:
	gv_driver_info_imp()
	{
		GVM_ZERO_ME;
	}
	char driver_name[256];
	gv_driver* instance;
	gv_func_driver_ctor* create_callback;
	gv_driver_info* next_info;

public:
	static gv_driver_info* first_info;
	static gv_driver_info* last_info;
};

gv_driver_info* gv_driver_info_imp::first_info = NULL;
gv_driver_info* gv_driver_info_imp::last_info = NULL;

gv_driver_info::gv_driver_info()
{
	GV_ASSERT(0);
};

gv_driver_info::gv_driver_info(const char* name, gv_func_driver_ctor func)
{
	m_imp = new gv_driver_info_imp;
	gvt_strncpy(m_imp->driver_name, name, 256);
	m_imp->create_callback = func;
};

gv_driver_info::~gv_driver_info()
{
	delete m_imp;
};

const char* gv_driver_info::get_name()
{
	return m_imp->driver_name;
};

gv_driver* gv_driver_info::create_driver()
{
	GV_ASSERT(!get_driver());
	m_imp->instance = m_imp->create_callback();
	GVM_DEBUG_LOG(main, "[DRV] create driver :" << get_name() << " as "
												<< m_imp->instance << gv_endl);
	return m_imp->instance;
};

gv_driver* gv_driver_info::get_driver()
{
	return m_imp->instance;
};

void gv_driver_info::static_register_driver_info(gv_driver_info* info)
{
	gv_driver_info* p = static_get_first_driver_info();
	while (p)
	{
		GV_ASSERT(p != info);
		p = p->get_next_driver_info();
	}

	if (!gv_driver_info_imp::first_info)
		gv_driver_info_imp::first_info = info;
	info->m_imp->next_info = NULL;
	if (gv_driver_info_imp::last_info)
		gv_driver_info_imp::last_info->m_imp->next_info = info;
	gv_driver_info_imp::last_info = info;
	GVM_DEBUG_OUT(" [DRV] " << info->get_name() << " registered " << gv_endl);
};

void gv_driver_info::static_unregister_all()
{
	gv_driver_info_imp::first_info = gv_driver_info_imp::last_info = NULL;
};

gv_driver_info* gv_driver_info::static_get_first_driver_info()
{
	return gv_driver_info_imp::first_info;
};

gv_driver_info* gv_driver_info::static_get_driver_info(const char* name)
{
	gv_driver_info* info = static_get_first_driver_info();
	while (info)
	{
		if (!gvt_strcmp(name, info->get_name()))
		{
			return info;
		}
		info = info->get_next_driver_info();
	}
	return NULL;
};

void gv_driver_info::static_destroy_drivers()
{
	// destroy need to be in a reverse order
	gvt_array_cached< gv_driver_info* > infos;
	gv_driver_info* info = static_get_first_driver_info();
	while (info)
	{
		if (info->get_driver())
		{
			infos.push_back(info);
		}
		info = info->get_next_driver_info();
	}
	for (int i = infos.size() - 1; i >= 0; i--)
	{
		info = infos[i];
		GVM_DEBUG_OUT(" [DRV] " << info->get_name() << " destroyed " << gv_endl);
		delete info->m_imp->instance;
		info->m_imp->instance = NULL;
	}
	gv_driver_info_imp::first_info = NULL;
	gv_driver_info_imp::last_info = NULL;
};

bool gv_driver_info::static_init_all_drivers(gv_sandbox* sandbox)
{
	gv_driver_info* info = static_get_first_driver_info();
	while (info)
	{
		if (info->get_driver())
		{
			info->get_driver()->init(sandbox);
		}
		info = info->get_next_driver_info();
	}
	return true;
};

bool gv_driver_info::static_uninit_all_drivers(gv_sandbox* sandbox)
{
	// uninit need to be in a reverse order
	gvt_array_cached< gv_driver_info* > infos;
	gv_driver_info* info = static_get_first_driver_info();
	while (info)
	{
		if (info->get_driver())
		{
			infos.push_back(info);
		}
		info = info->get_next_driver_info();
	}
	for (int i = infos.size() - 1; i >= 0; i--)
	{
		info = infos[i];
		info->get_driver()->uninit(sandbox);
	}
	return true;
};

bool gv_driver_info::static_create_event_processors(gv_sandbox* sandbox)
{
	gv_driver_info* driver = gv_driver_info::static_get_first_driver_info();
	while (driver)
	{

		if (driver->get_driver())
		{
			gvt_array< gv_event_processor_desc > descs;
			if (driver->get_driver()->get_processor_descs(descs))
			{
				for (int i = 0; i < descs.size(); i++)
				{
					gv_event_processor_desc& desc = descs[i];
					gv_int channel = gv_framework_channel_name_to_enum(desc.m_channel_id);
					GV_ASSERT(channel != gvc_max_object_event_channel &&
							  "delete the game_engine.ini to have a try!");
					sandbox->register_processor(desc.m_processor_cls, channel);
					if (sandbox->get_event_processor(
							channel)) // possible NULL when the class module not loaded!!
					{
						sandbox->get_event_processor(channel)
							->set_synchronization(desc.m_is_synchronization);
						sandbox->get_event_processor(channel)
							->set_autonomous(desc.m_is_autonomous);
					}
				} //
			}
		} // if driver
		driver = driver->get_next_driver_info();
	} // next driver
	return true;
}

void gv_driver_info::static_create_drivers()
{
	gv_string text;
	gvt_array< gv_string > result;
	if (gv_global::framework_config.driver_list.size())
	{
		static_create_drivers(gv_global::framework_config.driver_list);
	}
	else
	{
		gv_driver_info* info = static_get_first_driver_info();
		while (info)
		{
			result.push_back(info->get_name());
			text << info->get_name() << "  ";
			gv_driver* driver = info->create_driver();
			driver->m_info = info;
			info = info->get_next_driver_info();
		}
	}
};

void gv_driver_info::static_create_drivers(gvt_array< gv_string >& names)
{
	for (int i = 0; i < names.size(); i++)
	{
		gv_driver_info* pinfo = static_get_driver_info(*names[i]);
		if (pinfo)
		{
			gv_driver* driver = pinfo->create_driver();
			driver->m_info = pinfo;
		}
		else if (names[i] != "dummy")
		{
			GV_ERROR_OUT("failed to load driver :" << names[i]);
		}
	}
	return;
};

gv_driver_info* gv_driver_info::get_next_driver_info()
{
	return m_imp->next_info;
};

//====================================================================================
gv_driver::gv_driver()
{
	m_info = NULL;
};

gv_driver::~gv_driver(){

};

void gv_driver::init(gv_sandbox* p)
{
	GVM_DEBUG_LOG(main, " [DRV] " << m_info->get_name() << " inited in " << p
								  << gv_endl);
};

void gv_driver::uninit(gv_sandbox* p)
{
	GVM_DEBUG_LOG(main, " [DRV] " << m_info->get_name() << " uninited " << p
								  << gv_endl);
};

bool gv_driver::get_processor_descs(gvt_array< gv_event_processor_desc >& descs)
{
	return false;
};

gv_driver_info* gv_driver::get_driver_info()
{
	return m_info;
};

class gv_driver_dummy : public gv_driver
{
};
GVM_IMP_DRIVER(dummy)
