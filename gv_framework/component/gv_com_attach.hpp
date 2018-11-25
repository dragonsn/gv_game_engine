namespace gv
{
gv_com_attach::gv_com_attach()
{
	GVM_SET_CLASS(gv_com_attach);
	m_local_tm_is_dirty = true;
	m_local_scale.set(1, 1, 1);
	m_local_offest.set(0, 0, 0);
	m_local_rotation.set(0, 0, 0);
	update_local_tm();
};
gv_com_attach::~gv_com_attach(){

};

void gv_com_attach::on_attach()
{
	this->set_local_scale(get_entity()->get_scale());
	this->set_local_offset(get_entity()->get_position());
	this->set_local_rotation(get_entity()->get_rotation());
};

bool gv_com_attach::sync_to_entity(gv_entity* entity)
{
	if (!this->m_attached_entity)
	{
		return true;
	}
	if (this->m_attached_entity->is_deleted())
	{
		m_attached_entity = NULL;
		return true;
	}
	gv_matrix43 bone_tm = m_attached_entity->get_tm();
	gv_com_skeletal_mesh* ps =
		m_attached_entity->get_component< gv_com_skeletal_mesh >();
	if (ps)
	{
		gv_skeletal* skeletal;
		if (entity->get_synced_frame() > m_attached_entity->get_synced_frame())
			skeletal = ps->get_writable_skeletal();
		else
			skeletal = ps->get_render_skeletal();
		gv_bone* pbone = skeletal->find_bone(this->m_attached_bone);
		if (pbone)
		{
			gv_matrix44 m = pbone->m_tm * bone_tm;
			;
			m.copy_to(bone_tm);
		}
	}
	update_local_tm();
	gv_matrix43 mat;
	mat = m_local_tm * bone_tm;
	entity->set_tm(mat);
	// debug;
	// entity->debug_draw_axis();
	return true;
};
const gv_matrix43& gv_com_attach::get_local_tm()
{
	return m_local_tm;
}
const gv_vector3& gv_com_attach::get_local_offset() const
{
	return m_local_offest;
};
const gv_euler& gv_com_attach::get_local_rotation() const
{
	return m_local_rotation;
}
gv_vector3 gv_com_attach::get_local_scale() const
{
	return m_local_scale;
}
void gv_com_attach::set_local_offset(const gv_vector3& pos)
{
	m_local_tm_is_dirty = true;
	m_local_offest = pos;
};
void gv_com_attach::set_local_scale(const gv_vector3& scale)
{
	m_local_tm_is_dirty = true;
	m_local_scale = scale;
};
void gv_com_attach::set_local_rotation(const gv_euler& rot)
{
	m_local_tm_is_dirty = true;
	m_local_rotation = rot;
};

void gv_com_attach::update_local_tm()
{
	if (!this->m_local_tm_is_dirty)
		return;
	m_local_tm.set_identity();
	gv_matrix43 mat_rot;
	gv_matrix43 mat_s;
	mat_s.set_identity();
	mat_s.set_scale(m_local_scale);
	gv_math::convert(mat_rot, this->m_local_rotation);
	m_local_tm *= mat_s;
	m_local_tm *= mat_rot;
	m_local_tm.set_trans(this->m_local_offest);
	m_local_tm_is_dirty = false;
}

GVM_IMP_CLASS(gv_com_attach, gv_component)
GVM_VAR(gv_vector3, m_local_offest)
GVM_VAR(gv_vector3, m_local_scale)
GVM_VAR(gv_euler, m_local_rotation)
GVM_VAR(gv_matrix43, m_local_tm)
GVM_VAR(gv_bool, m_local_tm_is_dirty)
GVM_VAR(gvt_ref_ptr< gv_entity >, m_attached_entity)
GVM_VAR(gv_id, m_attached_bone)
GVM_END_CLASS
}
