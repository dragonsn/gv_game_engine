#pragma once
#include <bimg/decode.h>
#include <bgfx_utils.h>
namespace gv
{

	//=====================================================================================>>
	gv_texture_bgfx::gv_texture_bgfx()
	{
		link_class(gv_texture_bgfx::static_class());
	};
	gv_texture_bgfx::~gv_texture_bgfx()
	{
		this->uncache();
	};

	bool gv_texture_bgfx::set_origin(gv_resource* res)
	{
		gv_texture* ptex = gvt_cast< gv_texture >(res);
		if (!ptex)
			return false;
		this->m_gv_texture = ptex;
		return true;
	};

	gv_string_tmp gv_texture_bgfx::get_cached_texture_name()
	{
		gv_string_tmp file_name = this->m_gv_texture->get_file_name();
		gv_string_tmp name;
		if (!this->m_gv_texture->get_sandbox())
			return file_name;
		name = *gv_global::framework_config.texture_cache_path;
		name += this->m_gv_texture->get_sandbox()->get_file_manager()->get_main_name(
			*file_name);
		name += ".DDS";
		name =
			this->m_gv_texture->get_sandbox()->logical_resource_path_to_absolute_path(
				name);
		return name;
	};

	bx::AllocatorI* getDefaultAllocator()
	{
		static bx::DefaultAllocator s_allocator;
		return &s_allocator;
	}



	bgfx::TextureHandle createTexture(bimg::ImageContainer* imageContainer, uint32_t flags, bgfx::TextureInfo* info = NULL)
	{
		bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

		const bgfx::Memory* mem = NULL;
		if (imageContainer->m_data)
		{
			mem = bgfx::makeRef(
				imageContainer->m_data
				, imageContainer->m_size
				, 0
				, imageContainer
			);
		}

		if (imageContainer->m_cubeMap)
		{
			handle = bgfx::createTextureCube(
				uint16_t(imageContainer->m_width)
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				, flags
				, mem
			);
		}
		/*else if (1 < imageContainer->m_depth)
		{
		handle = bgfx::createTexture3D(
		uint16_t(imageContainer->m_width)
		, uint16_t(imageContainer->m_height)
		, uint16_t(imageContainer->m_depth)
		, 1 < imageContainer->m_numMips
		, bgfx::TextureFormat::Enum(imageContainer->m_format)
		, flags
		, mem
		);
		}*/
		else
		{
			handle = bgfx::createTexture2D(
				uint16_t(imageContainer->m_width)
				, uint16_t(imageContainer->m_height)
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				, flags
				, mem
			);
		}

		if (NULL != info)
		{
			bgfx::calcTextureSize(
				*info
				, uint16_t(imageContainer->m_width)
				, uint16_t(imageContainer->m_height)
				, uint16_t(imageContainer->m_depth)
				, imageContainer->m_cubeMap
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
			);
		}

		return handle;
	}

	bool gv_texture_bgfx::precache(gv_uint flag)
	{
		if (!m_gv_texture)
			return false;
		gv_string_tmp file_name = this->m_gv_texture->get_file_name();
		if (this->m_gv_texture->get_sandbox())
			file_name = this->m_gv_texture->get_sandbox()->get_physical_resource_path(
				file_name);
		int size = 0;
		gvt_array<gv_byte> data;
		gv_load_file_to_array (file_name, data);
		if (!data.size())
		{
			GVM_WARNING("Failed to read image from file :" <<file_name);
			return false;
		}

		// Parse data
		bimg::ImageContainer* imageContainer = bimg::imageParse(getDefaultAllocator(), (char*)data.get_data(), data.size());
		if (imageContainer == nullptr)
		{
			GVM_WARNING("Failed to parse image data from file '%s'.", filePath);
			return false;
		}

		bgfx::TextureInfo info;
		int flags = BGFX_TEXTURE_NONE;
		// create bgfx texture
		bgfx::TextureHandle handle = createTexture(imageContainer, flags, &info);
		GV_ASSERT(bgfx::isValid(handle));
		this->m_bgfx_texture_handle = handle; 
		return true;
	};

	bool gv_texture_bgfx::uncache()
	{
		if (bgfx::isValid(this->m_bgfx_texture_handle)) {
			bgfx::destroy(m_bgfx_texture_handle);
		}
		
		return true;
	};

	bool gv_texture_bgfx::recache()
	{
		this->uncache();
		return this->precache(NULL);
	}
	
	//.begin()
	//.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
	//.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
	//.end();
	gv_int gv_get_bgfx_vertex_decl(gv_uint vertex_format_flag, bgfx::VertexDecl & decl)
	{
		int vertex_size = 0; 
		decl.begin();
		if (vertex_format_flag & e_vtx_with_pos)
		{
			decl.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float); 
			vertex_size += sizeof(gv_float) * 3; 
		}
		if (vertex_format_flag& e_vtx_with_normal)
		{
			decl.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);
			vertex_size += sizeof(gv_float) * 3;
		}
		if (vertex_format_flag& e_vtx_with_binormal)
		{
			decl.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float); 
			vertex_size += sizeof(gv_float) * 3;
		}
		if (vertex_format_flag& e_vtx_with_tangent)
		{
			decl.add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float);
			vertex_size += sizeof(gv_float) * 3;
		}
		if (vertex_format_flag& e_vtx_with_color)
		{
			decl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8,true);
			vertex_size += sizeof(gv_byte) *4 ;
		}
		for (int i = 0; i < 8; i++)
		{
			int flag_mask = e_vtx_with_texcoord0<<i; 

			if (vertex_format_flag & flag_mask)
			{
				decl.add((bgfx::Attrib::Enum)((int)bgfx::Attrib::TexCoord0+i), 2, bgfx::AttribType::Float);
				vertex_size += sizeof(gv_float) * 2;
			}
		}
		if (vertex_format_flag& e_vtx_with_blend_index)
		{
			decl.add(bgfx::Attrib::Indices, 4 , bgfx::AttribType::Uint8);
			vertex_size += sizeof(gv_byte) *4 ;
		}

		if (vertex_format_flag& e_vtx_with_blend_weight)
		{
			decl.add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float);
			vertex_size += sizeof(gv_byte) * 4;
		}
		decl.end();
		return vertex_size;
	};

	//===========================================================================
	template < int usage_flag, class type_of_data >
	void gvt_vtx_element_pack(gv_byte* &tgt, type_of_data& src)
	{
		memcpy(tgt, &src, sizeof(src));
		tgt += sizeof(src);
	}

	template <>
	void gvt_vtx_element_pack< e_vtx_with_blend_index, const gv_vector4i >(
		gv_byte* &tgt, const gv_vector4i& src)
	{
		*tgt++ = (gv_byte)(src[0] > 0 ? src[0] : 0);
		*tgt++ = (gv_byte)(src[1] > 0 ? src[1] : 0);
		*tgt++ = (gv_byte)(src[2] > 0 ? src[2] : 0);
		*tgt++ = (gv_byte)(src[3] > 0 ? src[3] : 0);
	}

	template < >
	void gvt_vtx_element_pack< e_vtx_with_color, gv_vector4 >(
		gv_byte* &tgt, gv_vector4 &src)
	{
		*tgt++ = (gv_byte)(src[0] *255);
		*tgt++ = (gv_byte)(src[1] *255);
		*tgt++ = (gv_byte)(src[2] *255);
		*tgt++ = (gv_byte)(src[3] *255);
	}

#define GVM_COPY_VERTEX_ELEMENT(usage)                                      \
	if (vertex_flag & e_vtx_with_##usage)                                   \
	{                                                                       \
		gvt_vtx_element_pack<  e_vtx_with_##usage >(tgt, pvb->m_raw_##usage[i]);\
	}

	//create big vertex stream , and copy to bgfx
	bool gv_bgfx_vb_cache::create(gv_uint vertex_flag, gv_vertex_buffer* pvb)
	{
		m_count = pvb->m_raw_pos.size();
		if (!m_count)	return false;
		m_vertex_size = gv_get_bgfx_vertex_decl(vertex_flag, m_bgfx_vertex_decl);
		m_vertex_format = vertex_flag;
		/*if (bgfx::isValid(m_bgfx_vb_handle)){
			bgfx::destroy(m_bgfx_vb_handle);
		}*/
		const bgfx::Memory* mem;
		m_memory_buffer.resize(m_vertex_size*m_count); 
		//fill in data 
		gv_byte * tgt = m_memory_buffer.first();
		
		for (int i = 0; i < (int)m_count; i++)
		{
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
		}; 
		GV_ASSERT(tgt == m_memory_buffer.end());
		mem=bgfx::makeRef(m_memory_buffer.get_data(), m_memory_buffer.size());
		uint16_t flags = BGFX_BUFFER_NONE;
		m_bgfx_vb_handle = bgfx::createVertexBuffer(mem, m_bgfx_vertex_decl, flags);
		GV_ASSERT(bgfx::isValid(m_bgfx_vb_handle));
		this->m_precache_tag = pvb->m_precache_tag;
		return true;
	};

	//============================================================================>
	gv_bgfx_vb_cache::gv_bgfx_vb_cache()
	{
		m_precache_tag = (gv_uint)-1;
	};
	gv_bgfx_vb_cache::~gv_bgfx_vb_cache()
	{
		if (bgfx::isValid(m_bgfx_vb_handle)) {
			bgfx::destroy(m_bgfx_vb_handle);
		}
	}
	gv_vertex_buffer_bgfx::gv_vertex_buffer_bgfx()
	{
		link_class(gv_vertex_buffer_bgfx::static_class());
		m_stream_id = 0;
		this->m_gv_vb = NULL;
	};
	gv_vertex_buffer_bgfx::~gv_vertex_buffer_bgfx()
	{
		uncache();
	};
	bool gv_vertex_buffer_bgfx::set_origin(gv_resource* pres)
	{
		gv_vertex_buffer* vb = gvt_cast< gv_vertex_buffer >(pres);
		pres->set_hardware_cache(this);
		m_count = vb->m_raw_pos.size();
		m_gv_vb = vb;
		GV_ASSERT(m_count);
		return true;
	};

	bool gv_vertex_buffer_bgfx::precache(gv_uint flag)
	{
		if (!m_count)
			return false;
		if (this->get_cache(flag))
			return true; // cached;
	
		if ((m_gv_vb->get_biggest_vertex_format() & flag) != flag)
		{
			GVM_WARNING("error in bgfx caching vb !!");
			return false;
		}
		gvt_ref_ptr< gv_bgfx_vb_cache > cache = new gv_bgfx_vb_cache;
		if (cache->create(flag, this->m_gv_vb))
		{
			this->m_caches.push_back(cache);
			return true;
		}
		return false;
	};

	bool gv_vertex_buffer_bgfx::uncache()
	{
		this->m_caches.clear();
		return true;
	}

	bool gv_vertex_buffer_bgfx::recache()
	{
		uncache();
		return true;
	}

	bool gv_vertex_buffer_bgfx::set(gv_uint format_flag)
	{
		gv_bgfx_vb_cache* pcache = this->get_cache(format_flag);
		if (!pcache)
		{
			if (!precache(format_flag))
				return false;
			pcache = this->get_cache(format_flag);
		}
		if (!pcache)
			return false;
		bgfx::setVertexBuffer(0, pcache->m_bgfx_vb_handle); 
		return true;
	}

	bool gv_vertex_buffer_bgfx::set()
	{
		return set(this->m_gv_vb->get_biggest_vertex_format());
	};

	gv_bgfx_vb_cache* gv_vertex_buffer_bgfx::get_cache(gv_uint format_flag)
	{
		for (int i = 0; i < this->m_caches.size(); i++)
		{
			if (this->m_caches[i]->m_vertex_format == format_flag)
			{
				gv_bgfx_vb_cache* pcache = this->m_caches[i];
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
	gv_index_buffer_bgfx::gv_index_buffer_bgfx()
	{
		m_gv_ib = NULL;
		link_class(gv_index_buffer_bgfx::static_class());
	};

	gv_index_buffer_bgfx::~gv_index_buffer_bgfx()
	{
		uncache();
	};

	bool gv_index_buffer_bgfx::set_origin(gv_resource* pres)
	{
		gv_index_buffer* ib = gvt_cast< gv_index_buffer >(pres);
		pres->set_hardware_cache(this);
		m_gv_ib = ib;
		return true;
	};

	bool gv_index_buffer_bgfx::precache(gv_uint flag)
	{
		int m_count = this->m_gv_ib->m_raw_index_buffer.size();
		if (!m_count)	return false;
		/*if (bgfx::isValid(this->m_bgfx_ib_handle)) {
			bgfx::destroy(m_bgfx_ib_handle);
		}*/
		const bgfx::Memory* mem;
		uint16_t flags = BGFX_BUFFER_INDEX32;
		mem = bgfx::makeRef(m_gv_ib->m_raw_index_buffer.get_data(), m_gv_ib->m_raw_index_buffer.size()*4);
		m_bgfx_ib_handle = bgfx::createIndexBuffer(mem,  flags);
		GV_ASSERT(bgfx::isValid(m_bgfx_ib_handle));
		return true;
	};

	bool gv_index_buffer_bgfx::uncache()
	{
		if (bgfx::isValid(this->m_bgfx_ib_handle)) {
			bgfx::destroy(m_bgfx_ib_handle);
		}
		return true;
	};

	bool gv_index_buffer_bgfx::recache()
	{
		uncache();
		return precache(NULL);
	}

	bool gv_index_buffer_bgfx::set()
	{
		bgfx::setIndexBuffer(this->m_bgfx_ib_handle); 
		return true;
	}

	//============================================================================================
	//								:
	//============================================================================================
	
	gv_shader_bgfx::gv_shader_bgfx()
	{
		GVM_SET_CLASS(gv_shader_bgfx);
	};

	gv_shader_bgfx::~gv_shader_bgfx()
	{
	};

	bool gv_shader_bgfx::set_origin(gv_resource* pres) {
		this->m_origin = gvt_cast<gv_shader>(pres);
		if (!this->m_origin)
			return false;
		return true;
	}
	bool gv_shader_bgfx::precache(gv_uint flag) {
		m_bgfx_shader = loadShader(*this->m_origin->get_physical_path());
		return true;
	};
	bool gv_shader_bgfx::uncache() {
		if (bgfx::isValid(m_bgfx_shader)) {
			bgfx::destroy(m_bgfx_shader); 
		}
		return true;
	};
	bool gv_shader_bgfx::recache() {
		uncache(); 
		precache(0);
		return true;
	};
	//===============================================
	gv_effect_bgfx::gv_effect_bgfx()
	{
		GVM_SET_CLASS(gv_effect_bgfx);
	};

	gv_effect_bgfx::~gv_effect_bgfx()
	{
	};

	bool gv_effect_bgfx::set_origin(gv_resource* pres) {
		this->m_gv_effect = gvt_cast<gv_effect>(pres);
		if (!this->m_gv_effect)
			return false;
		return true;
	}
	bool gv_effect_bgfx::precache(gv_uint flag) {
		if (!this->m_gv_effect
			|| !this->m_gv_effect->get_vertex_shader()
			|| !this->m_gv_effect->get_vertex_shader())
			return false;
		gv_shader_bgfx * pvs = m_gv_effect->get_vertex_shader()->get_hardware_cache<gv_shader_bgfx>();
		gv_shader_bgfx * pps = m_gv_effect->get_pixel_shader()->get_hardware_cache<gv_shader_bgfx>();
		m_bgfx_shaders[gv_shader::gve_shader_type_vertex] = pvs->m_bgfx_shader; 
		m_bgfx_shaders[gv_shader::gve_shader_type_pixel] = pps->m_bgfx_shader;
		m_bgfx_program = bgfx::createProgram(pvs->m_bgfx_shader, pps->m_bgfx_shader);
		this->get_uniforms(pvs->m_bgfx_shader);
		this->get_uniforms(pps->m_bgfx_shader);
		return true;
	};
	bool gv_effect_bgfx::uncache() {
		if (bgfx::isValid(m_bgfx_program)) {
			bgfx::destroy(m_bgfx_program);
		}
		return true;
	};
	bool gv_effect_bgfx::recache() {
		uncache();
		precache(0);
		return true;
	};

	void	gv_effect_bgfx::get_uniforms(bgfx::ShaderHandle shader)
	{
		bgfx::UniformHandle uniforms[256];
		uint16_t activeUniforms = bgfx::getShaderUniforms(shader, &uniforms[0], 256);
		for (int i = 0; i < activeUniforms; ++i)
		{
			bgfx::UniformInfo info;
			bgfx::getUniformInfo(uniforms[i], info);
			m_bgfx_uniforms.push_back(info); 
		}
	};

#define GVM_PRECACHE(type)                         \
if (resource->is_a(gv_##type::static_class())) \
	return this->precache_##type(gvt_cast< gv_##type >(resource));

	bool gv_renderer_bgfx::precache_resource(gv_resource* resource)
	{
		if (resource->get_hardware_cache())
			return true;
		GVM_PRECACHE(static_mesh);
		GVM_PRECACHE(skeletal_mesh);
		GVM_PRECACHE(texture);
		GVM_PRECACHE(vertex_buffer);
		GVM_PRECACHE(index_buffer);
		GVM_PRECACHE(material);
		GVM_PRECACHE(effect);
		GVM_PRECACHE(shader);
		return false;
	}

	bool gv_renderer_bgfx::precache_index_buffer(gv_index_buffer* index_buffer)
	{
		if (index_buffer->get_hardware_cache())
			return true;
		gv_index_buffer_bgfx* pib =
			this->get_sandbox()->create_nameless_object< gv_index_buffer_bgfx >();
		pib->set_origin(index_buffer);
		index_buffer->set_hardware_cache(pib);
		return pib->precache(NULL);
	};

	bool gv_renderer_bgfx::precache_vertex_buffer(gv_vertex_buffer* vertex_buffer)
	{
		if (!vertex_buffer)
			return false;
		if (vertex_buffer->get_hardware_cache())
			return true;

		if (!vertex_buffer->m_raw_normal.size())
		{
			gvt_cast<gv_static_mesh>(vertex_buffer->get_owner())->rebuild_normal();
		}
		if (!vertex_buffer->m_raw_color.size())
		{
			vertex_buffer->m_raw_color = vertex_buffer->m_raw_normal;
		}
		if (vertex_buffer->m_raw_texcoord0.size() == 0)
		{
			vertex_buffer->m_raw_texcoord0.resize(vertex_buffer->m_raw_pos.size());
			vertex_buffer->m_raw_texcoord0 = gv_vector2(0, 0);
		}
		gv_vertex_buffer_bgfx* pvb =
			this->get_sandbox()->create_nameless_object< gv_vertex_buffer_bgfx >();
		pvb->set_origin(vertex_buffer);
	

		vertex_buffer->set_hardware_cache(pvb);
		return pvb->precache(vertex_buffer->get_biggest_vertex_format());
	};

	bool gv_renderer_bgfx::precache_static_mesh(gv_static_mesh* mesh)
	{
		if (!mesh)
			return false;
		if (!precache_vertex_buffer(mesh->m_vertex_buffer))
		{
			return false;
		}
		if (!precache_index_buffer(mesh->m_index_buffer))
		{
			return false;
		}
		if (mesh->m_diffuse_texture)
		{
			precache_texture(mesh->m_diffuse_texture);
		}
		return true;
	}

	bool gv_renderer_bgfx::precache_skeletal_mesh(gv_skeletal_mesh* mesh)
	{
		precache_static_mesh(mesh->get_t_mesh());
		return true;
	}

	bool gv_renderer_bgfx::precache_texture(gv_texture* tex)
	{
		if (tex->get_hardware_cache())
			return true;
		gv_texture_bgfx* tex_bgfx =	this->get_sandbox()->create_nameless_object< gv_texture_bgfx >();
		tex_bgfx->set_origin(tex);
		tex->set_hardware_cache(tex_bgfx);
		return tex_bgfx->precache(NULL);
		return true;
	}
	
	bool gv_renderer_bgfx::precache_effect(gv_effect* effect)
	{
		if (!effect)
			return true;
		effect->precache(this);
		for (int i = 0; i < effect->get_nb_pass(); i++)
		{
			gv_effect_pass* pass = effect->get_pass(i);
			if (pass->m_name.string().has_prefix("debug_"))
			{
				pass->m_enabled = false;
			}
		}
		if (effect->is_render_state_changed())
		{
			effect->set_is_render_state_changed(false);
			gv_int nb_rt = effect->get_nb_renderable_texture();
			//to do deal with render targets.
			//for (int i = 0; i < nb_rt; i++)
			//{
			//	gv_effect_renderable_texture* ptexture =
			//		effect->get_nth_renderable_target(i);
			//	get_render_target_mgr()->precache(ptexture);
			//}
			//gv_int nb_state = effect->get_nb_render_state();
			//for (int j = 0; j < nb_state; j++)
			//{
			//	gv_effect_render_state* pstate = effect->get_render_state(j);
			//	if (pstate->m_state == bgfxRS_ALPHABLENDENABLE)
			//	{
			//		effect->set_is_transparent(pstate->m_value != 0);
			//	}
			//	if (pstate->m_state == bgfxRS_ZENABLE)
			//	{
			//		effect->set_is_no_depth_writing(pstate->m_value == 0);
			//	}
			//}
		}
		return true;
	};
	bool gv_renderer_bgfx::precache_shader(gv_shader* shader)
	{
		if (shader->get_hardware_cache())
			return true;
		gv_shader_bgfx* shader_bgfx =
			this->get_sandbox()->create_nameless_object< gv_shader_bgfx >();
		shader_bgfx->set_origin(shader);
		shader->set_hardware_cache(shader_bgfx);
		return shader_bgfx->precache(NULL);
	};
	bool gv_renderer_bgfx::precache_material(gv_material* material)
	{
		if (material ) material->precache(this);
		return true;
	};


	GVM_IMP_CLASS(gv_texture_bgfx, gv_resource_cache)
	GVM_END_CLASS;

	GVM_IMP_CLASS(gv_vertex_buffer_bgfx, gv_resource_cache)
	GVM_END_CLASS;

	GVM_IMP_CLASS(gv_index_buffer_bgfx, gv_resource_cache)
	GVM_END_CLASS;

	GVM_IMP_CLASS(gv_shader_bgfx, gv_resource_cache)
	GVM_END_CLASS

	GVM_IMP_CLASS(gv_effect_bgfx, gv_resource_cache)
	GVM_END_CLASS
};