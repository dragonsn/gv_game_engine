
//physics & behavior & render & execute command
namespace gv
{

gv_rts_unit::gv_rts_unit()
{
	GVM_SET_CLASS(gv_rts_unit);
};

gv_rts_unit::~gv_rts_unit()
{
};

bool gv_rts_unit::tick_fixed(gv_int delta_time)
{
	super::tick_fixed(delta_time); 
	tick_cmds(delta_time);
	tick_movement(delta_time);
	return true; 
};

bool gv_rts_unit::tick_cmds(gv_int dt)
{
	//for
	return true;
};

bool gv_rts_unit::tick_movement(gv_int dt)
{
	if (m_target_pos == static_invalid_location())
	{
		return true;
	}
	gv_vector3i vi=m_target_pos-m_position;
	static int impulse_limit = 1000;
	vi = gv_math_fixed::normalize3d(vi)*10;
	add_impulse_to_control_velocity(vi, gv_vector3i(impulse_limit, impulse_limit, impulse_limit));
	return true;
};

bool gv_rts_unit::sync_to_entity(gv_entity* entity)
{
	super::sync_to_entity(entity);
	return true;
};

void	gv_rts_unit::set_target_pos(gv_vector3i pos)
{
	m_target_pos = pos;
};

void	gv_rts_unit::set_target_unit(gv_rts_unit *u)
{
	
};


//ai: check enemy &send command to unit

gv_rts_unit_ai::gv_rts_unit_ai()
{
	GVM_SET_CLASS(gv_rts_unit_ai);
	m_ai_added = false;
	//always one frame  behind
	m_fixed_delta_time = 0;
	m_move_target = static_invalid_location();
	m_faction = 0; 
};

gv_rts_unit_ai::~gv_rts_unit_ai()
{
};

bool gv_rts_unit_ai::tick(gv_float dt)
{
	//call tick fixed instead
	GV_ASSERT(0);
	return true;
};

bool gv_rts_unit_ai::tick_fixed()
{
	if (m_fixed_delta_time==0 )
	{
		return true; 
	}
	//update target enemy 
	//
	gv_int attack_distance = 10000; 
	if (!m_attack_target || distance_to(m_attack_target) > attack_distance)
	{
		m_attack_target = get_ai_system()->find_nearest_enemy(this,attack_distance);;
	}
	tick_ai_state();
	return true;
};

bool gv_rts_unit_ai::tick_ai_state()
{
	/*e_unit_ai_idle, 
			e_unit_ai_attack,
			e_unit_ai_dead, 
			e_unit_ai_wound,
			e_unit_ai_deleted,*/

	GVM_HSM
	{
		GVM_STATE(e_unit_ai_idle)
		{
			GVM_CHECK_POINT(0, false);
			if (this->m_distance_moved.abs_sum() > 5)
			{
				goto_state( e_unit_ai_move,0); 
			}
			break;

		}GVM_END_STATE

		GVM_STATE(e_unit_ai_move)
		{
			GVM_CHECK_POINT(0, false);
			if (this->m_distance_moved.abs_sum() < 10)
			{
				goto_state( e_unit_ai_idle,0);
			}
			break;

		}GVM_END_STATE


		GVM_STATE(e_unit_ai_attack)
		{
			GVM_CHECK_POINT(0, false);
			break;

		}GVM_END_STATE


		GVM_STATE(e_unit_ai_wound)
		{
			GVM_CHECK_POINT(0, false);
			break;

		}GVM_END_STATE

		GVM_STATE(e_unit_ai_dead)
		{
			GVM_CHECK_POINT(0, false);
			break;

		}GVM_END_STATE

		GVM_STATE(e_unit_ai_deleted)
		{
			GVM_CHECK_POINT(0, false);
			break;

		}GVM_END_STATE
	}
	GVM_END_HSM
	return true; 
};

void gv_rts_unit_ai::on_attach()
{
	//possiblly post in the middle of processing , do this in the synchronization phase for safe
	//GVM_POST_JOB(game_logic, (&gv_rts_ai_system::add_ai,  gv_rts_ai_system::static_get(), this));
	return ;
};

gv_bool gv_rts_unit_ai::is_controlled_by_player()
{
	if (!m_ai_system) return false;
	return (m_faction == m_ai_system->m_player_faction && m_team_id == m_ai_system->m_player_team);
	
};
void gv_rts_unit_ai::on_detach()
{
};


bool gv_rts_unit_ai::sync_to_entity(gv_entity* pe)
{
	if (!m_ai_added)
	{
		m_ai_added = true;
		gv_rts_ai_system::static_get()->add_ai(this);
		this->m_unit =(gv_rts_unit*) get_entity()->get_component<gv_rts_unit>();
		this->m_distance_moved = gv_vector3i::get_zero_vector();
	}
	else
	{
		this->m_distance_moved = m_unit->get_position() - m_pos_copy; 
	}
	m_fixed_delta_time = this->m_unit->get_world()->get_logic_frame_time();
	m_pos_copy = m_unit->get_position();
	m_rot_copy = m_unit->get_rotation();
	m_state_copy = m_unit->get_state(0);
	m_unit->set_target_pos(m_move_target);
	pe->set_user_flag(get_state(0));
	return true;
};

//ai system
gv_rts_ai_system * s_rts_ai_system;
gv_rts_ai_system::gv_rts_ai_system()
{
	GVM_SET_CLASS(gv_rts_ai_system);
	s_rts_ai_system = this;
}

gv_rts_ai_system::~gv_rts_ai_system()
{
};
bool gv_rts_ai_system::tick(float dt)
{
	super::tick(dt);
	//update cells and bounding box.
	for (int i = 0; i < m_hot_cells.size(); i++)
	{
		m_hot_cells[i]->m_faction_list[0] = nullptr; 
		m_hot_cells[i]->m_faction_list[1] = nullptr; 
	}
	m_hot_cells.clear_and_reserve(); 

	for (int i = 0; i <this->m_factions.size(); i++)
	{
		this->m_factions[i].reset_bounding();

	}
	for (int i = 0; i < m_ai_array.size(); i++)
	{
		gv_rts_unit_ai * ai = m_ai_array[i]; 
		if (!ai) continue; 
		gv_rts_ai_cell * pcell= m_ai_grid.get_cell(ai->m_pos_copy.v2); 
		if (pcell)
		{
			//push to the list
			ai->m_next_ai_in_grid = pcell->m_faction_list[ai->get_faction()];
			pcell->m_faction_list[ai->get_faction()] = ai;
		}
		gv_rts_faction_info * faction= get_faction(ai->get_faction()); 
		faction->add_ai(ai); 
		ai->tick_fixed();
	}

	tick_factions();
	
	return true; 
};

bool gv_rts_ai_system::tick_factions()
{
	for (int i = 0; i < m_factions.size(); i++)
	{
		gv_rts_faction_info * info = &m_factions[i]; 
		for (int team = 0; team < info->m_teams.size(); team++)
		{
			//stop moving after reach target..
			gv_rts_team_info * p = info->m_teams[team];
			if (p->m_move_target != static_invalid_location())
			{
				gv_boxi b = p->m_bb; 
				b.extend(gv_vector3i(1000, 3000, 1000));
				//test if reach the target , close and compact enough.
				if (b.is_point_inside(p->m_move_target) && b.get_size().abs_sum()<p->m_ai_array.size()*1000  )
				{
					int unstable_count = 0; 
					for (int u = 0; u < p->m_ai_array.size(); u++)
					{
						if (p->m_ai_array[u]->get_state() == gv_rts_unit_ai::e_unit_ai_move)
						{
							unstable_count++; 
							break; 
						}
					}
					if (unstable_count == 0)
					{
						exec_move_team_to(p, static_invalid_location());
					}
				}
			}
		}
	}
	return true;
}

bool gv_rts_ai_system::do_synchronization()
{
	if (m_ai_grid.m_cell_size.abs_sum() == 0)
	{
		gv_world_3d * pworld = get_sandbox()->get_event_processor<gv_world_3d>();
		gv_world_grid * pg = pworld->get_grid();
		m_ai_grid.setup(pg->m_cell_size * 8, pg->m_grid_rect);
		m_hot_cells.reserve(pg->m_cell_size.x * pg->m_cell_size.x); 
	}
	return true;
};

gv_rts_ai_system * gv_rts_ai_system::static_get()
{
	return s_rts_ai_system;
};

gv_int gv_rts_ai_system::add_ai(gvt_ref_ptr< gv_rts_unit_ai>  ai)
{
	 m_ai_array.add(ai);
	 ai->m_ai_system = this;
	 return m_ai_array.size() - 1;
};

void gv_rts_ai_system::remove_ai(gvt_ref_ptr< gv_rts_unit_ai>  ai)
{
	m_ai_array[ai->m_unique_index]=nullptr;
};

gv_rts_unit_ai * gv_rts_ai_system::get_ai_by_index(gv_int index)
{
	return m_ai_array[index];
};

gv_rts_unit_ai * gv_rts_ai_system::find_nearest_enemy(gv_rts_unit_ai * ai, gv_int distance )
{
	gv_recti r; 
	r.add(ai->m_pos_copy.v2); 
	r.extend(distance);
	int enemy_faction = ai->get_enemy_faction();
	int nearest_distance=distance;
	gv_rts_unit_ai *ret = nullptr; 
	//lambda
	m_ai_grid.visit_cells(r, [&ai, &enemy_faction,&nearest_distance ,& ret ](gv_rts_ai_cell & cell) {
		gv_rts_unit_ai * e = cell.m_faction_list[enemy_faction]; 
		while (e) {
			if (ai->distance_to(e) < nearest_distance)
			{
				nearest_distance = ai->distance_to(e);
				ret = e;
			}
		}
	});
	return ret;
};

//player command 
gv_int gv_rts_ai_system::exec_player_select_next_team()
{
	gv_rts_faction_info * faction = get_faction(m_player_faction); 
	m_player_team = m_player_team < faction->m_teams.size() - 1 ? m_player_team + 1 : 0; 
	return m_player_team; 
};

gv_int gv_rts_ai_system::exec_player_select_team(gv_int index)
{
	gv_rts_faction_info * faction = get_faction(m_player_faction);
	m_player_team = index < faction->m_teams.size() ? index : m_player_team;
	return m_player_team;
};

void gv_rts_ai_system::exec_move_team_to(gv_rts_team_info * team,   gv_vector3i pos)
{

	if (team)
	{
		team->m_move_target = pos;
		for (int i = 0; i < team->m_ai_array.size(); i++)
		{
			team->m_ai_array[i]->m_move_target = pos;
		}
	}
}
void gv_rts_ai_system::exec_player_move_to(gv_vector3i pos)
{
	gv_rts_team_info * team = get_team(m_player_faction, m_player_team);
	exec_move_team_to(team ,pos);
};

void  gv_rts_ai_system::exec_player_attack(gv_int faction, gv_int target_team)
{
	gv_rts_team_info * team = get_team(m_player_faction, m_player_team);
	if (team)
	{
		gv_rts_team_info * target= get_team(faction, target_team);
		team->m_target_enemy = target;
	}
};



class gv_com_rts_controller : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_rts_controller, gv_component)
		gv_com_rts_controller()
	{
		m_is_drag = false;
		m_cu_mouse_pos.set(0, 0);
		m_last_mouse_pos.set(0, 0);
		link_class(gv_com_rts_controller::static_class());
	};
	~gv_com_rts_controller()
	{
	}
	bool is_tickable()
	{
		return true;
	}
	bool tick(gv_float dt)
	{
		const gv_entity* my_camera = this->get_entity();
		pos = my_camera->get_position();
		dir = my_camera->get_tm().axis_z;
		rot = my_camera->get_rotation();
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_shift))
		{
			dt *= 10;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_up))
		{
			pos.y += 10.f * dt;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_down))
		{
			pos.y -= 10.f * dt;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_w))
		{
			gv_float step = 10 * (dt);
			pos += dir * step;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_s))
		{
			gv_float step = 10 * (dt);
			pos -= dir * step;
		}

		gv_matrix43 mat;
		gv_math::convert(mat, this->rot);
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_left))
		{
			pos += mat.axis_x * dt;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_right))
		{
			pos -= mat.axis_x * dt;
		}
		{
			bool down =
				get_sandbox()->get_input_manager()->is_key_down(e_key_lbutton);
			if (m_is_drag && !down)
			{
				m_is_drag = false;
			}
			if (down && !m_is_drag)
			{
			}
			m_is_drag = down;
			m_last_mouse_pos = m_cu_mouse_pos;
			get_sandbox()->get_input_manager()->get_mouse_pos(m_cu_mouse_pos);
			if (m_is_drag)
			{
				gv_vector2i delta = m_cu_mouse_pos - m_last_mouse_pos;
				rot.yaw -= gvt_clamp(delta.x, -100, +100) * dt;
			}
		}
		{
			gv_float step =
				get_sandbox()->get_input_manager()->get_mouse_wheel_delta() * (dt);
			pos += dir * step;
			get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
		}

		return true;
	}

	bool sync_to_entity(gv_entity* entity)
	{
		// gv_com_camera * pcamera=entity->get_component<gv_com_camera>();
		entity->set_rotation(rot);
		entity->set_position(pos);
		entity->update_matrix();
		return true;
	}

	bool m_is_drag;
	gv_vector2i m_cu_mouse_pos;
	gv_vector2i m_last_mouse_pos;

	gv_vector3 pos;
	gv_vector3 dir;
	gv_euler rot;
};
GVM_IMP_CLASS(gv_com_rts_controller, gv_component)
GVM_END_CLASS

GVM_IMP_CLASS(gv_rts_unit, gv_actor_3d)
GVM_END_CLASS

GVM_IMP_CLASS(gv_rts_ai_system, gv_event_processor)
GVM_END_CLASS

GVM_IMP_CLASS(gv_rts_unit_ai, gv_com_tasklet)
GVM_END_CLASS

}