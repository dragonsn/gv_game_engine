namespace gv
{
gv_actor_3d::gv_actor_3d()
{
	GVM_SET_CLASS(gv_actor_3d);

	m_scale = gv_math_fixed::one();
	m_position = 0;
	m_rotation = 0;

	m_impulse = 0;
	m_velocity = 0;

	m_mass = gv_math_fixed::one();
	m_aero_friction = 1000;
	m_turning_speed = 0;
	m_display_adjust_angle = 0;
	m_bouncingness = 800;
	m_stickiness = 100;
	m_slipperiness = 100;
	m_force = 0;
};

gv_actor_3d::~gv_actor_3d(){};

bool gv_actor_3d::sync_from_entity(const gv_entity* entity)
{
	gv_math_fixed::to_fixed(entity->get_local_aabb(), m_local_aabb);
	;
	gv_math_fixed::to_fixed(entity->get_position(), m_position);
	gv_math_fixed::to_fixed(entity->get_rotation(), m_rotation);
	gv_math_fixed::to_fixed(entity->get_scale(), m_scale);
	this->update_grids();
	return true;
}

bool gv_actor_3d::sync_to_entity(gv_entity* entity)
{
	gv_vector3 pos;
	gv_euler rot;
	gv_vector3 scale;
	gv_math_fixed::to_float(this->m_position, pos);
	gv_math_fixed::to_float(m_scale, scale);
	gv_math_fixed::to_float(m_rotation + m_display_adjust_angle, rot);
	entity->set_position(pos);
	entity->set_rotation(rot);
	entity->update_matrix();
	return true;
};

bool gv_actor_3d::is_tickable()
{
	return true;
};

void gv_actor_3d::on_detach()
{
	this->remove_from_all_grids();
};

void gv_actor_3d::on_attach()
{
	if (gv_global::framework_config.is_editor || this->m_local_aabb.is_empty())
	{
		this->sync_from_entity(get_entity());
	}
};

gv_int gv_actor_3d::find_colliders(gv_collider::cache& result)
{
	gv_entity* e = (gv_entity*)get_entity();
	e->query_components< gv_collider >(result);
	return result.size();
};

void gv_actor_3d::create_base_collider(gve_collider_3d_shape shape)
{
	gv_entity* e = (gv_entity*)get_entity();
	gv_collider* c = e->add_component< gv_collider >();
	c->m_collision_shape = shape;
	c->m_local_position = 0;
	c->m_local_aabb = this->m_local_aabb;
	c->m_world_position = m_position;
	c->update_grids();
};

void gv_actor_3d::create_colliders_for_empty_box(gv_int thickness,
												 bool with_bottom,
												 bool with_cap,
												 bool with_walls)
{
	gv_entity* e = (gv_entity*)get_entity();
	gv_boxi bw = this->m_local_aabb;
	gv_vector3i ext = bw.get_extend();
	if (with_bottom)
	{ // bottom
		gv_collider* c = e->add_component< gv_collider >();
		c->m_collision_shape = e_shape_aabb;
		c->m_local_position = bw.get_center();
		;
		c->m_local_position.y -= ext.y;
		c->m_local_aabb.add(gv_vector3i(0));
		c->m_local_aabb.set_extend(ext.x + thickness, thickness, ext.z + thickness);
		c->update_world_position();
		c->rename(gv_id_floor);
	}

	if (with_cap)
	{ // cap
		gv_collider* c = e->add_component< gv_collider >();
		c->m_collision_shape = e_shape_aabb;
		c->m_local_position = bw.get_center();
		;
		c->m_local_position.y += ext.y;
		c->m_local_aabb.add(gv_vector3i(0));
		c->m_local_aabb.set_extend(ext.x + thickness, thickness, ext.z + thickness);
		c->update_world_position();
		c->rename(gv_id_ceiling);
	}

	if (with_walls)
	{
		{ // left
			gv_collider* c = e->add_component< gv_collider >();
			c->m_collision_shape = e_shape_aabb;
			c->m_local_position = bw.get_center();
			c->m_local_position.x -= ext.x;
			c->m_local_aabb.add(gv_vector3i(0));
			c->m_local_aabb.set_extend(thickness, ext.y + thickness,
									   ext.z + thickness);
			c->update_world_position();
			c->rename(gv_id_left);
		}

		{ // right
			gv_collider* c = e->add_component< gv_collider >();
			c->m_collision_shape = e_shape_aabb;
			c->m_local_position = bw.get_center();
			c->m_local_position.x += ext.x;
			c->m_local_aabb.add(gv_vector3i(0));
			c->m_local_aabb.set_extend(thickness, ext.y + thickness,
									   ext.z + thickness);
			c->update_world_position();
			c->rename(gv_id_right);
		}

		{ // front
			gv_collider* c = e->add_component< gv_collider >();
			c->m_collision_shape = e_shape_aabb;
			c->m_local_position = bw.get_center();
			c->m_local_position.z -= ext.z;
			c->m_local_aabb.add(gv_vector3i(0));
			c->m_local_aabb.set_extend(ext.x + thickness, ext.y + thickness,
									   thickness);
			c->update_world_position();
			c->rename(gv_id_front);
		}

		{ // back
			gv_collider* c = e->add_component< gv_collider >();
			c->m_collision_shape = e_shape_aabb;
			c->m_local_position = bw.get_center();
			c->m_local_position.z += ext.z;
			c->m_local_aabb.add(gv_vector3i(0));
			c->m_local_aabb.set_extend(ext.x + thickness, ext.y + thickness,
									   thickness);
			c->update_world_position();
			c->rename(gv_id_back);
		}
	}
	this->update_grids();
};

void gv_actor_3d::update_colliders_world_position()
{
	gv_collider::cache r;
	find_colliders(r);
	for (int i = 0; i < r.size(); i++)
	{
		r[i]->update_world_position();
	}
};

gv_boxi gv_actor_3d::get_world_aabb()
{
	gv_boxi b;
	gv_collider::cache r;
	find_colliders(r);
	for (int i = 0; i < r.size(); i++)
	{
		b.add(r[i]->get_world_aabb());
	}
	return b;
};

gv_bool gv_actor_3d::tick_fixed(gv_int delta_time)
{
	if (!get_entity()->is_static())
	{
		gv_vector3i force =
			gv_math_fixed::mul(get_world()->get_physics()->get_gravity(), m_mass);
		// add_impulse( gv_math_fixed::mul(force, delta_time));
		this->m_force += force;
	};
	return true;
}

void gv_actor_3d::on_hit(gv_collide_result& result)
{
	{ // for debug
		gv_vector3 v0, v1;
		gv_math_fixed::to_float(result.contact, v0);
		gv_math_fixed::to_float(result.contact + result.normal, v1);
		gv_global::debug_draw->draw_line_3d(v0, v1, gv_color::ORANGE(),
											gv_color::BLACK());
	}
	//return;
	// default reflect the momenten
	gv_actor_3d* pa2 = result.target->get_actor();

	if (pa2->get_entity()->is_static())
	{
		this->m_velocity = gv_math_fixed::mul(
			gv_math_fixed::reflect3d(result.normal, this->m_velocity),
			m_bouncingness);
	}
	else
	{
		// http://docslide.us/documents/elastic-collision-with-c.html
		// https://www.physics.ohio-state.edu/~gan/teaching/spring99/C10.pdf
		// m1 = mass of  first spherical body.
		// m2 = mass of  second spherical body.
		// u1 = velocity of m1 before collision.
		// u2 = velocity of m2 before collision.
		// v1 = velocity of m1 after collision.
		// v2 = velocity of m2 after collision.
		// v1 = (m1 - m2 / m1 + m2)u1 + (2m2 / m1 + m2)u2;
		// v2 = (2m1 / m1 + m2)u1 + (m1 - m2 / m1 + m2)u2;
		gv_int u1, u2;
		gv_vector3i dir = -result.normal;
		// gv_vector3i dir = gv_math_fixed::normalize3d(m_position -
		// pa2->m_position);
		gv_vector3i vp_1 = gv_math_fixed::project_in_direction(m_velocity, dir, u1);
		gv_vector3i vp_2 =
			gv_math_fixed::project_in_direction(pa2->m_velocity, dir, u2);
		if (u1 > u2)
		{
			gv_int m1 = this->m_mass;
			gv_int m2 = pa2->m_mass;
			gv_int v1 = (m1 - m2) * u1 / (m1 + m2) + (2 * m2) * u2 / (m1 + m2);
			gv_int v2 = (2 * m1) * u1 / (m1 + m2) + (m1 - m2) * u2 / (m1 + m2);
			this->m_velocity = gv_math_fixed::mul(this->m_velocity - vp_1 +
													  gv_math_fixed::mul(dir, v1),
												  this->m_bouncingness);
			pa2->m_velocity = gv_math_fixed::mul(pa2->m_velocity - vp_2 +
													 gv_math_fixed::mul(dir, v2),
												 pa2->m_bouncingness);
		}
	}
	gv_int force_projection = m_force.dot(result.normal);
	if (force_projection <
		0)
	{ // collision will eliminate the force in the collide direction.
		m_force -= gv_math_fixed::mul(result.normal,
									  force_projection / gv_math_fixed::one());
	}
};

gv_vector3i gv_actor_3d::get_delta_position(gv_int time_fraction)
{
	gv_vector3i delta_pos = m_velocity * time_fraction / gv_math_fixed::one() *
							get_world()->get_logic_frame_time() /
							gv_math_fixed::one();
	return delta_pos;
};

void gv_actor_3d::apply_force_and_impulse(gv_int time_fraction,
										  gv_vector3i movement)
{
	m_velocity = movement * 1000 / get_world()->get_logic_frame_time();
	m_velocity += m_impulse * time_fraction / m_mass;
	m_impulse -= gv_math_fixed::mul(m_impulse, time_fraction);
	// gv_int energy = movement.dot(m_force);
	// energy = energy / m_mass;
	// energy = gv_math_fixed::div(energy, m_velocity.abs_sum());
	// gv_vector3i a = gv_math_fixed::mul(gv_math_fixed::normalize3d(m_force),
	// energy);
	gv_vector3i a = m_force * get_world()->get_logic_frame_time() / m_mass *
					time_fraction / gv_math_fixed::one();
	m_velocity += a;
	m_velocity *= get_world()->m_axis_freedom;
	gv_int friction =gvt_clamp( m_aero_friction*get_world()->get_logic_frame_time() / 1000,0,1000); 
	m_velocity = gv_math_fixed::mul(m_velocity, 1000 - friction);
};

void gv_actor_3d::add_impulse(gv_vector3i v)
{
	m_impulse += v;
};

void gv_actor_3d::add_impulse_to_control_velocity(gv_vector3i target_velocity,
												  gv_vector3i limit)
{
	gv_vector3i i = target_velocity - m_velocity;
	i = gv_math_fixed::mul(i, m_mass);
	i = gvt_clamp(i, -limit, limit);
	m_impulse += i;
};

void gv_actor_3d::on_be_hit(gv_collide_result& result)
{
}

gv_world_3d* gv_actor_3d::get_world()
{
	if (!m_world)
	{
		m_world =
			(gv_world_3d*)gvt_cast< const gv_world_3d >(get_entity()->get_world());
	}
	return m_world;
};

gv_matrix_fixed gv_actor_3d::get_world_matrix()
{
	gv_matrix_fixed mat;
	gv_euler_fixed rot;
	rot = m_rotation;
	gv_vector3_fixed pos;
	pos = m_position;
	gv_vector3_fixed scale;
	scale = m_scale;
	gv_math_ex_fixed::convert(mat, rot);
	gv_matrix_fixed mat_s;
	mat_s.set_scale(scale);
	mat *= mat_s;
	mat.set_trans(pos);
	return mat;
};

bool gv_actor_3d::tick(gv_float dt)
{

	return tick_fixed(get_world()->get_logic_frame_time());
}

void gv_actor_3d::update_grids()
{
	gv_collider::cache parts;
	find_colliders(parts);
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i]->update_grids();
	}
	m_in_grids = true;
};
void gv_actor_3d::remove_from_all_grids()
{
	if (!m_in_grids) return; 
	m_in_grids = false;
	gv_collider::cache parts;
	find_colliders(parts);
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i]->remove_from_all_grids();
	}
};
void gv_actor_3d::pre_physics(){};

void gv_actor_3d::post_physics()
{
	/*
  gv_collider::cache parts;
  find_colliders(parts);
  for debug only
  for (int i = 0; i < parts.size(); i++){
          parts[i]->debug_draw();
  }*/
	m_force = 0;
};

GVM_IMP_CLASS(gv_actor_3d, gv_com_tasklet)
{
	GVM_VAR(gv_boxi, m_local_aabb)
	GVM_VAR(gv_vector3i, m_scale)
	GVM_VAR(gv_vector3i, m_position)
	GVM_VAR(gv_euleri, m_rotation)
	GVM_VAR(gv_vector3i, m_impulse) 
	GVM_VAR(gv_vector3i, m_force)	
	GVM_VAR(gv_vector3i, m_velocity)
	GVM_VAR(gv_int, m_mass)			
	GVM_VAR(gv_vector3i, m_bouncingness)
	GVM_VAR(gv_vector3i, m_stickiness)
	GVM_VAR(gv_vector3i, m_slipperiness)
	GVM_VAR(gv_int, m_aero_friction)
	GVM_VAR(gv_euleri, m_turning_speed)
	GVM_VAR(gv_euleri, m_display_adjust_angle)
	GVM_VAR(gvt_ptr< gv_world_3d >, m_world)
}
GVM_END_CLASS;
}