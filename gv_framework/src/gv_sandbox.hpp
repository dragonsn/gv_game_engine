namespace gv
{

void gv_sandbox_handle::init(gv_sandbox* sandbox)
{
	if (sandbox)
		(*this) = sandbox->get_handle();
	else
	{
		m_sandbox_id = gvc_max_sand_box_nb;
		m_sandbox_uuid = 0;
	}
};

gv_sandbox* gv_sandbox_handle::get_sandbox()
{
	return gv_global::sandbox_mama->get_sand_box(*this);
};

class gv_sandbox_data
{
	friend class gv_sandbox;

public:
	typedef gvt_hash_map< gv_id, gvt_ref_ptr< gv_class_info >, 1024 > class_map;
	typedef gvt_pool_array< gvt_ref_ptr< gv_object > > object_table;
	typedef gvt_array< gvt_ref_ptr< gvi_type > > persistent_type_array;
	typedef gvt_array< gvt_ref_ptr< gv_object > > object_array;
	typedef gvt_array_static< gvt_ref_ptr< gv_event_processor >,
							  gvc_max_object_event_channel >
		event_processor_array;
	typedef gvt_hash_map< gv_id, gvt_ref_ptr< gv_enum_info >, 1024 > enum_map;

	gv_sandbox_data()
	{
		memset(m_object_hash_table, 0, sizeof(m_object_hash_table));
		m_is_destructing = false;
	}
	~gv_sandbox_data()
	{
		m_is_destructing = true;
		m_event_processors = NULL;
	}

	void clear_event_processor()
	{
		m_active_event_processors.clear();
		for (int i = 0; i < m_event_processors.size(); i++)
		{
			if (m_event_processors[i] &&
				m_event_processors[i]->get_sandbox() == m_owner)
			{
				m_event_processors[i]->set_autonomous(false);
				m_event_processors[i]->unregister_events();
			}
			m_event_processors[i] = NULL;
		}
	}

	gvt_ptr< gv_sandbox > m_owner;
	object_table m_obj_table;
	class_map m_cls_map;
	enum_map m_enum_map;
	persistent_type_array m_native_types;
	object_array m_objects_wait_for_delete;
	gv_object* m_object_hash_table[gvc_max_object_hash];
	gvt_ref_ptr< gv_module > m_native_module;
	gvt_array< gvt_ref_ptr< gv_module > > m_native_modules;
	event_processor_array m_event_processors;
	gvt_array< gvt_ref_ptr< gv_event_processor > > m_active_event_processors;
	gv_bool m_is_destructing;
};

gv_sandbox::gv_sandbox()
{
	m_pimpl = new gv_sandbox_data;
	m_pimpl->m_owner = this;
	m_max_name_id = 1;
	m_index = 0;
	m_outer = NULL;
	m_use_name_hash = true;
	m_delta_time = 0;
	m_last_update_time_stamp = m_start_time_stamp = get_time_in_seconds();
	m_tick_count = 0;
};

gv_sandbox::~gv_sandbox()
{
	gvt_safe_delete(m_pimpl);
};

bool gv_sandbox::is_destructing()
{
	return m_pimpl->m_is_destructing;
};

gv_ushort gv_sandbox::get_index()
{
	return m_index;
};
gv_uint gv_sandbox::get_runtime_uuid()
{
	return m_uuid;
}
gv_sandbox_handle gv_sandbox::get_handle()
{
	gv_sandbox_handle hd;
	hd.m_sandbox_id = this->m_index;
	hd.m_sandbox_uuid = this->m_uuid;
	return hd;
};

gv_sandbox* gv_sandbox::get_outer()
{
	return m_outer;
}
//---object  manager-------------------------------------------->
gv_int gv_sandbox::hash_object(gv_object* pobj)
{
	return hash_object(pobj->m_name);
};
gv_int gv_sandbox::hash_object(const gv_object_name& name)
{
	return gvt_hash(name) % gvc_max_object_hash;
};
gv_uint gv_sandbox::get_nb_object()
{
	return m_pimpl->m_obj_table.get_used_size();
};
gv_object* gv_sandbox::get_object(gv_int index)
{
	return m_pimpl->m_obj_table[index];
};
gv_object* gv_sandbox::get_object(const gv_object_handle& handle)
{
	if (this->is_valid(handle))
	{
		return m_pimpl->m_obj_table[handle.m_index];
	}
	return NULL;
};
gv_object* gv_sandbox::create_object(gv_class_info* class_info,
									 const gv_object_name& name,
									 gv_object* outer)
{
	gv_thread_lock lock(m_sandbox_mutex);
	gv_object* object = class_info->create_object();
	object->m_owner = outer;
	object->m_name = name;
	this->add_object(object);
	return object;
};

gv_object* gv_sandbox::create_object(gv_class_info* class_info,
									 gv_object* outer)
{
	return create_object(
		class_info, get_unique_object_name(class_info->get_name_id()), outer);
};

gv_object* gv_sandbox::create_nameless_object(gv_class_info* class_info)
{
	// no lock , much faster !
	GV_ASSERT(class_info);
	gv_object* object = class_info->create_object();
	return object;
};

gv_object* gv_sandbox::clone(gv_object* pobj)
{
	gv_thread_lock lock(m_sandbox_mutex);
	gv_string_tmp s;
	if (!pobj->get_name_id().string().has_prefix("clone_"))
	{
		s = "clone_of_";
	}
	s << pobj->get_name_id();
	gv_object_name name = get_unique_object_name(gv_id(*s));
	gv_object* new_obj = pobj->get_class()->create_object();
	pobj->get_class()->copy_to(pobj, new_obj);
	new_obj->m_name = name;
	this->add_object(new_obj);
	new_obj->post_load();
	return new_obj;
};

void gv_sandbox::add_object(gv_object* object)
{
	gv_object_ptr* pptr = m_pimpl->m_obj_table.allocate();
	object->m_runtime_index = m_pimpl->m_obj_table.get_index(pptr);
	// GVM_DEBUG_LOG(main,"sandbox "<<this<< "[ OBJ]m_obj_table allocate " <<
	// object->m_runtime_index<<"TO "<<object<<gv_endl);;
	(*pptr) = object;
	GV_ASSERT(object->m_sandbox == NULL);
	object->m_sandbox = this;

	if (m_use_name_hash)
	{
		this->add_object_to_hash(object);
	}
	object->m_is_in_sandbox = 1;
	return;
};
bool gv_sandbox::is_valid(const gv_object_handle& handle)
{
	if (handle.m_index >= (gv_uint)m_pimpl->m_obj_table.size())
		return false;
	gv_object* pobj = m_pimpl->m_obj_table[handle.m_index];
	if (!pobj)
		return false;
	if (pobj != handle.m_object)
		return false;
	if (pobj->get_name() == handle.m_name)
		return true;
	return false;
};

bool gv_sandbox::is_valid(const gv_object* s) const
{
	if (!s)
		return false;
	if (s->get_runtime_index() >= (gv_uint)m_pimpl->m_obj_table.size())
		return false;
	gv_object* pobj = m_pimpl->m_obj_table[s->get_runtime_index()];
	if (!pobj)
		return false;
	if (pobj != s)
		return false;
	// if (!s->is_deleted()){
	//	if (this->find_object(s->get_location()) != s) return false;
	//}
	return true;
};

gv_object* gv_sandbox::find_object(const gv_object_location& location,
								   bool find_in_outer)
{
	gv_thread_lock lock(m_sandbox_mutex);
	if (!location.m_object_location.size())
		return NULL;
	const gv_object_name& name = *location.m_object_location.last();
	int h = hash_object(name);
	gv_object* p = m_pimpl->m_object_hash_table[h];
	while (p)
	{
		if (!p->is_deleted() &&
			p->is_in_location(location) /*p->get_location()==location*/)
		{
			return p;
		}
		p = p->m_next;
	}
	if (!p && this->get_outer() && find_in_outer)
		return this->m_outer->find_object(location);
	return NULL;
};
gv_object* gv_sandbox::load_object(const gv_object_location& location)
{
	gv_thread_lock lock(m_sandbox_mutex);
	if (!location.m_object_location.size())
		return NULL;
	gv_object* pobject = find_object(location);
	if (pobject)
		return pobject;
	// load module
	load_module(location.m_object_location[0].get_id());
	return find_object(location);
	;
}
void gv_sandbox::add_object_to_hash(gv_object* obj)
{
	GV_ASSERT(obj->m_sandbox == this);
	if (obj->m_name.get_id() == gv_id_null)
		return;
	if (obj->m_name.get_id() != gv_id_native)
	{
// only allow the native module have multiple instance..
#if GV_DEBUG_VERSION
		gv_object* old = find_object(obj->get_location());
		if (old)
		{
			GVM_WARNING("name collision for " << obj->get_location());
		}
// GV_ASSERT(old);
#endif
	}
	int h = hash_object(obj);
	obj->m_next = m_pimpl->m_object_hash_table[h];
	m_pimpl->m_object_hash_table[h] = obj;
	// if (obj->get_name_id() == "gv_impexp_fbx_config")
	//{
	//	GV_DEBUG_BREAK;
	//}
	m_max_name_id =
		gvt_max(obj->get_name().get_postfix(), (gv_uint)m_max_name_id.get());
};
void gv_sandbox::remove_object_from_hash(gv_object* obj)
{

	if (obj->m_name.get_id() == gv_id_null)
		return;
	// if (obj->get_name_id() == "gv_impexp_fbx_config")
	//{
	//	GV_DEBUG_BREAK;
	//}
	int h = gvt_hash(obj->m_name) % gvc_max_object_hash;
	gv_object* p = m_pimpl->m_object_hash_table[h];
	gv_object* pre = NULL;
	while (p && p != obj)
	{
		pre = p;
		p = p->m_next;
	}
	// if (!p) return ;
	GV_ASSERT(p);
	if (pre)
		pre->m_next = obj->m_next;
	else
		m_pimpl->m_object_hash_table[h] = obj->m_next;
};

void gv_sandbox::rename_object(gv_object* obj, const gv_object_name new_name,
							   bool need_unique)
{
	gv_thread_lock lock(m_sandbox_mutex);
	GV_ASSERT(obj->m_sandbox == this);
	this->remove_object_from_hash(obj);
	obj->m_name = new_name;
	this->add_object_to_hash(obj);
};

bool gv_sandbox::pre_tick()
{
#if GV_WITH_OS_API
	if (!gv_global::config.no_reset_path)
		gv_global::fm->reset_work_path(); //
#endif
	int size = this->m_pimpl->m_active_event_processors.size();
	m_tick_count++;
	gv_float dt = (gv_float)(get_time_in_seconds() - m_last_update_time_stamp);
	m_last_update_time_stamp = get_time_in_seconds();
	this->m_delta_time = gvt_clamp(dt, 0.f, 0.1f);
	{
		GV_PROFILE_EVENT(gv_sand_box_start_event, 0)
		for (int i = 0; i < size; i++)
		{
			gv_event_processor* p = this->m_pimpl->m_active_event_processors[i];
			if (p->is_synchronized() && p->is_autonomous())
			{
				p->signal_frame_start();
			}
		}
	}
	return true;
};

bool gv_sandbox::post_tick()
{
	GV_PROFILE_EVENT(gv_sand_box_post_tick, 0)
	int size = this->m_pimpl->m_active_event_processors.size();
	for (int i = 0; i < size; i++)
	{
		gv_event_processor* p = this->m_pimpl->m_active_event_processors[i];
		if (p->is_synchronized() && p->is_autonomous())
		{
			p->wait_signal_frame_end();
			if (!p->do_synchronization())
				return false;
		}
	}
	for (int i = 0; i < size; i++)
	{
		gv_event_processor* p = this->m_pimpl->m_active_event_processors[i];
		if (!p->is_autonomous())
		{
			if (!p->do_synchronization())
				return false;
		}
	}
	if (this->m_engine)
	{
		if (!this->m_engine->do_synchronization())
			return false;
	}
	this->collect_garbage();
	return true;
};

bool gv_sandbox::do_tick()
{
	GV_PROFILE_EVENT(gv_sand_box_do_tick, 0)
	gv_global::input->tick();
	int size = this->m_pimpl->m_active_event_processors.size();
	for (int i = 0; i < size; i++)
	{
		gv_event_processor* p = this->m_pimpl->m_active_event_processors[i];
		if (!p->is_autonomous())
		{
			if (!p->tick(p->corrected_delta_time(m_delta_time)))
			{
				GVM_DEBUG_OUT(p->get_name() << " tick return false, signal of quit");
				return false;
			}
		}
	}
	if (this->m_engine)
	{
		if (!this->m_engine->tick(m_delta_time))
		{
			GVM_DEBUG_OUT(this->m_engine->get_name() << " tick return false, signal of quit");
			return false;
		}
		
	}
	return true;
};

//---tick-------------------------------------------------------->
bool gv_sandbox::tick()
{
	GV_PROFILE_EVENT(gv_sand_box_tick, 0)
	if (!pre_tick())
		return false;
	if (!do_tick())
		return false;
	if (!post_tick())
		return false;
	return true;
};

gv_float gv_sandbox::get_delta_time()
{
	return m_delta_time;
};

gv_double gv_sandbox::get_time_in_seconds()
{
	return gv_global::time->get_microsec_from_start() / 1000000.f;
	// return ( gv_global::time->time_stamp_unix()-m_start_time_stamp)/1000000.0;
};
gv_ulong gv_sandbox::get_perf_count()
{
	return gv_global::time->get_performance_counter();
};

bool gv_sandbox::init(gv_uint max_object_nb, bool enable_object_name_hash)
{
	this->m_pimpl->m_obj_table.reset(max_object_nb);
	this->m_use_name_hash = enable_object_name_hash;
	return true;
};

class gv_ptr_visitor_clear : public gvi_object_ptr_visitor
{
	// find all the reference to the object set , prepare to redirect these ptr.
	friend class gv_sandbox;

public:
	virtual gvi_object_ptr_visitor&
	operator()(gvt_ptr< gv_object >& obj_ptr, gvt_ref_ptr< gv_object >& obj_ref_ptr)
	{
		if (obj_ptr)
		{
			obj_ptr = NULL;
		}
		if (obj_ref_ptr)
		{
			GV_ASSERT(obj_ref_ptr->get_ref() >= 1 &&
					  "should not free object here!!!,maybe due to declare and "
					  "definition not match e.g. in c++ head use gvt_ptr, but in "
					  "macro use gvt_ref_ptr!!!");
			obj_ref_ptr = NULL;
		}
		return *this;
	};
};

void gv_sandbox::stop_event_processor()
{
	m_pimpl->clear_event_processor();
};

void gv_sandbox::destroy()
{
	m_pimpl->clear_event_processor();
	gv_object_iterator it(this);
	gv_ptr_visitor_clear v;
	while (!it.is_empty())
	{
		// call virtual function first , then use class inf to clear the rest
		it->clear_ref_to_others();
		gv_class_info* pcls = it->get_class();
		if (pcls != gv_class_info::static_class() &&
			pcls != gv_var_info::static_class() &&
			pcls != gv_func_info::static_class())
		{
			it->get_class()->visit_ptr((gv_byte*)(gv_object*)it, v);
		}
		it->m_is_deleted = 1;
		it->set_sandbox(NULL);
		++it;
	};
	// check all the dead object in obj_table
	for (int i = 0; i < m_pimpl->m_objects_wait_for_delete.size(); i++)
	{
		gv_object* obj = m_pimpl->m_objects_wait_for_delete[i];
		if (obj->get_ref() != 1)
		{
			GVM_WARNING("deleted object " << obj->get_name()
										  << "in dying sandbox ref count is "
										  << obj->get_ref());
		}
	}
	{ // check all the alive object in obj_table
		gv_object_iterator it(this);
		while (!it.is_empty())
		{

			if (it->get_ref() != 1)
			{
				GVM_WARNING("alive object " << it->get_name()
											<< "in dying sandbox ref count is "
											<< it->get_ref());
			}
			++it;
		};
	}
};

int gv_sandbox::delete_object(gv_object* pobj, bool force_clear)
{
	gv_thread_lock lock(m_sandbox_mutex);
	GV_ASSERT(pobj->m_is_in_sandbox);
	GV_ASSERT(pobj->get_sandbox() == this);
	GV_ASSERT(is_valid(pobj));
	pobj->set_owner(NULL);
	this->remove_object_from_hash(pobj);
	pobj->unregister_events();
	if (!force_clear)
	{
		pobj->m_is_deleted = 0; // to pass the inc ref
		this->m_pimpl->m_objects_wait_for_delete.push_back(pobj);
	}
	pobj->m_is_deleted = 1;
	// pobj->destroy();
	// GVM_DEBUG_LOG(main,"sandbox "<<this<< "[ OBJ]m_obj_table free " <<
	// pobj->m_runtime_index<<"FROM "<<pobj<<gv_endl);;
	this->m_pimpl->m_obj_table.free(pobj->m_runtime_index);
	this->m_pimpl->m_obj_table[pobj->m_runtime_index] = NULL;
	return 1;
}

int gv_sandbox::delete_object_tree(gv_object* top, bool clear_reference)
{
	gv_thread_lock lock(m_sandbox_mutex);
	if (!top)
		return 0;
	gvt_array< gv_object* > objects;
	this->query_objects_owned_by(top, objects);
	objects.push_back(top);
	if (clear_reference)
	{
		gvt_array< gv_object* > new_ptrs;
		new_ptrs.resize(objects.size());
		new_ptrs = NULL;
		this->redirect(objects, new_ptrs);
	};
	for (int i = 0; i < objects.size(); i++)
	{
		this->delete_object(objects[i]);
	}
	return objects.size();
};

bool gv_sandbox::collect_garbage()
{
	gv_thread_lock lock(m_sandbox_mutex);
	for (int i = 0; i < this->m_pimpl->m_objects_wait_for_delete.size(); i++)
	{
		if (m_pimpl->m_objects_wait_for_delete[i]->get_ref() == 1)
		{
			m_pimpl->m_objects_wait_for_delete.erase_fast(i);
			i--;
			continue;
		}
	}
	return true;
};
gv_object_name gv_sandbox::get_unique_object_name(const gv_id& s)
{
	return gv_object_name(s, ++this->m_max_name_id);
};

gv_file_manager* gv_sandbox::get_file_manager()
{
	return gv_global::fm.get();
};

gv_input_manager* gv_sandbox::get_input_manager()
{
	return gv_global::input.get();
};

gvi_debug_renderer* gv_sandbox::get_debug_renderer()
{
	return gv_global::debug_draw.get();
};

gv_game_engine* gv_sandbox::get_game_engine()
{
	gv_game_engine* p = this->m_engine;
	if (!p && this->get_outer())
	{
		return this->get_outer()->get_game_engine();
	}
	return p;
};

gv_enum_info* gv_sandbox::register_enum(const gv_id& name)
{
	gv_enum_info* info = new gv_enum_info;
	this->m_pimpl->m_enum_map.add(name, info);
	info->set_name(name);
	return info;
}

gv_enum_info* gv_sandbox::find_enum(const gv_id& name)
{
	gvt_ref_ptr< gv_enum_info >* ret = this->m_pimpl->m_enum_map.find(name);
	if (!ret && this->get_outer())
		return get_outer()->find_enum(name);
	return ret->ptr();
}
//---------------------------------------------------------------------------------
/*
gv_object *gv_sandbox::find			(const gv_string& loc){
        if (!loc.strlen()) return NULL;
        gvt_array<gv_string> result;
        loc.split(result,".");
        int h =gv_sandbox::hash(**result.last());
        gv_object * pobj=gv_object::s_hash_table[h] ;
        gv_object *  found=NULL;
        while (pobj){
                if (pobj->m_name==*result.last()){
                        gv_object * po=pobj->m_owner;
                        int i;
                        for (i=result.size()-2; i>=0; i--){
                                if (po->m_name!=result[i]) break;
                                po=po->m_owner;
                                if (!po) break;
                        }
                        if (i<=0 &&!po) {found=pobj;break;}
                }
                pobj=pobj->m_next;
        }

        return found;
};




gv_object *gv_sandbox::create(const gv_id & cls_name,const gv_id & name){
        gv_class_info * pcls=gv_class_info::static_get(*cls_name.string());
        if (!pcls) return	NULL;
        gv_object  * pobj= pcls->create_instance();
        if (name==gid_null){
                pobj->set_unique_name();
        }
        else{
                pobj->rename(name);
        }
        return pobj;
}

gv_string	gv_sandbox::get_owner_loc	(gv_string s)
{
        gvt_array<gv_string > r;
        s.split(r, ".");
        if (r.size()<=1) return gv_string("NULL");
        gv_string n;
        for ( int i=0; i< r.size()-1; i++){
                n+=r[i];
                if(i!=r.size()-2) n+=".";
        };
        return n;
};






*/
}