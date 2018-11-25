#pragma once
namespace gv
{
class gv_bone
{
public:
	gv_bone()
	{
		m_local_offset = 0.f;
		m_pivot = 0.f;
		m_tm.set_identity();
		m_local_rotation.set_identity();
		m_matrix_model_to_bone.set_identity();
		m_tm.set_identity();
		m_parent_idx = -1;
		m_world_matrix_is_dirty = false;
		m_local_tm.set_identity();
		m_hierachy_depth = 0;
		m_index = 0;
		m_user_data = -1;
	};
	gv_bone(const gv_bone& bone)
	{
		(*this) = bone;
	}
	gv_bone& operator=(const gv_bone& bone)
	{
		m_name = bone.m_name;
		m_parent_id = bone.m_parent_id;
		m_parent_idx = bone.m_parent_idx;
		m_matrix_model_to_bone = bone.m_matrix_model_to_bone;

		m_local_tm = bone.m_local_tm;
		m_tm = bone.m_tm;
		m_local_rotation = bone.m_local_rotation;
		m_local_offset = bone.m_local_offset;
		m_world_matrix_is_dirty = bone.m_world_matrix_is_dirty;
		m_hierachy_depth = bone.m_hierachy_depth;
		m_index = bone.m_index;
		m_user_data = bone.m_user_data;
		return (*this);
	};

	bool operator<(const gv_bone& bone) const
	{
		if (m_hierachy_depth < bone.m_hierachy_depth)
			return true;
		if (m_hierachy_depth > bone.m_hierachy_depth)
			return false;
		if (m_name < bone.m_name)
			return true;
		return false;
	}

	gv_int m_hierachy_depth;
	gv_int m_index;

	gv_id m_name;
	gv_id m_parent_id;
	gv_short m_parent_idx;
	gv_matrix44 m_matrix_model_to_bone;

	gv_matrix44 m_local_tm;
	gv_matrix44 m_tm;
	gv_quat m_local_rotation;
	gv_vector3 m_local_offset;
	gv_vector3 m_local_scale;
	gv_vector3 m_pivot;
	gv_bool m_world_matrix_is_dirty;
	gv_int_ptr m_user_data;
};

class gv_skeletal : public gv_object
{
public:
	GVM_DCL_CLASS(gv_skeletal, gv_object);
	gv_skeletal();
	~gv_skeletal();
	//============================================================================================
	//								:
	//============================================================================================
	gv_skeletal* clone(gv_sandbox* sandbox);
	void update_world_matrix();
	void update_world_matrix_one_bone(gv_int index);
	gv_short find_bone_index(const gv_id& name);
	gv_bone* find_bone(const gv_id& name);
	gv_bone* find_bone(const char* name);
	bool set_bone_local_rotation_trans(const gv_id& name, const gv_quat& q,
									   const gv_vector3& pos,
									   const gv_vector3& scale);
	void update_bone_parent_with_name();
	gv_int get_nb_bone();
	gv_bone* get_bone(gv_int i);
	void set_nb_bone(gv_int nb_bone);
	void update_bone_hierachy_depth(gv_bone* pbone);
	void update_bone_hierachy(bool sort = true);
	//============================================================================================
	//								:
	//============================================================================================
	gv_short m_root_bone;
	gv_bool m_ignore_root_motion;
	gv_int m_ignore_hierarchy;
	gv_matrix44 m_root_tm;
	gvt_array< gv_bone > m_bones;
};
};