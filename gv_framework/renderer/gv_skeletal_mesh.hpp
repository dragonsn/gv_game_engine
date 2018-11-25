namespace gv
{
gv_skeletal_mesh::gv_skeletal_mesh()
{
	GVM_SET_CLASS(gv_skeletal_mesh);
	m_nb_bone_after_map = 0;
}

void gv_skeletal_mesh::create_skeletal()
{
	this->m_skeletal = get_sandbox()->create_object< gv_skeletal >(this);
};

gv_box gv_skeletal_mesh::get_local_aabb()
{
	gv_box b;
	if (m_t_pose_mesh && m_skeletal)
	{
		b = m_t_pose_mesh->get_bbox();
		gv_matrix43 m;
		gvt_copy(m, m_skeletal->m_root_tm);
		b = b.transform(m);
		return b;
	}
	return b;
};

struct test_bone
{
	test_bone()
	{
		GVM_ZERO_ME;
	}
	float weight;
	int old_index;
	bool operator<(const test_bone& b) const
	{
		return weight > b.weight;
	}
};

void gv_skeletal_mesh::optimize_bones(gv_int target_bone_nb)
{

	if (m_skeletal->m_bones.size() <= target_bone_nb)
		return;
	if (m_bone_mapping.size())
		return;
	gvt_array< test_bone > bones;
	bones.resize(m_skeletal->m_bones.size());
	gv_vertex_buffer* pvb = this->m_t_pose_mesh->get_vb();
	if (!pvb)
		return;
	if (!pvb->m_raw_blend_index.size())
		return;
	for (int i = 0; i < pvb->m_raw_pos.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (pvb->m_raw_blend_index[i][j] == -1)
				continue;
			if (gvt_is_almost_zero(pvb->m_raw_blend_weight[i][j]))
				continue;
			test_bone& b = bones[pvb->m_raw_blend_index[i][j]];
			b.weight += pvb->m_raw_blend_weight[i][j];
			b.old_index = pvb->m_raw_blend_index[i][j];
		}
	}
	bones.sort();
	m_bone_mapping.resize(bones.size());
	m_bone_mapping = -1;
	for (int i = 0; i < bones.size(); i++)
	{
		m_bone_mapping[bones[i].old_index] = (gv_short)i;
	}
	if (bones[target_bone_nb].weight != 0)
	{
		// we need reduce bones
		for (int i = target_bone_nb; i < bones.size(); i++)
		{
			bones[i].weight = 0;
		}
	}

	for (int i = 0; i < pvb->m_raw_pos.size(); i++)
	{
		int first_index = pvb->m_raw_blend_index[i][0];
		int nb_bone = 0;
		for (int j = 0; j < 4; j++)
		{
			int old_index = pvb->m_raw_blend_index[i][j];
			if (old_index == -1)
				continue;
			int new_index = m_bone_mapping[old_index];
			test_bone& b = bones[new_index];
			if (gvt_is_almost_zero(b.weight))
			{
				pvb->m_raw_blend_index[i][j] = -1;
				pvb->m_raw_blend_weight[i][j] = 0;
			}
			else
			{
				pvb->m_raw_blend_index[i][j] = new_index;
				nb_bone++;
			}
		}
		if (!nb_bone)
		{
			// keep one bone
			while (first_index > 0 &&
				   gvt_is_almost_zero(bones[m_bone_mapping[first_index]].weight))
			{
				first_index = m_skeletal->m_bones[first_index].m_parent_idx;
			}
			pvb->m_raw_blend_index[i][0] = m_bone_mapping[first_index];
			pvb->m_raw_blend_weight[i][0] = 1.0f;
		}
	}

	m_nb_bone_after_map = 0;
	m_bone_inv_mapping.resize(bones.size());
	m_bone_inv_mapping = -1;
	for (int i = 0; i < bones.size(); i++)
	{
		if (gvt_is_almost_zero(bones[i].weight))
		{
			m_bone_mapping[bones[i].old_index] = -1;
		}
		else
			m_nb_bone_after_map++;
		m_bone_inv_mapping[i] = (gv_short)bones[i].old_index;
	}

	pvb->sort_blending_weight();
	pvb->normalize_blending_weight();
};

GVM_IMP_CLASS(gv_skeletal_mesh, gv_resource)
GVM_VAR(gvt_ref_ptr< gv_skeletal >, m_skeletal)
GVM_VAR(gvt_ref_ptr< gv_static_mesh >, m_t_pose_mesh)
GVM_VAR(gv_int, m_nb_bone_after_map)
GVM_VAR(gvt_array< gv_short >, m_bone_mapping)
GVM_VAR(gvt_array< gv_short >, m_bone_inv_mapping)
GVM_END_CLASS;
};