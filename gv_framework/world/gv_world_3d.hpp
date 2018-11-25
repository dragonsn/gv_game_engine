#define CHECK_OVERLAP 0
//#pragma   GV_DISABLE_OPTIMIZATION
namespace gv
{

class gv_c_result_comparator
{
public:
	gv_c_result_comparator(){};
	bool operator()(const gv_collide_result* c1, const gv_collide_result* c2)
	{
		gv_int k1 = c1->time;
		gv_int k2 = c2->time;
		gv_int t1 = (gv_int)c1->target->get_name().get_postfix();
		gv_int t2 = (gv_int)c2->target->get_name().get_postfix();
		return gv_less_than_i2(k1, k2, t1, t2);
	}
};

class gv_c_result_comparator_distance
{
public:
	gv_c_result_comparator_distance(){

	};

	bool operator()(const gv_collide_result* c1, const gv_collide_result* c2)
	{
		gv_int k1 = c1->moved.abs_sum();
		gv_int k2 = c2->moved.abs_sum();
		gv_int t1 = (gv_int)c1->target->get_name().get_postfix();
		gv_int t2 = (gv_int)c2->target->get_name().get_postfix();
		return gv_less_than_i2(k1, k2, t1, t2);
	}
};
//============================================================================================
gv_world_3d::gv_world_3d()
{
	m_logical_frame_time = 33;
	m_current_frame_count = 0;
	m_total_frame_time = 0;
	m_axis_freedom = 1;
	GVM_SET_CLASS(gv_world_3d);
};
gv_world_3d::~gv_world_3d(){};

gv_int gv_world_3d::get_unique_runtime_tag()
{
	return ++m_runtime_tag;
};

//-------------------------overridable ---------------------
void gv_world_3d::init_map_size(const gv_boxi& world_box,
								const gv_vector2i& cell_size)
{
	gv_world_grid* pgrid = get_grid(0);
	pgrid->setup(cell_size, pgrid->project(world_box));
	m_physics = get_physics();
	m_physics->set_world_box(world_box);
	// recreate bounding
	gv_entity* e = get_root_entity();
	e->set_static(true);
	e->detach_all_component();
	gv_actor_3d* a = e->add_component< gv_actor_3d >();
	a->set_local_aabb(world_box);
	// build the floor;
	// a->m_local_aabb.move(gv_vector3i(0, -world_box.get_ydelta(), 0)); //move
	// the box to floor;
	a->create_colliders_for_empty_box(30, true, true, true);
};

gv_entity* gv_world_3d::get_root_entity()
{
	if (m_entities.size() == 0)
	{
		gv_entity* e = this->get_sandbox()->create_object< gv_entity >(gv_id_root);
		this->add_entity(e);
		return e;
	}
	return m_entities[0];
};

gv_physics_fixed_setting* gv_world_3d::get_physics()
{
	if (!m_physics)
	{
		m_physics = get_sandbox()->create_object< gv_physics_fixed_setting >();
	}
	return m_physics;
}

gv_float gv_world_3d::get_height(gv_float x, gv_float z)
{
	return 0;
};

void gv_world_3d::add_collider(gv_collider* c)
{
	c->attach_grid(get_grid(0));
};

gv_int gv_world_3d::move_actor(gv_actor_3d* pactor,
							   const gv_vector3i& new_pos)
{
	//
	static bool use_3_dim_move = false;
	if (use_3_dim_move)
	{
		return move_actor_in_three_dimension(pactor, new_pos);
	}
	else
	{
		gv_int time = 0;
		time += move_actor_in_one_dimension(pactor, new_pos, 0);
		time += move_actor_in_one_dimension(pactor, new_pos, 1);
		time += move_actor_in_one_dimension(pactor, new_pos, 2);
		return time / 3;
	}
}

//------------------------collision detection & physice functions ------
gv_int gv_world_3d::move_actor_in_three_dimension(gv_actor_3d* pactor,
												  const gv_vector3i& new_pos)
{
	gvt_array_cached< gv_collide_result*, 256 > hits;
	gv_collider::cache parts;
	gv_collider::cache targets;
	gv_vector3i delta = new_pos - pactor->m_position;
	delta *= m_axis_freedom;
	pactor->find_colliders(parts);
	gv_boxi b = pactor->get_world_aabb();
	gv_boxi b_moving = b;
	b_moving.move(delta);
	b_moving.add(b);
	this->collect_colliders(b_moving, targets);
	gv_int hit_time = gv_math_fixed::one();

	// iterate all the pairs----------------------------
	for (int i = 0; i < parts.size(); i++)
	{
		gv_collider* src = parts[i];
		src->m_runtime_test_tag = get_unique_runtime_tag();
		for (int j = 0; j < targets.size(); j++)
		{
			gv_collider* tgt = targets[j];
			if (tgt == src ||
				tgt->get_actor() == src->get_actor())
			{ // no self collision
				continue;
			}
			gv_collide_result r;
			r.time = hit_time;
			if (src->sweep(tgt, delta, r, 10))
			{
				if (r.blocked)
				{
					hit_time = r.time;
				}
				gv_collide_result* nr = new gv_collide_result;
				*nr = r;
				hits.push_back(nr);
			}
		}
	}
	//-------sort hits , and find the first hit ----------------
	if (hits.size() && hit_time >= 0)
	{
		gv_c_result_comparator cmp;
		hits.sort(cmp);
		for (int i = 0; i < hits.size(); i++)
		{
			gv_collide_result* r = hits[i];
			if (r->time > hit_time)
			{ // first
				break;
			}
			pactor->on_hit(*r);
			r->target->m_owner_actor->on_be_hit(*r);
		}
	}

	//------update delta , if moved,move all the parts with delta
	delta = delta * hit_time / gv_math_fixed::one();
	if (delta.abs_sum() > 0)
	{
#if CHECK_OVERLAP
		if (1)
		{
			gv_boxi b = pactor->get_world_aabb();
			b.move(delta);
			if (m_physics->get_world_box().clip(b) != b)
			{
				GV_DEBUG_BREAK;
			}
			else
			{
				if (!teleport_actor(pactor, pactor->m_position + delta, true))
				{
					GV_DEBUG_BREAK;
				}
			}
		}
		else
#endif
		{
			for (int i = 0; i < parts.size(); i++)
			{
				gv_collider* src = parts[i];
				src->m_world_position += delta;
			};
			pactor->m_position += delta;
			pactor->update_grids();
		}
	}
	//-----------------------------------------
	gvt_for_each_delete(hits);
	return hit_time;
};

//------------------------collision detection & physics functions ------
gv_int gv_world_3d::move_actor_in_one_dimension(gv_actor_3d* pactor,
												const gv_vector3i& new_pos,
												gv_int dim)
{
	gvt_array_cached< gv_collide_result*, 256 > hits;
	gv_int hit_time = gv_math_fixed::one();
	gv_collider::cache parts;
	gv_collider::cache targets;
	if (!m_axis_freedom[dim])
	{
		return hit_time;
	}
	gv_vector3i delta = gv_vector3i::get_zero_vector();
	delta[dim] = (new_pos - pactor->m_position)[dim];
	if (!delta[dim]) return hit_time;
	pactor->find_colliders(parts);
	gv_boxi b = pactor->get_world_aabb();
	gv_boxi b_moving = b;
	b_moving.move(delta);
	b_moving.add(b);
	b_moving.extend(gv_vector3i(2,2,2));
	this->collect_colliders(b_moving, targets);
	gv_int distance = delta[dim];

	// iterate all the pairs----------------------------
	for (int i = 0; i < parts.size(); i++)
	{
		gv_collider* src = parts[i];
		src->m_runtime_test_tag = get_unique_runtime_tag();
		for (int j = 0; j < targets.size(); j++)
		{
			gv_collider* tgt = targets[j];
			if (tgt == src ||
				tgt->get_actor() == src->get_actor())
			{ // no self collision
				continue;
			}
			gv_collide_result r;
			if (src->sweep_one_dim(tgt, delta, r, dim, 10))
			{
				if (r.blocked )
				{
					delta[dim]=distance = r.moved[dim];
					hit_time = r.time;
				}
				gv_collide_result* nr = new gv_collide_result;
				*nr = r;
				hits.push_back(nr);
			}
		}
	}
	//-------sort hits , and find the first hit ----------------
	if (hits.size())
	{
		gv_c_result_comparator_distance cmp;
		hits.sort(cmp);
		for (int i = 0; i < hits.size(); i++)
		{
			gv_collide_result* r = hits[i];
			if (gvt_abs(r->moved[dim]) > gvt_abs(distance))
			{ // first
				break;
			}
			pactor->on_hit(*r);
			r->target->m_owner_actor->on_be_hit(*r);
		}
	}

	//------update delta , if moved,move all the parts with delta
	delta[dim] = distance;
	if (delta.abs_sum() > 0)
	{
#if CHECK_OVERLAP
		if (1)
		{
			gv_boxi b = pactor->get_world_aabb();
			b.move(delta);
			if (m_physics->get_world_box().clip(b) != b)
			{
				GV_DEBUG_BREAK;
			}
			else
			{
				if (!teleport_actor(pactor, pactor->m_position + delta, true))
				{
					GV_DEBUG_BREAK;
				}
			}
		}
		else
#endif
		{
			for (int i = 0; i < parts.size(); i++)
			{
				gv_collider* src = parts[i];
				src->m_world_position += delta;
			};
			pactor->m_position += delta;
			pactor->update_grids();
		}
	}
	//-----------------------------------------
	gvt_for_each_delete(hits);
	return hit_time;
};

gv_int gv_world_3d::collect_colliders(const gv_boxi& b, gv_collider::cache& r)
{
	gv_cell_3d::cache cells;
	get_grid()->collect_cells(b, cells);
	gv_int tag = get_unique_runtime_tag();
	// cells
	for (int i = 0; i < cells.size(); i++)
	{
		gv_obj_list::iterator it = cells[i]->m_actor_list.begin();
		while (it.get_node())
		{
			gv_collider* pc = gvt_cast< gv_collider >(it.ptr());
			if (pc && pc->m_runtime_test_tag != tag)
			{
				r.push_back(pc);
				pc->m_runtime_test_tag = tag;
			}
			it++;
		} // next node
	}	 // next cell
	return r.size();
};

bool gv_world_3d::teleport_actor(gv_actor_3d* pactor, const gv_vector3i& pos,
								 bool check_overlap)
{
	gvt_array_cached< gv_collide_result*, 256 > hits;
	gv_collider::cache parts;
	gv_collider::cache targets;
	gv_vector3i delta = pos - pactor->m_position;
	pactor->find_colliders(parts);
	gv_boxi b = pactor->get_world_aabb();
	b.move(delta);
	if (check_overlap)
	{
		this->collect_colliders(b, targets);
	}
	// iterate all the pairs----------------------------
	for (int i = 0; i < parts.size(); i++)
	{
		gv_collider* src = parts[i];
		src->m_runtime_test_tag = get_unique_runtime_tag();
		for (int j = 0; j < targets.size(); j++)
		{
			gv_collider* tgt = targets[j];
			if (tgt == src ||
				tgt->get_actor() == src->get_actor())
			{ // no self collision
				continue;
			}
			gv_vector3i new_loc = src->m_world_position + delta;
			if (src->overlap(tgt, new_loc))
			{
				return false;
			}
		}
	}
	// iterate all the pairs & set position--------------
	for (int i = 0; i < parts.size(); i++)
	{
		gv_collider* src = parts[i];
		src->m_world_position += delta;
		src->update_grids();
	};
	pactor->m_position += delta;
	return true;
};

gv_entity*
gv_world_3d::create_actor(gv_class_info* actor_class, const gv_string_tmp& name,
						  const gv_string_tmp& mesh, const gv_string_tmp& ani,
						  const gv_int random_range, const gv_boxi& aabb,
						  const gv_vector3i& pos, const gv_euleri& new_rot,
						  gv_bool test_overlap, gv_int retry)
{

	gv_entity* my_entity =
		get_sandbox()->create_object< gv_entity >(gv_id(*name), this);
	add_entity(my_entity);
	my_entity->set_static(false);
#if GV_WITH_OS_API
	if (mesh != "NULL")
	{
		gv_com_skeletal_mesh* com_sk_mesh = NULL;
		gv_material* my_material = NULL;
		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gv_ani_set* my_animation = NULL;
		int current_idx = 0;
		com_sk_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >(my_entity);
		my_skeletal_mesh =
			get_sandbox()->find_object< gv_skeletal_mesh >(mesh, false);
		if (!my_skeletal_mesh)
		{
			gv_module* module = get_sandbox()->try_load_module(gv_id(*mesh));
			if (module)
			{
				gvt_object_iterator_with_owner< gv_skeletal_mesh > it_sk(get_sandbox(),
																		 module);
				if (!it_sk.is_empty())
				{
					my_skeletal_mesh = it_sk;
				}
			}
		}

		my_animation = get_sandbox()->find_object< gv_ani_set >(ani, false);
		if (!my_animation)
		{
			gv_module* module = get_sandbox()->try_load_module(gv_id(*ani));
			if (module)
			{
				gvt_object_iterator_with_owner< gv_ani_set > it_ani(get_sandbox(),
																	module);
				if (!it_ani.is_empty())
				{
					my_animation = it_ani;
				}
			}
		}
		if (my_skeletal_mesh)
		{
			GV_ASSERT(my_animation);
			com_sk_mesh->set_resource(my_skeletal_mesh);
			com_sk_mesh->create_sub_material_from_resource();
			my_entity->add_component(com_sk_mesh);
			// link animation & material
			gv_com_animation* com_ani =
				get_sandbox()->create_object< gv_com_animation >(my_entity);
			com_ani->set_resource(my_animation);
			my_entity->add_component(com_ani);
		}
	}
#endif
	gv_actor_3d* actor =
		gvt_cast< gv_actor_3d >(my_entity->add_component(actor_class));
	if (actor)
	{
		actor->set_local_aabb(aabb);
		actor->set_position(pos);
		actor->set_rotation(new_rot);
		actor->create_base_collider();
		place_actor(actor, random_range, pos, new_rot, test_overlap, retry);
		actor->sync_to_entity(my_entity);
	}
	return my_entity;
};

bool gv_world_3d::place_actor(gv_actor_3d* actor, gv_int random_range,
							  const gv_vector3i& pos, const gv_euleri& new_rot,
							  gv_bool test_overlap, gv_int max_retry)
{
	if (random_range != 0)
	{
		gv_int n = 0;
		while (1)
		{
			gv_vector2i dv(m_random_for_gameplay.get_ranged(0, random_range),
						   m_random_for_gameplay.get_ranged(0, random_range));
			dv -= gv_vector2i(random_range / 2, random_range / 2);
			if (teleport_actor(actor, pos + dv, 0))
			{
				break;
			}
			if (n++ > max_retry)
			{
				GVM_WARNING("failed to find good place to spawn !!!");
				return teleport_actor(actor, pos, false);
			};
		};
	}
	else
	{
		return teleport_actor(actor, pos, false);
	}
	return true;
};

gv_world_grid* gv_world_3d::get_grid(gv_int index)
{
	if (!m_base_grid)
		m_base_grid = new gv_world_grid;
	return m_base_grid;
};

gv_int gv_world_3d::tick_actor_physics(gv_actor_3d* pactor,
									   gv_int remain_time)
{
	int hit_time = gv_math_fixed::one();
	;
	if (pactor->get_entity()->is_selected())
	{
		pactor = pactor; // for debug
	}
	gv_vector3i delta_pos = pactor->get_delta_position(remain_time);
	gv_vector3i old_pos = pactor->get_position();
	if (delta_pos != gv_vector3i(0, 0, 0))
	{
		hit_time = this->move_actor(pactor, pactor->get_position() + delta_pos);
	}
	pactor->apply_force_and_impulse(hit_time, pactor->get_position() - old_pos);
	return hit_time;
};

bool gv_world_3d::tick(gv_float dt)
{
	super::tick(dt);

	for (int i = 0; i < this->m_entities.size(); i++)
	{
		if (m_entities[i]->is_static())
			continue;
		gv_actor_3d* pactor = m_entities[i]->get_component< gv_actor_3d >();
		if (!pactor )
		{
			continue;
		}
		if (!pactor->is_enabled())
		{
			pactor->remove_from_all_grids();
			continue;
		}
		pactor->pre_physics();
	}

	{
		GV_PROFILE_EVENT(gv_world_3d__tick_physics, 0);
		gv_actor_3d::cache actors;
		for (int i = 0; i < this->m_entities.size(); i++)
		{
			if (m_entities[i]->is_static())
				continue;
			gv_actor_3d* pactor = m_entities[i]->get_component< gv_actor_3d >();
			if (!pactor || !pactor->is_enabled())
				continue;
			pactor->m_remain_time = gv_math_fixed::one();
			actors.push_back(pactor);
		}
		gv_int iteration_count = 1;
		gv_int min_update_time = 30;
		for (int i = 0; i < iteration_count; i++)
		{
			for (int j = 0; j < actors.size(); j++)
			{
				gv_actor_3d* pactor = actors[j];
				gv_int normalized_time = tick_actor_physics(pactor, pactor->m_remain_time);
				pactor->m_remain_time =
					pactor->m_remain_time -
					gv_math_fixed::mul(normalized_time, pactor->m_remain_time);
				if (pactor->m_remain_time <= min_update_time)
				{
					actors.erase_fast(j);
					j--;
				}
			}
		}
	}

	for (int i = 0; i < this->m_entities.size(); i++)
	{
		// if (m_entities[i]->is_static()) continue;
		gv_actor_3d* pactor = m_entities[i]->get_component< gv_actor_3d >();
		if (!pactor || !pactor->is_enabled())
			continue;
		if (pactor)
			pactor->post_physics();
	}

	m_current_frame_count++;
	m_total_frame_time += m_logical_frame_time;
	return true;
};

gv_int gv_world_3d::rotate_actor(gv_actor_3d* pactor,
								 const gv_euleri& new_rot)
{
	pactor->set_rotation(new_rot);
	pactor->update_colliders_world_position();
	pactor->update_grids();
	return 1;
};

bool gv_world_3d::move_entity(gv_entity* entity, gv_vector3 new_pos,
							  gv_euler new_rot)
{
	gv_actor_3d* pactor = entity->get_component< gv_actor_3d >();
	if (!pactor || !pactor->is_enabled())
		return false;
	gv_vector3i v;
	gv_math_fixed::to_fixed(new_pos, v);
	this->move_actor(pactor, v);
	gv_euleri r;
	gv_math_fixed::to_fixed(new_rot, r);
	r.v3 -= pactor->m_display_adjust_angle.v3;
	rotate_actor(pactor, r);
	pactor->sync_to_entity(entity);
	return true;
};

bool gv_world_3d::teleport_entity(gv_entity* entity, gv_vector3 pos_3d,
								  gv_euler rot, bool check_overlap)
{
	gv_bool ret;
	gv_actor_3d* pactor = entity->get_component< gv_actor_3d >();
	if (!pactor || !pactor->is_enabled())
		return false;
	gv_vector3i v;
	gv_math_fixed::to_fixed(pos_3d, v);
	ret = this->teleport_actor(pactor, v, check_overlap);

	gv_euleri r;
	gv_math_fixed::to_fixed(rot, r);
	r.v3 -= pactor->m_display_adjust_angle.v3;
	rotate_actor(pactor, r);
	pactor->sync_to_entity(entity);
	return true;
};

GVM_IMP_CLASS(gv_world_3d, gv_world)
GVM_VAR(gv_int, m_current_frame_count)
GVM_VAR(gv_int, m_total_frame_time)
GVM_VAR(gv_int, m_logical_frame_time)
GVM_VAR(gv_vector3i, m_axis_freedom)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_map_3d_init_text)
GVM_VAR(gv_vector3i, m_size)
GVM_VAR(gvt_array< gv_int_string_pair >, m_type_pairs)
GVM_VAR(gv_text, m_map)
GVM_END_STRUCT
GVM_IMP_STATIC_CLASS(gv_map_3d_init_text)
}