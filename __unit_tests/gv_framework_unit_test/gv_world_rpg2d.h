#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
inline gv_vector2 to_2d(const gv_vector3& v)
{
	return gv_vector2(v.z, v.x);
}
inline gv_vector3 to_3d(const gv_vector2& v)
{
	return gv_vector3(v.y, 0, v.x);
}
inline gv_rect to_2d(const gv_box& b)
{
	return gv_rect(to_2d(b.min_p), to_2d(b.max_p));
}
inline gv_box to_3d(const gv_rect& r)
{
	return gv_box(to_3d(r.min_p), to_3d(r.max_p));
}
//============================================================================================
//								:
//============================================================================================
class gv_com_rpg2d_actor : public gv_com_tasklet
{
	friend class gv_world_rpg2d;

public:
	GVM_DCL_CLASS(gv_com_rpg2d_actor, gv_com_tasklet);
	gv_com_rpg2d_actor();
	~gv_com_rpg2d_actor();
	bool tick(gv_float dt);
	bool sync_from_entity(const gv_entity* entity);
	bool sync_to_entity(gv_entity* entity);
	bool is_tickable();
	virtual gv_color get_state_color();

public:
	gv_bool m_auto_move;
	gv_float m_auto_move_speed;
	gv_int m_auto_reinit;
	gv_int m_auto_reinit_count;
	gv_float m_adjust_angle;

protected:
	gv_rect m_local_aabb;
	gv_rect m_world_aabb;
	gv_vector2 m_location;
	gv_vector2 m_delta_pos;
	gv_vector2 m_delta_init;
	gv_float m_facing;
	gv_uint m_test_tag;
};

//============================================================================================
//								:
//============================================================================================
class gv_rpg2d_cell
{
	friend class gv_world_rpg2d;

public:
	gv_rpg2d_cell();
	~gv_rpg2d_cell();

protected:
	void add_actor(gv_com_rpg2d_actor* a);
	void remove_actor(gv_com_rpg2d_actor* a);

public:
	typedef gvt_array_cached< gv_com_rpg2d_actor*, 256 > actor_cache;
	gv_rect m_rect;
	actor_cache m_actor_cache;
	bool m_can_move_on;
	gv_color m_color;
	gv_uint m_test_tag;
	gv_uint m_type;
};

//============================================================================================
//								:
//============================================================================================
class gv_world_rpg2d : public gv_world
{
public:
	typedef gvt_array_cached< gv_rpg2d_cell*, 1024 > cell_cache;

public:
	GVM_DCL_CLASS(gv_world_rpg2d, gv_world)
	gv_world_rpg2d();
	~gv_world_rpg2d();
	//-------------------------events---------------------
	virtual void init(gv_int layer, const gv_vector2i& grid_width_height,
					  const gv_box& world_size, void* data);
	virtual void register_events();
	virtual void unregister_events();
	virtual bool tick(gv_float dt);
	virtual gv_int on_event(gv_object_event* pevent);
	virtual bool add_entity(gv_entity* entity);
	virtual void delete_entity(gv_entity* entity);
	virtual bool move_entity(gv_entity* entity, gv_vector3 new_pos,
							 gv_euler new_rot);
	virtual bool move_actor(gv_com_rpg2d_actor* pactor, gv_vector2& delta_pos,
							gv_float new_rot, float& time_of_impact);
	virtual bool teleport_entity(gv_entity* entity, gv_vector3 pos_3d,
								 gv_euler rot, bool check_overlap = true);
	virtual void collect_entity(const gv_line_segment& line,
								collect_result& result);
	virtual void collect_entity(const gv_box& box, collect_result& result);
	virtual void collect_entity(const gv_rect& rect, collect_result& result);
	virtual void collect_entity(const gv_frustum& frustum,
								collect_result& result) const;
	gv_uint get_unique_tag();
	gv_vector2i get_cell_coordinate(const gv_vector2& vector);
	gv_int collect_cells(const gv_rect& rect, cell_cache& result);
	inline gvt_vector2< gv_ushort > compress_pos(const gv_vector2& pos)
	{
		gv_vector2 v = pos;
		v.x = v.x / (m_map_width * m_cell_size_x) * 65535.f;
		v.x = gvt_clamp(v.x, 0.f, 65535.f);
		v.y = v.y / (m_map_height * m_cell_size_y) * 65535.f;
		v.y = gvt_clamp(v.y, 0.f, 65535.f);
		gvt_vector2< gv_ushort > vs;
		vs.x = (gv_ushort)v.x;
		vs.y = (gv_ushort)v.y;
		return vs;
	};
	inline gv_vector2 uncompress_pos(const gvt_vector2< gv_ushort >& vs)
	{
		gv_vector2 v;
		v.x = (gv_float)vs.x * (m_map_width * m_cell_size_x) / 65535.f;
		v.y = (gv_float)vs.y * (m_map_height * m_cell_size_y) / 65535.f;
		return v;
	};
	void debug_draw_cell(const gv_rpg2d_cell& cell);
	void enable_debug_radar(gv_bool b = true)
	{
		m_debug_radar_enabled = b;
	};
	void set_current_debug_actor(gv_int idx)
	{
		m_debug_actor = idx;
	};

protected:
	gv_vector2 debug_radar_world_to_window(const gv_vector2& w_v);
	gv_vector2 debug_radar_window_to_world(const gv_vector2& w_v);
	void draw_in_debug_radar(gv_com_rpg2d_actor& actor);
	void draw_in_debug_radar(gv_rpg2d_cell& cell);
	gv_int draw_in_debug_radar();
	gv_rect debug_radar_world_to_window(const gv_rect& r)
	{
		return gv_rect(debug_radar_world_to_window(r.min_p),
					   debug_radar_world_to_window(r.max_p));
	}
	gv_rect debug_radar_window_to_world(const gv_rect& r)
	{
		return gv_rect(debug_radar_window_to_world(r.min_p),
					   debug_radar_window_to_world(r.max_p));
	}

protected:
	gvt_array< gvt_array< gv_rpg2d_cell > > m_cells;
	gv_int m_cell_size_x;
	gv_int m_cell_size_y;
	gv_int m_map_width, m_map_height;
	gvt_random< gv_float > m_random;
	gv_atomic_count m_tag;

	//============================================================
	bool m_debug_radar_enabled;
	gv_uint m_debug_actor;
	gv_vector2 m_debug_radar_camera_pos;
	gv_float m_debug_radar_camera_zoom;
	gv_vector2 m_debug_radar_display_window_size;
};


class gv_com_test_ai :public gv_com_tasklet
{
public:
	GVM_DCL_CLASS(gv_com_test_ai, gv_com_tasklet);
	gv_com_test_ai();
	bool tick(float dt);
	bool is_tickable() {
		return true;
	}
	int timer;
};

}