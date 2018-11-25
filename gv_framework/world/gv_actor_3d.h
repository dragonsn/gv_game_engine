#pragma once
namespace gv
{
class gv_world_3d;
class gv_actor_3d : public gv_com_tasklet
{
	friend class gv_world_3d;

public:
	typedef gvt_array_cached< gv_actor_3d*, 256 > cache;
	GVM_DCL_CLASS(gv_actor_3d, gv_com_tasklet);
	gv_actor_3d();
	~gv_actor_3d();
	//------------------------------------- for overrided---------------
	virtual bool sync_from_entity(const gv_entity* entity);
	virtual bool sync_to_entity(gv_entity* entity);
	virtual bool is_tickable();
	virtual void on_detach();
	virtual void on_attach();

public:
	//------------------------------------------------------------------
	gv_boxi get_world_aabb();
	void set_local_aabb(const gv_boxi& b)
	{
		m_local_aabb = b;
	};
	const gv_boxi& get_local_aabb()
	{
		return m_local_aabb;
	};
	void set_position(const gv_vector3i& pos)
	{
		m_position = pos;
	};
	gv_vector3i get_position()
	{
		return m_position;
	};
	gv_euleri get_rotation() const
	{
		return m_rotation;
	}
	void set_rotation(gv_euleri f)
	{
		m_rotation = f;
	}
	void set_display_adjust_angle(gv_vector3i d)
	{
		m_display_adjust_angle = d;
	};
	void set_velocity(gv_vector3i v)
	{
		m_velocity = v;
	}
	gv_vector3i get_velocity()
	{
		return m_velocity;
	}
	void set_bouncingness(gv_int b)
	{
		m_bouncingness = b;
	}
	gv_int get_bouncingness()
	{
		return m_bouncingness;
	}
	void set_mass(gv_int b)
	{
		m_mass = b;
	}
	gv_int get_mass()
	{
		return m_mass;
	}
	gv_matrix_fixed get_world_matrix();
	void create_base_collider(gve_collider_3d_shape shape = e_shape_aabb);
	void create_colliders_for_empty_box(gv_int thickness, bool with_bottom = true,
										bool with_cap = true,
										bool with_walls = true);
	void add_impulse(gv_vector3i impact);
	void add_impulse_to_control_velocity(gv_vector3i target_velocity,
										 gv_vector3i limit);
	gv_world_3d* get_world();
	virtual void update_grids();

private:
	//------------------------------------------------------------------
	virtual bool tick(gv_float dt); // hide the float delta time inteface
protected:
	virtual gv_bool tick_fixed(gv_int delta_time);
	;

	//------------------------------------------------------------------
	gv_int find_colliders(gv_collider::cache& result);
	void update_colliders_world_position();

	gv_vector3i get_delta_position(gv_int time_fraction);
	void apply_force_and_impulse(gv_int time_fraction, gv_vector3i movement);

	virtual void on_hit(gv_collide_result& result);	// activly
	virtual void on_be_hit(gv_collide_result& result); // passivly

	virtual void remove_from_all_grids();
	virtual void pre_physics();
	virtual void post_physics();
	void normalize_bouncing(); //
protected:
	gv_boxi m_local_aabb;
	gv_vector3i m_scale;
	gv_vector3i m_position;
	gv_euleri m_rotation;

	gv_int m_remain_time;
	gv_vector3i m_impulse; // force we can add to the unit in this frame
	gv_vector3i m_force;   // force we can add to the unit in this frame
	// The short answer is that velocity is the speed with a direction, while
	// speed does not have a direction.Speed is a scalar quantity -- it is the
	// magnitude of the velocity.Speed is measured in units of distance divided by
	// time, e.g.miles per hour, feet per second, meters per second, etc....Nov
	// 23, 2011
	gv_vector3i m_velocity; // new physics related parameter,linear velocity

	gv_int m_mass; // mass.
	gv_int m_bouncingness;
	gv_int m_stickiness;
	gv_int m_slipperiness;

	gv_int m_aero_friction; // aero_friction
	gv_euleri m_turning_speed;
	gv_euleri m_display_adjust_angle;
	gv_bool m_in_grids =false;
	gvt_ptr< gv_world_3d > m_world;
};
}