#pragma once
namespace gv
{
class gv_com_x_mesh_cache_d3d : public gv_resource_cache
{
	friend class gv_com_x_mesh_renderer;

public:
	GVM_DCL_CLASS(gv_com_x_mesh_cache_d3d, gv_resource_cache);

	gv_com_x_mesh_cache_d3d::gv_com_x_mesh_cache_d3d()
	{
		m_NumMaterials = 0;
		m_Mesh = NULL;
		link_class(gv_com_x_mesh_cache_d3d::static_class());
	}

	gv_com_x_mesh_cache_d3d::~gv_com_x_mesh_cache_d3d()
	{
		for (DWORD i = 0; i < m_NumMaterials; i++)
		{
			if (m_MeshTextures[i])
				m_MeshTextures[i]->Release();
		}
		if (m_Mesh != NULL)
			m_Mesh->Release();
	}

	bool load(const gv_string_tmp& name)
	{
		LPD3DXBUFFER pD3DXMtrlBuffer;
		// Load the mesh from the specified file
		if (FAILED(D3DXLoadMeshFromXA(*name, D3DXMESH_SYSTEMMEM, get_device_d3d9(),
									  NULL, &pD3DXMtrlBuffer, NULL, &m_NumMaterials,
									  &m_Mesh)))
		{
			GVM_WARNING("failed to load x file" << name);
			return false;
		}

		D3DXMATERIAL* d3dxMaterials =
			(D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
		m_MeshMaterials.resize(m_NumMaterials);
		m_MeshTextures.resize(m_NumMaterials);

		gv_string path = gv_global::fm->get_parent_path(*name);
		path << "/";
		for (DWORD i = 0; i < m_NumMaterials; i++)
		{
			// Copy the material
			m_MeshMaterials[i] = d3dxMaterials[i].MatD3D;
			// Set the ambient color for the material (D3DX does not do this)
			m_MeshMaterials[i].Ambient = m_MeshMaterials[i].Diffuse;
			gv_string tex_name = path;
			tex_name << d3dxMaterials[i].pTextureFilename;

			m_MeshTextures[i] = NULL;
			if (FAILED(D3DXCreateTextureFromFileA(get_device_d3d9(), *tex_name,
												  &m_MeshTextures[i])))
			{
				// If texture is not in current folder, try parent folder
				GVM_ERROR("failed to load x file" << name);
			}
		}
		pD3DXMtrlBuffer->Release();
		return true;
	}
	void update()
	{
		// D3DXMATRIXA16 matWorld;
		// D3DXMatrixRotationY( &matWorld, timeGetTime() / 1000.0f );
		// get_device_d3d9()->SetTransform( D3DTS_WORLD, &matWorld );

		// D3DXVECTOR3 vEyePt		( 0.0f, 3.0f,-5.0f );
		// D3DXVECTOR3 vLookatPt	( 0.0f, 0.0f, 0.0f );
		// D3DXVECTOR3 vUpVec		( 0.0f, 1.0f, 0.0f );
		// D3DXMATRIXA16 matView;
		// D3DXMatrixLookAtRH( &matView, &vEyePt, &vLookatPt, &vUpVec );
		// get_device_d3d9()->SetTransform( D3DTS_VIEW, &matView );
		get_device_d3d9()->SetTransform(
			D3DTS_VIEW,
			(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_view_matrix());
		// D3DXMATRIXA16 matProj;
		// D3DXMatrixPerspectiveFovRH( &matProj, D3DX_PI / 3, 0.75F, 0.1f, 300.0f );
		// get_device_d3d9()->SetTransform( D3DTS_PROJECTION, &matProj );
		get_device_d3d9()->SetTransform(
			D3DTS_PROJECTION,
			(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_projection_matrix());
	}

	void render()
	{
		update();
		get_device_d3d9()->SetPixelShader(NULL);
		get_device_d3d9()->SetVertexShader(NULL);
		get_device_d3d9()->SetFVF(m_Mesh->GetFVF());
		// get_device_d3d9()->SetFVF(D3DFVF_XYZ|D3DFVF_TEX0);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP,
												D3DTOP_SELECTARG1);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		for (DWORD i = 0; i < m_NumMaterials; i++)
		{
			// Set the material and texture for this subset
			get_device_d3d9()->SetMaterial(&m_MeshMaterials[i]);
			get_device_d3d9()->SetTexture(0, m_MeshTextures[i]);
			// Draw the mesh subset
			m_Mesh->DrawSubset(i);
		}
	}

	LPD3DXMESH m_Mesh;
	gvt_array< D3DMATERIAL9 > m_MeshMaterials;
	gvt_array< LPDIRECT3DTEXTURE9 > m_MeshTextures;
	DWORD m_NumMaterials; // Number of mesh materials
};

void gv_com_x_mesh_renderer::render_batch(
	gvt_array< gvt_ref_ptr< gv_component > >& batch){

};

void gv_com_x_mesh_renderer::render_component(gv_component* com)
{
	GV_PROFILE_EVENT_PIX(gv_com_x_mesh_renderer__render_component, 0);
	gv_resource* presource = com->get_resource();
	gv_com_x_mesh_cache_d3d* pcache =
		gvt_cast< gv_com_x_mesh_cache_d3d >(presource->get_hardware_cache());
	GV_ASSERT(pcache);
	gv_matrix44 mat = com->get_entity()->get_tm();
	get_device_d3d9()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);
	pcache->render();
}

void gv_com_x_mesh_renderer::precache_component(gv_com_graphic* com,
												gv_int pass)
{
	gv_resource* presource = com->get_resource();
	gv_string_tmp name = presource->get_physical_path();
	if (presource->get_hardware_cache())
		return; // already cached;
	gv_com_x_mesh_cache_d3d* pcache = new gv_com_x_mesh_cache_d3d;
	pcache->load(name);
	presource->set_hardware_cache(pcache);
}

GVM_IMP_CLASS(gv_com_x_mesh_renderer, gv_component_renderer)
GVM_END_CLASS

GVM_IMP_CLASS(gv_com_x_mesh_cache_d3d, gv_resource_cache)
GVM_END_CLASS
}