namespace gv
{
gv_math_fixed* gv_actor_2d::get_helper_2d()
{
	return gv_math_fixed::static_get();
}

gv_actor_2d::gv_actor_2d()
{
	GVM_SET_CLASS(gv_actor_2d)
	this->m_position_2d = gv_vector2i(0, 0);
	this->m_local_aabb = this->m_world_aabb = gv_recti(0, 0, 0, 0);
	m_facing = 0;
	m_speed = 0;
	m_turning_speed = 0;
	m_display_adjust_angle = 0;
	m_external_force = 0;
	// m_display_adjust_angle=-gv_math_fixed::half_pi();
	m_color = gv_color::RED();
	m_collidable = true;
	m_height = 0;
	m_ignore_me = false;
	m_size = get_helper_2d()->one();

	m_momentum = 0; // ----new physics related parameter,linear momentum
	// test m_momentum = 5000;
	m_mass = get_helper_2d()->one(); // mass.
	m_angular_momentum = 0;			 // angular
	m_aero_friction = 990;			 // aero_friction
	m_angular_friction = 900;		 // angular_friction
	m_penatrate_level = get_helper_2d()->one();
	m_backed_penatrate_level = m_penatrate_level;
	m_collision_type = gve_collision_type_aabb;
	m_speed_modifier = 1000;
}
gv_actor_2d::~gv_actor_2d()
{
}
bool gv_actor_2d::tick(gv_float dt)
{
	return true;
};
bool gv_actor_2d::sync_from_entity(const gv_entity* entity)
{
	if (!m_world)
	{
		m_world =
			(gv_world_2d*)gvt_cast< const gv_world_2d >(get_entity()->get_world());
	}
	to_2d(entity->get_local_aabb(), m_local_aabb);
	;
	to_2d(entity->get_position(), m_position_2d, m_height);
	m_height = 0;
	m_world_aabb = m_local_aabb;
	m_world_aabb.move_center_to(m_position_2d);
	m_facing = gv_math_fixed::rad_to_deg(entity->get_rotation().yaw) -
			   m_display_adjust_angle;
	if (m_world)
	{
		this->add_to_cell();
	}
	return true;
}
bool gv_actor_2d::sync_to_entity(gv_entity* entity)
{
	// if (entity->is_renderable() )
	{
		gv_vector3 pos;
		to_3d(this->m_position_2d, m_height, pos);
		pos.y += get_world()->get_height(pos.x, pos.z);
		gv_float s = get_helper_2d()->to_float(m_size);
		entity->set_scale(gv_vector3(s, s, s));
		entity->set_position(pos);
		entity->set_rotation(gv_euler(
			0, gv_math_fixed::deg_to_rad(-m_facing + m_display_adjust_angle), 0));
		entity->update_matrix();
	}
	return true;
};
bool gv_actor_2d::is_tickable()
{
	return true;
}

void gv_actor_2d::add_to_cells(gv_cell_2d::cache& cells)
{
	m_cells.init(cells.begin(), cells.size());
	this->m_obj_nodes.resize(cells.size());
	for (int i = 0; i < cells.size(); i++)
	{
		this->m_obj_nodes[i].object = this;
		cells[i]->m_actor_list.link(&this->m_obj_nodes[i]);
	}
};

void gv_actor_2d::add_to_cell()
{
	if (m_cells.size())
	{
		remove_from_cell();
	}
	if (get_collision_type() == gve_collision_type_aabb)
	{
		gv_cell_2d::cache cells;
		get_world()->collect_cells(m_world_aabb, cells);
		add_to_cells(cells);
	}
	else if (get_collision_type() == gve_collision_type_line_segs)
	{
		gv_vector2i pos = get_position();
		gv_line_actor_cell_collector h(get_world());
		for (int i = 0; i < m_collision_vertices.size() / 2; i++)
		{
			gv_vector2i p0 = m_collision_vertices[i * 2] + pos;
			gv_vector2i p1 = m_collision_vertices[i * 2 + 1] + pos;
			gvt_trace_line_fixed_point_2d(
				p0, p1,
				boost::bind(&gv::gv_line_actor_cell_collector::collect, &h, _1),
				1000);
		}
		add_to_cells(h.cells);
	}
};
void gv_actor_2d::remove_from_cell()
{
	if (m_cells.size())
	{
		for (int i = 0; i < m_cells.size(); i++)
		{
			gv_cell_2d* cell = m_cells[i];
			cell->m_actor_list.unlink(&m_obj_nodes[i]);
		}
	}
	m_cells.clear();
	m_obj_nodes.clear();
};
gv_world_2d* gv_actor_2d::get_world()
{
	if (!m_world)
	{
		m_world =
			(gv_world_2d*)gvt_cast< const gv_world_2d >(get_entity()->get_world());
	}
	GV_ASSERT(m_world);
	return m_world;
};
void gv_actor_2d::on_detach()
{
	this->remove_from_cell();
	// if (m_guid.is_valid() )  this->get_world()->remove_object_with_guid(m_guid,
	// this);
};
void gv_actor_2d::on_attach()
{
	m_world =
		(gv_world_2d*)gvt_cast< const gv_world_2d >(get_entity()->get_world());
	if (gv_global::framework_config.is_editor || this->m_local_aabb.is_empty())
	{
		this->sync_from_entity(get_entity());
	}
	// if (m_guid.is_valid() )  this->get_world()->add_object_with_guid(m_guid,
	// this);
};
void gv_actor_2d::set_local_aabb(const gv_recti& r)
{
	gv_vector2i pos = get_position();
	m_local_aabb = m_world_aabb = r;
	set_position(pos);
};
const gv_recti& gv_actor_2d::get_local_aabb()
{
	return m_local_aabb;
};
void gv_actor_2d::set_position(const gv_vector2i& pos)
{
	m_world_aabb.move_center_to(pos);
	if (!m_world_aabb.is_inside(get_world()->get_map_rect()))
	{
		GVM_DEBUG_LOG(ai, "actor " << get_name() << " out of world!!!! ");
	}
	m_position_2d = pos;
	GV_ASSERT(m_world_aabb.center() == m_position_2d);
};
gv_vector2i gv_actor_2d::get_position()
{
	return m_position_2d;
};

const gv_recti& gv_actor_2d::get_world_aabb()
{
	return m_world_aabb;
};
void gv_actor_2d::on_hit(gv_cell_2d* c, gv_vector2i pos, gv_vector2i normal)
{
	GVM_DEBUG_LOG(ai, "actor hit cell " << pos << " normal :" << normal);
	return;
};
void gv_actor_2d::on_hit(gv_actor_2d* a, gv_vector2i pos, gv_vector2i normal)
{
	GVM_DEBUG_LOG(ai, "actor hit actor " << pos << " normal :" << normal);
	return;
};
void gv_actor_2d::on_arrived(gv_actor_2d* a, gv_vector2i pos)
{
	GVM_DEBUG_LOG(ai, "actor arrived moving target");
	return;
}
gv_color gv_actor_2d::get_color()
{
	return m_color;
};
void gv_actor_2d::set_color(const gv_color& c)
{
	m_color = c;
};
void gv_actor_2d::set_collidable(gv_bool b)
{
	m_collidable = b;
};
gv_bool gv_actor_2d::get_collidable()
{
	return m_collidable;
};
void gv_actor_2d::set_display_adjust_angle(gv_int a)
{
	m_display_adjust_angle = a;
};

gv_vector2i gv_actor_2d::get_momentum_speed()
{
	GV_ASSERT(m_mass);
	return m_momentum * get_logic_frame_time() / m_mass;
};
gv_vector2i gv_actor_2d::get_estimated_movement_this_frame()
{
	return (m_speed + m_external_force + get_momentum_speed()) *
		   m_speed_modifier / 1000;
};

gv_int gv_actor_2d::get_logic_frame_time()
{
	return 33; // 30fps
};

gv_bool gv_actor_2d::can_penatrate(gv_actor_2d* other)
{
	return this->get_penatrate_level() > other->get_penatrate_level();
}

void gv_actor_2d::post_move_actor(gv_vector2i old_pos, gv_vector2i old_target)
{
	gv_vector2i moved = m_position_2d - old_pos;
	m_external_force = m_position_2d - old_target; // 20% energy lose
	// let's slide it alone the surface.
	if (this->is_slidable())
	{
		m_external_force =
			m_external_force.ortho(); // m_external_force =
									  // m_speed.ortho()*get_logic_frame_time()
									  // /1000*gvt_max(m_external_force.abs_sum()
									  // ,100)/ 100;
		if (m_external_force.dot(moved) < 0)
		{
			m_external_force = -m_external_force;
		}
	}
	m_momentum = m_momentum * m_aero_friction / 1000;
	m_angular_momentum = m_angular_momentum * m_angular_friction / 1000;
	m_speed = 0;
};

gv_bool gv_actor_2d::is_slidable()
{
	return m_speed.abs_sum() >= get_momentum_speed().abs_sum();
};

void gv_actor_2d::set_enabled(gv_bool b)
{
	super::set_enabled(b);
	if (b)
	{
		add_to_cell();
	}
	else
	{
		remove_from_cell();
	}
};

gve_collision_type_2d gv_actor_2d::get_collision_type()
{
	return m_collision_type;
};
void gv_actor_2d::set_collision_type(gve_collision_type_2d type)
{
	m_collision_type = type;
};

gvt_array< gv_vector2i >& gv_actor_2d::get_collision_vertices()
{
	return m_collision_vertices;
};

GVM_IMP_CLASS(gv_actor_2d, gv_com_tasklet);
GVM_VAR(gv_color, m_color)
GVM_VAR(gv_recti, m_local_aabb)
GVM_VAR(gv_recti, m_world_aabb)
GVM_VAR(gv_vector2i, m_position_2d)
GVM_VAR(gv_int, m_height)
GVM_VAR(gv_vector2i, m_speed)
GVM_VAR(gv_int, m_facing)
GVM_VAR(gv_int, m_turning_speed)
GVM_VAR(gv_int, m_display_adjust_angle)
GVM_VAR(gv_bool, m_collidable)
GVM_VAR(gv_vector2i, m_momentum)
GVM_VAR(gv_int, m_mass)
GVM_VAR(gv_int, m_angular_momentum)
GVM_VAR(gv_int, m_aero_friction)
GVM_VAR(gv_int, m_angular_friction)
GVM_VAR(gv_int, m_penatrate_level)
GVM_VAR(gvt_array< gv_vector2i >, m_collision_vertices)
GVM_VAR(gv_int, m_collision_type)
GVM_END_CLASS
}