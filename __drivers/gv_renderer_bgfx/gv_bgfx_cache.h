//=====================================================================================>>
namespace gv
{
	class gv_texture_bgfx : public gv_resource_cache
	{
	public:
		GVM_DCL_CLASS(gv_texture_bgfx, gv_resource_cache);
		gv_texture_bgfx();
		~gv_texture_bgfx();
		virtual bool set_origin(gv_resource*);
		virtual bool precache(gv_uint flag);
		virtual bool uncache();
		virtual bool recache();
		gv_string_tmp get_cached_texture_name();
		bgfx::TextureHandle m_bgfx_texture_handle;
		gvt_ptr< gv_texture > m_gv_texture;
	};

	//=====================================================================================>>
	class gv_bgfx_vb_cache : public gv_refable
	{
		friend class gv_vertex_buffer_bgfx;
	public:
		gv_bgfx_vb_cache();
		~gv_bgfx_vb_cache();
		bool create(gv_uint vertex_usage_flag, gv_vertex_buffer* pvb);
		gv_uint m_precache_tag;
		gv_uint m_count;
		gv_uint m_stream_number;
		gv_uint m_vertex_format;
		gv_uint m_vertex_size;
		bgfx::VertexBufferHandle m_bgfx_vb_handle;             // static vertex buffer handle
		bgfx::VertexDecl  m_bgfx_vertex_decl;               // vertex declaration
		gvt_array<gv_byte> m_memory_buffer;
	};


	//=====================================================================================>>
	class gv_vertex_buffer_bgfx : public gv_resource_cache
	{
	public:
		GVM_DCL_CLASS(gv_vertex_buffer_bgfx, gv_resource_cache);
		gv_vertex_buffer_bgfx();
		~gv_vertex_buffer_bgfx();
		virtual bool set_origin(gv_resource*);
		virtual bool precache(gv_uint flag);
		virtual bool uncache();
		virtual bool recache();
		bool set(gv_uint format_flag);
		bool set(); 
		inline gv_uint get_vertex_count()
		{
			return this->m_count;
		}
	protected:
		gv_bgfx_vb_cache * get_cache(gv_uint format_flag);
		gv_int m_stream_id;
		gv_int m_count;
		gv_vertex_buffer* m_gv_vb;
		gvt_array< gvt_ref_ptr< gv_bgfx_vb_cache > > m_caches;
	};

	//=====================================================================================>>
	class gv_index_buffer_bgfx : public gv_resource_cache
	{
	public:
		GVM_DCL_CLASS(gv_index_buffer_bgfx, gv_resource_cache)
			gv_index_buffer_bgfx();
		~gv_index_buffer_bgfx();
		virtual bool set_origin(gv_resource* pres);
		virtual bool precache(gv_uint flag);
		virtual bool uncache();
		virtual bool recache();
		bool set();
		gvt_ptr< gv_index_buffer>  m_gv_ib;
		bgfx::IndexBufferHandle m_bgfx_ib_handle;     // static index buffer handle
	};


	//============================================================================================
	class gv_shader_bgfx : public gv_resource_cache
	{
	public:
		friend class gv_com_effect_renderer;
		friend class gv_renderer_bgfx;
		friend class gv_effect_bgfx;
	public:
		GVM_DCL_CLASS(gv_shader_bgfx, gv_resource_cache)
		gv_shader_bgfx();
		~gv_shader_bgfx();
		virtual bool set_origin(gv_resource* pres) override;
		virtual bool precache(gv_uint flag) override;
		virtual bool uncache()override;
		virtual bool recache()override;
		bool set();
		
	protected:
		bool compile_shader();
		gv_int m_vertex_usage;
		gvt_ptr<gv_shader> m_origin;
		bgfx::ShaderHandle m_bgfx_shader;
	};

	//============================================================================================
	class gv_effect_bgfx : public gv_resource_cache
	{
	public:
		GVM_DCL_CLASS(gv_effect_bgfx, gv_resource_cache)
		gv_effect_bgfx(); 
		~gv_effect_bgfx();
		virtual bool set_origin(gv_resource* pres) override;
		virtual bool precache(gv_uint flag) override;
		virtual bool uncache()override;
		virtual bool recache()override;
		void	bind(gv_material * material);
		bgfx::ProgramHandle m_bgfx_program;
	protected:
		gv_uint	get_vertex_decl_flag();
		void	get_uniforms(bgfx::ShaderHandle shader);
		bgfx::ShaderHandle get_bgfx_shader(gv_shader::shader_type);
		bgfx::UniformHandle  get_uniform(const gv_id & name);
		gvt_ptr<gv_effect> m_gv_effect;
		gvt_array<bgfx::UniformInfo> m_bgfx_uniforms;
		bgfx::ShaderHandle m_bgfx_shaders[gv_shader::gve_shader_type_count];
		
	}; 

}