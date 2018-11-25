namespace gv
{
gv_skeletal::gv_skeletal()
{
	GVM_SET_CLASS(gv_skeletal);
	m_root_bone = 0;
	m_root_tm.set_identity();
	m_ignore_root_motion = false;
	m_ignore_hierarchy = 1;
}
gv_skeletal::~gv_skeletal()
{
}
void gv_skeletal::update_world_matrix_one_bone(gv_int index)
{
	gv_bone* pbone = &this->m_bones[index];
	if (!pbone->m_world_matrix_is_dirty)
		return;
	if (m_ignore_root_motion && pbone->m_hierachy_depth <= m_ignore_hierarchy)
	{
		pbone->m_tm.set_identity();
	}
	else if (pbone->m_hierachy_depth)
	{
		this->update_world_matrix_one_bone(pbone->m_parent_idx);
		pbone->m_tm = pbone->m_local_tm * m_bones[pbone->m_parent_idx].m_tm;
	}
	else
	{
		pbone->m_tm = pbone->m_local_tm * m_root_tm;
	}
	pbone->m_world_matrix_is_dirty = false;
	return;
};

void gv_skeletal::update_world_matrix()
{
	for (int i = 0; i < m_bones.size(); i++)
	{
		gv_bone& bone = m_bones[i];
		bone.m_world_matrix_is_dirty = true;
	}
	for (int i = 0; i < m_bones.size(); i++)
	{
		this->update_world_matrix_one_bone(i);
	}
};

gv_skeletal* gv_skeletal::clone(gv_sandbox* sandbox)
{
	return sandbox->clone_object< gv_skeletal >(this);
};

gv_short gv_skeletal::find_bone_index(const gv_id& name)
{
	for (int i = 0; i < m_bones.size(); i++)
		if (m_bones[i].m_name == name)
			return (gv_short)i;
	return -1;
}

gv_bone* gv_skeletal::find_bone(const gv_id& name)
{
	for (int i = 0; i < m_bones.size(); i++)
	{
		if (m_bones[i].m_name == name)
			return &m_bones[i];
	}
	return NULL;
}

gv_bone* gv_skeletal::find_bone(const char* name)
{
	for (int i = 0; i < m_bones.size(); i++)
	{
		if (m_bones[i].m_name == name)
			return &m_bones[i];
	}
	return NULL;
}

bool gv_skeletal::set_bone_local_rotation_trans(const gv_id& name,
												const gv_quat& q,
												const gv_vector3& pos,
												const gv_vector3& scale)
{
	gv_bone* pbone = find_bone(name);
	if (pbone)
	{
		gv_matrix44 mat_s;
		mat_s.set_identity();
		mat_s.set_scale(scale);
		gv_math::convert(pbone->m_local_tm, q);
		pbone->m_local_tm = mat_s * pbone->m_local_tm;
		gv_vector3 local_pivot = pbone->m_pivot;
		if (pbone->m_parent_idx != -1)
		{
			local_pivot -= m_bones[pbone->m_parent_idx].m_pivot;
		}
		pbone->m_local_tm.set_trans(pos + local_pivot);
		pbone->m_local_offset = pos;
		pbone->m_local_rotation = q;
		pbone->m_local_scale = scale;
		pbone->m_world_matrix_is_dirty = true;
		return true;
	}
	else
	{
		// GVM_WARNING("can't find matching bones in skeletal :" << name);
	}
	return false;
}

void gv_skeletal::update_bone_parent_with_name()
{
	m_root_bone = -1;
	for (int i = 0; i < m_bones.size(); i++)
	{
		gv_bone& bone = m_bones[i];
		bone.m_parent_idx = find_bone_index(bone.m_parent_id);
		if (bone.m_parent_idx == -1)
		{
			// GV_ASSERT(m_root_bone==-1);
			m_root_bone = (gv_short)i;
			bone.m_local_tm = bone.m_tm;
		}
		else
		{
			bone.m_local_tm =
				bone.m_tm * m_bones[bone.m_parent_idx].m_tm.get_inverse();
		}
	}
}
gv_int gv_skeletal::get_nb_bone()
{
	return m_bones.size();
}
gv_bone* gv_skeletal::get_bone(gv_int i)
{
	return &m_bones[i];
}
void gv_skeletal::set_nb_bone(gv_int nb_bone)
{
	m_bones.resize(nb_bone);
}

void gv_skeletal::update_bone_hierachy_depth(gv_bone* pbone)
{
	if (!pbone)
		return;
	gv_bone* pbone_p = find_bone(pbone->m_parent_id);
	if (!pbone_p)
	{
		pbone->m_hierachy_depth = 0;
	}
	else
	{
		update_bone_hierachy_depth(pbone_p);
		pbone->m_hierachy_depth = pbone_p->m_hierachy_depth + 1;
	}
	return;
}

void gv_skeletal::update_bone_hierachy(bool sort)
{
	for (int i = 0; i < m_bones.size(); i++)
	{
		if (!m_bones[i].m_hierachy_depth)
			update_bone_hierachy_depth(&m_bones[i]);
	}
	if (sort)
		m_bones.sort();
}

GVM_IMP_STRUCT(gv_bone)
GVM_VAR(gv_id, m_name)
GVM_VAR(gv_id, m_parent_id)
GVM_VAR(gv_short, m_parent_idx)
GVM_VAR(gv_matrix44, m_matrix_model_to_bone)
GVM_VAR(gv_matrix44, m_local_tm)
GVM_VAR(gv_matrix44, m_tm)
GVM_VAR(gv_quat, m_local_rotation)
GVM_VAR(gv_vector3, m_local_offset)
GVM_VAR(gv_vector3, m_local_scale)
GVM_VAR(gv_bool, m_world_matrix_is_dirty)
GVM_VAR(gv_int, m_hierachy_depth)
GVM_VAR(gv_uint, m_index)
GVM_VAR(gv_vector3, m_pivot)
GVM_END_STRUCT

GVM_IMP_CLASS(gv_skeletal, gv_object)
GVM_VAR(gv_matrix44, m_root_tm)
GVM_VAR(gvt_array< gv_bone >, m_bones)
GVM_END_CLASS
}
