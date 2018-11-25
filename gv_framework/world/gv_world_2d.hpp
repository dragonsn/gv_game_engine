#include "gv_actor_2d.hpp"

#define CHECK_OVERLAP 0
namespace gv
{
//============================================================================================

bool gv_cell_2d::can_move_on()
{
	if (m_type >= gv_cell_configs::instance().cell_infos.size())
		return true;
	return gv_cell_configs::instance().cell_infos[m_type].can_move_on;
}

bool gv_cell_2d::can_block_bullet()
{
	if (m_type >= gv_cell_configs::instance().cell_infos.size())
		return false;
	return gv_cell_configs::instance().cell_infos[m_type].can_block_bullet;
}

int gv_cell_2d::get_speed_modifier()
{
	return gv_cell_configs::instance().cell_infos[m_type].speed_modifier;
}

int gv_cell_2d::get_hp_generation_modifier()
{
	return gv_cell_configs::instance().cell_infos[m_type].hp_generation_modifier;
}

bool gv_cell_2d::is_override_normal()
{
	return gv_cell_configs::instance().cell_infos[m_type].override_normal != 0;
};

gv_vector2i gv_cell_2d::get_new_normal()
{
	return gv_cell_configs::instance().cell_infos[m_type].new_normal;
};

gv_vector2i gv_cell_2d::get_corrected_normal(const gv_vector2i& norm,
											 const gv_vector2i& dir)
{
	if (!is_override_normal())
		return norm;
	gv_vector2i new_norm = get_new_normal();
	if (new_norm.dot(dir) >= 0)
		return new_norm;
	else
		return -new_norm;
};
//============================================================================================

gv_world_2d::gv_world_2d()
{
	GVM_SET_CLASS(gv_world_2d);
	m_debug_actor = 0;
	m_debug_radar_enabled = false;
	m_debug_radar_camera_pos = gv_vector2i(0, 0);
	m_debug_radar_camera_zoom = 4;
	m_debug_radar_display_window_size = gv_vector2i(256, 256);
	m_current_frame_count = 0;
};
gv_world_2d::~gv_world_2d(){

};
void gv_world_2d::init(const gv_vector2i& grid_width_height,
					   const gv_vector2i& cell_size, const gv_string& map_data,
					   gv_bool use_token)
{
	init_map_size(grid_width_height, cell_size);
	init_map_data(map_data, use_token);
};
void gv_world_2d::init_map_size(const gv_vector2i& grid_width_height,
								const gv_vector2i& cell_size)
{
	m_cells.clear();
	m_cell_size = cell_size;
	m_map_size = grid_width_height;
	gv_int width = m_map_size.get_x();
	gv_int height = m_map_size.get_y();
	m_world_box.set(gv_vector3(0, 0, 0),
					gv_vector3((gv_float)(width * cell_size.get_x()),
							   (gv_float)(height * cell_size.get_y()), 20000));
	m_cells.resize(width);
	for (int i = 0; i < width; i++)
		m_cells[i].resize(height);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			gv_cell_2d& c = m_cells[i][j];
			c.m_rect.move_to(gv_vector2i(i, j) * m_cell_size);
			c.m_rect.set_size(m_cell_size);
		}
	}
};
void gv_world_2d::init_map_data(const gv_string& map_data, gv_bool use_token)
{
	gv_int width = m_map_size.get_x();
	gv_int height = m_map_size.get_y();
	if (!map_data.strlen())
		return;
	gv_lexer l;
	l.goto_string(*map_data);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			gv_cell_2d& c = m_cells[i][j];
			if (use_token)
				l >> c.m_type;
			else
			{
				char data = map_data[j * width + i];
				c.m_type = (gv_uint)(data);
			}
			// set_collision_flag(c);
		}
	}
};
bool gv_world_2d::tick(gv_float dt)
{
	GV_PROFILE_EVENT(gv_world_2d__tick, 0);
	bool ret = super::tick(dt);
	{
		GV_PROFILE_EVENT(gv_world_2d__tick_physics, 0);
		for (int i = 0; i < this->m_entities.size(); i++)
		{
			gv_actor_2d* pactor = m_entities[i]->get_component< gv_actor_2d >();
			if (!pactor || !pactor->is_enabled())
				continue;
#if CHECK_OVERLAP
// if (!teleport_actor(pactor, pactor->get_position(), pactor->get_facing(),
// true))
//{
//	GV_DEBUG_BREAK;
//}
#endif
			gv_vector2i delta_pos = pactor->get_estimated_movement_this_frame();
			if (delta_pos == gv_vector2i(0, 0))
				continue;
			gv_vector2i old_pos = pactor->m_position_2d;
			gv_vector2i target_pos = pactor->m_position_2d + delta_pos;
			this->move_actor(pactor, target_pos,
							 pactor->m_facing + pactor->m_turning_speed);
			pactor->post_move_actor(old_pos, target_pos);
		}
	}
	if (m_debug_radar_enabled)
		this->draw_in_debug_radar();
	m_current_frame_count++;
	return ret;
};

void gv_world_2d::debug_draw_3d_grid(const gv_recti& r, bool mirror)
{
	GV_PROFILE_EVENT(gv_world_2d__debug_draw_3d_grid, 0);
	if (r.is_empty())
	{
		for (int i = 0; i < m_cells.size(); i++)
		{
			for (int j = 0; j < m_cells[i].size(); j++)
			{
				gv_cell_2d& cell = m_cells[i][j];
				if (!cell.can_move_on())
				{
					this->debug_draw_cell(cell, mirror);
					continue;
				}
				if (!cell.m_actor_list.is_empty())
					this->debug_draw_cell(cell);
			}
		}

		for (int i = 0; i < m_entities.size(); i++)
		{
			gv_entity* e = m_entities[i];
			gv_actor_2d* punit = e->get_component< gv_actor_2d >();
			if (punit)
				debug_draw_unit_bound(punit, mirror);
		}
	}
	else
	{
		gv_cell_2d::cache c;
		// this->get_cell_coordinate
		this->collect_cells(r, c);
		for (int i = 0; i < c.size(); i++)
		{
			if (!c[i]->can_move_on())
				this->debug_draw_cell(*c[i], mirror);
		}
		gv_actor_2d::cache actors;
		this->collect_actors(r, actors);
		for (int i = 0; i < actors.size(); i++)
		{
			gv_actor_2d* punit = actors[i];
			if (punit)
			{
				debug_draw_unit_bound(punit, mirror);
			}
		}
	}
}

bool gv_world_2d::move_actor(gv_actor_2d* pactor, const gv_vector2i& new_pos,
							 gv_int new_facing)
{
	GV_PROFILE_EVENT(gv_world_2d__move_actor, 0);
	gv_vector2i old_pos = pactor->get_position();

	gv_vector2i delta_pos = new_pos - old_pos;
	GV_ASSERT(old_pos == pactor->m_world_aabb.center());
	static gv_int skin = 1;
	if (!pactor)
	{
		return false;
	}
	pactor->m_facing = gv_math_fixed::normalize_deg_angle(new_facing);
	if (delta_pos == gv_vector2i::get_zero_vector())
	{
		return true;
	}
	gv_vector2i moved_distance, normal, contact;
	moved_distance = 0;
	pactor->remove_from_cell();
	gv_recti cu_aabb = pactor->m_world_aabb;
	bool ignore_actor = pactor->m_world_aabb.area() == 0;

	for (int dim = 0; dim < 2; dim++)
	{
		gv_cell_2d::cache cells;

		gv_recti rect_moving_aabb = cu_aabb;
		gv_int speed = delta_pos[dim];
		gv_int move_dir = 0;
		if (speed == 0)
		{
			continue;
		}
		else if (speed < 0)
		{
			rect_moving_aabb.min_p[dim] += speed;
			move_dir = 1; // will collide the max point first
		}
		else
		{
			rect_moving_aabb.max_p[dim] += speed;
			move_dir = 0; // will collide the min point first.
		}
		gv_uint tag = get_unique_tag();
		this->collect_cells(rect_moving_aabb, cells);

		// OOS warning, sort must not contain equaled element, otherwise order is
		// not comfirmed.
		gv_cell_comparator comparator(dim, move_dir);
		cells.sort(comparator);

		gv_actor_2d* hit_actor = 0;
		gv_cell_2d* hit_cell = 0;
		gv_int moved = speed;

		for (int i = 0; i < cells.size(); i++)
		{

			if (!cells[i]->can_move_on())
			{
				// assume there is no overlap between the mover and the cell, since this
				// cell is not be moved one
				bool hit = gv_math_fixed::sweep(cu_aabb, cells[i]->m_rect, speed, moved,
												contact, normal, dim, skin);
				if (hit)
				{
					if (gvt_abs(moved) > gvt_abs(speed))
					{
						break; // we pass the first hit !
					}
					else
					{
						speed = moved;
						normal = cells[i]->get_corrected_normal(normal, -delta_pos);
						pactor->on_hit(cells[i], contact, normal);
					}
				}
			}
			gv_obj_list::iterator it = cells[i]->m_actor_list.begin();
			gvt_array_cached< gv_actor_2d*, 256 > actors;
			gvt_array_cached< gv_actor_2d*, 256 > line_actors;

			while (it.get_node() && !ignore_actor)
			{
				gv_actor_2d* pactor2 = gvt_cast< gv_actor_2d >(it.ptr());
				GV_ASSERT(pactor2);
				if (!pactor2->is_enabled())
				{
					it++;
					continue;
				}
				if (pactor2->m_test_tag == tag)
				{
					it++; // already tested in the round.
					continue;
				}
				if (pactor2->get_collision_type() == gve_collision_type_aabb)
					actors.push_back(pactor2);
				if (pactor2->get_collision_type() == gve_collision_type_line_segs)
					line_actors.push_back(pactor2);
				pactor2->m_test_tag = tag;
				it++;
			}
			//==go through line colliders =====================
			gv_vector2i normal_l, contact_l;
			gv_int speed_l = speed;
			gv_actor_2d* first_line_collider = NULL;
			for (int j = 0; j < line_actors.size(); j++)
			{
				gv_actor_2d* pactor2 = line_actors[j];
				gvt_array< gv_vector2i >& vertices = pactor2->get_collision_vertices();
				gv_vector2i pos = pactor2->get_position();
				for (int line = 0; line < vertices.size() / 2; line++)
				{
					if (gv_math_fixed::sweep(cu_aabb, vertices[line * 2] + pos,
											 vertices[line * 2 + 1] + pos, speed, moved,
											 contact, normal, dim, skin))
					{
						speed = speed_l = moved;
						normal_l = normal;
						contact_l = contact;
						first_line_collider = pactor2;
					}
				};
			}
			//====================================
			gv_actor_comparator c_actor(dim, move_dir);

			// OOS warning, sort must not contain equaled element, otherwise order is
			// not comfirmed.
			actors.sort(c_actor);

			gv_line_segment result;
			gv_bool find_closer_actor = false;
			for (int j = 0; j < actors.size(); j++)
			{
				gv_actor_2d* pactor2 = actors[j];
				if (pactor2->m_world_aabb.area() == 0)
				{
					continue;
				}
				if (gv_math_fixed::sweep(cu_aabb, pactor2->m_world_aabb, speed, moved,
										 contact, normal, dim, skin))
				{
					if (gvt_abs(moved) > gvt_abs(speed))
					{
						moved = speed;
						break; // we pass the first hit !
					}
					else
					{
						if (!pactor->can_penatrate(pactor2))
							speed = moved;

						if (!pactor->can_penatrate(pactor2))
						{
							speed = moved;
							find_closer_actor = true;
						}
						pactor->on_hit(pactor2, contact, normal);
						pactor2->on_hit(pactor, contact,
										gv_vector2i::get_zero_vector() - normal);
					}
				}
			} // next gv_actor_2d
			//==test if the line actor is the first ================
			if (!find_closer_actor && first_line_collider)
			{
				pactor->on_hit(first_line_collider, contact_l, normal_l);
				first_line_collider->on_hit(pactor, contact_l,
											gv_vector2i::get_zero_vector() - normal_l);
			}
		} // next gv_cell_2d
		moved_distance[dim] = speed;
		gv_vector2i this_delta;
		this_delta = 0;
		this_delta[dim] = speed;
		cu_aabb.move(this_delta);
	} // next dim !=0
	// move and update gv_cell_2d
	if (moved_distance != gv_vector2i::get_zero_vector())
	{
#if CHECK_OVERLAP
		if (pactor->m_penatrate_level >= 1000 &&
			!teleport_actor(pactor, old_pos + moved_distance, pactor->get_facing(),
							true))
		{
			GV_DEBUG_BREAK;
		}
		else
#endif
			pactor->set_position(old_pos + moved_distance);
	}
	pactor->add_to_cell();
	return true;
};
bool gv_world_2d::teleport_actor(gv_actor_2d* pactor, const gv_vector2i& pos,
								 gv_int new_rot, bool check_overlap)
{
	GV_PROFILE_EVENT(gv_world_2d__teleport_actor, 0);
	gv_cell_2d::cache cells;
	if (!pactor)
		return false;
	if (pactor->get_collision_type() == gve_collision_type_line_segs)
		return true;
	gv_recti r = pactor->m_local_aabb;
	r.move_center_to(pos);
	if (!r.is_inside(this->get_map_rect()))
		return false;
	if (check_overlap)
	{
		gv_uint tag = get_unique_tag();
		this->collect_cells(r, cells);
		for (int i = 0; i < cells.size(); i++)
		{
			gv_cell_2d* cell = cells[i];
			if (!cell->can_move_on())
				return false;
			gv_obj_list::iterator it = cells[i]->m_actor_list.begin();
			while (it.get_node())
			{
				gv_actor_2d* pactor2 = gvt_cast< gv_actor_2d >(it.ptr());
				GV_ASSERT(pactor2);
				if (pactor2 == pactor || pactor2->m_test_tag == tag ||
					!pactor2->is_enabled() || pactor->can_penatrate(pactor2))
				{
					it++; // already tested in the round or it's itself
					continue;
				}
				pactor2->m_test_tag = tag;
				if (pactor2->get_collision_type() == gve_collision_type_line_segs)
				{
					gvt_array< gv_vector2i >& vertices = pactor2->get_collision_vertices();
					gv_vector2i pos = pactor2->get_position();
					for (int line = 0; line < vertices.size() / 2; line++)
					{
						if (gv_math_fixed::segment_cast(vertices[line * 2] + pos,
														vertices[line * 2 + 1] + pos, r))
							return false;
					}
				}
				else if (pactor2->m_world_aabb.is_overlap(r) && check_overlap)
				{
					return false;
				}
				it++;
			}
		} // next gv_cell_2d
	}	 // overlap
	pactor->set_position(pos);
	pactor->add_to_cell();
	return true;
};
bool gv_world_2d::move_entity(gv_entity* entity, gv_vector3 new_pos,
							  gv_euler new_rot)
{
	gv_actor_2d* pactor = entity->get_component< gv_actor_2d >();
	if (pactor)
	{
		gv_vector2i v;
		gv_int h;
		to_2d(new_pos, v, h);
		if (this->move_actor(pactor, v, -gv_math_fixed::rad_to_deg(new_rot.yaw)))
		{
			pactor->sync_to_entity(entity);
			return true;
		}
	}
	return false;
};
bool gv_world_2d::teleport_entity(gv_entity* entity, gv_vector3 new_pos,
								  gv_euler rot, bool check_overlap)
{
	gv_actor_2d* pactor = entity->get_component< gv_actor_2d >();
	if (pactor)
	{
		gv_vector2i v;
		gv_int h;
		to_2d(new_pos, v, h);
		if (this->teleport_actor(pactor, v, -gv_math_fixed::rad_to_deg(rot.yaw),
								 check_overlap))
		{
			pactor->sync_to_entity(entity);
			return true;
		};
	}
	return false;
};
gv_uint gv_world_2d::get_unique_tag()
{
	return ++m_tag;
}
gv_vector2i gv_world_2d::get_cell_coordinate(const gv_vector2i& vector)
{
	gv_vector2i ret = vector / m_cell_size;
	return ret;
}
gv_vector2i gv_world_2d::get_cell_position(const gv_vector2i& vector)
{
	gv_vector2i ret = vector * m_cell_size;
	return ret;
}
gv_recti gv_world_2d::get_map_rect()
{
	return gv_recti(gv_vector2i(0, 0), m_map_size * m_cell_size);
}
gv_recti gv_world_2d::get_map_grid_rect()
{
	return gv_recti(gv_vector2i(0, 0), m_map_size - gv_vector2i(1, 1));
}
gv_int gv_world_2d::collect_cells(const gv_recti& rect,
								  gv_cell_2d::cache& result)
{
	GV_PROFILE_EVENT(gv_world_2d__collect_cells, 0);
	gv_recti map_rect = get_map_grid_rect();
	gv_recti r(get_cell_coordinate(rect.min_p), get_cell_coordinate(rect.max_p));
	r = map_rect.clip(r);
	for (int i = r.left; i <= r.right; i++)
	{
		for (int j = r.top; j <= r.bottom; j++)
		{
			result.add(&m_cells[i][j]);
		}
	}
	return result.size();
};
gv_int gv_world_2d::collect_cells_sorted(const gv_recti& rect,
										 gv_cell_2d::cache& result, gv_int dim,
										 gv_int move_dir)
{
	gv_recti map_rect = get_map_grid_rect();
	gv_recti r(get_cell_coordinate(rect.min_p), get_cell_coordinate(rect.max_p));
	r = map_rect.clip(r);
	for (int i = r.left; i <= r.right; i++)
	{
		for (int j = r.top; j <= r.bottom; j++)
		{
			result.add(&m_cells[i][j]);
		}
	}
	gv_cell_comparator comparator(dim, move_dir);
	result.sort(comparator);

	return result.size();
};

gv_cell_2d* gv_world_2d::get_cell(const gv_vector2i& p)
{
	if (p.x < 0 || p.x >= m_map_size.x)
		return NULL;
	if (p.y < 0 || p.y >= m_map_size.y)
		return NULL;
	return &m_cells[p.x][p.y];
};

gv_int gv_world_2d::collect_actors(const gv_recti& rect,
								   gv_actor_2d::cache& result)
{
	GV_PROFILE_EVENT(gv_world_2d__collect_actors, 0);
	gv_cell_2d::cache cells;
	// optimize the collector performance when the rect is tooo biiiigggggg
	gv_recti map_rect(get_cell_coordinate(rect.min_p),
					  get_cell_coordinate(rect.max_p));
	if (map_rect.area() >= m_entities.size() * 2)
	{
		for (int i = 0; i < m_entities.size(); i++)
		{
			gv_actor_2d* actor = m_entities[i]->get_component< gv_actor_2d >();
			if (actor && actor->get_world_aabb().area() == 0)
			{
				continue;
			}
			if (actor && actor->is_enabled() &&
				actor->get_world_aabb().is_overlap(rect))
			{
				result.push_back(actor);
			}
		}
		return result.size();
	}
	gv_uint tag = get_unique_tag();
	collect_cells(rect, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		gv_obj_list::iterator it = cells[i]->m_actor_list.begin();
		gvt_array_cached< gv_actor_2d*, 256 > actors;
		while (it.get_node())
		{
			gv_actor_2d* pactor2 = gvt_cast< gv_actor_2d >(it.ptr());
			GV_ASSERT(pactor2);
			if (pactor2->m_test_tag == tag || pactor2->m_ignore_me)
			{
				it++; // already tested in the round.
				continue;
			}
			if (!pactor2->get_world_aabb().is_overlap(rect))
			{
				it++; // already tested in the round.
				continue;
			}
			if (!pactor2->is_enabled())
			{
				it++;
				continue;
			}
			result.push_back(pactor2);
			pactor2->m_test_tag = tag;
			it++;
		}
	}
	return result.size();
};

void gv_world_2d::debug_draw_cell(gv_cell_2d& cell, bool mirror)
{
	debug_draw_rect(cell.m_rect, get_cell_color(cell), mirror);
}

void gv_world_2d::debug_draw_unit_bound(class gv_actor_2d* actor, bool mirror)
{
	if (actor->is_enabled())
	{
		debug_draw_rect(actor->get_world_aabb(), gv_color::RED(), mirror);
		if (actor->get_collision_type() == gve_collision_type_line_segs)
		{
			gvt_array< gv_vector2i >& vertices = actor->get_collision_vertices();
			gv_vector2i pos = actor->get_position();
			for (int i = 0; i < vertices.size() / 2; i++)
			{
				debug_draw_line2d(pos + vertices[i * 2], pos + vertices[i * 2 + 1],
								  gv_color::PINK(), mirror);
			}
		}
	}
	else
	{
		if (actor->get_world_aabb().area() > 100)
			debug_draw_rect(actor->get_world_aabb(), gv_color::GREY(), mirror);
	}
};
void gv_world_2d::debug_draw_rect(const gv_recti& r, const gv_color& c,
								  bool mirror)
{
	gv_vector2i v2d[4] = {
		r.min_p, r.max_p, r.min_p + gv_vector2i(r.width(), 0),
		r.min_p + gv_vector2i(0, r.height()),
	};
	gv_vector3 v[4];
	for (int i = 0; i < 4; i++)
	{
		to_3d(v2d[i], get_height(v2d[i]) + 100, v[i]);
		if (mirror)
			v[i].z = -v[i].z;
	}
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[1], c, c);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[2], c, c);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[3], c, c);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[1], v[2], c, c);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[1], v[3], c, c);
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[2], v[3], c, c);
};

void gv_world_2d::debug_draw_line2d(gv_vector2i start, gv_vector2i end,
									const gv_color& c, bool mirror)
{
	gv_vector2i v2d[2] = {start, end};
	gv_vector3 v[2];
	for (int i = 0; i < 2; i++)
	{
		to_3d(v2d[i], get_height(v2d[i]) + 100, v[i]);
		if (mirror)
			v[i].z = -v[i].z;
	}
	get_sandbox()->get_debug_renderer()->draw_line_3d(v[0], v[1], c, c);
};
gv_vector2i gv_world_2d::debug_radar_world_to_window(const gv_vector2i& w_v)
{
	gv_vector2i v = w_v;
	v -= m_debug_radar_camera_pos;
	v *= m_debug_radar_camera_zoom;
	v += m_debug_radar_display_window_size / 2;
	return v;
}
gv_vector2i gv_world_2d::debug_radar_window_to_world(const gv_vector2i& w_v)
{
	gv_vector2i v = w_v;
	v -= m_debug_radar_display_window_size / 2;
	v /= m_debug_radar_camera_zoom;
	v += m_debug_radar_camera_pos;
	return v;
}
void gv_world_2d::draw_in_debug_radar(gv_actor_2d& actor)
{
	gv_rect r;
	r = debug_radar_world_to_window(actor.m_world_aabb);
	r.extend(2);
	get_sandbox()->get_debug_renderer()->draw_rect(r, gv_color::BLACK(), 0.8f);
	r.extend(-2);
	gv_global::debug_draw->draw_rect(r, actor.get_color(), 0.8f);
}
void gv_world_2d::draw_in_debug_radar(gv_cell_2d& cell)
{
	if (cell.can_move_on())
	{
		gv_obj_list::iterator it = cell.m_actor_list.begin();
		while (it.get_node())
		{
			gv_actor_2d* pactor2 = gvt_cast< gv_actor_2d >(it.ptr());
			GV_ASSERT(pactor2);
			draw_in_debug_radar(*pactor2);
			it++;
		}
	}
	else
	{
		gv_rect r;
		r = debug_radar_world_to_window(cell.m_rect);
		r.extend(-1);
		gv_global::debug_draw->draw_rect(r, get_cell_color(cell), 0.8f);
	}
}
gv_int gv_world_2d::draw_in_debug_radar()
{
	gv_recti view_window;
	view_window = get_sandbox()->get_debug_renderer()->get_window_rect();
	if (this->m_debug_actor < (gv_uint) this->get_nb_entity())
	{
		gv_entity* pentity = this->get_entity(this->m_debug_actor);
		gv_actor_2d* actor = pentity->get_component< gv_actor_2d >();
		if (actor)
			m_debug_radar_camera_pos = actor->m_position_2d;
	}
	m_debug_radar_display_window_size = view_window.get_size();
	view_window = debug_radar_window_to_world(view_window);
	gv_cell_2d::cache cells;
	this->collect_cells(view_window, cells);
	for (int i = 0; i < cells.size(); i++)
	{
		this->draw_in_debug_radar(*cells[i]);
	}
	return cells.size();
}
gv_recti gv_world_2d::debug_radar_world_to_window(const gv_recti& r)
{
	return gv_recti(debug_radar_world_to_window(r.min_p),
					debug_radar_world_to_window(r.max_p));
}
gv_recti gv_world_2d::debug_radar_window_to_world(const gv_recti& r)
{
	return gv_recti(debug_radar_window_to_world(r.min_p),
					debug_radar_window_to_world(r.max_p));
}
gv_color gv_world_2d::get_cell_color(gv_cell_2d& c)
{
	gv_color colors[] = {
		gv_color::GREY_B(), gv_color::GREEN(), gv_color::PINK(),
	};
	/*
          if (c.can_move_on())	{
                  if (!c.m_actor_list.is_empty()) return gv_color::YELLOW();
                  else return gv_color::BLACK();
          }
          if (!c.m_actor_list.is_empty()) {
                  return gv_color::RED();
          }
          else {
                  return gv_color::GREEN();
  }*/
	if (!c.can_move_on())
	{
		int color_idx = c.m_type - '1';
		if (color_idx < gvt_array_length(colors))
		{
			return colors[color_idx];
		}
	}
	else
	{
		gv_color::YELLOW();
	}
	return gv_color::BLACK();
};
void gv_world_2d::set_collision_flag(gv_cell_2d& c)
{
	// c.m_can_move_on=( c.m_type==0);
}

gv_int gv_world_2d::get_height(const gv_vector2i& vector)
{
	// this is still float , can't use it for gameplay..
	gv_int h = 0;
	gv_vector3 v3;
	to_3d(vector, h, v3);
	return gv_math_fixed::to_fixed(get_height(v3.x, v3.z));
};

gv_float gv_world_2d::get_height(gv_float x, gv_float z)
{
#if GV_WITH_OS_API
	if (m_entities.size())
	{
		gv_com_terrain_roam* terrain =
			m_entities[0]->get_component< gv_com_terrain_roam >();
		if (terrain && !terrain->is_empty())
		{
			return terrain->get_height_world(gv_vector3(x, 0, z));
		}
	}
#endif
	return 0;
}
//---------------------------------------------------------------------------

GVM_IMP_CLASS(gv_world_2d, gv_world)
GVM_VAR(gvt_array< gvt_array< gv_cell_2d > >, m_cells)
GVM_VAR(gv_vector2i, m_cell_size)
GVM_VAR(gv_vector2i, m_map_size)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_cell_2d)
GVM_VAR(gv_int, m_type)
GVM_VAR(gv_recti, m_rect)
GVM_END_STRUCT
}