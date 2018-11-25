#pragma once
namespace gv
{
//============================================================================================
// coodinate: => height y axis
//			  => horizontal x axis  (towards left)
//			  => vertical z axis (towards you)
//============================================================================================
class gv_world_3d : public gv_world
{
public:
	GVM_DCL_CLASS(gv_world_3d, gv_world)
	gv_world_3d();
	~gv_world_3d();

	virtual void init_map_size(const gv_boxi& world_box,
							   const gv_vector2i& cell_size);

	virtual gv_float get_height(gv_float x, gv_float z);

	virtual bool move_entity(gv_entity* entity, gv_vector3 new_pos,
							 gv_euler new_rot);

	virtual bool teleport_entity(gv_entity* entity, gv_vector3 pos_3d,
								 gv_euler rot, bool check_overlap = true);

	//------------------------collision detection & physics functions ------

	virtual void add_collider(gv_collider* c);

	virtual gv_int move_actor(gv_actor_3d* pactor, const gv_vector3i& new_pos);

	virtual gv_int rotate_actor(gv_actor_3d* pactor, const gv_euleri& new_rot);

	virtual bool teleport_actor(gv_actor_3d* pactor, const gv_vector3i& pos,
								bool check_overlap = true);

	virtual gv_entity*
	create_actor(gv_class_info* actor_class, const gv_string_tmp& name,
				 const gv_string_tmp& mesh, const gv_string_tmp& ani,
				 gv_int random_range, const gv_boxi& aabb, const gv_vector3i& pos,
				 const gv_euleri& new_rot, gv_bool test_overlap = false,
				 gv_int max_retry = 20);

	virtual bool place_actor(gv_actor_3d* actor, gv_int random_range,
							 const gv_vector3i& pos, const gv_euleri& new_rot,
							 gv_bool test_overlap = false, gv_int max_retry = 20);

	virtual gv_world_grid*
	get_grid(gv_int index = 0); // a map might have different layers of grid

	//------------------------ functions ------------------------------
	gv_physics_fixed_setting* get_physics();

	gv_entity* get_root_entity();

	gv_int get_unique_runtime_tag();

	gv_int get_world_frame() const
	{
		return m_current_frame_count;
	}

	void set_world_frame(gv_int f)
	{
		m_current_frame_count = f;
	}

	gv_int get_logic_frame_time()
	{
		return m_logical_frame_time;
	};

	void set_logic_frame_time(gv_int i)
	{
		m_logical_frame_time = i;
	};

	gv_int get_total_logic_time()
	{
		return m_total_frame_time;
	}

	gv_random_uint& get_random_for_gameplay()
	{
		return m_random_for_gameplay;
	}

	gv_random_uint& get_random_for_none_gameplay()
	{
		return m_random_for_none_gameplay;
	}

protected:
	virtual gv_int get_height(const gv_vector2i& vector)
	{
		return 0;
	};

	virtual gv_int collect_colliders(const gv_boxi& b, gv_collider::cache& r);
	// hide not used interfaces
	virtual bool tick(gv_float dt);

	virtual gv_int tick_actor_physics(gv_actor_3d*, gv_int remain_time);

	virtual gv_int move_actor_in_one_dimension(gv_actor_3d* pactor,
											   const gv_vector3i& new_pos,
											   gv_int dim);

	virtual gv_int move_actor_in_three_dimension(gv_actor_3d* pactor,
												 const gv_vector3i& new_pos);

public:
	gv_vector3i m_axis_freedom;

protected:
	gv_atomic_count m_runtime_tag;

	gv_int m_current_frame_count;
	gv_int m_total_frame_time;
	gv_int m_logical_frame_time;
	gvt_ptr< gv_world_grid > m_base_grid;
	gvt_ptr< gv_physics_fixed_setting > m_physics;
	gv_random_uint m_random_for_gameplay;
	gv_random_uint m_random_for_none_gameplay;
};

struct gv_map_3d_init_text
{
	GVM_WITH_STATIC_CLS;
	gv_vector3i m_size;
	gvt_array< gv_int_string_pair > m_type_pairs;
	gv_text m_map;
};
}