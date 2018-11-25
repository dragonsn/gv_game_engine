#pragma once
namespace gv
{

enum gve_vertex_format
{
#define GVM_DCL_VERTEX_FORMAT_ENUM
#include "gv_vertex_format.h"
};

/*
inline const char* gv_string_from_vertex_format(gve_vertex_format f)
{
	switch (f)
	{
#define GVM_DCL_VERTEX_FORMAT_GET_STRING
#include "gv_vertex_format.h"
	}
	GV_ASSERT(0);
	return "Unknown";
}

template < class type_of_string >
inline gve_vertex_format gv_vertex_format_from_string(const type_of_string& s)
{
#define GVM_DCL_VERTEX_FORMAT_FROM_STRING
#include "gv_vertex_format.h"
	GV_ASSERT(0);
	return gve_vertex_format_MAX;
}

*/
enum gve_vertex_usage
{
#define GVM_DCL_VERTEX_USAGE_ENUM
#include "gv_vertex_usages.h"
};

//inline const char* gv_string_from_vertex_usage(gve_vertex_usage f)
//{
//	switch (f)
//	{
//#define GVM_DCL_VERTEX_USAGE_GET_STRING
//#include "gv_vertex_usages.h"
//	}
//	GV_ASSERT(0);
//	return "Unknown";
//}
//
//template < class type_of_string >
//inline gve_vertex_usage gv_vertex_usage_from_string(const type_of_string& s)
//{
//#define GVM_DCL_VERTEX_USAGE_FROM_STRING
//#include "gv_vertex_usages.h"
//	GV_ASSERT(0);
//	return gve_vertex_usage_MAX;
//}

enum gve_vertex_buffer_format
{
	e_vtx_with_pos = 1,
	e_vtx_with_normal = 2,
	e_vtx_with_binormal = 4,
	e_vtx_with_tangent = 8,
	e_vtx_with_texcoord0 = 16,
	e_vtx_with_texcoord1 = 32,
	e_vtx_with_texcoord2 = 64,
	e_vtx_with_texcoord3 = 128,
	e_vtx_with_texcoord4 = 256,
	e_vtx_with_texcoord5 = 512,
	e_vtx_with_texcoord6 = 1024,
	e_vtx_with_texcoord7 = 2048,
	e_vtx_with_blend_index = 4096,
	e_vtx_with_blend_weight = 8192,
	e_vtx_with_color = 16384,
};

#define GVM_SET_VERTEX_FORMAT_FLAG(usage) \
	if (m_raw_##usage.size())             \
		format_flag |= e_vtx_with_##usage;

inline gve_vertex_buffer_format
gv_get_vertex_flag_from_usage(gve_vertex_usage usage, gv_int usage_index)
{
	switch (usage)
	{
	case gve_vertex_usage_POSITION:
		return e_vtx_with_pos;
	case gve_vertex_usage_BLENDWEIGHT:
		return e_vtx_with_blend_weight;
	case gve_vertex_usage_BLENDINDICE:
		return e_vtx_with_blend_index;
	case gve_vertex_usage_NORMAL:
		return e_vtx_with_normal;
	case gve_vertex_usage_TEXCOORD:
		return (gve_vertex_buffer_format)(e_vtx_with_texcoord0 + usage_index);
	case gve_vertex_usage_TANGENT:
		return e_vtx_with_tangent;
	case gve_vertex_usage_BINORMAL:
		return e_vtx_with_binormal;
	case gve_vertex_usage_COLOR:
		return e_vtx_with_color;
	default:
		GV_ASSERT(0);
		return e_vtx_with_color;
	};
}

class gv_vertex_buffer : public gv_resource
{
public:
	GVM_DCL_CLASS(gv_vertex_buffer, gv_resource);
	gv_vertex_buffer()
	{
		link_class(gv_vertex_buffer::static_class());
		m_is_static_vertex_buffer = true;
		m_precache_tag = 1;
	}
	~gv_vertex_buffer(){};
	inline gv_uint get_biggest_vertex_format()
	{
		gv_uint format_flag = 0;
		GVM_SET_VERTEX_FORMAT_FLAG(pos);
		GVM_SET_VERTEX_FORMAT_FLAG(normal);
		GVM_SET_VERTEX_FORMAT_FLAG(binormal);
		GVM_SET_VERTEX_FORMAT_FLAG(tangent);
		GVM_SET_VERTEX_FORMAT_FLAG(blend_index);
		GVM_SET_VERTEX_FORMAT_FLAG(blend_weight);
		GVM_SET_VERTEX_FORMAT_FLAG(color);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord0);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord1);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord2);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord3);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord4);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord5);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord6);
		GVM_SET_VERTEX_FORMAT_FLAG(texcoord7);
		return format_flag;
	};
	void set_blending_info(int vidx, gv_int bone_idx, gv_float weight)
	{
		if (weight <= 0)
			return;
		if (vidx >= m_raw_pos.size())
			return;
		if (!m_raw_blend_index.size())
			return;
		if (!m_raw_blend_weight.size())
			return;
		for (int i = 0; i < 4; i++)
		{
			if (m_raw_blend_index[vidx][i] == -1)
			{
				m_raw_blend_index[vidx][i] = bone_idx;
				m_raw_blend_weight[vidx][i] = weight;
				return;
			}
		}
		// full , replace the min
		int min_index = 0;
		float min_w = m_raw_blend_weight[vidx][0];
		for (int i = 1; i < 4; i++)
		{
			if (min_w > m_raw_blend_weight[vidx][i])
			{
				min_w = m_raw_blend_weight[vidx][i];
				min_index = i;
			}
		}
		if (weight < min_w)
			return;
		m_raw_blend_weight[vidx][min_index] = weight;
		m_raw_blend_index[vidx][min_index] = bone_idx;
		return;
	}

	void normalize_blending_weight()
	{
		for (int i = 0; i < m_raw_blend_index.size(); i++)
		{
			gv_float weight = 0;
			for (int j = 0; j < 4; j++)
			{
				if (m_raw_blend_index[i][j] == -1)
					break;
				weight += m_raw_blend_weight[i][j];
			}
			if (gvt_value< gv_float >::is_almost_equal(weight, 1.0f))
				continue;
			for (int j = 0; j < 4; j++)
			{
				if (m_raw_blend_index[i][j] == -1)
					break;
				m_raw_blend_weight[i][j] /= weight;
			}
		}
	}

	struct weighted_vtx
	{
		gv_float weight;
		gv_int index;
		bool operator<(const weighted_vtx& b) const
		{
			return weight > b.weight;
		}
	};

	void sort_blending_weight()
	{
		for (int i = 0; i < m_raw_blend_index.size(); i++)
		{
			gvt_array_cached< weighted_vtx, 10 > a;
			a.resize(4);
			for (int j = 0; j < 4; j++)
			{
				a[j].weight = m_raw_blend_weight[i][j];
				a[j].index = m_raw_blend_index[i][j];
			}
			a.sort();
			for (int j = 0; j < 4; j++)
			{
				m_raw_blend_weight[i][j] = a[j].weight;
				m_raw_blend_index[i][j] = a[j].index;
			}
		}
	}

	void set_dirty()
	{
		m_precache_tag++;
	};

	gvt_array< gv_vector2 >& get_nth_tex_coord(int i)
	{
		GV_ASSERT(i >= 0 && i < 7);
		gvt_array< gv_vector2 >* p = &m_raw_texcoord0;
		return p[i];
	}

	void clear()
	{
		m_raw_pos.clear();
		m_raw_normal.clear();
		m_raw_binormal.clear();
		m_raw_tangent.clear();
		m_raw_texcoord0.clear();
		m_raw_texcoord1.clear();
		m_raw_texcoord2.clear();
		m_raw_texcoord3.clear();
		m_raw_texcoord4.clear();
		m_raw_texcoord5.clear();
		m_raw_texcoord6.clear();
		m_raw_texcoord7.clear();
		m_raw_blend_index.clear();
		m_raw_blend_weight.clear();
		m_raw_color.clear();
	}

public:
	gv_bool m_is_static_vertex_buffer;
	gv_uint m_precache_tag;
	gvt_array< gv_vector3 > m_raw_pos;
	gvt_array< gv_vector3 > m_raw_normal;
	gvt_array< gv_vector3 > m_raw_binormal;
	gvt_array< gv_vector3 > m_raw_tangent;
	gvt_array< gv_vector2 > m_raw_texcoord0;
	gvt_array< gv_vector2 > m_raw_texcoord1;
	gvt_array< gv_vector2 > m_raw_texcoord2;
	gvt_array< gv_vector2 > m_raw_texcoord3;
	gvt_array< gv_vector2 > m_raw_texcoord4;
	gvt_array< gv_vector2 > m_raw_texcoord5;
	gvt_array< gv_vector2 > m_raw_texcoord6;
	gvt_array< gv_vector2 > m_raw_texcoord7;
	gvt_array< gv_vector4i > m_raw_blend_index;
	gvt_array< gv_vector4 > m_raw_blend_weight;
	gvt_array< gv_vector4 > m_raw_color;
};
}
