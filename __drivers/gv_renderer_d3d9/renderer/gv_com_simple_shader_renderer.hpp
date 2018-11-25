namespace gv
{
struct vertex_shader_d3d
{
	IDirect3DVertexShader9* shader;
	ID3DXConstantTable* const_table;
};

struct pixel_shader_d3d
{
	IDirect3DPixelShader9* shader;
	ID3DXConstantTable* const_table;
	// gvt_array< sampler_info_d3d>	sample_infos;
};

void gv_com_simple_shader_renderer::render_batch(
	gvt_array< gvt_ref_ptr< gv_component > >& batch)
{
	GV_PROFILE_EVENT_PIX(gv_com_simple_shader_renderer__render_batch, 0);
	for (int i = 0; i < batch.size(); i++)
	{
		render_component(batch[i]);
	}
};

void gv_com_simple_shader_renderer::render_component(gv_component* com)
{
	GV_PROFILE_EVENT_PIX(gv_com_simple_shader_renderer__render_component, 0);
	gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
	if (mesh)
	{
		if (!mesh->m_vertex_buffer)
			return;
		if (!mesh->m_index_buffer)
			return;
		if (!mesh->m_diffuse_texture)
			return; // todo add a default texture ..
		// render the mesh;
		get_device_d3d9()->SetPixelShader(NULL);
		get_device_d3d9()->SetVertexShader(NULL);
		get_device_d3d9()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP,
												D3DTOP_SELECTARG1);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		get_device_d3d9()->SetTransform(
			D3DTS_VIEW,
			(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_view_matrix());
		get_device_d3d9()->SetTransform(
			D3DTS_PROJECTION,
			(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_projection_matrix());

		gv_vertex_buffer_d3d* pvb =
			mesh->m_vertex_buffer->get_hardware_cache< gv_vertex_buffer_d3d >();
		GV_ASSERT(pvb);
		if (!pvb->set(e_vtx_with_pos | e_vtx_with_texcoord0))
		{
			if (pvb->set(e_vtx_with_pos))
			{
				get_device_d3d9()->SetFVF(D3DFVF_XYZ);
			}
			else
				return;
		}
		gv_index_buffer_d3d* pib =
			mesh->m_index_buffer->get_hardware_cache< gv_index_buffer_d3d >();
		GV_ASSERT(pib);
		pib->set();
		gv_texture_d3d* tex =
			mesh->m_diffuse_texture->get_hardware_cache< gv_texture_d3d >();
		tex->set(0);
		get_device_d3d9()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
												pvb->get_vertex_count(), 0,
												pib->get_prim_count());
	}
}

void gv_com_simple_shader_renderer::precache_component(gv_com_graphic* com,
													   gv_int pass)
{
	gv_resource* presource = com->get_resource();
	bool ret = gv_renderer_d3d9::static_get()->precache_resource(presource);
	if (!ret)
	{
		GVM_WARNING("failed to cache " << com->get_name_id()
									   << "for gv_com_simple_shader_renderer");
	};
}

GVM_IMP_CLASS(gv_com_simple_shader_renderer, gv_component_renderer)
GVM_END_CLASS
};