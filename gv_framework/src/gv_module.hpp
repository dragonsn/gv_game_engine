namespace gv
{
//============================================================================================
//								:utility class to check object
//offset in file and build export table.
//============================================================================================
class gv_stream_imp_exp_builder : public gv_stream_proxy
{
public:
	gv_stream_imp_exp_builder(){};
	~gv_stream_imp_exp_builder(){};
	void set_user_data(void* pdata)
	{
		m_user_data = pdata;
		m_module = (gv_module*)pdata;
		m_real_stream->set_user_data(pdata);
	}
	void visit_ptr_to_object(void*& p)
	{
		gvt_ptr< gv_object >* pobj = (gvt_ptr< gv_object >*)&p;
		gv_int index = m_module->object_to_index(pobj->ptr(), true);
		GV_ASSERT(index);
	};
	void visit_ptr_to_struct(void*& p)
	{
		gvt_ptr< gv_byte >* pobj = (gvt_ptr< gv_byte >*)&p;
		// to check validation here.
		pobj = pobj;
	};
	void visit_ptr_to_type(void*& p)
	{
		gvt_ptr< gvi_type >* ptype = (gvt_ptr< gvi_type >*)&p;
		gv_int index = m_module->type_to_index(ptype->ptr(), true);
		GV_ASSERT(index);
	};

	void visit_ref_ptr_to_object(void*& p)
	{
		gvt_ref_ptr< gv_object >* pobj = (gvt_ref_ptr< gv_object >*)&p;
		gv_int index = m_module->object_to_index(pobj->ptr(), true);
		GV_ASSERT(index);
	};
	void visit_ref_ptr_to_struct(void*& p)
	{
		gvt_ref_ptr< gv_refable >* pobj = (gvt_ref_ptr< gv_refable >*)&p;
		// to check validation here.
		pobj = pobj;
	};
	void visit_ref_ptr_to_type(void*& p)
	{
		gvt_ref_ptr< gvi_type >* ptype = (gvt_ref_ptr< gvi_type >*)&p;
		gv_int index = m_module->type_to_index(ptype->ptr(), true);
		GV_ASSERT(index);
	};

	gv_module* m_module;
};

//============================================================================================
//								:some implementation data
//.
//============================================================================================
class gv_module_imp : public gv_refable
{
public:
	gv_module_imp(gv_module* module)
	{
		m_module = module;
		m_byte_counter = m_stream_counter =
			gv_global::fm->open_binary_size_counter();
		m_stream_counter->set_user_data(module);
		m_stream_imp_exp_builder = new gv_stream_imp_exp_builder;
		m_stream_imp_exp_builder->set_proxy(
			gv_global::fm->open_binary_size_counter());
		m_stream_imp_exp_builder->set_user_data(module);
	};
	~gv_module_imp()
	{
		delete m_stream_imp_exp_builder;
		delete m_stream_counter;
	}
	struct ptr_pair
	{
		ptr_pair(){};
		ptr_pair(const ptr_pair& p)
		{
			ptr = p.ptr;
			ref_ptr = p.ref_ptr;
		}
		ptr_pair& operator=(const ptr_pair& p)
		{
			ptr = p.ptr;
			ref_ptr = p.ref_ptr;
			return *this;
		}
		gvt_ptr< gvt_ptr< gv_object > > ptr;
		gvt_ptr< gvt_ref_ptr< gv_object > > ref_ptr;
	};

	typedef gvt_dlist< ptr_pair > ptr_list;
	typedef gvt_hash_map< gv_object_location_with_ptr, ptr_list, 1024 > ptr_map;
	gv_module* m_module;
	ptr_map m_import_map;
	ptr_map m_export_map;
	gvi_stream* m_byte_counter;
	gv_stream_imp_exp_builder* m_stream_imp_exp_builder;
	gvi_stream* m_stream_counter;
	gvt_hash_map< gv_object_location, gv_int, 4096 > m_object_index_map;
	gvt_hash_map< gv_string, gv_int, 4096 > m_type_index_map;
};
//============================================================================================
//								:
//============================================================================================
gv_module::gv_module()
{
	m_load_completed = true;
	m_module_version = 0; // only set version when  gvc_cu_module_version;
	link_class(gv_module::static_class());
}
gv_module::~gv_module()
{
	gvt_safe_delete(m_pimpl);
}

void gv_module::prepare_imp_exp()
{
	if (!this->m_pimpl)
	{
		this->m_pimpl = new gv_module_imp(this);
	}
};

gv_int gv_module::register_import_ref_ptr(const gv_object_location& loc,
										  gvt_ref_ptr< gv_object >& reference)
{
	prepare_imp_exp();
	gv_module_imp::ptr_pair pair;
	pair.ref_ptr = &reference;
	this->m_pimpl->m_import_map[loc].push_back(pair);
	return 0;
};
gv_int gv_module::register_import_ptr(const gv_object_location& loc,
									  gvt_ptr< gv_object >& reference)
{
	prepare_imp_exp();
	gv_module_imp::ptr_pair pair;
	pair.ptr = &reference;
	this->m_pimpl->m_import_map[loc].push_back(pair);
	return 0;
};

gv_int gv_module::register_export_ref_ptr(const gv_object_location& loc,
										  gvt_ref_ptr< gv_object >& reference)
{
	prepare_imp_exp();
	gv_module_imp::ptr_pair pair;
	pair.ref_ptr = &reference;
	this->m_pimpl->m_export_map[loc].push_back(pair);
	return 0;
};
gv_int gv_module::register_export_ptr(const gv_object_location& loc,
									  gvt_ptr< gv_object >& reference)
{
	prepare_imp_exp();
	gv_module_imp::ptr_pair pair;
	pair.ptr = &reference;
	this->m_pimpl->m_export_map[loc].push_back(pair);
	return 0;
};

void gv_module::refresh_import_reference()
{
	GV_PROFILE_EVENT(refresh_import_reference, 0)
	if (!this->m_pimpl)
		return;
	gv_module_imp::ptr_map::iterator it = this->m_pimpl->m_import_map.begin();
	while (it != this->m_pimpl->m_import_map.end())
	{
		if (!it.is_empty())
		{
			gv_module_imp::ptr_list& list = *it;
			gv_object_location_with_ptr& loc = it.get_key();
			if (!loc.m_ptr && !loc.m_cached)
			{
				loc.m_ptr = this->get_sandbox()->find_object(loc);
				loc.m_cached = true;
			}
			gv_object* pobj = loc.m_ptr;
			if (!pobj && loc[0].get_id() != gv_id_none)
			{
				gv_module* pmod = get_sandbox()->find_module(loc[0].get_id());
				if (!pmod)
					pmod = get_sandbox()->load_module(loc[0].get_id());
				if (!pmod)
					pmod = get_sandbox()->import_module(loc[0].get_id()); //
				if (!pmod)
				{
					GVM_WARNING("import object : module "
								<< loc[0] << "not found  referenceed  in module "
								<< this->get_name() << gv_endl);
					GV_DEBUG_BREAK;
				}
				else
				{
					pobj = this->get_sandbox()->find_object(loc); // find again..
					if (!pobj && pmod)
					{
						GVM_WARNING("[load] import object :"
									<< loc << "not found  referenceed  in module "
									<< this->get_name() << gv_endl);
						// GV_DEBUG_BREAK;
					}
				}
			}

			{ // assign the ptr
				gv_module_imp::ptr_list::iterator lit = list.begin();
				while (lit != list.end())
				{
					if (lit->ptr)
						(*lit->ptr) = gvt_ptr< gv_object >(pobj);
					if (lit->ref_ptr)
						(*lit->ref_ptr) = gvt_ref_ptr< gv_object >(pobj);
					++lit;
				}
			}
		}
		++it;
	} // next map it
};
void gv_module::refresh_export_reference()
{
	GV_PROFILE_EVENT(refresh_export_reference, 0)
	if (!this->m_pimpl)
		return;
	gv_module_imp::ptr_map::iterator it = this->m_pimpl->m_export_map.begin();
	while (it != this->m_pimpl->m_export_map.end())
	{
		if (!it.is_empty())
		{
			gv_module_imp::ptr_list& list = *it;
			gv_object_location_with_ptr& loc = it.get_key();
			if (!loc.m_ptr && !loc.m_cached)
			{
				loc.m_ptr = this->get_sandbox()->find_object(loc);
				loc.m_cached = true;
			}
			gv_object* pobj = loc.m_ptr;
			if (!pobj)
			{
				if (!pobj)
				{
					GVM_WARNING("export object :"
								<< loc << "not found   in module " << this->get_name()
								<< " the reference is set to NULL " << gv_endl);
				}
			}

			{ // assign the ptr
				gv_module_imp::ptr_list::iterator lit = list.begin();
				while (lit != list.end())
				{
					if (lit->ptr)
						(*lit->ptr) = gvt_ptr< gv_object >(pobj);
					if (lit->ref_ptr)
						(*lit->ref_ptr) = gvt_ref_ptr< gv_object >(pobj);
					++lit;
				}
			}
		}
		++it;
	} // next map it ;
};
void gv_module::clear_import_export_reference()
{
	gvt_safe_delete(m_pimpl);
};

gv_object_location gv_module::index_to_object_location(gv_int index)
{
	if (index > 0)
	{
		return this->m_import_table[index - 1].m_location;
	}
	if (index < 0)
	{
		return this->m_export_table[-index - 1].m_location;
	}
	return gv_object_location();
};

gv_int gv_module::object_to_index(gv_object* object, bool add_entry)
{
	gv_int* ret;
	if (!object)
		return 0;
	ret = this->get_imp()->m_object_index_map.find(object->get_location());
	if (ret)
		return *ret;
	GV_ASSERT(add_entry);
	bool is_import = true;
	GV_ASSERT(object->get_module());
	if (object->get_module() == this)
	{
		is_import = false;
	}
	gv_object_location_info info;
	info.m_location = object->get_location().m_object_location;
	gv_int object_index = 0;
	info.m_resolved_address = object;
	if (is_import)
	{
		this->m_import_table.push_back(info);
		object_index = this->m_import_table.size();
	}
	else
	{
		this->m_export_table.push_back(info);
		object_index = -this->m_export_table.size();
	}
	get_imp()->m_object_index_map[object->get_location()] = object_index;
	return object_index;
};

gv_object* gv_module::index_to_object(gv_int index)
{
	if (!index)
		return NULL;
	gv_object* ret = NULL;
	if (index > 0)
	{
		gv_object_location_info& info = this->m_import_table[index - 1];
		ret = info.m_resolved_address;
	}
	else
	{
		gv_object_location_info& info = this->m_export_table[-index - 1];
		ret = info.m_resolved_address;
	}
	return ret;
};

gv_int gv_module::type_to_index(gvi_type* type, bool add_entry)
{
	gv_int* ret;
	if (!type)
		return 0;
	ret = this->get_imp()->m_type_index_map.find(*type->get_type_string());
	if (ret)
		return *ret;
	GV_ASSERT(add_entry);
	gv_module_type_info info;
	info.m_type_string = *type->get_type_string();
	info.m_index = this->m_type_table.size();
	info.m_type = type;
	m_type_table.push_back(info);
	gv_int index = info.m_index + 1;
	m_pimpl->m_type_index_map[*info.m_type_string] = index;
	return index;
};

gvi_type* gv_module::index_to_type(gv_int index)
{
	if (!index)
		return NULL;
	gv_module_type_info& info = m_type_table[index - 1];
	gvi_type* type = info.m_type;
	if (!type)
	{
		gv_string_tmp name;
		name = info.m_type_string;
		gv_xml_restore_gt_lt(name);
		info.m_type = get_sandbox()->create_type(*name);
	}
	return info.m_type;
};

bool gv_module::read(gvi_stream* ps, gvt_array< gv_object* >& objects)
{
	GV_PROFILE_EVENT(gv_module__read, 0);
	gvt_safe_delete(m_pimpl);
	prepare_imp_exp();
	;
	gv_int nb_obj;
	gvi_stream& str = *ps;
	str.set_user_data(this);
	/*{
          gvt_ref_ptr<gvi_type> type=get_sandbox()->create_type("gvt_array<
  gv_object_location_info>");
          type->read_data(gvt_byte_ptr(this->m_import_table) ,ps);
          type->read_data(gvt_byte_ptr(this->m_export_table) ,ps);
  }
  {
          gvt_ref_ptr<gvi_type>
  type=get_sandbox()->create_type("gvt_array<gv_module_type_info>");
          type->read_data(gvt_byte_ptr(this->m_type_table) ,ps);
  }*/
	{ // rebuild imp exp map
		GV_PROFILE_EVENT(gv_module__rebuild_imp_exp_map, 0);
		str >> this->m_import_table;
		str >> this->m_export_table;
		str >> this->m_type_table;
		for (int i = 0; i < this->m_import_table.size(); i++)
		{
			this->get_imp()->m_object_index_map[this->m_import_table[i].m_location] =
				i + 1;
		}
		for (int i = 0; i < this->m_export_table.size(); i++)
		{
			this->get_imp()->m_object_index_map[this->m_export_table[i].m_location] =
				-i - 1;
		}
		for (int i = 0; i < this->m_type_table.size(); i++)
		{
			this->get_imp()->m_type_index_map[this->m_type_table[i].m_type_string] =
				i + 1;
		}
	}
	int index = this->object_to_index(this);
	this->m_export_table[0].m_resolved_address = this;
	for (int i = 0; i < this->m_import_table.size(); i++)
	{
		gv_id import_module_name = this->m_import_table[i].m_location[0].get_id();
		gv_module* ref = this->get_sandbox()->find_module(import_module_name);
		if (!ref)
		{
			ref = get_sandbox()->load_module(import_module_name);
			// to do check newer file to load.
			if (!ref)
				ref = get_sandbox()->import_module(import_module_name);
			GV_ASSERT(ref);
		}
		else
		{
			if (!ref->m_load_completed)
			{
				GVM_WARNING("!!! circular reference for "
							<< ref->get_name_id() << " and  " << this->get_name_id()
							<< gv_endl);
				GV_ASSERT(0);
			}
		}
		this->m_import_table[i].m_resolved_address =
			get_sandbox()->find_object(this->m_import_table[i].m_location);
	}
	this->get_class()->read_data(gvt_byte_ptr(*this), ps);
	str >> nb_obj;
	objects.resize(nb_obj);
	for (int i = 0; i < nb_obj; i++)
	{
		gv_int cls_index;
		str >> cls_index;
		gv_class_info* cls =
			gvt_cast< gv_class_info >(this->index_to_object(cls_index));
		GV_ASSERT(cls);
		gv_byte* new_obj = cls->create_instance();
		cls->get_persistent_class()->read_data(new_obj, ps);
		objects[i] = (gv_object*)new_obj;
		get_sandbox()->add_object(objects[i]);
		gv_int index = object_to_index(objects[i]);
		GV_ASSERT(index);
		this->m_export_table[-index - 1].m_resolved_address = objects[i];
	}
	return true;
};

bool gv_module::do_write(gvi_stream* ps, bool create_imp_exp)
{
	gvi_stream& str = *ps;
	// write objects
	gvt_array< gv_object* > objects;
	get_sandbox()->query_module_object(this->get_name().get_id(), objects, true);
	/*if (create_imp_exp)
  {
          for ( int i=0; i< objects.size(); i++)
          {
                  object_to_index(objects[i],true);
          }
  }
  {
          gvt_ref_ptr<gvi_type> type=get_sandbox()->create_type("gvt_array<
  gv_object_location_info>");
          type->write_data(gvt_byte_ptr(this->m_import_table) ,ps);
          type->write_data(gvt_byte_ptr(this->m_export_table) ,ps);
  }
  {
          gvt_ref_ptr<gvi_type>
  type=get_sandbox()->create_type("gvt_array<gv_module_type_info>");
          type->write_data(gvt_byte_ptr(this->m_type_table) ,ps);
  }*/
	str << this->m_import_table;
	str << this->m_export_table;
	str << this->m_type_table;
	this->get_class()->write_data(gvt_byte_ptr(*this), &str);
	gv_int nb_obj = objects.size();
	str << nb_obj;
	for (int i = 0; i < nb_obj; i++)
	{
		gv_object_name cls =
			objects[i]->get_class()->get_persistent_class()->get_name();
		gv_object* pobj = objects[i];
		gv_int index = this->object_to_index(pobj, create_imp_exp);
		gv_object_location_info& info = this->m_export_table[-index - 1];
		info.m_offset_in_file = str.tell();
		gv_int cls_index = object_to_index(objects[i]->get_class(), create_imp_exp);
		str << cls_index;
		objects[i]->get_class()->get_persistent_class()->write_data(
			(gv_byte*)objects[i], &str);
	}
	return true;
};

bool gv_module::write(gvi_stream* ps)
{
	ps->set_user_data(this);
	m_module_version = gvc_cu_module_version;
	prepare_imp_exp();
	rebuild_imp_exp_map();
	this->do_write(ps, false);
	return true;
};

void gv_module::rebuild_imp_exp_map()
{
	prepare_imp_exp();
	this->m_import_table.clear();
	this->m_export_table.clear();
	this->m_type_table.clear();
	m_pimpl->m_export_map.clear();
	m_pimpl->m_import_map.clear();
	m_pimpl->m_object_index_map.clear();
	m_pimpl->m_type_index_map.clear();
	get_imp()->m_stream_counter->seek_from_begin(0);
	get_imp()->m_stream_imp_exp_builder->seek_from_begin(0);
	get_imp()->m_byte_counter = get_imp()->m_stream_imp_exp_builder;
	this->object_to_index(this, true);
	this->do_write(get_imp()->m_stream_counter, true);
	// GV_ASSERT(get_imp()->m_stream_counter->tell()==get_imp()->m_stream_imp_exp_builder->tell());
	get_imp()->m_byte_counter = get_imp()->m_stream_counter;
	m_pimpl->m_byte_counter->seek_from_begin(0);
}

gvi_stream* gv_module::get_size_counter()
{
	return m_pimpl->m_byte_counter;
};

gv_module_imp* gv_module::get_imp()
{
	return m_pimpl;
}
//============================================================================================
//								:
//============================================================================================
GVM_IMP_STRUCT(gv_object_location_info)
GVM_VAR(gvt_array< gv_object_name >, m_location)
GVM_VAR(gv_int, m_offset_in_file)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_module_type_info)
GVM_VAR(gv_text, m_type_string)
GVM_END_STRUCT

GVM_IMP_CLASS(gv_module, gv_object)
GVM_VAR(gv_id, m_tag)
GVM_VAR(gv_int, m_module_version)
GVM_VAR_ATTRIB_SET(transient)
// GVM_VAR(gvt_array< gv_object_location_info> ,m_export_table)
// GVM_VAR(gvt_array< gv_object_location_info> ,m_import_table);
GVM_END_CLASS
}