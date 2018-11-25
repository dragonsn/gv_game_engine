
namespace gv
{
gv_renderer::gv_renderer()
{
	m_current_render_pass = gve_render_pass_max;
	gvt_zero(m_render_pass_flag);
	link_class(gv_renderer::static_class());
	m_projection_matrix.set_identity();
	m_inv_view_matrix.set_identity();
	m_view_matrix.set_identity();
	m_world_matrix.set_identity();
	m_world_view_project_matrix.set_identity();
}

gv_renderer::~gv_renderer(){

};

bool gv_renderer::update_matrix_from_camera(gv_com_camera* camera)
{
	// this->m_projection_matrix=camera->m_projection_matrix;
	// this->m_view_matrix=camera->m_world_view_matrix;
	// this->m_world_view_project_matrix= camera->m_world_view_project_matrix;
	// this->m_inv_view_matrix=m_view_matrix.get_inverse();
	return true;
};

GVM_IMP_CLASS(gv_renderer, gv_event_processor)
GVM_END_CLASS
}

namespace gv
{
GVM_IMP_CLASS(gv_index_buffer, gv_resource)
GVM_VAR_ATTRIB_SET(hide_in_editor)
GVM_VAR(gvt_array< gv_int >, m_raw_index_buffer)
GVM_END_CLASS;

GVM_IMP_CLASS(gv_vertex_buffer, gv_resource)
GVM_VAR_ATTRIB_SET(hide_in_editor)
GVM_VAR(gvt_array< gv_vector3 >, m_raw_pos)
GVM_VAR(gvt_array< gv_vector3 >, m_raw_normal)
GVM_VAR(gvt_array< gv_vector3 >, m_raw_binormal)
GVM_VAR(gvt_array< gv_vector3 >, m_raw_tangent)
GVM_VAR(gvt_array< gv_vector4i >, m_raw_blend_index)
GVM_VAR(gvt_array< gv_vector4 >, m_raw_blend_weight)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord0)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord1)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord2)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord3)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord4)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord5)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord6)
GVM_VAR(gvt_array< gv_vector2 >, m_raw_texcoord7)
GVM_VAR(gvt_array< gv_vector4 >, m_raw_color);
GVM_END_CLASS;

GVM_IMP_CLASS(gv_component_renderer, gv_object)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_mesh_segment)
GVM_VAR(gv_int, m_start_index)
GVM_VAR(gv_int, m_index_size)
GVM_VAR(gvt_ref_ptr< gv_material >, m_material)
GVM_VAR(gv_bool, m_is_hidden);
GVM_END_STRUCT
}

namespace gv
{
namespace gv_global
{
gv_render_option rnd_opt;
}
}
