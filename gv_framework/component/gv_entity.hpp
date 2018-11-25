namespace gv
{
gv_entity::gv_entity()
{
	link_class(gv_entity::static_class());
	m_rotation.set(0, 0, 0);
	m_pos.set(0, 0, 0);
	m_scale.set(1, 1, 1);
	m_local_aabb.set(gv_vector3(-0.5f, -0.5f, -0.5f),
					 gv_vector3(0.5f, 0.5f, 0.5f));
	m_local_bounding_sphere.set(gv_vector3(0, 0, 0), 0.5);
	m_flag_tm_is_dirty = 1;
	m_flag_is_selected = 0;
	m_flag_is_pending_for_kill = 0;
	m_renderable_count = 0;
	m_tickable_count = 0;
	m_is_static = true;
	m_is_hided = false;
	m_ignore_pause = false;
	m_ignore_line_check = false;
	m_synced_frame = 0;
	this->update_matrix();
	m_total_life_time = 0;
	m_current_life_time = 0;
	m_ready_to_delete = false;
}
gv_entity::~gv_entity()
{
	this->detach_all_component();
}
//---------------------------transformation------------------------

const gv_vector3& gv_entity::get_position() const
{
	return m_pos;
};
const gv_euler& gv_entity::get_rotation() const
{
	return m_rotation;
}
const gv_vector3& gv_entity::get_scale() const
{
	return m_scale;
};
const gv_box& gv_entity::get_world_aabb() const
{
	return this->m_world_aabb;
};
const gv_box& gv_entity::get_local_aabb() const
{
	return this->m_local_aabb;
};
const gv_sphere& gv_entity::get_world_bounding_sphere() const
{
	return this->m_world_bounding_sphere;
}
const gv_sphere& gv_entity::get_local_bounding_sphere() const
{
	return this->m_local_bounding_sphere;
};
const gv_matrix43& gv_entity::get_tm() const
{
	return this->m_tm;
};
const gv_matrix43& gv_entity::get_inv_tm() const
{
	return this->m_inv_tm;
};

void gv_entity::update_matrix()
{
	if (!this->m_flag_tm_is_dirty)
		return;

	m_tm.set_identity();
	m_tm.set_scale(m_scale);

	gv_matrix43 mat_rot;
	gv_math::convert(mat_rot, this->m_rotation);
	m_tm *= mat_rot;
	m_tm.set_trans(this->m_pos);

	m_world_bounding_sphere = m_local_bounding_sphere.transform(m_tm);
	m_world_aabb = m_local_aabb.transform(m_tm);
	m_inv_tm = this->m_tm.get_inverse_with_scale_3dtransform();

	m_flag_tm_is_dirty = 0;
}

void gv_entity::update_from_matrix()
{
	gv_math::convert(this->m_rotation, m_tm);
	m_tm.get_scale(m_scale);
	m_pos = m_tm.get_trans();
	m_world_bounding_sphere = m_local_bounding_sphere.transform(m_tm);
	m_world_aabb = m_local_aabb.transform(m_tm);
	m_inv_tm = this->m_tm.get_inverse_with_scale_3dtransform();
	m_flag_tm_is_dirty = 0;
}
//-----------components manager-------------------------------------
gv_component* gv_entity::get_component(gv_class_info* pcls)
{
	const gv_entity& e = (*this);
	const gv_component* pcom = e.get_component(pcls);
	return (gv_component*)pcom;
}

const gv_component* gv_entity::get_component(gv_class_info* pcls) const
{

	if (pcls == gv_com_skeletal_mesh::static_class())
	{
		return this->m_com_skeletal_mesh;
	}
	if (pcls == gv_com_static_mesh::static_class())
	{
		return this->m_com_static_mesh;
	}
	component_list::const_iterator it = this->m_components.begin();
	component_list::const_iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		if ((*it)->is_a(pcls))
		{
			return (*it);
		}
		++it;
	}
	return NULL;
}

bool gv_entity::detach_component_from_list(gvt_ref_ptr< gv_component >& list,
										   gv_component* com)
{
	gv_component* ptr = list;
	gv_component* ptr_pre = NULL;
	while (ptr && ptr != com)
	{
		ptr_pre = ptr;
		ptr = ptr->get_next_component();
	}
	if (ptr == com)
	{
		if (ptr_pre)
		{
			ptr_pre->set_next_component(ptr->get_next_component());
		}
		else
		{
			list = ptr->get_next_component();
		}
		delete_component(com);
		return true;
	}
	return false;
};

bool gv_entity::detach_whole_component_list(gvt_ref_ptr< gv_component >& list)
{
	gvt_ref_ptr< gv_component > ptr = list;
	gvt_ref_ptr< gv_component > ptr_next;
	while (ptr)
	{
		ptr_next = ptr->get_next_component();
		ptr->set_next_component(NULL);
		this->delete_component(ptr);
		ptr = ptr_next;
	}
	list = NULL;
	return true;
}

bool gv_entity::detach_component(gv_component* com)
{
	GV_ASSERT(com);
	if (com->get_class() == gv_com_skeletal_mesh::static_class())
	{
		return this->detach_component_from_list(this->m_com_skeletal_mesh, com);
	}
	if (com->get_class() == gv_com_static_mesh::static_class())
	{
		return this->detach_component_from_list(this->m_com_static_mesh, com);
	}
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		if ((*it)->get_class() == com->get_class())
		{
			if (!detach_component_from_list(*it, com))
				return false;
			if (!(*it))
				m_components.erase_fast(it);
			return true;
		}
		++it;
	}
	return false;
};

bool gv_entity::detach_component(gv_class_info* pcls)
{
	if (pcls == gv_com_skeletal_mesh::static_class())
	{
		this->detach_whole_component_list(this->m_com_skeletal_mesh);
		return true;
	}
	if (pcls == gv_com_static_mesh::static_class())
	{
		this->detach_whole_component_list(this->m_com_static_mesh);
		return true;
	}
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		if ((*it)->get_class() == pcls)
		{
			this->detach_whole_component_list(*it);
			m_components.erase_fast(it);
			return true;
		}
		++it;
	}
	return false;
}

void gv_entity::add_component(gv_component* com)
{
	com->set_owner(this);
	com->set_entity(this);
	if (com->is_renderable())
	{
		GVM_POST_EVENT(add_component, render, (pe->component = com));
		this->m_renderable_count++;
		// gv_event_render_add_component * pe=new gv_event_render_add_component;
		// pe->component=com;
		// get_sandbox()->post_event(pe,gve_event_channel_render);
	}
	if (com->is_a(gv_com_skeletal_mesh::static_class()))
	{
		if (this->m_com_skeletal_mesh)
		{
			com->m_next = this->m_com_skeletal_mesh->m_next;
			m_com_skeletal_mesh->m_next = com;
		}
		else
		{
			this->m_flag_tm_is_dirty = true;
			this->m_com_skeletal_mesh = gvt_cast< gv_com_skeletal_mesh >(com);
			this->m_local_aabb = com->get_local_bbox();
			this->m_local_bounding_sphere = com->get_local_bsphere();
			this->update_matrix();
		}
		return;
	}
	if (com->is_a(gv_com_static_mesh::static_class()))
	{
		if (this->m_com_static_mesh)
		{
			com->m_next = this->m_com_static_mesh->m_next;
			m_com_static_mesh->m_next = com;
		}
		else
		{
			this->m_flag_tm_is_dirty = true;
			this->m_com_static_mesh = gvt_cast< gv_com_static_mesh >(com);
			this->m_local_aabb.add(com->get_local_bbox());
			this->m_local_bounding_sphere = com->get_local_bsphere();
			this->update_matrix();
		}
		return;
	}
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		if ((*it)->get_class() == com->get_class())
		{
			com->set_next_component((*it));
			;
			*it = com;
			break;
		}
		++it;
	}
	if (it == it_end)
		m_components.push_back(com);
	if (com->get_sandbox() == get_sandbox())
	{
		com->on_attach();
	}
	return;
};

gv_component* gv_entity::add_component(const gv_id& com_class_id)
{
	return this->add_component(get_sandbox()->find_class(com_class_id));
};

bool gv_entity::detach_component(const gv_id& com_class_id)
{
	return this->detach_component(get_sandbox()->find_class(com_class_id));
};
gv_component* gv_entity::add_component(gv_class_info* pcls)
{
	if (!pcls)
		return NULL;
	gv_object* p = this->get_sandbox()->create_object(
		pcls, this->get_sandbox()->get_unique_object_name(pcls->get_name_id()),
		this);
	GV_ASSERT(p);
	GV_ASSERT(p->is_a(gv_component::static_class()));
	this->add_component(gvt_cast< gv_component >(p));
	return (gv_component*)p;
};

gv_component* gv_entity::add_component(const gv_string_tmp& description)
{
	gv_xml_parser parser;
	parser.load_string(*description);
	gv_byte* pdata;
	gv_bool closed;
	gv_class_info* pcls =
		get_sandbox()->import_xml_element_simple(&parser, pdata, closed);
	if (!pcls)
		return NULL;
	if (!pdata || !pcls->is_derive_from(gv_component::static_class()))
	{
		GVM_ERROR(" add_component " << description << "failed !!" << gv_endl);
		return NULL;
	}
	gv_component* pcom = (gv_component*)pdata;
	this->add_component(pcom);
	return pcom;
};

void gv_entity::delete_component(gv_component* com)
{
	if (com->is_renderable())
	{
		GVM_POST_EVENT(remove_component, render, (pe->component = com));
		this->m_renderable_count--;
	}
	if (com->is_tickable())
	{
		this->m_tickable_count--;
	}
	if (com->get_sandbox() == get_sandbox())
	{
		com->on_detach();
		get_sandbox()->delete_object(com, 1);
	}
	else
	{
		com->set_owner(NULL);
		com->set_entity(NULL);
	}
};

void gv_entity::query_components_list(gvt_ref_ptr< gv_component >& list,
									  com_query_result& result)
{
	gv_component* ptr;
	ptr = list;
	while (ptr)
	{
		result.push_back(ptr);
		ptr = ptr->get_next_component();
	}
}

void gv_entity::query_components(com_query_result& result)
{
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		query_components_list(*it, result);
		++it;
	}
	if (this->m_com_static_mesh)
		query_components_list(this->m_com_static_mesh, result);
	if (this->m_com_skeletal_mesh)
		query_components_list(this->m_com_skeletal_mesh, result);
};

void gv_entity::detach_all_component()
{
	com_query_result result;
	this->query_components(result);
	for (int i = 0; i < result.size(); i++)
	{
		delete_component(result[i]);
	}
	this->m_components.clear();
	// if (this->m_com_static_mesh)
	// detach_whole_component_list(this->m_com_static_mesh);
	// if (this->m_com_skeletal_mesh)
	// detach_whole_component_list(this->m_com_skeletal_mesh);
	this->m_com_skeletal_mesh = NULL;
	this->m_com_static_mesh = NULL;
};

void gv_entity::notify_change_to_components()
{
	com_query_result result;
	this->query_components(result);
	for (int i = 0; i < result.size(); i++)
	{
		result[i]->sync_from_entity(this);
	}
}

void gv_entity::reset_bounding_from_components()
{
	m_flag_tm_is_dirty = true;
	m_local_aabb.empty();

	if (this->get_component< gv_com_skeletal_mesh >())
	{
		m_local_aabb =
			this->get_component< gv_com_skeletal_mesh >()->get_local_bbox();
	}
	else if (this->get_component< gv_com_static_mesh >())
	{
		m_local_aabb = this->get_component< gv_com_static_mesh >()->get_local_bbox();
	}
	else
	{
		com_query_result result;
		this->query_components(result);
		for (int i = 0; i < result.size(); i++)
		{
			m_local_aabb.add(result[i]->get_local_bbox());
		}
	}
	m_local_bounding_sphere.pos = m_local_aabb.get_center();
	m_local_bounding_sphere.r = m_local_aabb.get_diagonal() * 0.5f;
	update_matrix();
}

bool gv_entity::is_good_to_delete()
{
	if (!is_ready_to_delete())
		return false;

	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		while (ptr)
		{
			if (!ptr->is_safe_to_delete())
				return false;
			ptr = ptr->get_next_component();
		}
		++it;
	}
	return true;
}

bool gv_entity::tick(gv_float dt)
{
	GV_PROFILE_EVENT(gv_entity__tick, 0);
	m_current_life_time += dt;
	return this->tick_components(dt);
};

gv_entity* gv_entity::clone(gv_sandbox* sandbox)
{
	// gv_object_name
	// name=this->get_sandbox()->get_unique_object_name(this->get_name_id() );
	// gv_entity * pentity =this->get_sandbox()->create_object<gv_entity>(name);
	// this->get_class()->copy_to(this,pentity);
	if (!sandbox)
		sandbox = get_sandbox();
	GV_ASSERT(sandbox);
	gv_entity* pentity = gvt_cast< gv_entity >(sandbox->clone(this));
	com_query_result result;
	this->query_components(result);
	for (int i = 0; i < result.size(); i++)
	{
		gv_component* pcom = result[i]->clone(sandbox);
		pentity->add_component(pcom);
	}
	if (!this->get_world())
		pentity->m_template = this;
	return pentity;
};

bool gv_entity::is_template() const
{
	return this->m_template == NULL;
	;
};

bool gv_entity::is_static() const
{
	return this->m_is_static;
}; // can' t move
bool gv_entity::is_selected() const
{
	return this->m_flag_is_selected;
};
bool gv_entity::is_hided() const
{
	return this->m_is_hided;
};
void gv_entity::set_hided(gv_bool enable)
{
	m_is_hided = enable;
};
void gv_entity::set_selected(gv_bool enable)
{
	m_flag_is_selected = enable;
};
void gv_entity::set_static(gv_bool enable)
{
	m_is_static = enable;
};

void gv_entity::set_position(const gv_vector3& pos)
{
	this->m_pos = pos;
	this->m_flag_tm_is_dirty = 1;
};

void gv_entity::set_local_aabb(const gv_box& box)
{
	m_local_aabb = box;
	this->m_flag_tm_is_dirty = 1;
};
void gv_entity::set_rotation(const gv_euler& euler)
{
	this->m_rotation = euler;
	this->m_flag_tm_is_dirty = 1;
};

void gv_entity::set_scale(const gv_vector3& pos)
{
	this->m_scale = pos;
	this->m_flag_tm_is_dirty = 1;
};
void gv_entity::set_tm(const gv_matrix44& m)
{
	m.copy_to(m_tm);
	this->update_from_matrix();
};

void gv_entity::set_tm(const gv_matrix43& mat)
{
	m_tm = mat;
	this->update_from_matrix();
};

bool gv_entity::tick_components(gv_float dt)
{
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	bool ret;
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		if (ptr->is_tickable())
		{
			while (ptr)
			{
				if (ptr->is_enabled())
				{
					ret = ptr->tick(dt);
					if (!ret)
						return false;
				}
				ptr = ptr->get_next_component();
			}
		}
		++it;
	}
	return true;
};

bool gv_entity::set_enabled(gv_bool e)
{
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		while (ptr)
		{
			ptr->set_enabled(e);
			ptr = ptr->get_next_component();
		}
		++it;
	}
	return true;
}

void gv_entity::get_components_info(gv_stream_cached& sc)
{
	gvi_stream& ret = sc;
	gv_int count = 0;
	gv_stream_cached str;

	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		while (ptr)
		{
			ptr->get_info(count, str);
			ptr = ptr->get_next_component();
		}
		++it;
	}

	ret << count;
	ret.write(str.get_data(), str.size());
}

void gv_entity::synchronize_from_components()
{
	if (m_flag_is_pending_for_kill ||
		(m_current_life_time > m_total_life_time && m_total_life_time > 0))
	{
		get_world()->delete_entity(this);
		return;
	}
	if (get_world())
		m_synced_frame = get_world()->get_tick_count();
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		while (ptr)
		{
			// GV_ASSERT(ptr->get_entity() ==this);
			if (ptr->is_enabled())
				ptr->sync_to_entity(this);
			ptr = ptr->get_next_component();
		}
		++it;
	}
	if (this->m_com_skeletal_mesh)
	{
		gv_component* ptr = this->m_com_skeletal_mesh;
		while (ptr)
		{
			ptr->sync_to_entity(this);
			ptr = ptr->get_next_component();
		};
	}
}; // this is the only

bool gv_entity::post_load()
{
	this->m_tickable_count = 0;
	this->m_renderable_count = 0;
	component_list::iterator it = this->m_components.begin();
	component_list::iterator it_end = this->m_components.end();
	while (it != it_end)
	{
		gv_component* ptr;
		ptr = (*it);
		while (ptr)
		{
			if (ptr->is_tickable())
			{
				this->m_tickable_count++;
			}
			if (ptr->is_renderable())
			{
				this->m_renderable_count++;
				GVM_POST_EVENT(add_component, render, (pe->component = ptr));
			}
			ptr->on_attach();
			ptr = ptr->get_next_component();
		}
		++it;
	}
	if (this->m_com_skeletal_mesh)
	{
		gv_component* ptr = this->m_com_skeletal_mesh;
		while (ptr)
		{
			GVM_POST_EVENT(add_component, render, (pe->component = ptr));
			this->m_renderable_count++;
			ptr = ptr->get_next_component();
		};
	}
	if (this->m_com_static_mesh)
	{
		gv_component* ptr = this->m_com_static_mesh;
		while (ptr)
		{
			GVM_POST_EVENT(add_component, render, (pe->component = ptr));
			this->m_renderable_count++;
			ptr = ptr->get_next_component();
		};
	}
	return true;
}

void gv_entity::debug_draw_axis()
{
	gvi_debug_renderer* pdebug = get_sandbox()->get_debug_renderer();
	gv_vector3 pos = this->get_position();

	pdebug->draw_line_3d(pos, pos + m_tm.axis_x * 10, gv_color::RED(),
						 gv_color::RED_B());
	pdebug->draw_line_3d(pos, pos + m_tm.axis_y * 10, gv_color::GREEN(),
						 gv_color::GREEN());
	pdebug->draw_line_3d(pos, pos + m_tm.axis_z * 10, gv_color::BLUE(),
						 gv_color::BLUE());
};

void gv_entity::debug_draw_aabb(gv_color c)
{
	gvi_debug_renderer* pdebug = get_sandbox()->get_debug_renderer();
	pdebug->draw_box_3d(this->get_world_aabb(), c);
};

gv_float gv_entity::get_unified_life_time() const
{
	if (get_total_life_time() > 0)
	{
		return get_current_life_time() / get_total_life_time();
	}
	return gvt_frac(get_current_life_time() / 120.f);
};
}

namespace gv
{
GVM_IMP_CLASS(gv_entity, gv_object)
GVM_VAR_ATTRIB_SET(no_clone)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_component > >, m_components)
GVM_VAR(gvt_ref_ptr< gv_component >, m_com_static_mesh)
GVM_VAR(gvt_ref_ptr< gv_component >, m_com_skeletal_mesh)
GVM_VAR_ATTRIB_UNSET(no_clone)
GVM_VAR(gvt_ptr< gv_entity >, m_template)
GVM_VAR(gvt_ptr< gv_world >, m_world)
GVM_VAR(gv_bool, m_is_static)
GVM_VAR(gv_bool, m_is_hided)
GVM_VAR(gv_bool, m_ignore_pause)
GVM_VAR(gv_bool, m_ignore_line_check)

GVM_VAR(gv_matrix43, m_tm)
GVM_VAR(gv_matrix43, m_inv_tm)
GVM_VAR(gv_vector3, m_pos)
GVM_VAR(gv_euler, m_rotation)
GVM_VAR(gv_vector3, m_scale)

GVM_VAR(gv_box, m_local_aabb)
GVM_VAR(gv_box, m_world_aabb)
GVM_VAR(gv_sphere, m_local_bounding_sphere)
GVM_VAR(gv_sphere, m_world_bounding_sphere)

GVM_VAR(gv_short, m_renderable_count)
GVM_VAR(gv_short, m_tickable_count)
GVM_VAR(gv_float, m_total_life_time)
GVM_END_CLASS

GVM_BGN_FUNC(gv_entity, scp_rebuild_world_matrix)
m_flag_tm_is_dirty = 1;
update_matrix();
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_entity, scp_rebuild_bound)
reset_bounding_from_components();
return 1;
GVM_END_FUNC
}
