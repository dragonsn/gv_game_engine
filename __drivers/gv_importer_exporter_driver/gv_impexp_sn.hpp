#include "gv_framework_private.h"

#define GVM_WITH_SN 1
#if GVM_WITH_SN

#include "gv_importer_exporter.h"
#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
#include "..\renderer\gv_skeletal_mesh.h"
#include "../animation/gv_animation.h"
#include "../renderer/gv_model.h"

#include ".\sn\sn_objects.h"
using namespace sn;
namespace gv
{

class gv_impexp_sn : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_sn, gv_importer_exporter);
	gv_impexp_sn()
	{
		link_class(gv_impexp_sn::static_class());
	};
	~gv_impexp_sn(){};
	gv_model* m_result_model;
	SN_File* m_sn_file;
	gv_sandbox* m_result_sandbox;
	gvt_array< gv_object* > m_result_objects;

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id("sn"));
	};

	gv_class_info* get_import_target_class()
	{
		return gv_model::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_obj__do_import, 0)
		gv_string ext = gv_global::fm->get_extension(*file_name);
		;
		ext.to_upper();
		if (ext != ".SN")
			return false;

		m_result_model = gvt_cast< gv_model >(target);
		if (!m_result_model)
			return false;
		m_result_sandbox = m_result_model->get_sandbox();
		SN_File file(*file_name);
		file.Read_To_Memory();
		m_sn_file = &file;
		m_result_objects.resize(file.g_Object_Number);
		m_result_objects = NULL;
		for (int i = 0; i < file.g_Object_Number; i++)
		{
			SN_Object* pobj = file.g_pObject_Table[i];
			switch (pobj->type)
			{
			case OBJ_MESH_OBJECT:
				m_result_objects[i] = do_import_mesh((SN_Mesh_Object*)pobj);
				break;

			case OBJ_MAT_OBJECT:
				m_result_objects[i] = do_import_material((SN_Material*)pobj);
				break;

			case OBJ_K_ANI_OBJECT:
				m_result_objects[i] = do_import_animation((SN_Kine_Ani_Object*)pobj);
				break;

			case OBJ_NULL_OBJECT:
			case OBJ_BASIS_OBJECT:
			case OBJ_PARTICLE_SYSTEM_OBJECT:
				break;

			case SN_SKIN_OBJECT:
				m_result_objects[i] = do_import_skeletal_mesh((SN_Skin*)pobj);
				break;

			case OBJ_CAM_OBJECT:
				break;

			default:
				break;
			};
		}
		return true;
	};

	gv_colorf to_gv(SN_Color& color)
	{
		gv_colorf c;
		c.set(color.r, color.g, color.b, color.a);
		return c;
	}

	gv_matrix43 to_gv(SN_Matrix3& mat)
	{
		return *((gv_matrix43*)&mat);
	}

	gv_object* do_import_material(SN_Material* pmat)
	{
		gv_material* my_mat = m_result_sandbox->create_object< gv_material >(
			gv_id(pmat->name), m_result_model);
		my_mat->m_ambient_color = to_gv(pmat->Ambient_Color);
		my_mat->m_diffuse_color = to_gv(pmat->Diffuse_Color);
		my_mat->m_specular_color = to_gv(pmat->Specular_Color);
		my_mat->add_texture(pmat->FileName);
		m_result_model->add_material(my_mat);
		return my_mat;
	}

	gv_object* do_import_skeletal_mesh(SN_Skin* pmesh)
	{
		gv_skeletal_mesh* my_mesh =
			m_result_sandbox->create_object< gv_skeletal_mesh >(gv_id(pmesh->name),
																m_result_model);
		my_mesh->m_t_pose_mesh =
			gvt_cast< gv_static_mesh >(m_result_objects[pmesh->Target]);
		my_mesh->create_skeletal();
		gv_skeletal* my_skeletal = my_mesh->get_skeletal();
		m_result_model->add_skeletal_mesh(my_mesh);
		my_mesh->get_skeletal()->set_nb_bone(pmesh->iNumBones);
		my_mesh->m_t_pose_mesh->create_skin_vertex();
		gv_vertex_buffer* pvb = my_mesh->m_t_pose_mesh->get_vb();
		for (int i = 0; i < pmesh->iNumBones; i++)
		{
			SN_Bone* pbone = pmesh->pBones + i;
			gv_bone* my_bone = my_mesh->get_skeletal()->get_bone(i);
			SN_Object* pso = this->m_sn_file->g_pObject_Table[pbone->iBoneID];
			my_bone->m_matrix_model_to_bone = to_gv(pbone->mat_ll);
			my_bone->m_name = (char*)pso->name;
			my_bone->m_user_data = (gv_int_ptr)pso;
			;
			// m_index
			GV_ASSERT(pso->father != -1);
			my_bone->m_parent_id =
				(char*)this->m_sn_file->g_pObject_Table[pso->father];
			my_bone->m_parent_idx = -1;
			my_bone->m_tm = to_gv(pso->tm);
			for (int j = 0; j < pbone->iNumberVertex; j++)
			{
				pvb->set_blending_info(pbone->pVertexIndex[j], pbone->iBoneID,
									   pbone->pWeight[j]);
			}
		}
		my_skeletal->update_bone_hierachy();
		my_skeletal->update_bone_parent_with_name();
		for (int i = 0; i < pvb->m_raw_pos.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (pvb->m_raw_blend_index[i][j] != -1)
				{
					int boneid = pvb->m_raw_blend_index[i][j];
					SN_Object* pso = this->m_sn_file->g_pObject_Table[boneid];
					gv_bone* pbone = my_skeletal->find_bone(pso->name);
					GV_ASSERT(pbone);
					pvb->m_raw_blend_index[i][j] = my_skeletal->m_bones.index(pbone);
				}
			}
		}
		//================================================================
		return NULL;
	}

	gv_object* do_import_mesh(SN_Mesh_Object* pmesh)
	{
		gv_static_mesh* my_mesh = m_result_sandbox->create_object< gv_static_mesh >(
			gv_id(pmesh->name), m_result_model);
		my_mesh->create_vb_ib();
		SN_Mesh* psn_mesh = pmesh->pmesh;
		my_mesh->get_vb()->m_raw_pos.resize(psn_mesh->V_Number);
		my_mesh->get_vb()->m_raw_normal.resize(psn_mesh->V_Number);
		my_mesh->get_vb()->m_raw_texcoord0.resize(psn_mesh->V_Number);
		for (int i = 0; i < my_mesh->get_nb_vertex(); i++)
		{
			SN_Vertex* pvertex = psn_mesh->p_Vertex + i;
			my_mesh->get_vb()->m_raw_pos[i].set(pvertex->x, pvertex->y, pvertex->z);
			my_mesh->get_vb()->m_raw_texcoord0[i].set(pvertex->u, pvertex->v);
			my_mesh->get_vb()->m_raw_normal[i].set(pvertex->nx, pvertex->ny,
												   pvertex->nz);
		}

		tdstTriangle* pt = psn_mesh->p_Tri;
		if (!(psn_mesh->Mesh_Flags & SN_STRIP))
		{

			my_mesh->get_ib()->set_nb_index(psn_mesh->T_Number * 3);

			gv_int* pidx = my_mesh->get_ib()->m_raw_index_buffer.begin();
			for (int i = 0; i < psn_mesh->T_Number; i++, pt++)
			{
				*pidx++ = pt->v[0];
				*pidx++ = pt->v[1];
				*pidx++ = pt->v[2];
			}
			pt = psn_mesh->p_Tri;
			int count = 0;
			if (psn_mesh->Mesh_Flags & SN_MESH_MULTI_TEXTURE)
			{
				while (pt < psn_mesh->p_Tri + psn_mesh->T_Number)
				{

					int mid = pt->Mtl_ID & 0xffff;
					int mcount = (pt->Mtl_ID >> 16) & 0xffff;
					my_mesh->add_segment(count * 3,
										 gvt_cast< gv_material >(m_result_objects[mid]),
										 mcount * 3);
					count += mcount;
					pt += mcount;
				}
			}
		}
		else
		{
			GV_ASSERT(0);
		}
		my_mesh->rebuild_bounding_volumn();
		m_result_model->add_static_mesh(my_mesh);
		return my_mesh;
	}

	gv_object* do_import_animation(SN_Kine_Ani_Object* pani)
	{
		/*
    gv_ani_set * my_ani_set=NULL;
    gv_ani_sequence * my_ani_seq=NULL;
    if (!m_result_model->get_nb_animation())
    {
             my_ani_set=m_result_sandbox->create_object<gv_ani_set>
    (m_result_model);
             m_result_model->m_skeletal_ani_set.push_back(my_ani_set);
             my_ani_seq=pmodel->get_sandbox()->create_object<gv_ani_sequence>(gv_id
    ((char*)lAnimStack->GetName()), my_ani_set );
             my_ani_set->add_sequence(pani_seq);
    }
    my_ani_set=m_result_model->get_animation(0);
    my_ani_set->add_sequence();
    */
		return NULL;
	}

	void get_export_extension(gvt_array< gv_id >& result){

	};
	gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		return false;
	};
};

GVM_IMP_CLASS(gv_impexp_sn, gv_importer_exporter)
GVM_END_CLASS
}

#endif