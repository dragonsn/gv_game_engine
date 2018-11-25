namespace gv
{

gv_collider::gv_collider()
{
	m_collider_tags = m_target_test_tags = m_target_block_tags = 1;
	m_collision_shape = e_shape_aabb;
	link_class(gv_collider::static_class());
};

gv_collider::~gv_collider()
{
	remove_from_all_grids();
};

gv_grid_link_node* gv_collider::get_link_node(gv_world_grid* grid)
{
	gv_grid_link_node* ret;
	for (int i = 0; i < m_link_nodes.size(); i++)
	{
		ret = m_link_nodes[i];
		if (ret->m_owner == grid)
			return ret;
	}
	ret = new gv_grid_link_node;
	m_link_nodes.push_back(ret);
	ret->m_owner = grid;
	return ret;
};

void gv_collider::attach_grid(gv_world_grid* grid)
{
	get_link_node(grid);
};

void gv_collider::update_grids()
{
	remove_from_all_grids();
	for (int i = 0; i < m_link_nodes.size(); i++)
	{
		gv_grid_link_node* node = m_link_nodes[i];
		gv_cell_3d::cache cells;
		node->m_owner->collect_cells(get_world_aabb(), cells);
		add_to_cells(node, cells);
	}
};

void gv_collider::add_to_cells(gv_grid_link_node* node,
							   gv_cell_3d::cache& cells)
{
	node->m_cells.add(cells.begin(), cells.size());
	node->m_obj_nodes.resize(cells.size());
	for (int i = 0; i < cells.size(); i++)
	{
		node->m_obj_nodes[i].object = this;
		cells[i]->m_actor_list.link(&node->m_obj_nodes[i]);
	}
};

void gv_collider::add_to_grid(gv_world_grid* grid)
{
	gv_grid_link_node* node = get_link_node(grid);
	if (node->m_cells.size())
	{
		remove_from_grid(grid);
	}
	gv_cell_3d::cache cells;
	grid->collect_cells(get_world_aabb(), cells);
	add_to_cells(node, cells);
};

void gv_collider::remove_from_grid(gv_world_grid* grid)
{
	gv_grid_link_node* node = get_link_node(grid);
	if (node->m_cells.size())
	{
		for (int i = 0; i < node->m_cells.size(); i++)
		{
			gv_cell_3d* cell = node->m_cells[i];
			cell->m_actor_list.unlink(&node->m_obj_nodes[i]);
		}
	}
	node->m_cells.clear();
	node->m_obj_nodes.clear();
};

void gv_collider::remove_from_all_grids()
{
	for (int i = 0; i < m_link_nodes.size(); i++)
	{
		this->remove_from_grid(m_link_nodes[i]->m_owner);
	}
};

void gv_collider::on_detach()
{
	remove_from_grid(NULL);
};

void gv_collider::on_attach()
{
	gv_world_3d* w =
		(gv_world_3d*)(gvt_cast< const gv_world_3d >(get_entity()->get_world()));
	if (w)
		w->add_collider(this);
	const gv_actor_3d* a = this->get_entity()->get_component< gv_actor_3d >();
	GV_ASSERT(a);
	this->m_owner_actor = (gv_actor_3d*)a;
};

void gv_collider::debug_draw()
{
	gv_box b;
	gv_math_fixed::to_float(this->get_world_aabb(), b);
	gv_global::debug_draw->draw_box_3d(b, gv_color::GREEN());
	gv_global::debug_draw->draw_axis(this->get_entity()->get_tm());
};

gv_bool gv_collider::is_test_target(gv_collider* target)
{
	if (this->m_runtime_test_tag == target->m_runtime_test_tag)
	{
		return false; // already tested
	}
	target->m_runtime_test_tag = m_runtime_test_tag;

	return can_collide(target);
}

gv_bool gv_collider::can_penetrate(gv_collider* target)
{
	if ((this->m_collider_tags & target->m_target_block_tags) == 0)
	{
		return true; // can penetrate target
	}
	return false;
};

gv_bool gv_collider::can_collide(gv_collider* target)
{
	if ((this->m_collider_tags & target->m_target_test_tags) == 0)
	{
		return false; // not a test target
	}
	return true;
};

gv_bool gv_collider::sweep_one_dim(gv_collider* target, gv_vector3i speed,
								   gv_collide_result& result, gv_int dim,
								   gv_int skin)
{
	gv_bool ret = false;
	if (!is_test_target(target))
	{
		return ret;
	}
	if (this->m_collision_shape == e_shape_aabb &&
		target->m_collision_shape == e_shape_aabb)
	{
		gv_boxi sb = get_world_aabb();
		gv_boxi tb = target->get_world_aabb();
		gv_int distance;
		gv_int speed_1d = speed[dim];
		gv_vector3i contact, normal;
		if (gv_math_fixed::sweep(sb, tb, speed_1d, distance, contact, normal, dim,
								 skin))
		{
			gv_int this_time = distance * gv_math_fixed::one() / speed_1d;
			if ( gvt_abs(distance ) < gvt_abs(speed_1d) )
			{
				result.blocked = !can_penetrate(target);
				result.contact = contact;
				result.normal = normal;
				result.time = this_time;
				result.target = target;
				result.moved = gv_vector3i::get_zero_vector();
				result.moved[dim] = distance;
				ret = true;
			}
		}
	}
	else
	{
		GVM_UNDER_CONSTRUCT;
	}
	return ret;
};

gv_bool gv_collider::sweep(gv_collider* target, gv_vector3i speed,
						   gv_collide_result& result, gv_int skin)
{
	gv_bool ret = false;
	if (!is_test_target(target))
	{
		return ret;
	}
	if (this->m_collision_shape == e_shape_aabb &&
		target->m_collision_shape == e_shape_aabb)
	{
		gv_boxi sb = get_world_aabb();
		gv_boxi tb = target->get_world_aabb();

		// use separated dimension is more expensive ,but more stable.
		gv_vector3i v0 = sb.get_center();
		tb.move(-v0);
		tb.extend(sb.get_extend());
		gv_vector3i normal, contact;
		gv_int time =
			gv_math_fixed::ray_intersect_box(speed, tb, contact, normal, skin);
		contact += v0;
		if (time >= 0 && time < result.time)
		{
			result.blocked = !can_penetrate(target);
			result.contact = contact;
			result.normal = normal;
			result.time = time;
			result.target = target;
			result.moved = speed * time / gv_math_fixed::one();
			ret = true;
		}
	}
	else
	{
		GVM_UNDER_CONSTRUCT;
	}
	return ret;
};

gv_bool gv_collider::overlap(gv_collider* target, gv_vector3i new_pos,
							 gv_int skin)
{
	if (this->m_runtime_test_tag == target->m_runtime_test_tag)
	{
		return false; // already tested
	}
	target->m_runtime_test_tag = m_runtime_test_tag;
	if ((this->m_collider_tags & target->m_target_test_tags) == 0)
	{
		return false; // not a test target
	}
	if (this->m_collision_shape == e_shape_aabb &&
		target->m_collision_shape == e_shape_aabb)
	{
		gv_boxi sb = m_local_aabb;
		sb.move(new_pos);
		//sb.extend(gv_vector3i(skin));
		gv_bool ret= sb.is_overlap(target->get_world_aabb());
		if (ret) 
		{
			//something wrong
			return true;
		}

	}
	return false;
};

void gv_collider::update_world_position()
{
	gv_actor_3d* a = m_owner_actor;
	gv_vector3_fixed v;
	v = m_local_position;
	gv_matrix_fixed mat = a->get_world_matrix();
	m_world_position = v * mat;
};

gv_boxi gv_collider::get_world_aabb()
{
	gv_boxi b = m_local_aabb;
	b.move(m_world_position);
	return b;
};

GVM_IMP_CLASS(gv_collider, gv_component)
// to simplify our collision , our collider don't have rotation .
GVM_VAR(gv_vector3i, m_local_position)
GVM_VAR(gv_vector3i, m_world_position)
GVM_VAR(gv_boxi, m_local_aabb)
GVM_VAR(gv_vector3i, m_smoothed_history_speed)
GVM_VAR_ENUM(gve_collider_3d_shape, m_collision_shape)
GVM_END_CLASS
}