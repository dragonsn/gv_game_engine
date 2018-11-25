namespace gv
{
class gv_com_wire_frame_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_com_wire_frame_renderer,
								 gv_component_renderer);
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_static_mesh::static_class()))
			return true;
		if (p->is_a(gv_com_skeletal_mesh::static_class()))
			return true;
		return false;
	}
	virtual void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch){};
	virtual void render_component(gv_component* com)
	{
		GV_PROFILE_EVENT_PIX(gv_com_wire_frame_renderer__render_component, 0);
		gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
		if (mesh)
		{
			if (!mesh->m_vertex_buffer)
				return;
			get_device_d3d9()->SetPixelShader(NULL);
			get_device_d3d9()->SetVertexShader(NULL);
			get_device_d3d9()->SetFVF(D3DFVF_XYZ);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP,
													D3DTOP_SELECTARG1);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1,
													D3DTA_CONSTANT);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_CONSTANT,
													gv_color::BLUE().fixed32);
			get_device_d3d9()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			get_device_d3d9()->SetTransform(
				D3DTS_VIEW,
				(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_view_matrix());
			get_device_d3d9()->SetTransform(
				D3DTS_PROJECTION, (D3DMATRIX*)&gv_renderer_d3d9::static_get()
									  ->get_projection_matrix());

			get_device_d3d9()->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 0, mesh->m_vertex_buffer->m_raw_pos.size(),
				mesh->m_index_buffer->m_raw_index_buffer.size() / 3,
				mesh->m_index_buffer->m_raw_index_buffer.begin(), D3DFMT_INDEX32,
				mesh->m_vertex_buffer->m_raw_pos.begin(), sizeof(gv_vector3));

			get_device_d3d9()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		};
	};
	virtual void precache_component(gv_com_graphic* com, gv_int pass){

	};
};

GVM_IMP_CLASS(gv_com_wire_frame_renderer, gv_component_renderer)
GVM_END_CLASS
}