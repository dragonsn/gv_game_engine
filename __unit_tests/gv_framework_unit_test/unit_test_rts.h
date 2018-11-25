namespace gv
{
	enum gve_rts_cmd
	{
		e_cmd_move_to,
		e_cmd_do_action,
	};

	struct gv_rts_cmd
	{
		gv_ushort m_code; 
		gv_vector3i m_target;
	};

	static gv_vector3i static_invalid_location()
	{
		return gv_vector3i(-100000, -100000, -10000);
	}

	//physics & behavior & render & execute command
	class gv_rts_unit :public gv_actor_3d
	{
	public:
		GVM_DCL_CLASS(gv_rts_unit, gv_actor_3d);
		gv_rts_unit();
		~gv_rts_unit();
		virtual bool tick_fixed(gv_int delta_time);
		virtual bool tick_cmds(gv_int dt);
		virtual bool tick_movement(gv_int dt);
		virtual bool sync_to_entity(gv_entity* entity);
		void	set_target_pos(gv_vector3i pos) ;
		void	set_target_unit(gv_rts_unit *);
		gv_vector3i get_target_pos() const
		{
			return m_target_pos;
		}
			
	protected:
		gvt_array_cached< gv_rts_cmd,16> m_cmd_buffer;
		gv_int m_ani_timer; 
		gv_vector3i m_target_pos;
	};

	//ai: check enemy &send command to unit
	class gv_rts_unit_ai :public gv_com_tasklet
	{
		friend class gv_rts_ai_system; 
		friend class gv_rts_team_info; 
		friend class gv_rts_faction_info;
	public:
		GVM_DCL_CLASS(gv_rts_unit_ai, gv_com_tasklet)
		gv_rts_unit_ai();
		~gv_rts_unit_ai();
		enum
		{
			e_unit_ai_idle=0, 
			e_unit_ai_move,
			e_unit_ai_attack,
			e_unit_ai_dead, 
			e_unit_ai_wound,
			e_unit_ai_deleted,
		}; 
		virtual bool tick(gv_float dt);
		virtual void on_attach();
		virtual void on_detach();
		virtual bool sync_to_entity(gv_entity*);
		virtual bool is_tickable()
		{
			return false;
		}
		void set_faction(int f) 
		{
			m_faction = f; 
		};
		void set_type(int t)
		{
			m_type = t;
		};
		void set_team(int t)
		{
			m_team_id = t;
		};
		class gv_rts_ai_system * get_ai_system()
		{
			return m_ai_system;
		};

		gv_int distance_to(gv_rts_unit_ai * a)
		{
			return (m_pos_copy - a->m_pos_copy).abs_sum(); 
		}

		gv_int get_faction()
		{
			return  m_faction;
		}

		gv_int get_enemy_faction()
		{
			return 1 - m_faction;
		}

		gv_bool is_controlled_by_player();

	protected:
		virtual bool tick_fixed();
		virtual bool tick_ai_state();
		gv_int		m_ai_timer;
		gv_int		m_faction;
		gv_int      m_team_id; 
		gv_int		m_type;
		gvt_ptr< gv_rts_unit> m_unit; 
		gvt_ref_ptr< gv_rts_unit_ai> m_attack_target; 
		gvt_ptr< gv_rts_unit_ai> m_next_ai_in_grid; 
		gvt_ptr<gv_rts_ai_system> m_ai_system;
		gv_bool		m_ai_added;
		gv_int		m_fixed_delta_time;
		gv_int		m_unique_index;
		gv_vector3i m_pos_copy;
		gv_vector3i m_distance_moved;
		gv_int      m_move_stop_count_down; 
		gv_vector3i m_move_target;
		gv_euleri	m_rot_copy;
		gv_int		m_state_copy;
	}; 

	//ai system
	struct gv_rts_ai_cell
	{
		gvt_ptr< gv_rts_unit_ai> m_faction_list[2];
	};

	class gv_rts_team_info:public  gv_refable
	{
	public:
		gv_rts_team_info()
		{
			m_move_target = static_invalid_location(); 
		}
		gv_int m_nb_units;
		gv_boxi m_bb;
		gv_bool m_player_controlled;
		gv_vector3i m_move_target;
		gvt_array<gv_rts_unit_ai * > m_ai_array;
		gvt_ptr<gv_rts_team_info> m_target_enemy;
		void reset_bounding()
		{
			m_nb_units = 0;
			m_bb = gv_boxi();
			m_ai_array.clear_and_reserve(); 
		}
		void add_ai(gv_rts_unit_ai * pai)
		{
			m_nb_units++;
			m_bb.add(pai->m_pos_copy);
			m_ai_array.add(pai);
		}
	};
	class gv_rts_faction_info:public gv_refable
	{
	public:
		gv_rts_faction_info()
		{
			m_move_target = static_invalid_location();
		}
		gv_int m_nb_units;
		gv_boxi m_bb; 
		gv_bool m_player_controlled; 
		gv_vector3i m_move_target;
		gvt_array< gvt_ref_ptr<gv_rts_team_info >  > m_teams;
		gv_rts_team_info * get_team(gv_int index)
		{
			if (m_teams.size() <= index)
			{
				m_teams.resize( index+1); 
				gv_rts_team_info * team = new gv_rts_team_info;
				m_teams[index] = team;
			}
			return m_teams[index]; 
		}
		void reset_bounding()
		{
			m_nb_units = 0; 
			m_bb = gv_boxi(); 
		}
		void add_ai( gv_rts_unit_ai * pai)
		{
			m_nb_units++; 
			m_bb.add(pai->m_pos_copy);
			get_team(pai->m_team_id)->add_ai(pai);
		}
		
	};
	struct gv_rts_unit_info
	{
		gv_int m_max_hp; 
		gv_bool m_is_range_attack;
		gv_int m_attack_cool_down;
		gv_int m_attack_ani_length;
		gv_int m_wound_cool_down;
		gv_int m_wound_ani_length;

	};

	class gv_rts_ai_system :public gv_event_processor
	{
	public:
		GVM_DCL_CLASS(gv_rts_ai_system, gv_event_processor)
		gv_rts_ai_system(); 
		~gv_rts_ai_system();
		static gv_rts_ai_system * static_get();

		int  add_ai(gvt_ref_ptr< gv_rts_unit_ai>  ai); 
		void remove_ai(gvt_ref_ptr< gv_rts_unit_ai>  ai);
		gv_rts_unit_ai * get_ai_by_index( gv_int index);
		gv_rts_unit_ai * find_nearest_enemy(gv_rts_unit_ai * ai , gv_int distance=10000);

		virtual bool tick(float dt);
		virtual bool tick_factions();
		virtual bool do_synchronization();
		static gve_event_channel static_channel()
		{
			return gve_event_channel_game_logic;
		}
		void set_factor_player_control(int faction, bool b)
		{
			m_factions[faction].m_player_controlled = b;
			m_player_faction = b;
		}
		gv_rts_faction_info * get_faction( gv_int index)
		{
			return &m_factions[index]; 
		}
		gv_rts_team_info * get_team(gv_int faction, gv_int team)
		{
			return get_faction(faction)->get_team(team);
		};
	public:
		//player command 
		gv_int exec_player_select_next_team();
		gv_int exec_player_select_team(gv_int index);
		void exec_player_move_to( gv_vector3i pos); 
		void  exec_player_attack(gv_int faction, gv_int target_team);
		gv_int m_player_faction;
		gv_int m_player_team;
	protected:
		//internal 
		void exec_move_team_to(gv_rts_team_info * team,   gv_vector3i pos); 
		
		gvt_array< gvt_ref_ptr< gv_rts_unit_ai> > m_ai_array; 
		gvt_grid_2d< gv_rts_ai_cell> m_ai_grid;
		gvt_array< gv_rts_ai_cell* > m_hot_cells;
		gvt_array_fixed< gv_rts_faction_info ,2> m_factions;
	

	};

	
}
