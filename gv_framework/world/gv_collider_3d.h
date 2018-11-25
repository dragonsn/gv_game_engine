#pragma once
namespace gv
{
class gv_actor_3d;
class gv_collider;

enum gve_collider_3d_shape
{
	e_shape_aabb,
	e_shape_sphere,
	e_shape_cylinder,
	e_shape_circle_planar,
	e_shape_convex,
	e_shape_line_2d,
};

struct gv_collide_result : public gv_pooled_struct_256
{
	gv_collide_result()
	{
		GVM_ZERO_ME;
		time = gv_math_fixed::one();
	}
	gv_vector3i contact;
	gv_vector3i normal;
	gv_vector3i moved;
	gv_int time;
	gv_bool blocked;
	// gvt_ptr< gv_collider> src;
	gvt_ptr< gv_collider > target;
};

class gv_grid_link_node : public gv_refable
{
public:
	gv_grid_link_node(){};
	gvt_ptr< gv_world_grid > m_owner;
	gvt_array_cached< gv_obj_node, 16 > m_obj_nodes;
	gvt_array_cached< gv_cell_3d*, 16 > m_cells;
};

class gv_collider : public gv_component
{
	friend class gv_actor_3d;
	friend class gv_world_3d;

protected:
	typedef gvt_array_cached< gv_collider*, 256 > cache;

	GVM_DCL_CLASS(gv_collider, gv_component);

	gv_collider();

	~gv_collider();

	virtual void on_detach();

	virtual void on_attach();

	virtual void debug_draw();

	virtual void attach_grid(gv_world_grid* grid);

	virtual void update_grids();

	virtual void remove_from_all_grids();

	virtual gv_bool sweep(gv_collider* target, gv_vector3i speed,
						  gv_collide_result& result, gv_int skin);

	virtual gv_bool sweep_one_dim(gv_collider* target, gv_vector3i speed,
								  gv_collide_result& result, gv_int dim,
								  gv_int skin);

	virtual gv_bool overlap(gv_collider* target, gv_vector3i new_pos,
							gv_int skin = 0);

	virtual gv_boxi get_world_aabb();

	gv_grid_link_node* get_link_node(gv_world_grid* grid);

	gv_actor_3d* get_actor()
	{
		return m_owner_actor;
	}

	virtual gv_bool can_penetrate(gv_collider* target);

	virtual gv_bool can_collide(gv_collider* target);

private:
	virtual void add_to_grid(gv_world_grid* grid);
	virtual void remove_from_grid(gv_world_grid* grid);
	virtual gv_bool is_test_target(gv_collider* target);

protected:
	virtual void add_to_cells(gv_grid_link_node* node, gv_cell_3d::cache& cells);
	virtual void update_world_position();
	//----------------------------------------------------------------
	// bitwise flags for collision test
	gv_uint m_collider_tags;
	gv_uint m_target_test_tags;
	gv_uint m_target_block_tags;
	gv_int m_runtime_test_tag;
	// to simplify our collision , our collider don't have rotation .
	gv_vector3i m_local_position;
	gv_vector3i m_world_position;
	gv_boxi m_local_aabb;
	gv_vector3i m_smoothed_history_speed;
	gve_collider_3d_shape m_collision_shape;
	gvt_ptr< gv_actor_3d > m_owner_actor;
	gvt_array< gvt_ref_ptr< gv_grid_link_node > > m_link_nodes;
};
}