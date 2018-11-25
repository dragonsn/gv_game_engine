#pragma once
namespace gv
{
class gv_com_attach : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_attach, gv_component);
	gv_com_attach();
	~gv_com_attach();
	virtual bool sync_to_entity(gv_entity*);
	virtual bool is_renderable()
	{
		return false;
	}
	virtual bool is_local_tm_enabled()
	{
		return true;
	}
	virtual const gv_matrix43& get_local_tm();
	virtual const gv_vector3& get_local_offset() const;
	virtual const gv_euler& get_local_rotation() const;
	virtual gv_vector3 get_local_scale() const;
	virtual void set_local_offset(const gv_vector3& pos);
	virtual void set_local_scale(const gv_vector3& pos);
	virtual void set_local_rotation(const gv_euler& rot);

public:
	inline void set_attached_entity(gv_entity* entity)
	{
		m_attached_entity = entity;
	};
	inline void set_attached_bone_id(const gv_id& id)
	{
		m_attached_bone = id;
	}
	virtual void on_attach();

protected:
	void update_local_tm();

protected:
	gv_vector3 m_local_offest;
	gv_vector3 m_local_scale;
	gv_euler m_local_rotation;
	gv_matrix43 m_local_tm;
	gv_bool m_local_tm_is_dirty;
	gvt_ref_ptr< gv_entity > m_attached_entity;
	gv_id m_attached_bone;
};
}