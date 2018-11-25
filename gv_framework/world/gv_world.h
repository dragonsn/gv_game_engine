#pragma once
namespace gv
{
static const gv_uint gvc_invalid_collsion_tag = 0xffffffff;

struct gv_world_light_info
{
	gv_world_light_info();
	gv_vector4 m_sun_dir;
	gv_color m_ambient_color;
	gv_color m_specular_color;
};

class gv_world : public gv_event_processor
{
public:
	typedef gvt_array_cached< const gv_entity*, 2048 > collect_result;

	GVM_DCL_CLASS(gv_world, gv_event_processor)
	gv_world();
	~gv_world();
	//-------------------------init---------------------
	virtual void build(){};
	virtual void init(gv_int layer, const gv_vector2i& grid_width_height,
					  const gv_box& world_size, void* external_data = NULL);
	//-------------------------overridden---------------
	virtual bool tick(gv_float dt);
	virtual gv_int on_event(gv_object_event* pevent);
	virtual bool do_synchronization();
	virtual bool refresh_from_editor(gv_float dt)
	{
		return true;
	};
	virtual bool post_load();
	//-----------------------------------------------------
	virtual bool add_entity(gv_entity* entity);
	virtual void delete_entity(gv_entity* entity);
	virtual void delete_all_entity();
	virtual bool move_entity(gv_entity* entity, gv_vector3 new_pos,
							 gv_euler new_rot);
	virtual bool teleport_entity(gv_entity* entity, gv_vector3 pos, gv_euler rot,
								 bool check_overlap = true);
	//-----------------------------------------------------
	virtual void collect_entity(const gv_line_segment& line,
								collect_result& result);
	virtual void collect_entity(const gv_box& box, collect_result& result);
	virtual void collect_entity(const gv_rect& rect, collect_result& result);
	virtual void collect_entity(const gv_frustum& frustum,
								collect_result& result);
	//-----------------------------------------------------
	virtual bool line_check(const gv_line_segment& line, gv_vector3& pos,
							gv_entity*& entity);
	//-----------------------------------------------------
	virtual gv_int get_nb_entity() const;
	virtual gv_entity* get_entity(gv_int idx);
	virtual const gv_entity* get_entity(gv_int idx) const;
	virtual gv_entity* find_entity(const gv_id& name);
	virtual const gv_box& get_world_box() const;
	virtual void set_main_actor(gv_entity* entity)
	{
		m_main_entity = entity;
	};
	virtual gv_entity* get_main_actor() const
	{
		return m_main_entity;
	}
	virtual gv_float get_ground_height(const gv_vector3& pos);
	virtual gv_float get_ground_height_2d(const gv_vector2& pos_2d)
	{
		return 0;
	};

	//-----------------------------------------------------
	inline bool is_hided() const
	{
		return m_hided;
	}
	inline void set_hided(gv_bool b)
	{
		m_hided = b;
	}
	inline const gv_world_light_info& get_light_info() const
	{
		return m_light_info;
	};
	inline void set_light_info(const gv_world_light_info& info)
	{
		m_light_info = info;
	}
	static gve_event_channel static_channel()
	{
		return gve_event_channel_world;
	}
	//-----------------------------------------------------
	GVM_DCL_FUNC(scp_reinit_world);

protected:
	gv_box m_world_box;
	gv_vector2i m_grid_size;
	gvt_ref_ptr< gv_entity > m_main_entity;
	gv_bool m_hided;
	gv_world_light_info m_light_info;
	gv_text m_height_map;
	gv_text m_blue_print_map;
	gv_vector3 m_scale;
	gvt_array< gvt_ref_ptr< gv_entity > > m_entities;
};
}