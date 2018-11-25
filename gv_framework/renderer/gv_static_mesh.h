#pragma once

namespace gv
{

struct gv_mesh_segment
{
	gv_mesh_segment()
	{
		m_is_hidden = false;
	};
	gv_mesh_segment(const gv_mesh_segment& seg)
	{
		(*this) = seg;
	};
	gv_mesh_segment& operator=(const gv_mesh_segment& s)
	{
		m_start_index = s.m_start_index;
		m_index_size = s.m_index_size;
		m_material = s.m_material;
		m_is_hidden = s.m_is_hidden;
		return *this;
	}
	gv_int m_start_index;
	gv_int m_index_size;
	gv_bool m_is_hidden;
	gvt_ref_ptr< gv_material > m_material;
};

class gv_static_mesh : public gv_resource
{
public:
	// only implement indexed triangle list
	GVM_DCL_CLASS(gv_static_mesh, gv_resource);
	gv_static_mesh();
	~gv_static_mesh(){};
	void create_vb_ib();
	void create_vb_ib(gv_int vertex_size, gv_uint stream_decl, gv_int index_size);
	void merge_optimize_vertex();
	void rebuild_normal(bool smooth = true);
	void rebuild_bounding_volumn();
	void rebuild_binormal_tangent();
	void rebuild_normal_smooth();
	void create_planar_tex_mapping();
	void create_sphere_tex_mapping();
	void create_skin_vertex();
	void transform_pos(const gv_matrix44& mat);
	void transform_normal(const gv_matrix44& mat);
	void prepare_vertex_stream(gv_uint stream_decl);
	void copy_uv(gv_int src_index, int tgt_index);
	void clear()
	{
		m_segments.clear();
		m_vertex_buffer = NULL;
		m_index_buffer = NULL;
	};
	gv_int get_nb_vertex();
	gv_int get_nb_normal();
	gv_int get_nb_triangle();
	gv_int get_index(gv_int i);
	gv_int get_nb_index();
	void add_segment(gv_int start_index = 0, gv_material* material = NULL,
					 gv_int size = -1);

	gv_mesh_segment* get_segment(gv_int idx);
	gv_int get_nb_segment();
	void set_nb_segment(gv_int nb);
	const gv_sphere& get_bsphere()
	{
		return m_local_bounding_sphere;
	};
	const gv_box& get_bbox()
	{
		return m_local_bounding_box;
	};
	gv_index_buffer* get_ib()
	{
		return m_index_buffer;
	}
	gv_vertex_buffer* get_vb()
	{
		return m_vertex_buffer;
	}

	void create_instancing_copy(gv_int nb_batch = 32);
	gv_static_mesh* get_instancing_copy();
	bool is_instancing_copy()
	{
		return m_is_instancing_copy;
	}
	// script here
	GVM_DCL_FUNC(scp_rescale);
	GVM_DCL_FUNC(scp_rebuild_normal);
	GVM_DCL_FUNC(scp_rebuild_normal_smooth);
	GVM_DCL_FUNC(scp_rebuild_bounding);
	GVM_DCL_FUNC(scp_rotate);

public:
	gv_box m_local_bounding_box;
	gv_sphere m_local_bounding_sphere;
	gvt_array< gv_mesh_segment > m_segments;
	gvt_ref_ptr< gv_vertex_buffer > m_vertex_buffer;
	gvt_ref_ptr< gv_index_buffer > m_index_buffer;
	gvt_ref_ptr< gv_texture > m_diffuse_texture;
	gvt_ref_ptr< gv_static_mesh > m_instancing_copy;
	gv_bool m_is_instancing_copy;
};
}