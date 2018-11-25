#pragma once

namespace gv
{

//=====================================================================================>>
gv_texture_d3d::gv_texture_d3d()
{
	m_d3d_texture = NULL;
	m_gv_texture = NULL;
	m_d3d_surface = NULL;
	link_class(gv_texture_d3d::static_class());
};
gv_texture_d3d::~gv_texture_d3d()
{
	this->uncache();
};

bool gv_texture_d3d::set_origin(gv_resource* res)
{
	gv_texture* ptex = gvt_cast< gv_texture >(res);
	if (!ptex)
		return false;
	this->m_gv_texture = ptex;
	return true;
};

gv_string_tmp gv_texture_d3d::get_cached_texture_name()
{
	gv_string_tmp file_name = this->m_gv_texture->get_file_name();
	gv_string_tmp name;
	if (!this->m_gv_texture->get_sandbox())
		return file_name;
	name = gv_framework_config::get_texture_cache_path();
	name += this->m_gv_texture->get_sandbox()->get_file_manager()->get_main_name(
		*file_name);
	name += ".DDS";
	name =
		this->m_gv_texture->get_sandbox()->logical_resource_path_to_absolute_path(
			name);
	return name;
};

bool gv_texture_d3d::precache(gv_uint flag)
{
	if (!m_gv_texture)
		return false;
	gv_string_tmp file_name = this->m_gv_texture->get_file_name();
	if (this->m_gv_texture->get_sandbox())
		file_name = this->m_gv_texture->get_sandbox()->get_physical_resource_path(
			file_name);
	if (m_gv_texture->is_file_in_memory())
	{
		if (m_gv_texture->is_volume_texture())
		{
			LPDIRECT3DVOLUMETEXTURE9 tex;
			if (FAILED(D3DXCreateVolumeTextureFromFileInMemory(
					get_device_d3d9(), m_gv_texture->get_texture_data(),
					m_gv_texture->get_texture_data_size(), &tex)))
			{
				GVM_ERROR("failed to load texture in memory as file " << file_name
																	  << gv_endl);
				return false;
			}
			m_d3d_texture = tex;
		}

		else if (m_gv_texture->is_cube_texture())
		{
			LPDIRECT3DCUBETEXTURE9 tex;
			if (FAILED(D3DXCreateCubeTextureFromFileInMemory(
					get_device_d3d9(), m_gv_texture->get_texture_data(),
					m_gv_texture->get_texture_data_size(), &tex)))
			{
				GVM_ERROR("failed to load texture in memory as file " << file_name
																	  << gv_endl);
				return false;
			}
			m_d3d_texture = tex;
		}

		else
		{
			LPDIRECT3DTEXTURE9 tex;
			if (FAILED(D3DXCreateTextureFromFileInMemory(
					get_device_d3d9(), m_gv_texture->get_texture_data(),
					m_gv_texture->get_texture_data_size(), &tex)))
			{
				GVM_ERROR("failed to load texture in memory as file " << file_name
																	  << gv_endl);
				return false;
			}
			m_d3d_texture = tex;
		}
		return true;
	}
	if (m_gv_texture->is_volume_texture())
	{
		LPDIRECT3DVOLUMETEXTURE9 tex;
		// TO DO load directly from texture data.
		if (FAILED(D3DXCreateVolumeTextureFromFileA(get_device_d3d9(), *file_name,
													&tex)))
		{
			gv_string_tmp name = get_cached_texture_name();
			if (FAILED(D3DXCreateVolumeTextureFromFileA(get_device_d3d9(), *name,
														&tex)))
			{
				GVM_WARNING("failed to load texture from file " << file_name
																<< gv_endl);
				return false;
			}
		}
		m_d3d_texture = tex;
		return true;
	}
	else if (m_gv_texture->is_cube_texture())
	{
		LPDIRECT3DCUBETEXTURE9 tex;
		// TO DO load directly from texture data.
		if (FAILED(D3DXCreateCubeTextureFromFileA(get_device_d3d9(), *file_name,
												  &tex)))
		{
			gv_string_tmp name = get_cached_texture_name();
			if (FAILED(
					D3DXCreateCubeTextureFromFileA(get_device_d3d9(), *name, &tex)))
			{
				GVM_WARNING("failed to load texture from file " << file_name
																<< gv_endl);
				return false;
			}
		}
		m_d3d_texture = tex;
		return true;
	}
	else
	{
		LPDIRECT3DTEXTURE9 tex = NULL;
		// try local directory first.
		gv_string_tmp name;
		name = get_sandbox()->get_file_manager()->get_filename(*file_name);
		if (!FAILED(D3DXCreateTextureFromFileA(get_device_d3d9(), *name, &tex)))
		{
		}
		// TO DO load directly from texture data.
		else if (FAILED(D3DXCreateTextureFromFileA(get_device_d3d9(), *file_name,
												   &tex)))
		{
			// try load DDS in the exe folder!!!
			gv_string_tmp name = get_cached_texture_name();
			if (FAILED(D3DXCreateTextureFromFileA(get_device_d3d9(), *name, &tex)))
			{
				GVM_WARNING("failed to load texture from file " << file_name
																<< gv_endl);
				return false;
			}
		}
		m_d3d_texture = tex;
		return true;
	}
	return true;
};

bool gv_texture_d3d::uncache()
{

	if (m_d3d_surface)
		m_d3d_surface->Release();
	if (m_d3d_texture)
		m_d3d_texture->Release();
	m_d3d_surface = NULL;
	m_d3d_texture = NULL;
	return true;
};

bool gv_texture_d3d::recache()
{
	this->uncache();
	return this->precache(NULL);
}

bool gv_texture_d3d::set(gv_int stage)
{
	get_device_d3d9()->SetTexture(stage, this->m_d3d_texture);
	return true;
};

IDirect3DSurface9* gv_texture_d3d::get_surface_d3d()
{
	if (!m_d3d_surface)
		((IDirect3DTexture9*)m_d3d_texture)->GetSurfaceLevel(0, &m_d3d_surface);
	return m_d3d_surface;
};

//=====================================================================================>>

/*	D3DDECLTYPE_FLOAT1
  D3DDECLTYPE_FLOAT2
  D3DDECLTYPE_FLOAT3
  D3DDECLTYPE_FLOAT4
  D3DDECLTYPE_D3DCOLOR

  D3DDECLTYPE_UBYTE4
  D3DDECLTYPE_SHORT2
  D3DDECLTYPE_SHORT4

  D3DDECLTYPE_UBYTE4N
  D3DDECLTYPE_SHORT2N
  D3DDECLTYPE_SHORT4N
  D3DDECLTYPE_USHORT2N
  D3DDECLTYPE_USHORT4N
  D3DDECLTYPE_UDEC3
  D3DDECLTYPE_DEC3N
  D3DDECLTYPE_FLOAT16_2
  D3DDECLTYPE_FLOAT16_4
  D3DDECLTYPE_UNUSED
  */
#pragma GV_REMINDER( \
	"[MEMO]gvt_vtx_element_pack must match the declaration in gv_vtx_d3d_usage !!")
template < int usage_flag, class type_of_data >
void gvt_vtx_element_pack(gv_byte* tgt, type_of_data& src)
{
	memcpy(tgt, &src, sizeof(src));
}

template <>
void gvt_vtx_element_pack< e_vtx_with_blend_index, const gv_vector4i >(
	gv_byte* tgt, const gv_vector4i& src)
{
	*tgt++ = (gv_byte)(src[0] > 0 ? src[0] : 0);
	*tgt++ = (gv_byte)(src[1] > 0 ? src[1] : 0);
	*tgt++ = (gv_byte)(src[2] > 0 ? src[2] : 0);
	*tgt++ = (gv_byte)(src[3] > 0 ? src[3] : 0);
}

template < int usage_flag, class type_of_data >
void gvt_vtx_element_pack_array(gv_byte* tgt,
								const gvt_array< type_of_data >& src,
								gv_int stride, gv_int count)
{
	for (int i = 0; i < count; i++)
	{
		gvt_vtx_element_pack< usage_flag >(tgt, src[i]);
		tgt += stride;
	}
}

void gv_vtx_d3d_usage(gv_uint vertex_format_flag, BYTE& usage,
					  BYTE& usage_index, WORD& element_size, BYTE& element_tp)
{
	usage = 0;
	usage_index = 0;
	switch (vertex_format_flag)
	{
	case e_vtx_with_pos:
		usage = D3DDECLUSAGE_POSITION;
		element_size = sizeof(float) * 3;
		element_tp = D3DDECLTYPE_FLOAT3;
		break;

	case e_vtx_with_binormal:
		usage = D3DDECLUSAGE_BINORMAL;
		element_size = sizeof(float) * 3;
		element_tp = D3DDECLTYPE_FLOAT3;
		break;

	case e_vtx_with_normal:
		usage = D3DDECLUSAGE_NORMAL;
		element_size = sizeof(float) * 3;
		element_tp = D3DDECLTYPE_FLOAT3;
		break;

	case e_vtx_with_color:
		usage = D3DDECLUSAGE_COLOR;
		element_size = sizeof(D3DCOLOR);
		element_tp = D3DDECLTYPE_D3DCOLOR;
		break;

	case e_vtx_with_tangent:
		usage = D3DDECLUSAGE_TANGENT;
		element_size = sizeof(float) * 3;
		element_tp = D3DDECLTYPE_FLOAT3;
		break;

	case e_vtx_with_texcoord0:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 0;
		break;

	case e_vtx_with_texcoord1:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 1;
		break;

	case e_vtx_with_texcoord2:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 2;
		break;

	case e_vtx_with_texcoord3:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 3;
		break;

	case e_vtx_with_texcoord4:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 4;
		break;

	case e_vtx_with_texcoord5:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 5;
		break;

	case e_vtx_with_texcoord6:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 6;
		break;

	case e_vtx_with_texcoord7:
		usage = D3DDECLUSAGE_TEXCOORD;
		element_size = sizeof(float) * 2;
		element_tp = D3DDECLTYPE_FLOAT2;
		usage_index = 7;
		break;

	case e_vtx_with_blend_index:
		usage = D3DDECLUSAGE_BLENDINDICES;
		element_size = sizeof(gv_byte) * 4;
		element_tp = D3DDECLTYPE_UBYTE4;
		break;

	case e_vtx_with_blend_weight:
		usage = D3DDECLUSAGE_BLENDWEIGHT;
		element_size = sizeof(float) * 4;
		element_tp = D3DDECLTYPE_FLOAT4;
		break;

	default:
		GV_ASSERT(0 && "unknown vertex flag ");
	}
};

static D3DVERTEXELEMENT9 vtx_ele_end = D3DDECL_END();

#undef GVM_SET_VERTEX_D3D_DECL
#define GVM_SET_VERTEX_D3D_DECL(usage)                                        \
	if (vertex_flag & e_vtx_with_##usage)                                     \
	{                                                                         \
		if (pvb->m_raw_##usage.size() != m_count)                             \
			return false;                                                     \
		D3DVERTEXELEMENT9 ele;                                                \
		gvt_zero(ele);                                                        \
		gv_vtx_d3d_usage(e_vtx_with_##usage, ele.Usage, ele.UsageIndex, size, \
						 ele.Type);                                           \
		ele.Offset = offset;                                                  \
		m_elements.push_back(ele);                                            \
		offset += size;                                                       \
	}

#define GVM_COPY_VERTEX_ELEMENT(usage)                                      \
	if (vertex_flag & e_vtx_with_##usage)                                   \
	{                                                                       \
		D3DVERTEXELEMENT9& ele = m_elements[cu_element];                    \
		gvt_vtx_element_pack_array< e_vtx_with_##usage >(                   \
			d3d_vtx_buffer + ele.Offset, pvb->m_raw_##usage, m_vertex_size, \
			m_count);                                                       \
		cu_element++;                                                       \
	}

bool gv_d3d_vb_cache::create(gv_uint vertex_flag, gv_vertex_buffer* pvb)
{
	WORD offset = 0;
	WORD size = 0;
	m_count = pvb->m_raw_pos.size();
	;
	if (!m_count)
		return false;

	GVM_SET_VERTEX_D3D_DECL(pos);
	GVM_SET_VERTEX_D3D_DECL(normal);
	GVM_SET_VERTEX_D3D_DECL(binormal);
	GVM_SET_VERTEX_D3D_DECL(tangent);
	GVM_SET_VERTEX_D3D_DECL(blend_index);
	GVM_SET_VERTEX_D3D_DECL(blend_weight);
	GVM_SET_VERTEX_D3D_DECL(color);
	GVM_SET_VERTEX_D3D_DECL(texcoord0);
	GVM_SET_VERTEX_D3D_DECL(texcoord1);
	GVM_SET_VERTEX_D3D_DECL(texcoord2);
	GVM_SET_VERTEX_D3D_DECL(texcoord3);
	GVM_SET_VERTEX_D3D_DECL(texcoord4);
	GVM_SET_VERTEX_D3D_DECL(texcoord5);
	GVM_SET_VERTEX_D3D_DECL(texcoord6);
	GVM_SET_VERTEX_D3D_DECL(texcoord7);
	m_elements.push_back(vtx_ele_end);
	m_vertex_format = vertex_flag;
	m_vertex_size = offset;
	GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexDeclaration(
		m_elements.begin(), &this->m_vertex_dcl));

	int buffer_size = m_vertex_size * m_count;
	GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexBuffer(
		buffer_size, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED,
		&m_vertex_buffer_d3d, NULL));
	VOID* plock;
	GVM_VERIFY_D3D(m_vertex_buffer_d3d->Lock(0, buffer_size, &plock, 0));
	gv_byte* d3d_vtx_buffer = (gv_byte*)plock;
	int cu_element = 0;
	GVM_COPY_VERTEX_ELEMENT(pos);
	GVM_COPY_VERTEX_ELEMENT(normal);
	GVM_COPY_VERTEX_ELEMENT(binormal);
	GVM_COPY_VERTEX_ELEMENT(tangent);
	GVM_COPY_VERTEX_ELEMENT(blend_index);
	GVM_COPY_VERTEX_ELEMENT(blend_weight);
	GVM_COPY_VERTEX_ELEMENT(color);
	GVM_COPY_VERTEX_ELEMENT(texcoord0);
	GVM_COPY_VERTEX_ELEMENT(texcoord1);
	GVM_COPY_VERTEX_ELEMENT(texcoord2);
	GVM_COPY_VERTEX_ELEMENT(texcoord3);
	GVM_COPY_VERTEX_ELEMENT(texcoord4);
	GVM_COPY_VERTEX_ELEMENT(texcoord5);
	GVM_COPY_VERTEX_ELEMENT(texcoord6);
	GVM_COPY_VERTEX_ELEMENT(texcoord7);
	// GV_memcpy( plock,
	// pd3d->converted_data?pd3d->converted_data:p->memory_cache, buffer_size );
	GVM_VERIFY_D3D(m_vertex_buffer_d3d->Unlock());
	this->m_precache_tag = pvb->m_precache_tag;
	return true;
};

//============================================================================>
gv_d3d_vb_cache::gv_d3d_vb_cache()
{
	m_vertex_buffer_d3d = NULL;
	m_vertex_dcl = NULL;
	m_stream_number = 0;
	m_precache_tag = (gv_uint)-1;
};
gv_d3d_vb_cache::~gv_d3d_vb_cache()
{
	if (m_vertex_dcl)
		m_vertex_dcl->Release();
	if (m_vertex_buffer_d3d)
		m_vertex_buffer_d3d->Release();
}
gv_vertex_buffer_d3d::gv_vertex_buffer_d3d()
{
	link_class(gv_vertex_buffer_d3d::static_class());
	m_stream_id = 0;
	this->m_gv_vb = NULL;
};
gv_vertex_buffer_d3d::~gv_vertex_buffer_d3d()
{
	uncache();
};
bool gv_vertex_buffer_d3d::set_origin(gv_resource* pres)
{
	gv_vertex_buffer* vb = gvt_cast< gv_vertex_buffer >(pres);
	pres->set_hardware_cache(this);
	m_count = vb->m_raw_pos.size();
	m_gv_vb = vb;
	GV_ASSERT(m_count);
	return true;
};

bool gv_vertex_buffer_d3d::precache(gv_uint flag)
{
	if (!m_count)
		return false;
	if (this->get_cache(flag))
		return true; // cached;
	if ((m_gv_vb->get_biggest_vertex_format() & flag) != flag)
	{
		if (m_gv_vb->m_raw_texcoord0.size() == 0)
		{
			m_gv_vb->m_raw_texcoord0.resize(m_gv_vb->m_raw_pos.size());
			m_gv_vb->m_raw_texcoord0 = gv_vector2(0, 0);
		}
		if (m_gv_vb->m_raw_normal.size() == 0)
		{
			m_gv_vb->m_raw_normal.resize(m_gv_vb->m_raw_pos.size());
			m_gv_vb->m_raw_normal = gv_vector3(1, 0, 0);
		}
	}
	if ((m_gv_vb->get_biggest_vertex_format() & flag) != flag)
		return false;
	gvt_ref_ptr< gv_d3d_vb_cache > cache = new gv_d3d_vb_cache;
	if (cache->create(flag, this->m_gv_vb))
	{
		this->m_caches.push_back(cache);
		return true;
	}
	return false;
};

bool gv_vertex_buffer_d3d::uncache()
{
	this->m_caches.clear();
	return true;
}

bool gv_vertex_buffer_d3d::recache()
{
	uncache();
	return true;
}

bool gv_vertex_buffer_d3d::set(gv_uint format_flag)
{
	gv_d3d_vb_cache* pcache = this->get_cache(format_flag);
	if (!pcache)
	{
		if (!precache(format_flag))
			return false;
		pcache = this->get_cache(format_flag);
	}
	if (!pcache)
		return false;
	get_device_d3d9()->SetVertexDeclaration(pcache->m_vertex_dcl);
	get_device_d3d9()->SetStreamSource(pcache->m_stream_number,
									   pcache->m_vertex_buffer_d3d, 0,
									   pcache->m_vertex_size);
	return true;
}

gv_d3d_vb_cache* gv_vertex_buffer_d3d::get_cache(gv_uint format_flag)
{
	for (int i = 0; i < this->m_caches.size(); i++)
	{
		if (this->m_caches[i]->m_vertex_format == format_flag)
		{
			gv_d3d_vb_cache* pcache = this->m_caches[i];
			if (pcache->m_precache_tag != this->m_gv_vb->m_precache_tag)
			{
				// found but, it's dirty;
				m_caches.erase_fast(i);
				return NULL;
			}
			return this->m_caches[i];
		}
	}
	return NULL;
};
//=====================================================================================>>
gv_index_buffer_d3d::gv_index_buffer_d3d()
{
	m_index_buffer_d3d = NULL;
	m_gv_ib = NULL;
	link_class(gv_index_buffer_d3d::static_class());
};

gv_index_buffer_d3d::~gv_index_buffer_d3d()
{
	uncache();
};

bool gv_index_buffer_d3d::set_origin(gv_resource* pres)
{
	gv_index_buffer* ib = gvt_cast< gv_index_buffer >(pres);
	pres->set_hardware_cache(this);
	m_count = ib->m_raw_index_buffer.size();
	m_gv_ib = ib;
	GV_ASSERT(m_count);
	return true;
};

bool gv_index_buffer_d3d::precache(gv_uint flag)
{
	gv_index_buffer* ib = m_gv_ib;
	ib->m_raw_index_buffer;
	int ib_size = m_count * sizeof(INT);
	GVM_VERIFY_D3D(get_device_d3d9()->CreateIndexBuffer(
		ib_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED,
		&m_index_buffer_d3d, NULL));
	VOID* pIndex;
	GVM_VERIFY_D3D(m_index_buffer_d3d->Lock(0, ib_size, &pIndex, 0));
	memcpy(pIndex, ib->m_raw_index_buffer.begin(), ib_size);
	GVM_VERIFY_D3D(m_index_buffer_d3d->Unlock());
	return true;
};

bool gv_index_buffer_d3d::uncache()
{
	if (m_index_buffer_d3d)
		m_index_buffer_d3d->Release();
	m_index_buffer_d3d = NULL;
	return true;
};

bool gv_index_buffer_d3d::recache()
{
	uncache();
	return precache(NULL);
}

bool gv_index_buffer_d3d::set()
{
	get_device_d3d9()->SetIndices(this->m_index_buffer_d3d);
	return true;
}

//============================================================================================
//								:
//============================================================================================
//class gv_shader_d3d:public gv_resource_cache
gv_shader_d3d::gv_shader_d3d()
{
	GVM_SET_CLASS(gv_shader_d3d);
	m_is_pixel_shader = false;
	m_vertex_shader_d3d = NULL;
	m_vertex_skin_shader_d3d = NULL;
	m_pixel_shader_d3d = NULL;
	m_const_table_d3d = NULL;
	m_origin = NULL;
	m_effect = NULL;
};

gv_shader_d3d::~gv_shader_d3d()
{
	uncache();
};
bool gv_shader_d3d::set_origin(gv_resource* pres)
{
	this->m_origin = gvt_cast< gv_shader >(pres);
	if (!this->m_origin)
		return false;
	this->m_is_pixel_shader = this->m_origin->is_pixel_shader();
	return true;
};

#pragma GV_REMINDER("[MEMO]to do , save the compiled result")

const char* skin_vertex_decl =
	"float4 weights:			 BLENDWEIGHT;\r\n"
	"float4 bone_indices  :      BLENDINDICES;\r\n";

const char* skin_vertex_input =
	",float4 weights:			BLENDWEIGHT"
	",float4 bone_indices :     BLENDINDICES";

const char* skin_func_var_decl =
	"float4x3 bone_matrices[ 256 ]: register(c64); \r\n"
	"void apply_skin_with_binormal( in float4 bone_indices,in float4 weights,  "
	"inout float4 position, inout float3 normal, inout float3 binormal, inout "
	"float3 tangent ) \r\n"
	"{ \r\n"
	"	float4x3 blendMatrix; \r\n"
	"	blendMatrix  = weights.x * bone_matrices[ bone_indices.x ]; \r\n"
	"	blendMatrix += weights.y * bone_matrices[ bone_indices.y ]; \r\n"
	"	blendMatrix += weights.z * bone_matrices[ bone_indices.z ]; \r\n"
	"	blendMatrix += weights.w * bone_matrices[ bone_indices.w ]; \r\n"
	"	position.xyz = mul( position, blendMatrix ); \r\n"
	"	normal		 = mul( normal  , (float3x3)blendMatrix ); \r\n"
	"	tangent		 = mul( tangent , (float3x3)blendMatrix ); \r\n"
	"	binormal	 = mul( binormal, (float3x3)blendMatrix ); \r\n"
	"} \r\n"
	"void apply_skin( in float4 bone_indices,in float4 weights,inout float4 "
	"position, inout float3 normal ) \r\n"
	"{ \r\n"
	"	float4x3 blendMatrix; \r\n"
	"	blendMatrix  = weights.x * bone_matrices[ bone_indices.x ]; \r\n"
	"	blendMatrix += weights.y * bone_matrices[ bone_indices.y ]; \r\n"
	"	blendMatrix += weights.z * bone_matrices[ bone_indices.z ]; \r\n"
	"	blendMatrix += weights.w * bone_matrices[ bone_indices.w ]; \r\n"
	"	position.xyz = mul( position, blendMatrix ); \r\n"
	"	normal		 = mul( normal  , (float3x3)blendMatrix ); \r\n"
	"} \r\n"
	"void apply_skin_pos_only( in float4 bone_indices,in float4 weights,inout "
	"float4 position ) \r\n"
	"{ \r\n"
	"	float4x3 blendMatrix; \r\n"
	"	blendMatrix  = weights.x * bone_matrices[ bone_indices.x ]; \r\n"
	"	blendMatrix += weights.y * bone_matrices[ bone_indices.y ]; \r\n"
	"	blendMatrix += weights.z * bone_matrices[ bone_indices.z ]; \r\n"
	"	blendMatrix += weights.w * bone_matrices[ bone_indices.w ]; \r\n"
	"	position.xyz = mul( position, blendMatrix ); \r\n"
	"} \r\n";

D3DXMACRO macro_do_skin = {"DO_SKIN", "1"};
D3DXMACRO macro_in_gv = {"GV", "1"};
D3DXMACRO macro_end = {NULL};

bool gv_shader_d3d::compile_shader(bool for_skin)
{
	gv_shader* shader = this->m_origin;
	ID3DXBuffer* buffer = NULL;
	ID3DXBuffer* error = NULL;
	const char* pcode = *shader->get_source_code();
	int code_size = shader->get_source_code().size();
	gv_string_tmp skin_shader;
	gvt_array_cached< D3DXMACRO, 16 > macros;
	if (for_skin)
	{
		if (shader->is_pixel_shader())
			return false;
		if (!shader->get_source_code().find_string("DO_SKIN"))
			return false;
		skin_shader = pcode;
		skin_shader.replace_all("SKIN_VTX_DCL", skin_vertex_decl);
		skin_shader.replace_all("SKIN_VTX_INPUT", skin_vertex_input);
		skin_shader.replace_all("SKIN_FUNC_DCL", skin_func_var_decl);
		pcode = *skin_shader;
		code_size = skin_shader.size();
		macros.push_back(macro_do_skin);
	};
	GVM_CONSOLE_OUT("compiler shader:" << shader->get_location() << gv_endl);
	macros.push_back(macro_in_gv);
	macros.push_back(macro_end);
	gv_uint compile_flag = NULL;
	GVM_SAFE_RELEASE_D3D(m_const_table_d3d);
#if defined(_DEBUG_SHADER)
	compile_flag |= D3DXSHADER_DEBUG;
	// if (!this->m_is_pixel_shader) compile_flag|=D3DXSHADER_SKIPOPTIMIZATION;
	gv_string_tmp fn;
	fn << "\\";
	fn << gv_crc32(pcode, code_size);
	fn << get_sandbox()->get_perf_count();
	fn << "shd.tmp";
	gv_save_string_to_file(fn, pcode, code_size);
	GVM_DEBUG_LOG(render, "compile shader ....." << shader->get_location()
												 << "as file  " << fn
												 << "..........." << gv_endl);
	if (FAILED(D3DXCompileShaderFromFileA(
			*fn, macros.begin(), 0, shader->get_compile_entry(),
			shader->get_compile_target(), compile_flag, &buffer, &error,
			&this->m_const_table_d3d)))

#else
	compile_flag |= D3DXSHADER_DEBUG;
	if (FAILED(D3DXCompileShader(pcode, code_size, macros.begin(), 0,
								 shader->get_compile_entry(),
								 shader->get_compile_target(), compile_flag,
								 &buffer, &error, &this->m_const_table_d3d)))
#endif //_DEBUG
	{
		if (error)
		{
			GV_ERROR_OUT("COMPILE SHADER ERROR!!!"
						 << gv_endl << shader->get_file_name()
						 << (char*)error->GetBufferPointer() << gv_endl);
		}
		GVM_SAFE_RELEASE_D3D(buffer);
		return false;
	};
	if (error && error->GetBufferPointer())
	{
		GVM_DEBUG_LOG(render, "compiler output for:"
								  << shader->get_location() << gv_endl
								  << shader->get_file_name()
								  << (char*)error->GetBufferPointer() << gv_endl);
	}
	GVM_SAFE_RELEASE_D3D(error);
	if (this->m_is_pixel_shader)
	{
		GVM_VERIFY_D3D(get_device_d3d9()->CreatePixelShader(
			(DWORD*)buffer->GetBufferPointer(), &this->m_pixel_shader_d3d));
	}
	else
	{
		if (for_skin)
		{
			GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexShader(
				(DWORD*)buffer->GetBufferPointer(),
				&this->m_vertex_skin_shader_d3d));
		}
		else
		{
			GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexShader(
				(DWORD*)buffer->GetBufferPointer(), &this->m_vertex_shader_d3d));
		}
	}
	GVM_SAFE_RELEASE_D3D(buffer);
	GVM_SAFE_RELEASE_D3D(error);
	return true;
}

bool gv_shader_d3d::precache(gv_uint flag)
{
	/*D3DXMACRO Macro[] = {
          { "BOX_HORIZONTAL_21x21", "TRUE" }, NULL };
  */

	gv_shader* shader = this->m_origin;
	if (this->m_vertex_shader_d3d || this->m_pixel_shader_d3d)
		this->uncache();
	gv_effect* effect = gvt_cast< gv_effect >(shader->get_owner());
	if (!effect)
		return false;
	m_effect = effect;
	// get sample table
	if (!shader->is_pixel_shader())
		this->compile_shader(true);
	if (!this->compile_shader())
		return false;
	m_vertex_stream_decl = effect->get_current_stream_decl();
	D3DXHANDLE hd;
	int count = 0;
	while (hd = m_const_table_d3d->GetConstant(NULL, count))
	{
		count++;
		D3DXCONSTANT_DESC table[512];
		UINT desc_count;
		m_const_table_d3d->GetConstantDesc(hd, table, &desc_count);
		GV_ASSERT(desc_count < 512);
		gv_id const_name;
		const_name = table[0].Name;
		if (table[0].RegisterSet == D3DXRS_SAMPLER)
		{
			sampler_info sampler;
			sampler.name = table[0].Name;
			sampler.d3d_index = m_const_table_d3d->GetSamplerIndex(hd);
			if (sampler.d3d_index == -1)
				continue;
			this->m_samplers.push_back(sampler);
		}
		else
		{
			gv_shader_const* pconst = shader->get_constant_var(const_name);
			GV_ASSERT(pconst);
			// check type!
			gv_effect_param_info* pinfo = effect->get_param_info(pconst->m_name);
			if (!pinfo || pinfo->m_is_static)
			{
				pconst->m_register_count = 0;
				continue;
			}
			pconst->m_type = (gve_shader_data_type)pinfo->m_type;
			pconst->m_semantic_idx = pinfo->m_semantic_idx;
			switch (pconst->m_type)
			{
			case e_shader_data_float:
				GV_ASSERT(table[0].RegisterSet == D3DXRS_FLOAT4);
				GV_ASSERT(table[0].Rows == 1);
				GV_ASSERT(table[0].Columns == 1);
				break;

			case e_shader_data_vector2:
			case e_shader_data_vector3:
			case e_shader_data_vector4:
			case e_shader_data_colorf:
				GV_ASSERT(table[0].RegisterSet == D3DXRS_FLOAT4);
				GV_ASSERT(table[0].Rows == 1);
				break;

			case e_shader_data_bool:
				GV_ASSERT(table[0].RegisterSet == D3DXRS_BOOL);
				GV_ASSERT(table[0].Rows == 1);
				GV_ASSERT(table[0].Columns == 1);
				break;

			case e_shader_data_matrix4:
				GV_ASSERT(table[0].RegisterSet == D3DXRS_FLOAT4);
				GV_ASSERT(table[0].Rows == 4);
				GV_ASSERT(table[0].Columns == 4);
				break;

			case e_shader_data_matrix43:
				GV_ASSERT(table[0].RegisterSet == D3DXRS_FLOAT4);
				GV_ASSERT(table[0].Rows == 3);
				GV_ASSERT(table[0].Columns == 4);
				break;

			case e_shader_data_dynamic:
				GV_ASSERT(table[0].Rows == pinfo->m_row);
				GV_ASSERT(table[0].Columns == pinfo->m_col);
				break;

			default:
				GV_ASSERT(0);
			};
			pconst->m_register = table[0].RegisterIndex;
			pconst->m_register_count = table[0].RegisterCount;
		}
	}
	return true;
};

bool gv_shader_d3d::uncache()
{
	GVM_SAFE_RELEASE_D3D(m_pixel_shader_d3d);
	GVM_SAFE_RELEASE_D3D(m_vertex_shader_d3d);
	GVM_SAFE_RELEASE_D3D(m_vertex_skin_shader_d3d);
	GVM_SAFE_RELEASE_D3D(m_const_table_d3d);
	return true;
}
bool gv_shader_d3d::recache()
{
	this->uncache();
	return this->precache(NULL);
}

bool gv_shader_d3d::set_pixel_shader()
{
	gv_shader* shader = this->m_origin;
	GVM_VERIFY_D3D(get_device_d3d9()->SetPixelShader(this->m_pixel_shader_d3d));
	for (int i = 0; i < shader->get_nb_constant_var(); i++)
	{
		gv_shader_const* pconst = shader->get_constant_var(i);
		GV_ASSERT(pconst);
		if (!pconst->m_register_count)
			continue;
		gv_byte* pvar = NULL;
		if (pconst->m_semantic_idx > 0)
		{
			pvar = (gv_byte*)get_renderer_d3d9()->get_param_from_semantric(
				(gve_effect_semantic)pconst->m_semantic_idx);
		}
		else
		{
			pvar = (gv_byte*)get_renderer_d3d9()->get_param_from_material(
				pconst->m_name, pconst->m_type);
			if (!pvar)
			{
				pvar = m_effect->get_param(pconst->m_name);
			}
		}
		if (pconst->m_type == e_shader_data_bool)
		{
			BOOL bConsts[] = {(*pvar ? TRUE : FALSE)};
			get_device_d3d9()->SetPixelShaderConstantB(pconst->m_register, bConsts,
													   1);
		}
		else if (pconst->m_type == e_shader_data_int)
		{
			get_device_d3d9()->SetPixelShaderConstantI(pconst->m_register,
													   (INT*)pvar, 1);
		}
		else
		{
			get_device_d3d9()->SetPixelShaderConstantF(
				pconst->m_register, (FLOAT*)pvar, pconst->m_register_count);
		}
	}
	for (int i = 0; i < m_samplers.size(); i++)
	{
		sampler_info& sampler = m_samplers[i];
		gv_effect_texture_object* ptex_obj =
			m_effect->get_texture_object(sampler.name);
		for (int j = 0; j < ptex_obj->m_texture_states.size(); j++)
		{
			gv_effect_render_state& state = ptex_obj->m_texture_states[j];
			get_device_d3d9()->SetSamplerState(sampler.d3d_index,
											   (D3DSAMPLERSTATETYPE)state.m_state,
											   (DWORD)state.m_value);
		}
		gv_texture* tex = get_renderer_d3d9()->get_texture_from_material(
			ptex_obj->m_texture_name);
		if (!tex)
		{
			gv_effect_texture* ptex =
				m_effect->get_texture_sample(ptex_obj->m_texture_name);
			if (ptex)
				tex = ptex->m_texture;
		}
		if (!tex)
		{
			gv_effect_renderable_texture* ptex =
				m_effect->get_renderable_texture(ptex_obj->m_texture_name);
			if (ptex)
				tex = ptex->m_texture;
		}
		if (ptex_obj->m_texture_name == gv_id_g_TexBase)
		{
			if (get_renderer_d3d9()->get_tex_base())
				tex = get_renderer_d3d9()->get_tex_base();
		}
		else if (ptex_obj->m_texture_name == gv_id_g_TexNormal)
		{
			if (get_renderer_d3d9()->get_tex_normal())
				tex = get_renderer_d3d9()->get_tex_normal();
		}
		else if (ptex_obj->m_texture_name == gv_id_g_TexEnv)
		{
			if (get_renderer_d3d9()->get_tex_env())
				tex = get_renderer_d3d9()->get_tex_env();
		}
		else if (ptex_obj->m_texture_name == gv_id_g_color_buffer)
		{
			tex = get_renderer_d3d9()->get_color_buffer_tex();
		}
		else if (ptex_obj->m_texture_name == gv_id_g_depth_buffer)
		{
			tex = get_renderer_d3d9()->get_depth_buffer_tex();
		}
		if (!tex)
			tex = get_renderer_d3d9()->get_default_texture();
		gv_texture_d3d* tex_d3d = tex->get_hardware_cache< gv_texture_d3d >();
		if (!tex_d3d)
		{
			get_renderer_d3d9()->precache_resource(tex);
			tex_d3d = tex->get_hardware_cache< gv_texture_d3d >();
		}
		if (!tex_d3d->get_texture_d3d())
		{
			tex = get_renderer_d3d9()->get_default_texture();
			tex_d3d = tex->get_hardware_cache< gv_texture_d3d >();
		}
		if (tex_d3d)
			tex_d3d->set(sampler.d3d_index);
	}

	return true;
};
bool gv_shader_d3d::set_vertex_shader(bool do_skin)
{
	gv_shader* shader = this->m_origin;
	do_skin = do_skin && this->m_vertex_skin_shader_d3d;
	if (!do_skin)
	{
		GVM_VERIFY_D3D(
			get_device_d3d9()->SetVertexShader(this->m_vertex_shader_d3d));
	}
	else
	{
		GVM_VERIFY_D3D(
			get_device_d3d9()->SetVertexShader(this->m_vertex_skin_shader_d3d));
	}
	for (int i = 0; i < shader->get_nb_constant_var(); i++)
	{
		gv_shader_const* pconst = shader->get_constant_var(i);
		GV_ASSERT(pconst);
		if (!pconst->m_register_count)
			continue;
		gv_byte* pvar = NULL;
		if (pconst->m_semantic_idx > 0)
		{
			pvar = (gv_byte*)get_renderer_d3d9()->get_param_from_semantric(
				(gve_effect_semantic)pconst->m_semantic_idx);
		}
		else
		{
			pvar = (gv_byte*)get_renderer_d3d9()->get_param_from_material(
				pconst->m_name, pconst->m_type);
			if (!pvar)
			{
				pvar = m_effect->get_param(pconst->m_name);
			}
		}
		GV_ASSERT(pvar);
		if (pconst->m_type == e_shader_data_bool)
		{
			BOOL bConsts[] = {
				(*pvar ? TRUE : FALSE),
			};
			get_device_d3d9()->SetVertexShaderConstantB(pconst->m_register, bConsts,
														1);
		}
		else if (pconst->m_type == e_shader_data_int)
		{
			get_device_d3d9()->SetVertexShaderConstantI(pconst->m_register,
														(INT*)pvar, 1);
		}
		else
		{
			get_device_d3d9()->SetVertexShaderConstantF(
				pconst->m_register, (FLOAT*)pvar, pconst->m_register_count);
		}
	}

	for (int i = 0; i < m_samplers.size(); i++)
	{
		sampler_info& sampler = m_samplers[i];
		gv_effect_texture_object* ptex_obj =
			m_effect->get_texture_object(sampler.name);
		gv_effect_texture* ptex =
			m_effect->get_texture_sample(ptex_obj->m_texture_name);
		gv_texture_d3d* tex_d3d =
			ptex->m_texture->get_hardware_cache< gv_texture_d3d >();
		tex_d3d->set(sampler.d3d_index);
	}

	return true;
};
bool gv_shader_d3d::set(bool do_skin)
{
	if (this->m_is_pixel_shader)
		return this->set_pixel_shader();
	else
		return this->set_vertex_shader(do_skin);
	return true;
}

GVM_IMP_CLASS(gv_texture_d3d, gv_resource_cache)
GVM_END_CLASS;

GVM_IMP_CLASS(gv_vertex_buffer_d3d, gv_resource_cache)
GVM_END_CLASS;

GVM_IMP_CLASS(gv_index_buffer_d3d, gv_resource_cache)
GVM_END_CLASS;

GVM_IMP_CLASS(gv_shader_d3d, gv_resource_cache)
GVM_END_CLASS
};