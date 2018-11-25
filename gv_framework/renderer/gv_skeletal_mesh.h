#pragma once
#include "../animation/gv_ani_skeletal.h"
namespace gv
{

class gv_skeletal_mesh : public gv_resource
{
public:
	GVM_DCL_CLASS(gv_skeletal_mesh, gv_resource);
	gv_skeletal_mesh();
	~gv_skeletal_mesh(){};
	gv_static_mesh* get_t_mesh()
	{
		return m_t_pose_mesh;
	}
	gv_skeletal* get_skeletal()
	{
		return m_skeletal;
	}
	void create_skeletal();
	void optimize_bones(gv_int target_bone_nb = 64);
	gv_box get_local_aabb();

public:
	gvt_ref_ptr< gv_static_mesh > m_t_pose_mesh;
	gvt_ref_ptr< gv_skeletal > m_skeletal;
	gv_int m_nb_bone_after_map;
	gvt_array< gv_short > m_bone_mapping; // the mapping is created for optimize the
										  // bone number ,so we can use GPU
										  // skinning.
	gvt_array< gv_short > m_bone_inv_mapping;
};
}