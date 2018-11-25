namespace gv
{

static gv_int comp_location(gv_object** p1, gv_object** p2)
{

	bool is_module1 = (*p1)->is_a(gv_module::static_class());
	bool is_module2 = (*p2)->is_a(gv_module::static_class());
	if (is_module1 && !is_module2)
		return -1;
	if (!is_module1 && is_module2)
		return 1;
	/*
  bool  is_class1=(*p1)->is_a(gv_class_info::static_class()) ;
  bool  is_class2=(*p2)->is_a(gv_class_info::static_class()) ;
  if (is_class1&& !is_class2)  return -1;
  if (!is_class1&& is_class2)  return 1;

  bool  is_var1=(*p1)->is_a(gv_var_info::static_class()) ;
  bool  is_var2=(*p2)->is_a(gv_var_info::static_class()) ;
  if (is_var1&& !is_var2)  return -1;
  if (!is_var1&& is_var2)  return 1; */

	gv_object_location s1 = (*p1)->get_location();
	gv_object_location s2 = (*p2)->get_location();
	if (s1 < s2)
		return -1;
	else if (s1 == s2)
		return 0;
	return 1;
}
struct object_p
{
	object_p()
	{
		m_p = 0;
	}
	object_p(gv_object* p)
	{
		m_p = p;
	};
	void operator=(gv_object* p)
	{
		m_p = p;
	}
	bool operator<(const object_p& p)
	{
		gv_object* p1 = m_p;
		gv_object* p2 = p.m_p;
		bool is_module1 = p1->is_a(gv_module::static_class());
		bool is_module2 = p2->is_a(gv_module::static_class());
		if (is_module1 && !is_module2)
			return true;
		if (!is_module1 && is_module2)
			return false;

		bool is_class1 = p1->is_a(gv_class_info::static_class());
		bool is_class2 = p2->is_a(gv_class_info::static_class());
		if (is_class1 && !is_class2)
			return true;
		if (!is_class1 && is_class2)
			return false;

		gv_object_location s1 = p1->get_location();
		gv_object_location s2 = p2->get_location();
		if (s1 < s2)
			return true;
		return false;
	}
	gv_object* m_p;
};

class gv_ptr_export_register : public gvi_object_ptr_visitor
{
	// find all the reference to the object set , and clear to zero , prepare to
	// redirect these ptr.
	friend class gv_sandbox;

public:
	virtual gvi_object_ptr_visitor&
	operator()(gvt_ptr< gv_object >& obj_ptr, gvt_ref_ptr< gv_object >& obj_ref_ptr)
	{
		GV_ASSERT(m_mod);
		if (obj_ptr)
		{
			gv_int index;
			if (this->m_object_set.find(obj_ptr, index))
			{
				if (index != 0)
				{
					m_mod->register_export_ptr(obj_ptr->get_location(), obj_ptr);
					obj_ptr = NULL;
				}
			}
		}
		if (obj_ref_ptr)
		{
			gv_int index;
			if (this->m_object_set.find(obj_ref_ptr, index))
			{
				if (index != 0)
				{
					m_mod->register_export_ref_ptr(obj_ref_ptr->get_location(),
												   obj_ref_ptr);
					obj_ref_ptr = NULL;
				}
			}
		}
		return *this;
	};
	gvt_ptr< gv_module > m_mod;
	gvt_array< gv_object* > m_object_set;
};

bool gv_sandbox::query_module_object(const gv_id& mod_name,
									 gvt_array< gv_object* >& result,
									 bool ignore_module)
{
	gv_thread_lock lock(m_sandbox_mutex);
	for (int i = 0; i < gvc_max_object_hash; i++)
	{
		gv_object* pobj = this->m_pimpl->m_object_hash_table[i];
		while (pobj)
		{
			if (!pobj->is_deleted())
			{
				gv_object_location loc = pobj->get_location();
				if (!ignore_module || loc.size() > 1)
					if (loc.m_object_location[0] == mod_name)
						result.add(pobj);
			}
			pobj = pobj->m_next;
		}
	}
	// result.sort(&gv_object::static_compare_ptr);
	gvt_qsort(result.begin(), result.size(), comp_location);
	// gvt_array<object_p> test_a ;
	// test_a=result ;
	// test_a.sort();
	return result.size() != 0;
};

bool gv_sandbox::query_objects_owned_by(gv_object* owner,
										gvt_array< gv_object* >& result)
{
	gv_thread_lock lock(m_sandbox_mutex);
	for (int i = 0; i < gvc_max_object_hash; i++)
	{
		gv_object* pobj = this->m_pimpl->m_object_hash_table[i];
		while (pobj)
		{
			if (!pobj->is_deleted() && pobj->is_owned_by(owner))
			{
				result.add(pobj);
			}
			pobj = pobj->m_next;
		}
	}
	return result.size() != 0;
};

bool gv_sandbox::save_module(const gv_id& mod_name,
							 const gv_string_tmp& path_name)
{
	GV_PROFILE_EVENT(gv_sandbox__save_module, 0)
	gv_module* pmodule = this->find_module(mod_name);
	GV_ASSERT(pmodule);
	// gv_string filename;
	// filename<<path_name<<mod_name.string()<<"."<<gvc_gv_bin_ext;
	gv_sandbox_manager* pc = gv_global::sandbox_mama.get();
	gv_string filename = pc->get_bin_module_file_path(mod_name);
	if (filename.is_empty_string())
	{
		gv_global::sandbox_mama->add_new_bin_module(mod_name, this, true);
		gv_string_tmp path = gv_framework_config::get_binary_module_path();
		path << "/" << mod_name << "." << gvc_gv_bin_ext;
		gv_global::sandbox_mama->register_new_bin_module_path(mod_name, path);
		filename = pc->get_bin_module_file_path(mod_name);
	}
	gvi_stream* ps = gv_global::fm->open_binary_file_for_write(*filename);
	if (ps)
	{
		gvi_stream& str = *ps;
		pmodule->write(ps);
		delete ps;
		return true;
	}
	else
	{
		GVM_ERROR("failed to write module: " << filename);
	}
	return false;
};

gv_module* gv_sandbox::try_load_module(const gv_id& mod_name)
{
	gv_module* pmodule = this->load_module(mod_name);
	if (!pmodule)
		pmodule = this->import_module(mod_name);
	return pmodule;
}

gv_module* gv_sandbox::load_module(const gv_id& mod_name,
								   const gv_string_tmp& path_name,
								   bool force_reload)
{
	GV_PROFILE_EVENT(gv_sandbox__load_module, 0)
	gv_module* pmod = this->find_module(mod_name);
	if (pmod != NULL)
	{
		if (force_reload)
		{
			prepare_clear_reference_to_module(pmod);
		}
		else
			return pmod; // already loaded in memory
	}
	gv_sandbox_manager* pc = gv_global::sandbox_mama.get();
	gv_string filename = pc->get_bin_module_file_path(mod_name);
	if (filename.is_empty())
		return NULL;
	// gvi_stream * ps=gv_global::fm->open_binary_file_for_read(*filename);
	gvi_stream* ps = gv_global::fm->cache_file_for_read(*filename);
	if (!ps)
		return NULL;
	gvi_stream& str = *ps;
	if (!pmod)
	{
		pmod = this->create_object< gv_module >(gv_object_name(mod_name, 0));
	}
	gvt_array< gv_object* > objects;
	pmod->m_load_completed = false;
	pmod->read(ps, objects);
	delete ps;
	pmod->refresh_export_reference(); // write back the reference;
	pmod->refresh_import_reference();
	// pmod->refresh_import_reference();
	for (int i = 0; i < objects.size(); i++)
		objects[i]->post_load();
	pmod->m_load_completed = true;
	return pmod;
};
bool gv_sandbox::unload_module(const gv_id& mod_name)
{
	gv_module* pmod = this->find_module(mod_name);
	if (!pmod)
		return true;
	prepare_clear_reference_to_module(pmod);
	this->delete_object(pmod, true);
	return true;
};

void gv_sandbox::prepare_clear_reference_to_module(gv_module* pmod)
{
	gv_ptr_export_register v;
	this->query_module_object(pmod->get_name().get_id(), v.m_object_set, true);
	v.m_mod = pmod;
	gv_object_iterator_safe it(this);
	while (!it.is_empty())
	{
		it->get_class()->visit_ptr((gv_byte*)(gv_object*)it, v);
		++it;
	};
	for (int i = 0; i < v.m_object_set.size(); i++)
	{
		this->delete_object(v.m_object_set[i], true);
	};
};

gv_module* gv_sandbox::import_module(const gv_id& mod_name, gv_xml_parser& text,
									 bool force_reload)
{
	GV_PROFILE_EVENT(gv_sandbox__import_module, 0)
	text.read_element_open("gv_module", false);
	gv_uint version;
	text.read_attribute("version", version);
	gvt_array< gv_object* > new_objects;
	new_objects.reserve(2000);

	if (gvc_cu_xml_module_version != version)
	{
		GVM_PENDING;
	}
	gv_ulong time_stamp;
	text.read_attribute("modify_time", time_stamp);
	gv_module* pmod;
	if ((pmod = find_module(mod_name)) != NULL)
	{
		if (force_reload)
		{
			prepare_clear_reference_to_module(pmod);
		}
		else
			return pmod; // already loaded in memory
	}
	if (!pmod)
	{
		pmod = this->create_object< gv_module >(gv_object_name(mod_name, 0));
	}
	pmod->m_load_completed = false;
	gv_int nb_object;
	text.read_attribute("object_number", nb_object);
	text.read_element_open();
	for (int i = 0; i < nb_object; i++)
	{
		gv_id cls_id;
		text >> "<";
		text >> cls_id;
		gv_class_info* pcls = this->find_class(cls_id);
		if (!pcls)
		{
			GVM_WARNING(text.get_file_info() << "can't find the referenced class "
											 << cls_id << gv_endl);
			text.step_out_element(*cls_id.string());
			continue;
		}
		if (i == 0)
		{
			GV_ASSERT(pcls == gv_module::static_class());
			pcls->import_from_xml((gv_byte*)pmod, &text, false, this, pmod);
		}
		else
		{
			gv_object* pobj = NULL;
			pobj = pcls->create_object();
			pcls->import_from_xml((gv_byte*)pobj, &text, false, this, pmod);
			this->add_object(pobj);
			new_objects.push_back(pobj);
		}
	}
	text.read_element_close("gv_module");
	pmod->refresh_export_reference(); // write back the reference;
	pmod->refresh_import_reference();
	for (int i = 0; i < new_objects.size(); i++)
		new_objects[i]->post_load();
	pmod->m_load_completed = true;
	return pmod;
};

bool gv_sandbox::export_module(const gv_id& mod_name, gv_string_tmp& text)
{
	GV_PROFILE_EVENT(gv_sandbox__export_module, 0)
	gvt_array< gv_object* > objects;
	query_module_object(mod_name, objects);
	if (!objects.size())
		return false;

	gvt_xml_write_element_open(text, "gv_module", false);
	gvt_xml_write_attribute(text, "version", gvc_cu_xml_module_version);
	gvt_xml_write_attribute(text, "modify_time",
							gv_global::time->time_stamp_unix());
	gvt_xml_write_attribute(text, "object_number", objects.size());
	gvt_xml_write_element_open(text);
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->export_object_xml(text);
	}
	gvt_xml_write_element_close(text, "gv_module");
	return true;
};

gv_module* gv_sandbox::get_native_module()
{
	if (!m_pimpl->m_native_module)
	{
		m_pimpl->m_native_module = add_native_module(gv_id_native);
	}
	gv_module* pmod = m_pimpl->m_native_module;
	pmod->m_class = gv_module::static_class(); // in case the module is not
	return pmod;
};

void gv_sandbox::set_native_module(gv_module* m)
{

	m_pimpl->m_native_module = m;
	m_pimpl->m_native_module->m_is_native = 1;
}

gv_module* gv_sandbox::add_native_module(const gv_id& mod_name)
{
	gv_module* mod = new gv_module();
	mod->m_name = mod_name;
	mod->m_class = gv_module::static_class();
	this->add_object(mod);
	mod->m_is_native = 1;
	m_pimpl->m_native_modules.add_unique(mod);
	return mod;
};

gv_int gv_sandbox::get_nb_native_modules()
{
	return m_pimpl->m_native_modules.size();
};

gv_module* gv_sandbox::get_nth_native_module(int index)
{
	gv_module* pmod = m_pimpl->m_native_modules[index];
	pmod->m_class = gv_module::static_class();
	return pmod;
};

gv_module* gv_sandbox::find_module(const gv_id& mod_name)
{
	gv_object_location loc;
	loc.m_object_location.add(mod_name);
	return gvt_cast< gv_module >(this->find_object(loc));
}

gv_module* gv_sandbox::import_module(const gv_id& id)
{
	gv_sandbox_manager* pc = gv_global::sandbox_mama.get();
	gv_string file_name = pc->get_xml_module_file_path(id);
	if (!file_name.strlen())
	{
		GVM_WARNING("can't find module " << id << " to load !!" << gv_endl);
		// GV_DEBUG_BREAK;
		return NULL;
	};
	gvt_ref_ptr< gv_xml_parser > ps;
	ps = new gv_xml_parser;
	if (ps->load_file(*file_name))
	{
		return this->import_module(id, *ps);
	}
	else
	{
		GVM_WARNING("can't find module xml file " << id << " to load !!"
												  << gv_endl);
		return NULL;
	}
};

bool gv_sandbox::export_module(const gv_id& id)
{
	gv_sandbox_manager* pc = gv_global::sandbox_mama.get();
	gv_string file_name = pc->get_xml_module_file_path(id);
	if (!file_name.strlen())
	{
		GVM_WARNING("can't find module " << id << ",export to a new module !!"
										 << gv_endl);
		file_name = id.string();
		file_name << "." << gvc_gv_xml_ext;
	};
	gv_string_tmp text;
	if (!this->export_module(id, text))
	{
		GVM_WARNING("error in exporting module " << id << gv_endl);
		return false;
	}
	gv_save_string_to_file(*file_name, text);
	return true;
};

void gv_sandbox::set_resource_root_path(const gv_string_tmp& name)
{
	this->m_resource_root = name;
};
const gv_string& gv_sandbox::get_resource_root_path()
{
	return this->m_resource_root;
};
gv_string_tmp
gv_sandbox::get_physical_resource_path(const gv_string_tmp& name)
{
	gv_string_tmp s = name;
	s.replace_all(GV_RESOURCE_ROOT, *m_resource_root);
	return s;
};
gv_string_tmp gv_sandbox::get_logical_resource_path(const gv_string_tmp& name)
{
	gv_string_tmp s = name;
	s.replace_all(*m_resource_root, GV_RESOURCE_ROOT);
	return s;
};

gv_string_tmp
gv_sandbox::absolute_path_to_logical_resource_path(const gv_string_tmp& name)
{
	gv_string_tmp s = name;
	s.to_upper();
	gv_string_tmp resource_root;
	resource_root = gv_global::fm->get_work_path();
	resource_root += get_resource_root_path();
	resource_root = gv_get_full_path_name(resource_root);
	resource_root.to_upper();
	s.replace_all(resource_root, GV_RESOURCE_ROOT);
	return s;
};

gv_string_tmp
gv_sandbox::logical_resource_path_to_absolute_path(const gv_string_tmp& name)
{
	gv_string_tmp s = name;
	gv_string_tmp resource_root;
	resource_root = gv_global::fm->get_work_path();
	resource_root += get_resource_root_path();
	resource_root = gv_get_full_path_name(resource_root);
	s.replace_all(GV_RESOURCE_ROOT, resource_root);
	return s;
};

class gv_ptr_redirector : public gvi_object_ptr_visitor
{
	// find all the reference to the object set , and clear to zero , prepare to
	// redirect these ptr.
public:
	gv_ptr_redirector()
	{
		m_processed_count = 0;
	}
	friend class gv_sandbox;

public:
	virtual gvi_object_ptr_visitor&
	operator()(gvt_ptr< gv_object >& obj_ptr, gvt_ref_ptr< gv_object >& obj_ref_ptr)
	{
		gv_object* new_object;
		if (this->m_rediect_map.find(obj_ptr, new_object))
		{
			if (obj_ptr)
			{
				obj_ptr = new_object;
				m_processed_count++;
			}
		}
		if (this->m_rediect_map.find(obj_ref_ptr, new_object))
		{
			if (obj_ref_ptr)
			{
				obj_ref_ptr = new_object;
				m_processed_count++;
			}
		}
		return *this;
	};
	gvt_hash_map< gv_object*, gv_object*, 1024 > m_rediect_map;
	int m_processed_count;
};

int gv_sandbox::redirect(gv_object* pobj_old, gv_object* pnew)
{
	gv_ptr_redirector v;
	v.m_rediect_map.add(pobj_old, pnew);
	gv_object_iterator_safe it(this);
	while (!it.is_empty())
	{
		it->get_class()->visit_ptr((gv_byte*)(gv_object*)it, v);
		++it;
	};
	return v.m_processed_count;
};

int gv_sandbox::redirect(gvt_array< gv_object* >& pobj_old,
						 gvt_array< gv_object* >& pobj_new)
{
	gv_ptr_redirector v;
	for (int i = 0; i < pobj_old.size(); i++)
	{
		v.m_rediect_map.add(pobj_old[i], pobj_new[i]);
	}
	gv_object_iterator_safe it(this);
	while (!it.is_empty())
	{
		it->get_class()->visit_ptr((gv_byte*)(gv_object*)it, v);
		++it;
	};
	return v.m_processed_count;
};

gv_native_module_guard::gv_native_module_guard(gv_sandbox* sandbox,
											   const char* name)
{
	m_sandbox = sandbox;
	m_module = sandbox->get_native_module();
	gv_module* m = m_sandbox->find_module(gv_id(name));
	if (!m)
	{
		m = sandbox->add_native_module(gv_id(name));
	}
	sandbox->set_native_module(m);
};
gv_native_module_guard::~gv_native_module_guard()
{
	m_sandbox->set_native_module(m_module);
};
}