#pragma once
namespace gv
{
class gv_entity;
class gv_component : public gv_object
{
	friend class gv_entity;

public:
	GVM_DCL_CLASS(gv_component, gv_object);
	gv_component();
	~gv_component();

	inline void set_entity(gv_entity* p)
	{
		m_entity = p;
	}
	inline const gv_entity* get_entity() const
	{
		return m_entity;
	}
	virtual gv_component* clone(gv_sandbox* sandbox);
	virtual bool tick(gv_float dt);
	virtual bool sync_to_entity(gv_entity*);
	virtual bool sync_from_entity(const gv_entity*);
	virtual bool is_tickable();
	virtual bool is_safe_to_delete()
	{
		return true;
	}
	virtual gve_event_channel get_channel();
	virtual bool set_resource(gv_resource*);
	virtual gv_resource* get_resource();

	virtual void get_info(gv_int& count, gv_stream_cached& sc)
	{
	}
	template < class type_of_resource >
	inline type_of_resource* get_resource()
	{
		return gvt_cast< type_of_resource >(get_resource());
	}
	virtual bool is_renderable();
	virtual bool is_local_tm_enabled();
	virtual const gv_matrix43& get_local_tm();
	virtual const gv_vector3& get_local_offset() const;
	virtual const gv_euler& get_local_rotation() const;
	virtual gv_vector3 get_local_scale() const;
	virtual void set_local_tm(const gv_matrix43& tm);
	virtual void set_local_offset(const gv_vector3& pos);
	virtual void set_local_scale(const gv_vector3& pos);
	virtual void set_local_rotation(const gv_euler& rot);
	virtual gv_box get_local_bbox();
	virtual gv_sphere get_local_bsphere();
	virtual void on_attach();
	virtual void on_detach();

	inline gv_component* get_next_component()
	{
		return m_next;
	}
	inline void set_next_component(gv_component* p)
	{
		m_next = p;
	}
	inline gv_bool is_enabled()
	{
		return m_enabled;
	}
	inline virtual void set_enabled(gv_bool b)
	{
		m_enabled = b;
	}
	inline void set_user_data(void* p)
	{
		m_user_data = (gv_byte*)p;
	}
	inline void* get_user_data()
	{
		return m_user_data;
	}

protected:
	gvt_ptr< gv_entity > m_entity;
	gvt_ref_ptr< gv_component > m_next;
	gv_bool m_enabled;
	gvt_ptr< gv_byte > m_user_data;
};
}