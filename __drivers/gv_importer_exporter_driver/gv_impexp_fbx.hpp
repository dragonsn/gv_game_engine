#include "gv_framework_private.h"

#if GV_WITH_FBX
#pragma warning(disable : 4244)
#include "gv_importer_exporter.h"
#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
#include "..\renderer\gv_skeletal_mesh.h"
#include "..\animation\gv_animation.h"
#include "..\renderer\gv_model.h"

//#define KFBX_DLLINFO
#include <fbxsdk.h>

#if defined(GV_USE_DLL)

#if defined(_DEBUG)
#if GV_64
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x86/debug/libfbxsdk-md.lib")
#endif
#else
#if GV_64
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x64/release/libfbxsdk-md.lib")
#else
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x86/release/libfbxsdk-md.lib")
#endif
#endif

#elif defined(_DEBUG)
#if GV_64
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x64/debug/libfbxsdk-mt.lib")
#else
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x86/debug/libfbxsdk-mt.lib")
#endif

#else // release
#if GV_64
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x64/release/libfbxsdk-mt.lib")
#else
#pragma comment(lib, "/fbxsdk/2016.1.1/lib/vs2015/x86/release/libfbxsdk-mt.lib")
#endif
#endif

#include <stdio.h>

namespace gv_fbx
{
#pragma warning(disable : 4740)
#pragma warning(disable : 4748)

using namespace gv;

static gv_string_tmp the_xml;
static gv_string_tmp the_path;
FbxTime gPeriod, gStart, gStop, gCurrentTime;
FbxScene* gScene;
FbxManager* gSdkManager;
gv_float gFps;

#pragma GV_REMINDER( \
	"[MEMO] keep it a a main reference when the key frame not working !!")
static bool use_key_frame = true;

void gv_print_to_string(const char* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	static char s_temp_buffer[4096];
	gv_get_var_args(s_temp_buffer, gvt_array_length(s_temp_buffer), format,
					ArgPtr);

	the_xml << s_temp_buffer;
}

gv_model* the_model = NULL;
gv_vector3 fbx_ani_scale;
static gvt_array< gv_material* > s_polygon_material_map;

void copy(gv_matrix44& mat, const FbxMatrix& fbx_matrix)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat.m[i][j] = (gv_float)fbx_matrix[i][j];
}

void copy(gv_vector2& v, const FbxVector2& pValue)
{
	v.set((gv_float)pValue[0], (gv_float)pValue[1]);
}

void copy(gv_vector3& v, const FbxVector4& pValue)
{
	v.set((gv_float)pValue[0], (gv_float)pValue[1], (gv_float)pValue[2]);
}

void copy(gv_quat& q, const FbxVector4& pValue)
{
	q.set((gv_float)pValue[0], (gv_float)pValue[1], (gv_float)pValue[2],
		  (gv_float)pValue[3]);
}

#define printf gv_print_to_string
#include "./fbx/Common.h"
#include "./fbx/DisplayCommon.h"
#include "./fbx/DisplayHierarchy.h"
#include "./fbx/DisplayAnimation.h"
#include "./fbx/DisplayMarker.h"
#include "./fbx/DisplaySkeleton.h"

#include "./fbx/DisplayMesh.h"
#include "./fbx/DisplayNurb.h"
#include "./fbx/DisplayPatch.h"

#include "./fbx/DisplayLodGroup.h"
#include "./fbx/DisplayCamera.h"
#include "./fbx/DisplayLight.h"
#include "./fbx/DisplayGlobalSettings.h"
#include "./fbx/DisplayPose.h"

#include "./fbx/DisplayPivotsAndLimits.h"
#include "./fbx/DisplayUserProperties.h"
#include "./fbx/DisplayGenericInfo.h"

#include "./fbx/DisplayTexture.h"
#include "./fbx/DisplayMaterial.h"
#include "./fbx/DisplayLink.h"
#include "./fbx/DisplayShape.h"

#include "./fbx/Common.hpp"
#include "./fbx/DisplayCommon.hpp"
#include "./fbx/DisplayHierarchy.hpp"
#include "./fbx/DisplayAnimation.hpp"

//#include "./fbx/DisplayMarker.hpp"
//#include "./fbx/DisplaySkeleton.hpp"
#include "./fbx/DisplayMesh.hpp"
//#include "./fbx/DisplayNurb.hpp"
//#include "./fbx/DisplayPatch.hpp"
//#include "./fbx/DisplayLodGroup.hpp"
//#include "./fbx/DisplayCamera.hpp"
//#include "./fbx/DisplayLight.hpp"
#include "./fbx/DisplayGlobalSettings.hpp"
//#include "./fbx/DisplayPose.hpp"
//#include "./fbx/DisplayPivotsAndLimits.hpp"
//#include "./fbx/DisplayUserProperties.hpp"
//#include "./fbx/DisplayGenericInfo.hpp"
#include "./fbx/DisplayTexture.hpp"
#include "./fbx/DisplayMaterial.hpp"
//#include "./fbx/DisplayLink.hpp"
//#include "./fbx/DisplayShape.hpp"
#include "./fbx/main.hpp"
};

// Global variables.

namespace gv
{
using namespace gv_fbx;
class gv_impexp_fbx_config : public gv_object
{
public:
	GVM_DCL_CLASS(gv_impexp_fbx_config, gv_object);
	gv_impexp_fbx_config()
	{
		scale = 1.0f;
		offset = 0.f;
		rotation.set(0, 0, 0);
		override_root = true;
		link_class(gv_impexp_fbx_config::static_class());
		override_name = false;
		only_animation = false;
	}
	gv_vector3 scale;
	gv_vector3 offset;
	gv_euler rotation;
	gv_bool only_animation;
	gv_bool override_root;
	gv_bool override_name;
	gv_string new_name;
};

class gv_impexp_fbx : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_fbx, gv_importer_exporter);
	gv_impexp_fbx()
	{
		link_class(gv_impexp_fbx::static_class());
	};
	~gv_impexp_fbx(){};
	gv_impexp_fbx_config* get_config()
	{
		return gvt_cast< gv_impexp_fbx_config >(m_configure);
	}
	gv_string m_file_name;
	gvt_hash_map< FbxNode*, gvt_ptr< gv_object > > m_node_map;

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_fbx);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_model::static_class();
	}
	gv_class_info* get_import_config_class()
	{
		return gv_impexp_fbx_config::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_fbx__do_import, 0)

		m_result_model = gvt_cast< gv_model >(target);
		if (!m_result_model)
			return false;
		the_model = m_result_model;
		bool animation_only = false;
		if (target->get_owner() &&
			target->get_owner()->is_a(gv_ani_set::static_class()))
		{
			animation_only = true;
		}
		gv_impexp_fbx_config* cfg = this->get_config();
		if (cfg->only_animation)
		{
			animation_only = true;
		}
		if (cfg && !cfg->override_root)
		{
			gv_fbx::fbx_ani_scale = cfg->scale;
		}
		else
		{
			fbx_ani_scale = 1.0f;
		}

		gv_string ext = gv_global::fm->get_extension(*file_name);
		;
		ext.to_upper();
		if (ext != ".FBX")
			return false;
		// m_file_name=*file_name;
		the_path =
			m_result_model->get_sandbox()->get_file_manager()->get_parent_path(
				*file_name);
		the_path =
			m_result_model->get_sandbox()->get_logical_resource_path(the_path);
		the_path << "/";
		exp_main(*file_name);
		//
		// if (!this->init_fbx_sdk())
		//{
		//	on_exit();
		//	return false;
		//};
		// if (!this->import_fbx_file() )
		//{
		//	on_exit();
		//	return false;
		//}
		// on_exit();
		// ExportScene(gScene, gCurrentTime);
		//
		if (this->get_config())
		{
			gv_matrix44 mat;
			gv_matrix44 s;
			gv_matrix44 r;
			s.set_scale(get_config()->scale);
			gv_math::convert(r, get_config()->rotation);
			mat = r * s;
			mat.set_trans(get_config()->offset);
			for (int i = 0; i < the_model->get_nb_skeletal_mesh(); i++)
			{
				gv_skeletal_mesh* sm = the_model->get_skeletal_mesh(i);
				if (get_config()->override_root)
					sm->get_skeletal()->m_root_tm = mat;
				else
					sm->get_skeletal()->m_root_tm = sm->get_skeletal()->m_root_tm * mat;
			}
		}
		// if (this->get_out)
		if (animation_only && the_model && the_model->get_nb_animation())
		{
			gv_ani_set* ani = the_model->get_animation(0);
			gv_ani_set* ani_old = gvt_cast< gv_ani_set >(target->get_owner());
			if (ani_old)
			{
				gvt_ref_ptr< gv_ani_sequence > seq = ani->get_sequence(0);
				ani->remove_sequence(seq);
				ani_old->add_sequence(seq);
				ani_old->get_sandbox()->delete_object_tree(target);
			}
			else
			{
				ani->set_owner(target->get_owner());
				target->get_sandbox()->delete_object_tree(target);
			}
		}
		the_xml.clear();
		the_model = NULL;
		m_node_map.clear();
		return true;
	};
	gv_model* m_result_model;
};

GVM_IMP_CLASS(gv_impexp_fbx, gv_importer_exporter)
GVM_END_CLASS

GVM_IMP_CLASS(gv_impexp_fbx_config, gv_object)
GVM_VAR(gv_vector3, scale)
GVM_VAR(gv_vector3, offset)
GVM_VAR(gv_euler, rotation)
GVM_VAR(gv_bool, override_root)
GVM_VAR(gv_bool, override_name)
GVM_VAR(gv_string, new_name)
GVM_VAR(gv_bool, only_animation)
GVM_VAR_TOOLTIP(
	"only import animation, set parent as one of the animation set!!!");
GVM_END_CLASS
}
#endif
