namespace gv
{
void gv_register_framework_classes();
void gv_unregister_framework_classes();

class gv_sandbox_manager_data
{
public:
	typedef gvt_hash_map< gv_id, gv_string, 1024 > module_map;
	module_map m_modules_xml;
	module_map m_modules_bin;
};

gv_sandbox_manager::gv_sandbox_manager()
{
	m_cu_uuid = 0;
	m_max_sandbox = 0;
	m_sandbox_nb = 0;
	m_max_object_per_sand_box = 0;
	m_enable_name_hash = 0;
	m_is_inited = false;
	this->m_pimpl = new gv_sandbox_manager_data;
};

gv_sandbox_manager::~gv_sandbox_manager()
{
	this->destroy();
	gvt_safe_delete(m_pimpl);
};

bool gv_sandbox_manager::init(gv_ushort nb_sand_box,
							  gv_uint max_object_per_sand_box,
							  bool enable_name_hash, bool is_editor)
{
	GV_PROFILE_EVENT(gv_sandbox_manager_init, 0)
	if (m_is_inited)
		return false;
	m_is_inited = true;
	m_sandboxs.resize(nb_sand_box);
	m_max_sandbox = nb_sand_box;
	m_max_object_per_sand_box = max_object_per_sand_box;
	m_enable_name_hash = enable_name_hash;
	create_sandbox(); // create base_sandbox
	this->get_base_sandbox()->init_persistent_type();
	gv_register_framework_classes();
	gv_register_user_drivers();
	gv_driver_info::static_create_drivers();
	gv_driver_info::static_init_all_drivers(get_base_sandbox());
	get_base_sandbox()->init_classes();
	this->get_base_sandbox()->register_processor(
		gv_event_processor::static_class(), gve_event_channel_default);
	this->init_data_dir();
	return true;
};
void gv_sandbox_manager::destroy()
{
	gv_driver_info::static_uninit_all_drivers(
		gv_global::sandbox_mama->get_base_sandbox());
	gv_unregister_framework_classes();
	gv_driver_info::static_destroy_drivers();
};
gv_sandbox* gv_sandbox_manager::get_sand_box(const gv_sandbox_handle& hd)
{
	if (!is_valid(hd))
		return NULL;
	return m_sandboxs[hd.m_sandbox_id];
};
gv_sandbox* gv_sandbox_manager::get_base_sandbox()
{
	return m_sandboxs[0];
};
gv_sandbox* gv_sandbox_manager::create_sandbox(gv_sandbox* outer)
{
	GV_PROFILE_EVENT(create_sandbox, 0)
	gv_thread_lock lock(m_mutex);
	if (m_sandbox_nb >= m_max_sandbox)
		return NULL;
	int i;
	for (i = 0; i < m_sandboxs.size(); i++)
	{
		if (!m_sandboxs[i])
			break;
	}
	GV_ASSERT(i < m_sandboxs.size());
	gv_sandbox* p = new gv_sandbox;
	m_sandboxs[i] = p;
	p->m_uuid = m_cu_uuid++;
	p->m_index = (gv_ushort)i;
	p->init(m_max_object_per_sand_box, m_enable_name_hash);
	if (!outer && i != 0)
		p->m_outer = m_sandboxs[0].ptr();
	else
		p->m_outer = outer;
	return p;
};

bool gv_sandbox_manager::delete_sandbox(gv_sandbox* p)
{
	gv_thread_lock lock(m_mutex);
	p->destroy();
	m_sandboxs[p->m_index] = NULL;
	return true;
}

bool gv_sandbox_manager::is_valid(const gv_sandbox_handle& hd)
{
	if (hd.m_sandbox_id >= m_max_sandbox)
		return false;
	gv_sandbox* psandbox = m_sandboxs[hd.m_sandbox_id];
	if (!psandbox)
		return false;
	if (psandbox->m_uuid != hd.m_sandbox_uuid)
		return false;
	return true;
};
bool gv_sandbox_manager::is_valid(const gv_sandbox* sandbox)
{
	for (int i = 0; i < m_max_sandbox; i++)
	{
		gv_sandbox* psandbox = m_sandboxs[i];
		if (psandbox == sandbox)
			return true;
	}
	return false;
};
bool gv_sandbox_manager::run()
{
	return true;
};

bool gv_sandbox_manager::init_data_dir()
{
	GV_PROFILE_EVENT(gv_sandbox_manager_init_data_dir, 0)
	// find all the modules , and add to a list , may use different way in
	// mobile..
	gv_thread_lock lock(m_mutex);
	this->m_pimpl->m_modules_xml.clear();
	this->m_pimpl->m_modules_bin.clear();
	gv_global::fm->reset_work_path();
	gvt_array< gv_string > names;
	gv_string fn;
	fn << "*." << gvc_gv_xml_ext;
	gv_global::fm->find_file_with_pattern(	*fn, names, *gv_framework_config::get_xml_module_path());
	GVM_DEBUG_OUT("[fileio] find " << names.size() << " xml module");
	gv_file_manager* p = gv_global::fm.get();
	for (int i = 0; i < names.size(); i++)
	{
		gv_string id = p->get_main_name(names[i]);
		gv_id mod_name(*id);
		if (this->m_pimpl->m_modules_xml.find(mod_name))
		{
			GVM_WARNING("module name collision !!! the name is: "
						<< names[i] << "and :"
						<< this->m_pimpl->m_modules_xml[mod_name] << gv_endl);
		}
		GVM_DEBUG_OUT("register module " << mod_name << " as file " << names[i]
										 << gv_endl);
		this->m_pimpl->m_modules_xml[mod_name] = names[i];
	}

	names.clear();
	fn = "";
	fn << "*." << gvc_gv_bin_ext;
	gv_global::fm->find_file_with_pattern(
		*fn, names, *gv_framework_config::get_binary_module_path());
	GVM_DEBUG_OUT("[fileio] find " << names.size() << " gvb module");
	for (int i = 0; i < names.size(); i++)
	{
		gv_string id = p->get_main_name(names[i]);
		gv_id mod_name(*id);
		if (this->m_pimpl->m_modules_bin.find(mod_name))
		{
			GVM_WARNING("module name collision !!! the name is: "
						<< names[i] << "and :"
						<< this->m_pimpl->m_modules_bin[mod_name] << gv_endl);
		}
		GVM_DEBUG_OUT("register module " << mod_name << " as file " << names[i]
										 << gv_endl);
		this->m_pimpl->m_modules_bin[mod_name] = names[i];
	}
	return true;
}

gv_string gv_sandbox_manager::get_xml_module_file_path(const gv_id& name)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s = this->m_pimpl->m_modules_xml.find(name);
	if (s)
	{
		return *s;
	}
	return gv_string();
};

gv_string gv_sandbox_manager::get_bin_module_file_path(const gv_id& name)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s = this->m_pimpl->m_modules_bin.find(name);
	if (s)
	{
		return *s;
	}
	return gv_string();
};

void gv_sandbox_manager::register_new_xml_module_path(
	const gv_id& name, const gv_string_tmp& new_s)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s;
	s = this->m_pimpl->m_modules_xml.find(name);
	if (s && *s != new_s)
	{
		GVM_WARNING("module name collision !!! the name is: " << *s << "and :"
															  << new_s << gv_endl);
	}
	this->m_pimpl->m_modules_xml[name] = new_s;
}

void gv_sandbox_manager::register_new_bin_module_path(
	const gv_id& name, const gv_string_tmp& new_s)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s;
	s = this->m_pimpl->m_modules_bin.find(name);
	if (s && *s != new_s)
	{
		GVM_WARNING("module name collision !!! the name is: " << *s << "and :"
															  << new_s << gv_endl);
	}
	this->m_pimpl->m_modules_bin[name] = new_s;
}

gv_int gv_sandbox_manager::query_registered_modules(gvt_array< gv_id >& modules,
													gvt_array< gv_string >& paths,
													bool is_binary)
{
	if (is_binary)
	{
		return this->m_pimpl->m_modules_bin.query_all(modules, paths);
	}
	else
	{
		return this->m_pimpl->m_modules_xml.query_all(modules, paths);
	}
};

bool gv_sandbox_manager::add_new_xml_module(const gv_id& name,
											gv_sandbox* outer,
											bool force_create)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s;
	s = this->m_pimpl->m_modules_xml.find(name);
	if (s)
		return false;
	if (!outer)
		return false;
	if (outer->find_module(name))
		return false;
	gv_string_tmp path = gv_framework_config::get_xml_module_path();
	path << "/" << name << "." << gvc_gv_xml_ext;
	this->register_new_xml_module_path(name, path);
	if (force_create)
	{
		gv_module* pnew = gvt_cast< gv_module >(
			outer->create_object(gv_module::static_class(), name, NULL));
		outer->export_module(name);
	}
	return true;
};

bool gv_sandbox_manager::add_new_bin_module(const gv_id& name,
											gv_sandbox* outer,
											bool force_create)
{
	gv_thread_lock lock(m_mutex);
	gv_string* s;
	s = this->m_pimpl->m_modules_bin.find(name);
	if (s)
		return false;
	if (!outer)
		return false;
	if (outer->find_module(name))
		return false;
	gv_string_tmp path = gv_framework_config::get_binary_module_path();
	path << "/" << name << "." << gvc_gv_bin_ext;
	this->register_new_bin_module_path(name, path);
	if (force_create)
	{
		gv_module* pnew = gvt_cast< gv_module >(
			outer->create_object(gv_module::static_class(), name, NULL));
		gv_bool b = outer->save_module(name);
	}
	return true;
};
}