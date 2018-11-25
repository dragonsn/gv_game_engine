#pragma once
namespace gv
{
class gv_component;
class gv_bone;
class gv_com_skeletal_mesh;
class gv_com_static_mesh;

class gv_entity : public gv_object
{
	friend class gv_world;

public:
	GVM_DCL_CLASS(gv_entity, gv_object);
	gv_entity();
	~gv_entity();

	virtual bool post_load();

	//----------------------world-----------------------------------
	inline void set_world(gv_world* world)
	{
		this->m_world = world;
	}

	inline const gv_world* get_world() const
	{
		return this->m_world;
	}

	inline gv_world* get_world()
	{
		return this->m_world;
	}
	//----------------------pos & rot-------------------------------
	const gv_vector3& get_position() const;
	const gv_euler& get_rotation() const;
	const gv_vector3& get_scale() const;
	const gv_box& get_world_aabb() const;
	const gv_box& get_local_aabb() const;
	const gv_sphere& get_world_bounding_sphere() const;
	const gv_sphere& get_local_bounding_sphere() const;
	const gv_matrix43& get_tm() const;
	const gv_matrix43& get_inv_tm() const;

	void set_local_aabb(const gv_box& box);
	//----------------------component manager------------------------
	typedef gvt_array_cached< gv_component*, 256 > com_query_result;

	gv_component* get_component(gv_class_info* pcls);

	const gv_component* get_component(gv_class_info* pcls) const;

	template < class T >
	inline T* get_component()
	{
		return get_component(gvt_identity< T >());
	}

	template < class T >
	inline const T* get_component() const
	{
		return get_component(gvt_identity< T >());
	}

	template < class T >
	bool detach_component()
	{
		return this->detach_component(T::static_class());
	}

	bool detach_component(const gv_id& id);

	bool detach_component(gv_component* com);

	bool detach_component(gv_class_info* pcls);

	template < class T >
	T* add_component()
	{
		return gvt_cast< T >(this->add_component(T::static_class()));
	}

	gv_component* add_component(const gv_id& com_class_id);

	gv_component* add_component(gv_class_info* pcls);

	void add_component(gv_component* com);

	gv_component* add_component(const gv_string_tmp& description);

	void query_components(com_query_result& result);

	template < class T, class component_container >
	void query_components(component_container& result)
	{
		com_query_result all;
		query_components(all);
		for (int i = 0; i < all.size(); i++)
		{
			if (all[i]->is_a(T::static_class()))
				result.push_back(gvt_cast< T >(all[i]));
		}
	};

	void detach_all_component();

	void notify_change_to_components();

	void reset_bounding_from_components();

	void get_components_info(gv_stream_cached& sc);

	//----------------------------------------------------------------------------------------
	bool tick(gv_float dt);

	virtual gv_entity* clone(gv_sandbox* sandbox = NULL);

	void set_ready_to_delete(gv_bool v)
	{
		m_ready_to_delete = v;
	}

	bool is_ready_to_delete()
	{
		return m_ready_to_delete;
	}

	bool is_good_to_delete();
	//----------------------------------------------------------------------------------------
	bool is_template() const; //

	bool is_static() const; // can' t move

	bool is_selected() const;

	bool is_hided() const;

	bool is_tickable() const
	{
		return m_tickable_count > 0;
	}

	bool is_renderable() const
	{
		return m_renderable_count > 0;
	}

	bool is_ignore_pause() const
	{
		return m_ignore_pause;
	};

	bool is_ignore_line_check() const
	{
		return m_ignore_line_check;
	};

	void set_hided(gv_bool enable);

	void set_selected(gv_bool enable);

	void set_static(gv_bool enable);

	void set_ignore_pause(gv_bool enable)
	{
		m_ignore_pause = enable;
	};

	void set_ignore_line_check(gv_bool enable)
	{
		m_ignore_line_check = enable;
	};

public:
	// we should limit the  component that can update the
	void set_position(const gv_vector3& pos);

	void set_rotation(const gv_euler& euler);

	void set_scale(const gv_vector3& scale);

	void set_tm(const gv_matrix44& mat);

	void set_tm(const gv_matrix43& mat);

	void update_matrix();

	void update_from_matrix();

	void synchronize_from_components(); // this is the only place components can
										// write back positions.

	void debug_draw_axis();

	void debug_draw_aabb(gv_color color = gv_color::RED());

	inline gv_uint get_synced_frame() const
	{
		return m_synced_frame;
	}

	inline gv_float get_total_life_time() const
	{
		return m_total_life_time;
	};

	inline gv_float get_current_life_time() const
	{
		return m_current_life_time;
	};

	inline void set_total_life_time(gv_float t)
	{
		m_total_life_time = t;
	}

	inline void set_current_life_time(gv_float t)
	{
		m_current_life_time = t;
	}

	gv_float get_unified_life_time() const;

	void kill_self()
	{
		m_flag_is_pending_for_kill = 1;
	}

	bool is_killed()
	{
		return m_flag_is_pending_for_kill;
	}

	bool set_enabled(gv_bool e);
	//user defined visual flag.
	gv_long get_user_flag() const
	{
		return m_user_flag;
	};

	void set_user_flag(gv_long flag)
	{
		m_user_flag = flag;
	};

public:
	GVM_DCL_FUNC(scp_rebuild_world_matrix);
	GVM_DCL_FUNC(scp_rebuild_bound);

protected:
	bool tick_components(gv_float dt);

	bool detach_component_from_list(gvt_ref_ptr< gv_component >& list,
									gv_component* com);

	bool detach_whole_component_list(gvt_ref_ptr< gv_component >& list);

	void query_components_list(gvt_ref_ptr< gv_component >& list,
							   com_query_result& result);

	void delete_component(gv_component* com);

	// short cut for get components...
	template < class T >
	inline T* get_component(gvt_identity< T >)
	{
		return gvt_cast< T >(get_component(T::static_class()));
	}

	template < class T >
	inline const T* get_component(gvt_identity< T >) const
	{
		return gvt_cast< T >(get_component(T::static_class()));
	}

	inline gv_com_static_mesh* get_component(gvt_identity< gv_com_static_mesh >)
	{
		return (gv_com_static_mesh*)(m_com_static_mesh.ptr());
	};

	inline gv_com_skeletal_mesh*
		get_component(gvt_identity< gv_com_skeletal_mesh >)
	{
		return (gv_com_skeletal_mesh*)(m_com_skeletal_mesh.ptr());
	};

private:
	typedef gvt_array< gvt_ref_ptr< gv_component > > component_list;

	//----------------------------------------------------------------------------------------
	component_list m_components;
	gvt_ref_ptr< gv_component >
		m_com_static_mesh; // main model of the entity as static	mesh
	gvt_ref_ptr< gv_component >
		m_com_skeletal_mesh; // main model of the entity as skeletal	mesh.

	gvt_ptr< gv_entity > m_template; // template of the entity
	gvt_ptr< gv_world > m_world;	 // the world hold the entity;
	gv_bool m_is_static;
	gv_bool m_is_hided;
	gv_bool m_ignore_pause;
	gv_bool m_ignore_line_check;
	gv_bool m_ready_to_delete;
	struct
	{
		gv_uint m_flag_tm_is_dirty : 1;
		gv_uint m_flag_is_selected : 1;
		gv_uint m_flag_is_pending_for_kill : 1;
	};
	gv_matrix43 m_tm;
	gv_matrix43 m_inv_tm;
	gv_vector3 m_pos;
	gv_euler m_rotation;
	gv_vector3 m_scale;
	gv_box m_local_aabb;
	gv_box m_world_aabb;
	gv_sphere m_local_bounding_sphere;
	gv_sphere m_world_bounding_sphere;
	gv_short m_renderable_count;
	gv_short m_tickable_count;
	gv_uint m_synced_frame;
	gv_float m_total_life_time;
	gv_float m_current_life_time;
	gv_long  m_user_flag;

public:
	void* m_runtime_user_data;
};
}