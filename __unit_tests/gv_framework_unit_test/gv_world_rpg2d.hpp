#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
gv_com_rpg2d_actor::gv_com_rpg2d_actor()
{
	GVM_SET_CLASS(gv_com_rpg2d_actor)
	this->m_location = gv_vector2(0, 0);
	this->m_local_aabb = this->m_world_aabb = gv_rect(0, 0, 0, 0);
	m_facing = 0;
	m_delta_init.v.randomize();
	m_delta_pos = 0;
	m_auto_move = false;
	m_auto_move_speed = 20.f;
	m_auto_reinit = rand() % 100 + 100;
	m_auto_reinit_count = 0;
	m_adjust_angle = gv_float_trait::pi();
}
gv_com_rpg2d_actor::~gv_com_rpg2d_actor()
{
}
bool gv_com_rpg2d_actor::tick(gv_float dt)
{
	if (m_auto_move)
	{
		// test code;
		m_delta_pos = m_delta_init;
		m_delta_pos -= gv_vector2(0.5, 0.5);
		m_delta_pos *= dt * m_auto_move_speed;
		m_facing = gv_math::dir_to_angle(m_delta_pos) + m_adjust_angle;
		m_auto_reinit_count++;
		if (m_auto_reinit_count > m_auto_reinit)
		{
			m_auto_reinit_count = 0;
			m_delta_init.v.randomize();
		}
	}
	return true;
};
bool gv_com_rpg2d_actor::sync_from_entity(const gv_entity* entity)
{
	m_local_aabb.max_p = to_2d(entity->get_local_aabb().max_p);
	m_local_aabb.min_p = to_2d(entity->get_local_aabb().min_p);
	// m_local_aabb.max_p.set(1,1);
	// m_local_aabb.min_p.set(-1,-1);
	m_location = to_2d(entity->get_position());
	m_world_aabb = m_local_aabb;
	m_world_aabb.move_center_to(m_location);
	m_facing = entity->get_rotation().yaw;
	return true;
}
bool gv_com_rpg2d_actor::sync_to_entity(gv_entity* entity)
{
	if (entity->is_renderable())
	{
		gv_vector3 pos = to_3d(this->m_location);
		pos.y = entity->get_position().y;
		gv_entity* owner = gvt_cast< gv_entity >(entity->get_owner());
		if (owner)
		{
			gv_com_terrain_roam* terrain =
				owner->get_component< gv_com_terrain_roam >();
			if (terrain)
			{
				pos.y = terrain->get_height_world(pos);
				pos.y = pos.y - entity->get_local_aabb().min_p.y + 0.3f;
				entity->set_position(pos);
			}
		}
		entity->set_position(pos);
		entity->set_rotation(gv_euler(0, m_facing, 0));
		entity->update_matrix();
	}
	return true;
};
bool gv_com_rpg2d_actor::is_tickable()
{
	return true;
}

gv_color gv_com_rpg2d_actor::get_state_color()
{
	return gv_color::RED();
}
//============================================================================================
//								:
//============================================================================================
gv_rpg2d_cell::gv_rpg2d_cell(){};
gv_rpg2d_cell::~gv_rpg2d_cell(){};
void gv_rpg2d_cell::add_actor(gv_com_rpg2d_actor* a)
{
	m_actor_cache.add(a);
};
void gv_rpg2d_cell::remove_actor(gv_com_rpg2d_actor* a)
{
	m_actor_cache.erase_item_fast(a);
};
//============================================================================================
//								:
//============================================================================================
gv_world_rpg2d::gv_world_rpg2d()
{
	GVM_SET_CLASS(gv_world_rpg2d);
	m_debug_actor = 0;
	m_debug_radar_enabled = false;
	m_debug_radar_camera_pos = gv_vector2(0, 0);
	m_debug_radar_camera_zoom = 4.0f;
};
gv_world_rpg2d::~gv_world_rpg2d(){

};
//-------------------------events---------------------
void gv_world_rpg2d::init(gv_int layer, const gv_vector2i& grid_width_height,
						  const gv_box& world_size, void* data)
{
	m_world_box = world_size;
	char* map_data = (char*)data;
	m_cell_size_x =
		(gv_int)(world_size.get_zdelta() / (gv_float)grid_width_height.x);
	m_cell_size_y =
		(gv_int)(world_size.get_xdelta() / (gv_float)grid_width_height.y);
	m_map_width = grid_width_height.x;
	m_map_height = grid_width_height.y;
	if (!map_data)
		return;
	gv_int width = m_map_width;
	gv_int height = m_map_height;
	m_cells.resize(height);
	for (int i = 0; i < height; i++)
		m_cells[i].resize(width);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			char data = map_data[i * width + j];
			gv_rpg2d_cell& c = m_cells[i][j];
			c.m_type = (gv_uint)data;
			switch (data)
			{
			case ' ':
				c.m_color = gv_color::BLACK();
				c.m_can_move_on = true;
				break;
			case '1':
				c.m_color = gv_color::GREEN_B();
				c.m_can_move_on = false;
				break;
			case '2':
				c.m_color = gv_color::GREY_B();
				c.m_can_move_on = false;
				break;
			case '3':
				c.m_color = gv_color::BLUE_B();
				c.m_can_move_on = false;
				break;
			default:
				c.m_color = gv_color::BLUE_B();
				c.m_can_move_on = false;
				break;
			}
			c.m_rect.move_to((gv_float)j * m_cell_size_x,
							 (gv_float)i * m_cell_size_y);
			c.m_rect.set_size((gv_float)m_cell_size_x, (gv_float)m_cell_size_y);
		}
};
//-------------------------events---------------------
void gv_world_rpg2d::register_events()
{
	super::register_events();
};
void gv_world_rpg2d::unregister_events()
{
	super::unregister_events();
};

bool gv_world_rpg2d::tick(gv_float dt)
{
	bool ret = super::tick(dt);
#pragma GV_REMINDER( \
	"strange bug here , make pix crash, when f12!!, disable it when need pix!")

	static bool draw_debug_world = true;
	if (draw_debug_world)
	{
		for (int i = 0; i < m_cells.size(); i++)
			for (int j = 0; j < m_cells[i].size(); j++)
			{
				gv_rpg2d_cell& cell = m_cells[i][j];
				if (!cell.m_can_move_on)
					this->debug_draw_cell(cell);
			}
	}

	for (int i = 0; i < this->m_entities.size(); i++)
	{
		float t = 0;
		gv_com_rpg2d_actor* pactor =
			m_entities[i]->get_component< gv_com_rpg2d_actor >();
		if (!pactor)
			continue;
		if (pactor->m_delta_pos.is_almost_zero())
			continue;
		this->move_actor(pactor, pactor->m_delta_pos, pactor->m_facing, t);
		if (t < 0.5f)
		{
			this->move_actor(pactor, pactor->m_delta_pos, pactor->m_facing, t);
		}
		pactor->m_delta_pos = gv_vector2::get_zero_vector();
	}
	if (m_debug_radar_enabled)
		this->draw_in_debug_radar();
	return ret;
};

gv_int gv_world_rpg2d::on_event(gv_object_event* pevent)
{
	gv_int ret = super::on_event(pevent);
	return ret;
};

bool gv_world_rpg2d::add_entity(gv_entity* entity)
{
	super::add_entity(entity);
	gv_com_rpg2d_actor* pactor = entity->get_component< gv_com_rpg2d_actor >();
	if (pactor)
	{
		pactor->sync_from_entity(entity);
	}
	return true;
};
void gv_world_rpg2d::delete_entity(gv_entity* entity)
{
	cell_cache cells;
	gv_com_rpg2d_actor* pactor = entity->get_component< gv_com_rpg2d_actor >();
	if (!pactor)
		return;
	this->collect_cells(pactor->m_world_aabb, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		cells[i]->remove_actor(pactor);
	}
	super::delete_entity(entity);
};

bool gv_world_rpg2d::move_entity(gv_entity* entity, gv_vector3 new_pos,
								 gv_euler new_rot)
{
	gv_vector2 delta_pos = to_2d(new_pos - entity->get_position());
	gv_float t = 1.0f;
	gv_com_rpg2d_actor* pactor = entity->get_component< gv_com_rpg2d_actor >();
	if (!pactor)
	{
		return super::move_entity(entity, new_pos, new_rot);
	}
	this->move_actor(pactor, delta_pos, new_rot.yaw, t);
	if (t < 0.5f)
	{
		this->move_actor(pactor, delta_pos, new_rot.yaw, t);
	}
	pactor->sync_to_entity(entity);
	return true;
};

bool gv_world_rpg2d::move_actor(gv_com_rpg2d_actor* pactor,
								gv_vector2& delta_pos, gv_float new_rot,
								float& time_of_impact)
{
	cell_cache cells;
	if (!pactor)
	{
		return false;
	}
	pactor->m_facing = new_rot;
	if (delta_pos.is_almost_zero())
	{
		return true;
	}
	this->collect_cells(pactor->m_world_aabb, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		cells[i]->remove_actor(pactor);
	}
	cells.clear();
	gv_rect rect_moving_aabb;
	rect_moving_aabb.add(delta_pos);
	rect_moving_aabb.move(pactor->m_world_aabb.center());
	rect_moving_aabb.extend(pactor->m_world_aabb);
	rect_moving_aabb.extend(2);

	gv_uint tag = get_unique_tag();
	this->collect_cells(rect_moving_aabb, cells);
	time_of_impact = 1.0f;
	gv_vector3 normal;
	gv_com_rpg2d_actor* hit_actor = 0;
	gv_rpg2d_cell* hit_cell = 0;
	for (int i = 0; i < cells.size(); i++)
	{
		if (!cells[i]->m_can_move_on)
		{
			gv_sweep_result result;
			bool hit = gv_geom::sweep(pactor->m_world_aabb, cells[i]->m_rect,
									  delta_pos, result, 0.01f);
			if (hit && result.time_of_impact < time_of_impact)
			{
				hit_actor = NULL;
				hit_cell = cells[i];
				time_of_impact = result.time_of_impact;
				normal = result.normal;
			}
		}
		for (int j = 0; j < cells[i]->m_actor_cache.size(); j++)
		{
			gv_com_rpg2d_actor* pactor2 = cells[i]->m_actor_cache[j];
			if (pactor2->m_test_tag == tag)
				continue;
			pactor2->m_test_tag = tag;
			gv_sweep_result result;
			bool hit = gv_geom::sweep(pactor->m_world_aabb, pactor2->m_world_aabb,
									  delta_pos, result, 0.01f);
			if (hit && result.time_of_impact < time_of_impact)
			{
				hit_actor = pactor2;
				hit_cell = 0;
				time_of_impact = result.time_of_impact;
				normal = result.normal;
			}
		} // next gv_com_rpg2d_actor
	}	 // next gv_rpg2d_cell
	// move and update gv_rpg2d_cell
	cells.clear();
	pactor->m_world_aabb.move(delta_pos * time_of_impact);
	collect_cells(pactor->m_world_aabb, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		cells[i]->add_actor(pactor);
	}
	pactor->m_location = pactor->m_world_aabb.center();
	if (time_of_impact < 1.0f)
	{
		// hit happen;
		delta_pos *= (1.0f - gvt_max(time_of_impact, 0.f));
		if (hit_cell)
		{
			delta_pos = gv_geom::slide(normal.v2, delta_pos);
		}
		if (hit_actor)
		{
			delta_pos = gv_geom::slide(normal.v2, delta_pos);
		}
	}
	return true;
};

bool gv_world_rpg2d::teleport_entity(gv_entity* entity, gv_vector3 pos_3d,
									 gv_euler rot, bool check_overlap)
{
	if (!get_world_box().is_point_inside(pos_3d))
		return false;
	cell_cache cells;
	gv_vector2 pos = to_2d(pos_3d);
	gv_com_rpg2d_actor* pactor = entity->get_component< gv_com_rpg2d_actor >();
	if (!pactor)
		return false;
	this->collect_cells(pactor->m_world_aabb, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		cells[i]->remove_actor(pactor);
	}
	cells.clear();
	pactor->m_world_aabb.move_center_to(pos);
	pactor->m_location = pos;
	gv_uint tag = get_unique_tag();
	this->collect_cells(pactor->m_world_aabb, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		if (!cells[i]->m_can_move_on && check_overlap)
			return false;
		for (int j = 0; j < cells[i]->m_actor_cache.size(); j++)
		{
			gv_com_rpg2d_actor* pactor2 = cells[i]->m_actor_cache[j];
			if (pactor2->m_test_tag == tag)
				continue;
			pactor2->m_test_tag = tag;
			if (pactor2->m_world_aabb.is_overlap(pactor->m_world_aabb) &&
				check_overlap)
			{
				return false;
			}
		} // next gv_com_rpg2d_actor
	}	 // next gv_rpg2d_cell
	for (int i = 0; i < cells.size(); i++)
	{
		cells[i]->add_actor(pactor);
	}
	pactor->sync_to_entity(entity);
	return true;
};
//-----------------------------------------------------
void gv_world_rpg2d::collect_entity(const gv_line_segment& line,
									collect_result& result){

};
void gv_world_rpg2d::collect_entity(const gv_box& box, collect_result& result){

};
void gv_world_rpg2d::collect_entity(const gv_rect& rect,
									collect_result& result)
{
	cell_cache cells;
	this->collect_cells(rect, cells);
	gv_uint tag = get_unique_tag();
	int nb_actors = 0;
	for (int i = 0; i < cells.size(); i++)
	{
		if (!cells[i]->m_can_move_on)
			continue;
		for (int j = 0; j < cells[i]->m_actor_cache.size(); j++)
		{
			gv_com_rpg2d_actor* pactor2 = cells[i]->m_actor_cache[j];
			if (pactor2->m_test_tag == tag)
				continue;
			pactor2->m_test_tag = tag;
			result.push_back(pactor2->get_entity());
			nb_actors++;
		} // next gv_com_rpg2d_actor
	}	 // next gv_rpg2d_cell
};
void gv_world_rpg2d::collect_entity(const gv_frustum& frustum,
									collect_result& result) const {

};
//-----------------------------------------------------
gv_uint gv_world_rpg2d::get_unique_tag()
{
	return ++m_tag;
}
gv_vector2i gv_world_rpg2d::get_cell_coordinate(const gv_vector2& vector)
{
	gv_vector2i ret;
	ret.x = (gv_int)floorf(vector.x / m_cell_size_x);
	ret.y = (gv_int)floorf(vector.y / m_cell_size_y);
	return ret;
}
gv_int gv_world_rpg2d::collect_cells(const gv_rect& rect, cell_cache& result)
{
	gv_vector2i top_left = get_cell_coordinate(rect.min_p);
	gv_vector2i down_right = get_cell_coordinate(rect.max_p);
	for (int i = top_left.y; i <= down_right.y; i++)
	{
		if (!gvt_between(i, 0, m_map_height - 1))
			continue;
		for (int j = top_left.x; j <= down_right.x; j++)
		{
			if (!gvt_between(j, 0, m_map_width - 1))
				continue;
			result.add(&m_cells[i][j]);
		}
	}
	return result.size();
};

//-------------------------------------------------------------------------------------------
void gv_world_rpg2d::debug_draw_cell(const gv_rpg2d_cell& cell)
{
	gv_vector3 v[4];
	v[0] = to_3d(cell.m_rect.min_p);
	v[1] = to_3d(cell.m_rect.max_p);
	v[2] = to_3d(cell.m_rect.min_p + gv_vector2(cell.m_rect.width(), 0));
	v[3] = to_3d(cell.m_rect.min_p + gv_vector2(0, cell.m_rect.height()));
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[1], cell.m_color,
													  cell.m_color);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[2], cell.m_color,
													  cell.m_color);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[3], cell.m_color,
													  cell.m_color);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[1], v[2], cell.m_color,
													  cell.m_color);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[1], v[3], cell.m_color,
													  cell.m_color);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[2], v[3], cell.m_color,
													  cell.m_color);
}

gv_vector2 gv_world_rpg2d::debug_radar_world_to_window(const gv_vector2& w_v)
{
	gv_vector2 v = w_v;
	v -= m_debug_radar_camera_pos;
	v *= m_debug_radar_camera_zoom;
	v += m_debug_radar_display_window_size / 2.0f;
	v.x = floorf(v.x);
	v.y = floorf(v.y);
	return v;
}
gv_vector2 gv_world_rpg2d::debug_radar_window_to_world(const gv_vector2& w_v)
{
	gv_vector2 v = w_v;
	v -= m_debug_radar_display_window_size / 2.0f;
	v /= m_debug_radar_camera_zoom;
	v += m_debug_radar_camera_pos;
	return v;
}

void gv_world_rpg2d::draw_in_debug_radar(gv_com_rpg2d_actor& actor)
{
	gv_rect r = debug_radar_world_to_window(actor.m_world_aabb);
	r.extend(2);
	get_sandbox()->get_debug_renderer()->draw_rect(r, gv_color::BLACK(), 0.8f);
	r.extend(-2);
	gv_global::debug_draw->draw_rect(r, actor.get_state_color(), 0.8f);
}
void gv_world_rpg2d::draw_in_debug_radar(gv_rpg2d_cell& cell)
{
	if (cell.m_can_move_on)
	{
		for (int i = 0; i < cell.m_actor_cache.size(); i++)
		{
			draw_in_debug_radar(*cell.m_actor_cache[i]);
		}
	}
	else
	{
		gv_rect r = debug_radar_world_to_window(cell.m_rect);
		r.extend(-1);
		gv_global::debug_draw->draw_rect(r, cell.m_color, 0.8f);
	}
}
gv_int gv_world_rpg2d::draw_in_debug_radar()
{
	gv_rect view_window = get_sandbox()->get_debug_renderer()->get_window_rect();
	if (this->m_debug_actor < (gv_uint) this->get_nb_entity())
	{
		gv_entity* pentity = this->get_entity(this->m_debug_actor);
		gv_com_rpg2d_actor* actor = pentity->get_component< gv_com_rpg2d_actor >();
		if (actor)
			m_debug_radar_camera_pos = actor->m_location;
	}
	m_debug_radar_display_window_size = view_window.get_size();
	view_window = debug_radar_window_to_world(view_window);
	cell_cache cells;
	this->collect_cells(view_window, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		this->draw_in_debug_radar(*cells[i]);
	}
	return cells.size();
}

gv_com_test_ai::gv_com_test_ai()
{
	timer = 3;
};
	bool gv_com_test_ai::tick(float dt)
	{
		GVM_HSM
		{

			GVM_STATE(0)
			{

					GVM_CHECK_POINT(0, timer-- > 0);
						timer += 4;
					GVM_CHECK_POINT(1, false);
						GVM_SUB_HSM
						{
							GVM_STATE(0)
							{
								GVM_CHECK_POINT(0, false);
								if (timer--)
								{
									goto_state(1);
									break;
								}
								GVM_CHECK_POINT(1, false);
								timer = 2;
								goto_state(0, 0);
								break;
								//GVM_END_STATE
							}
							GVM_END_STATE
						}
						GVM_END_SUB_HSM
			}
		    GVM_END_STATE
		}
	   GVM_END_HSM
		
		return true;
	}

//============================================================================================
//								:
//============================================================================================
GVM_IMP_CLASS(gv_com_test_ai, gv_com_tasklet)
{

}
GVM_END_CLASS

GVM_IMP_CLASS(gv_com_rpg2d_actor, gv_com_tasklet)
{
	GVM_VAR(gv_rect, m_local_aabb)
	GVM_VAR(gv_rect, m_world_aabb)
	GVM_VAR(gv_vector2, m_location)
	GVM_VAR(gv_vector2, m_delta_pos)
	GVM_VAR(gv_uint, m_test_tag)
	GVM_VAR(gv_bool, m_auto_move)
	GVM_VAR(gv_float, m_auto_move_speed)
	GVM_VAR(gv_int, m_auto_reinit)
	GVM_VAR(gv_float, m_facing)
	GVM_VAR(gv_float, m_adjust_angle)
}
GVM_END_CLASS

GVM_IMP_CLASS(gv_world_rpg2d, gv_world)
GVM_VAR(gvt_array< gvt_array< gv_rpg2d_cell > >, m_cells)
GVM_VAR(gv_int, m_cell_size_x)
GVM_VAR(gv_int, m_cell_size_y)
GVM_VAR(gv_int, m_map_width)
GVM_VAR(gv_int, m_map_height)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_rpg2d_cell)
GVM_VAR(gv_rect, m_rect)
GVM_VAR(gv_bool, m_can_move_on)
GVM_VAR(gv_color, m_color)
GVM_VAR(gv_uint, m_test_tag)
GVM_VAR(gv_uint, m_type)
GVM_END_STRUCT
}