#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================

enum gve_collision_type_2d
{
	gve_collision_type_aabb,
	gve_collision_type_line_segs,
};

class gv_cell_2d;
class gv_world_2d;

class gv_2d_helper
{
public:
	static void to_2d(const gv_vector3& v, gv_vector2i& v_2d, gv_int& height);
	static void to_2d(const gv_vector3& v, gv_vector2i& v_2d);
	static gv_vector2i to_2d(const gv_vector3& v);
	static void to_2d(const gv_box& box, gv_recti& rect);
	static gv_vector2i to_2d(const gv_vector3i& v);
	static gv_recti to_2d(const gv_boxi& box);
	static void to_3d(const gv_vector2i& v_2d, gv_int height, gv_vector3& v);
	static void to_3d(const gv_vector2i& v_2d, gv_vector3& v);
	static void to_3d(const gv_recti& rect, gv_box& box);
};

class gv_cell_2d
{
	friend class gv_world_2d;
	friend class gv_actor_2d;

public:
	typedef gvt_array_cached< gv_cell_2d*, 256 > cache;
	gv_cell_2d()
	{
		GVM_ZERO_ME;
	}
	~gv_cell_2d()
	{
	}

	bool can_move_on();
	bool can_block_bullet();
	int get_speed_modifier();
	int get_hp_generation_modifier();
	bool is_override_normal();
	gv_vector2i get_new_normal();
	gv_vector2i get_corrected_normal(const gv_vector2i& norm,
									 const gv_vector2i& dir);

public:
	gv_int m_type;
	gv_recti m_rect;
	gv_obj_list m_actor_list;
	gv_uint m_dynamic_obstacle_count;
	gv_uint m_test_tag;
};

class gv_actor_2d : public gv_com_tasklet, public gv_2d_helper
{
	friend class gv_world_2d;

public:
	typedef gvt_array_cached< gv_actor_2d*, 256 > cache;
	GVM_DCL_CLASS(gv_actor_2d, gv_com_tasklet);
	gv_actor_2d();
	~gv_actor_2d();
	virtual bool tick(gv_float dt);
	virtual bool sync_from_entity(const gv_entity* entity);
	virtual bool sync_to_entity(gv_entity* entity);
	virtual bool is_tickable();
	virtual void add_to_cell();
	virtual void add_to_cells(gv_cell_2d::cache& cells);
	virtual void remove_from_cell();
	virtual void on_hit(gv_cell_2d* c, gv_vector2i pos, gv_vector2i normal);
	virtual void on_hit(gv_actor_2d* a, gv_vector2i pos, gv_vector2i normal);
	virtual void on_arrived(gv_actor_2d* a, gv_vector2i pos);
	virtual void on_detach();
	virtual void on_attach();
	virtual void post_move_actor(gv_vector2i old_pos, gv_vector2i old_target);
	virtual gv_color get_color();
	virtual void set_color(const gv_color& c);
	virtual gv_bool is_slidable();
	virtual gv_int get_logic_frame_time();
	virtual gv_bool can_penatrate(gv_actor_2d* other);
	gv_math_fixed* get_helper_2d();

public:
	gv_world_2d* get_world();
	void set_local_aabb(const gv_recti&);
	const gv_recti& get_local_aabb();
	void set_position(const gv_vector2i& pos);
	gv_vector2i get_position();
	const gv_recti& get_world_aabb();
	gv_int get_facing() const
	{
		return m_facing;
	}
	void set_facing(gv_int f)
	{
		m_facing = f;
	}
	void set_collidable(gv_bool b);
	gv_bool get_collidable();
	void set_display_adjust_angle(gv_int);
	void set_penatrate_level(gv_int l)
	{
		m_penatrate_level = l;
	};
	gv_int get_penatrate_level()
	{
		return m_penatrate_level;
	}
	void backup_penatrate_level()
	{
		m_backed_penatrate_level = m_penatrate_level;
	}
	void restore_penatrate_level()
	{
		m_penatrate_level = m_backed_penatrate_level;
	}
	gv_vector2i get_momentum_speed();
	gv_vector2i get_estimated_movement_this_frame();
	gv_int get_mass()
	{
		return m_mass;
	}
	void set_mass(gv_int m)
	{
		m_mass = m;
	}
	virtual void set_enabled(gv_bool b);
	gve_collision_type_2d get_collision_type();
	void set_collision_type(gve_collision_type_2d type);
	gvt_array< gv_vector2i >& get_collision_vertices();

protected:
	gvt_array_cached< gv_obj_node, 16 > m_obj_nodes;
	gvt_array_cached< gv_cell_2d*, 16 > m_cells;
	gvt_ptr< gv_world_2d > m_world;
	gvt_array< gv_vector2i > m_collision_vertices;
	gve_collision_type_2d m_collision_type;
	gv_color m_color;
	gv_recti m_local_aabb;
	gv_recti m_world_aabb;
	gv_vector2i m_position_2d;
	gv_int m_height;			  // as a 2.5d game , we still have height ...
	gv_vector2i m_speed;		  // per frame speed of the unit motor,
	gv_vector2i m_external_force; // external force we can add to the unit
	gv_vector2i m_momentum;		  // ----new physics related parameter,linear momentum
	gv_int m_mass;				  // mass.
	gv_int m_angular_momentum;	// angular
	gv_int m_aero_friction;		  // aero_friction
	gv_int m_angular_friction;	// angular_friction
	gv_int m_penatrate_level;
	gv_int m_backed_penatrate_level;
	gv_int m_facing;
	gv_int m_size;
	gv_int m_turning_speed;
	gv_int m_display_adjust_angle;
	gv_uint m_test_tag;
	gv_bool m_collidable;
	gv_bool m_ignore_me;
	gv_int m_speed_modifier;
};
//============================================================================================
//								:
//============================================================================================

struct gv_cell_info
{
public:
	gv_uint cell_type;
	gv_bool can_move_on;
	gv_int speed_modifier;
	gv_int hp_generation_modifier;
	gv_uint can_block_bullet : 1;
	gv_uint override_normal : 1;
	gv_vector2i new_normal;
	gv_cell_info()
		: cell_type(0), can_move_on(true), speed_modifier(1000),
		  hp_generation_modifier(0), can_block_bullet(0)
	{
		override_normal = 0;
		new_normal = 0;
	}

	gv_cell_info(gv_uint _cell_type, gv_bool _can_move_on, gv_int _speed_modifier,
				 gv_int _hp_generation_modifier, gv_uint _can_block_bullet)
		: cell_type(_cell_type), can_move_on(_can_move_on),
		  speed_modifier(_speed_modifier),
		  hp_generation_modifier(_hp_generation_modifier),
		  can_block_bullet(_can_block_bullet)
	{
		override_normal = 0;
		new_normal = 0;
	}
};

class gv_cell_configs : public gvt_singleton< gv_cell_configs >
{
public:
	gvt_array< gv_cell_info > cell_infos;
};

inline bool gv_less_than_i(gv_int k1, gv_int k2)
{
	// OOS warning!!
	// should not be equal,otherwise sort result will be not deterministic in
	// different sort implementation(ms stl vs gcc stl) ;
	GV_ASSERT(k1 != k2);
	return k1 < k2;
}

inline bool gv_less_than_i2(gv_int k1, gv_int k2, gv_int s1, gv_int s2)
{
	if (k1 == k2)
		return gv_less_than_i(s1, s2);
	return k1 < k2;
}

inline bool gv_less_than_i3(gv_int j1, gv_int j2, gv_int k1, gv_int k2,
							gv_int s1, gv_int s2)
{
	if (j1 == j2)
		return gv_less_than_i2(k1, k2, s1, s2);
	return j1 < j2;
}

class gv_cell_comparator
{
public:
	gv_cell_comparator(gv_int dim, gv_int move_dir)
	{
		m_dim = dim;
		m_move_dir = move_dir;
	};
	bool operator()(gv_cell_2d* c1, gv_cell_2d* c2)
	{
		gv_int k1 = c1->m_rect.p[m_move_dir][m_dim];
		gv_int k2 = c2->m_rect.p[m_move_dir][m_dim];
		gv_int dim2 = 1 - m_dim;
		gv_int s1 = c1->m_rect.p[m_move_dir][dim2];
		gv_int s2 = c2->m_rect.p[m_move_dir][dim2];
		if (m_move_dir == 0)
		{
			return gv_less_than_i2(k1, k2, s1, s2);
		}
		else
		{
			return gv_less_than_i2(k2, k1, s1, s2);
		}
	}
	gv_int m_dim;
	gv_int m_move_dir;
};

class gv_actor_comparator
{
public:
	gv_actor_comparator(gv_int dim, gv_int move_dir)
	{
		m_dim = dim;
		m_move_dir = move_dir;
	};
	bool operator()(gv_actor_2d* a1, gv_actor_2d* a2)
	{
		const gv_recti& r1 = a1->get_world_aabb();
		const gv_recti& r2 = a2->get_world_aabb();
		gv_int k1 = r1.p[m_move_dir][m_dim];
		gv_int k2 = r2.p[m_move_dir][m_dim];
		gv_int dim2 = 1 - m_dim;
		gv_int s1 = r1.p[m_move_dir][dim2];
		gv_int s2 = r2.p[m_move_dir][dim2];
		gv_int t1 = (gv_int)a1->get_name().get_postfix();
		gv_int t2 = (gv_int)a2->get_name().get_postfix();

		if (m_move_dir == 0)
		{
			return gv_less_than_i3(k1, k2, s1, s2, t1, t2);
		}
		else
		{
			return gv_less_than_i3(k2, k1, s1, s2, t1, t2);
		}
	}
	gv_int m_dim;
	gv_int m_move_dir;
};

//============================================================================================
//								:
//============================================================================================
class gv_world_2d : public gv_world, public gv_2d_helper
{
public:
	GVM_DCL_CLASS(gv_world_2d, gv_world)
	gv_world_2d();
	~gv_world_2d();
	//-------------------------events---------------------
	virtual void init(const gv_vector2i& grid_width_height,
					  const gv_vector2i& cell_size = gv_vector2i(1000, 1000),
					  const gv_string& map_data = "", gv_bool use_token = false);
	virtual void init_map_size(const gv_vector2i& grid_width_height,
							   const gv_vector2i& cell_size = gv_vector2i(1000,
																		  1000));
	virtual void init_map_data(const gv_string& s, bool use_token);
	virtual bool tick(gv_float dt);
	virtual bool move_actor(gv_actor_2d* pactor, const gv_vector2i& new_pos,
							gv_int new_rot);
	virtual bool teleport_actor(gv_actor_2d* entity, const gv_vector2i& pos,
								gv_int new_rot, bool check_overlap = true);
	virtual bool move_entity(gv_entity* entity, gv_vector3 new_pos,
							 gv_euler new_rot);
	virtual bool teleport_entity(gv_entity* entity, gv_vector3 pos_3d,
								 gv_euler rot, bool check_overlap = true);
	virtual gv_float get_height(gv_float x, gv_float z);

	gv_uint get_unique_tag();
	gv_vector2i get_cell_coordinate(const gv_vector2i& vector);
	gv_vector2i get_cell_position(const gv_vector2i& vector);
	gv_cell_2d* get_cell(const gv_vector2i& cell_coordinate);
	gv_cell_2d* get_cell(int x, int y)
	{
		return get_cell(gv_vector2i(x, y));
	};
	gv_cell_2d* get_cell_world(const gv_vector2i& cp)
	{
		gv_vector2i cor = get_cell_coordinate(cp);
		gv_cell_2d* cell = get_cell(cor);
		return cell;
	}
	gv_int collect_cells(const gv_recti& rect, gv_cell_2d::cache& result);
	gv_int collect_actors(const gv_recti& rect, gv_actor_2d::cache& result);
	gv_int collect_cells_sorted(const gv_recti& rect, gv_cell_2d::cache& result,
								gv_int dim, gv_int move_dir);
	void enable_debug_radar(gv_bool b = true)
	{
		m_debug_radar_enabled = b;
	};
	void set_current_debug_actor(gv_int idx)
	{
		m_debug_actor = idx;
	};
	gv_recti get_map_rect();
	gv_recti get_map_grid_rect();
	gv_vector2i get_map_size() const
	{
		return m_map_size;
	}
	//-------------------------------------------------------------------------------debug
	//draw functions
	gv_int get_world_frame() const
	{
		return m_current_frame_count;
	}
	void set_world_frame(gv_int f)
	{
		m_current_frame_count = f;
	}

	void debug_draw_3d_grid(const gv_recti& r = gv_recti(), bool mirror = false);
	void debug_draw_rect(const gv_recti& r, const gv_color& c,
						 bool mirror = false);
	void debug_draw_line2d(gv_vector2i start, const gv_vector2i end,
						   const gv_color& c, bool mirror = false);
	void debug_draw_cell(gv_cell_2d& cell, bool mirror = false);
	void debug_draw_unit_bound(class gv_actor_2d* unit, bool mirror = false);

protected:
	gv_vector2i debug_radar_world_to_window(const gv_vector2i& w_v);
	gv_vector2i debug_radar_window_to_world(const gv_vector2i& w_v);
	void draw_in_debug_radar(gv_actor_2d& actor);
	void draw_in_debug_radar(gv_cell_2d& cell);
	gv_int draw_in_debug_radar();

	gv_recti debug_radar_world_to_window(const gv_recti& r);
	gv_recti debug_radar_window_to_world(const gv_recti& r);
	virtual gv_color get_cell_color(gv_cell_2d&);
	virtual void set_collision_flag(gv_cell_2d& c);
	virtual gv_int get_height(const gv_vector2i& vector);

protected:
	gvt_array< gvt_array< gv_cell_2d > > m_cells;
	gv_vector2i m_cell_size; // size of each cell
	gv_vector2i m_map_size;  // size of the map in cells;
	gv_atomic_count m_tag;
	gv_bool m_debug_radar_enabled;
	gv_uint m_debug_actor;
	gv_vector2i m_debug_radar_camera_pos;
	gv_int m_debug_radar_camera_zoom;
	gv_vector2i m_debug_radar_display_window_size;
	gv_int m_current_frame_count;
};

class gv_line_actor_cell_collector
{
public:
	gv_line_actor_cell_collector(gv_world_2d* map)
	{
		m_map = map;
		tag = map->get_unique_tag();
	}
	bool collect(const gv_vector2i& v)
	{
		gv_cell_2d* pcell = m_map->get_cell_world(v);
		if (!pcell)
			return true;
		if (pcell->m_test_tag == tag)
			return true;
		pcell->m_test_tag = tag;
		cells.push_back(pcell);
		return true;
	};
	gv_uint tag;
	gvt_ptr< gv_world_2d > m_map;
	gv_cell_2d::cache cells;
};
}