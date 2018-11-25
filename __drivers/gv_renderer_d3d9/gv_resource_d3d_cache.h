#pragma once

namespace gv
{
//=====================================================================================>>
class gv_texture_d3d : public gv_resource_cache
{
public:
	GVM_DCL_CLASS(gv_texture_d3d, gv_resource_cache);
	gv_texture_d3d();
	~gv_texture_d3d();
	virtual bool set_origin(gv_resource*);
	virtual bool precache(gv_uint flag);
	virtual bool uncache();
	virtual bool recache();
	bool set(gv_int stage);
	IDirect3DBaseTexture9* get_texture_d3d()
	{
		return m_d3d_texture;
	}
	IDirect3DSurface9* get_surface_d3d();
	gv_texture* get_gv_texture()
	{
		return m_gv_texture;
	}
	void set_texture_d3d(IDirect3DBaseTexture9* tex)
	{
		m_d3d_texture = tex;
	}

protected:
	gv_string_tmp get_cached_texture_name();

	IDirect3DBaseTexture9* m_d3d_texture;
	IDirect3DSurface9* m_d3d_surface;
	gvt_ptr< gv_texture > m_gv_texture;
};

//=====================================================================================>>
void gv_vtx_d3d_usage(gv_uint vertex_format_flag, BYTE& usage,
					  BYTE& usage_index, WORD& element_size, BYTE& element_tp);
class gv_d3d_vb_cache : public gv_refable
{
	friend class gv_vertex_buffer_d3d;

public:
	gv_d3d_vb_cache();
	~gv_d3d_vb_cache();
	bool create(gv_uint vertex_flag, gv_vertex_buffer* pvb);
	IDirect3DVertexDeclaration9* get_decl()
	{
		return m_vertex_dcl;
	}
	IDirect3DVertexBuffer9* get_vb_d3d()
	{
		return m_vertex_buffer_d3d;
	}

protected:
	gv_uint m_precache_tag;
	gv_uint m_count;
	gv_uint m_stream_number;
	gv_uint m_vertex_format;
	gv_uint m_vertex_size;
	IDirect3DVertexDeclaration9* m_vertex_dcl;
	gvt_array< D3DVERTEXELEMENT9 > m_elements;
	IDirect3DVertexBuffer9* m_vertex_buffer_d3d;
};

class gv_vertex_buffer_d3d : public gv_resource_cache
{
public:
	GVM_DCL_CLASS(gv_vertex_buffer_d3d, gv_resource_cache);
	gv_vertex_buffer_d3d();
	~gv_vertex_buffer_d3d();

	virtual bool set_origin(gv_resource*);
	virtual bool precache(gv_uint flag);
	virtual bool uncache();
	virtual bool recache();
	bool set(gv_uint format_flag);
	void set_stream_id(gv_int stream_id);
	inline gv_uint get_vertex_count()
	{
		return this->m_count;
	}

protected:
	gv_d3d_vb_cache* get_cache(gv_uint format_flag);

	gv_int m_stream_id;
	gv_int m_count;
	gv_vertex_buffer* m_gv_vb;
	gvt_array< gvt_ref_ptr< gv_d3d_vb_cache > > m_caches;
};

//=====================================================================================>>
class gv_index_buffer_d3d : public gv_resource_cache
{
public:
	GVM_DCL_CLASS(gv_index_buffer_d3d, gv_resource_cache)
	gv_index_buffer_d3d();
	~gv_index_buffer_d3d();

	virtual bool set_origin(gv_resource* pres);

	virtual bool precache(gv_uint flag);
	virtual bool uncache();
	virtual bool recache();
	bool set();
	inline gv_uint get_prim_count()
	{
		return this->m_count / 3;
	} // currently only triangle list is supported.
protected:
	gv_int m_count;
	gv_index_buffer* m_gv_ib;
	;
	IDirect3DIndexBuffer9* m_index_buffer_d3d;
};
//============================================================================================
//								:
//============================================================================================
class gv_shader_d3d : public gv_resource_cache
{

public:
	friend class gv_com_effect_renderer;
	friend class gv_renderer_d3d9;
	struct sampler_info
	{
		sampler_info()
		{
			GVM_ZERO_ME;
		}
		gv_id name;
		gv_int d3d_index;
	};

public:
	GVM_DCL_CLASS(gv_shader_d3d, gv_resource_cache)
	gv_shader_d3d();
	~gv_shader_d3d();

	virtual bool set_origin(gv_resource* pres);
	virtual bool precache(gv_uint flag);
	virtual bool uncache();
	virtual bool recache();
	bool set(bool do_skin = false);
	bool set_pixel_shader();
	bool set_vertex_shader(bool do_skin = false);
	bool is_pixel_shader()
	{
		return m_is_pixel_shader;
	}

protected:
	bool compile_shader(bool do_skin = false);

	bool m_is_pixel_shader;
	IDirect3DVertexShader9* m_vertex_shader_d3d;
	IDirect3DVertexShader9* m_vertex_skin_shader_d3d;
	IDirect3DPixelShader9* m_pixel_shader_d3d;
	ID3DXConstantTable* m_const_table_d3d;
	gv_shader* m_origin;
	gv_effect* m_effect;
	gvt_array< sampler_info > m_samplers;
	gv_uint m_vertex_stream_decl;
};

} // gv;