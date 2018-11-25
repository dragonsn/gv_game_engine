
namespace gv
{

class gv_com_ai_player_controller_rpg : public gv_com_tasklet
{
public:
	GVM_DCL_CLASS(gv_com_ai_player_controller_rpg, gv_com_tasklet)
	gv_com_ai_player_controller_rpg()
	{
		GVM_SET_CLASS(gv_com_ai_player_controller_rpg);
		m_speed = 0;
		m_direction = 0;
		m_run_speed = 20.f;
		m_acceleration = 10.f;
		m_max_speed = 20.f;
		m_yaw_speed = 1.f;
	}
	~gv_com_ai_player_controller_rpg(){

	};
	enum
	{
		e_idle = 1,
		e_dead,
		e_walk,
		e_run,
		e_jump,
		e_attack,
	};

	enum
	{
		e_dead_go_to_death = 1,
		e_dead_come_back,
	};
	enum
	{
		e_attack_start = 1,
		e_attack_power_attack,
		e_attack_normal_attack,
	};

	gv_float m_speed;
	gv_float m_acceleration;
	gv_float m_direction;
	gv_float m_run_speed;
	gv_float m_max_speed;
	gv_float m_yaw_speed;

	gvt_ptr< gv_com_animation > m_com_ani;

	bool tick(gv_float dt)
	{
		super::tick(dt);
		m_com_ani =
			(gv_com_animation*)get_entity()->get_component< gv_com_animation >();
		GVM_BT_START
		GVM_BT_NODE_SELECT(e_idle);
		GVM_BT_LEAF(1, play_idle_ani())
		GVM_BT_LEAF(2, on_idle(dt))
		// GVM_BT_LEAF_LATENT(3,task_wait_next_frame() );
		GVM_BT_LEAF(4, goto_state(1))
		GVM_BT_END_NODE

		GVM_BT_NODE_SELECT(e_dead);
		GVM_BT_NODE_SQ(e_dead_go_to_death)
		GVM_BT_LEAF(1, play_dead_ani())
		GVM_BT_LEAF(2, on_dead())
		GVM_BT_LEAF(3, task_sleep(5.f))
		GVM_BT_END_NODE
		GVM_BT_NODE_SQ(e_dead_come_back)
		GVM_BT_LEAF(3, play_standup_ani())
		GVM_BT_LEAF(4, goto_state(e_idle, -2))
		GVM_BT_END_NODE
		GVM_BT_END_NODE

		GVM_BT_NODE_SELECT(e_walk);
		GVM_BT_LEAF(4, play_walk_ani())
		GVM_BT_LEAF(1, on_walk(dt))
		GVM_BT_LEAF_LATENT(2, task_wait_next_frame());
		GVM_BT_LEAF(3, goto_state(1))
		GVM_BT_END_NODE

		GVM_BT_NODE_SELECT(e_attack)
		GVM_BT_LEAF(e_attack_start, on_attack(dt))
		GVM_BT_NODE_SELECT(e_attack_power_attack)
		GVM_BT_LEAF(1, play_attack_ani())
		GVM_BT_LEAF(2, goto_state(e_idle, -2))
		GVM_BT_END_NODE
		GVM_BT_NODE_SELECT(e_attack_normal_attack)
		GVM_BT_LEAF(1, play_attack_ani_2())
		GVM_BT_LEAF(2, goto_state(e_idle, -2))
		GVM_BT_END_NODE
		GVM_BT_END_NODE
		GVM_BT_END
		return true;
	};

	virtual bool play_dead_ani()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Death"), 0.5f, false, true, this);
	}

	virtual bool play_idle_ani()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Stand"), 1.5f, true, true);
	}

	virtual bool play_walk_ani()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Walk"), 0.5f, true, true);
	}

	virtual bool play_standup_ani()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Stand"), 0.5f, false, true, this);
	}

	virtual bool play_attack_ani()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Attack1H"), 0.5f, false, true,
										 this);
	}

	virtual bool play_attack_ani_2()
	{
		GV_ASSERT(m_com_ani);
		return m_com_ani->play_animation(gv_id("Attack1H1"), 0.5f, false, true,
										 this);
	}

	virtual bool on_idle(float dt)
	{
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_w) ||
			get_sandbox()->get_input_manager()->is_key_down(e_key_a) ||
			get_sandbox()->get_input_manager()->is_key_down(e_key_s) ||
			get_sandbox()->get_input_manager()->is_key_down(e_key_d))
		{
			goto_state(e_walk, -1);
			return true;
		}
		else if (get_sandbox()->get_input_manager()->is_key_down(e_key_lbutton))
		{
			goto_state(e_attack, -1);
			return true;
		}
		return false;
	}

	virtual bool on_walk(float dt)
	{
		gv_float factor = 1.0f;
		;
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_shift))
		{
			factor = 2.f;
			if (get_sandbox()->get_input_manager()->is_key_down(e_key_w) &&
				m_com_ani->get_animation_name() != gv_id("Run"))
			{
				m_com_ani->play_animation(gv_id("Run"), 0.5f, true, true);
			}
		}
		else if (m_com_ani->get_animation_name() != gv_id("Walk"))
		{
			m_com_ani->play_animation(gv_id("Walk"), 0.5f, true, true);
		}

		m_speed = 0;
		bool is_moving = false;
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_w))
		{
			m_speed = m_run_speed * factor * dt;
			is_moving = true;
		}
		else if (get_sandbox()->get_input_manager()->is_key_down(e_key_s))
		{
			m_speed = -m_run_speed * factor * dt;
			is_moving = true;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_a))
		{
			m_direction -= m_yaw_speed * dt * factor;
			is_moving = true;
		}
		else if (get_sandbox()->get_input_manager()->is_key_down(e_key_d))
		{
			m_direction += m_yaw_speed * dt * factor;
			is_moving = true;
		}

		if (!is_moving)
		{
			goto_state(e_idle, -1);
			m_speed = 0;
			return true;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_lbutton))
		{
			goto_state(e_attack, -1);
			m_speed = 0;
			return true;
		}
		return false;
	}

	bool on_attack(gv_float dt)
	{
		if (rand() % 5 == 1)
		{
			goto_state(e_attack_power_attack);
		}
		else
			goto_state(e_attack_normal_attack);
		return true;
	}
	bool on_dead()
	{
		return false;
	}
	bool sync_to_entity(gv_entity* entity)
	{
		gv_vector3 pos = entity->get_position();
		gv_vector3 dir = entity->get_tm().axis_x;
		dir.y = 0;
		gv_euler rot = entity->get_rotation();
		rot.yaw = m_direction;
		pos += dir * m_speed;
		entity->get_world()->move_entity(entity, pos, rot);
		return true;
	}

	gv_int on_killed(gv_object_event* pevent)
	{
		this->goto_state(e_dead, 0);
		return 1;
	}
};

GVM_IMP_CLASS(gv_com_ai_player_controller_rpg, gv_com_tasklet)
GVM_END_CLASS
}