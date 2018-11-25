#pragma once
namespace gv
{
#define GV_MAX_GPU_SKIN_BONE_NUMBER 64
#define GV_GPU_SKIN_BONE_REGISTER 64
GV_STATIC_ASSERT(GV_GPU_SKIN_BONE_REGISTER + GV_MAX_GPU_SKIN_BONE_NUMBER * 3 <=
				 256);

class gv_effect;
class gv_shader;
class gv_render_target_mgr_d3d;
class gv_post_effect_mgr_d3d;
class gv_texture_d3d;

extern D3DFORMAT gv_to_d3d_format(gve_pixel_format format);
extern

	enum gve_vertex_decl_d3d {
		e_v_decl_constant_color,
		e_v_decl_vertex_color,
		e_v_decl_textured,
		e_v_decl_textured_constant_color,
		e_v_decl_textured_vertex_color,
		e_v_decl_max,
	};

const static gv_uint gvc_render_priority_last = 2000;
const static gv_uint gvc_render_priority_after_bloom = 300;
const static gv_uint gvc_render_priority_after_outline = 250;
const static gv_uint gvc_render_priority_post_process = 200;
const static gv_uint gvc_render_priority_normal = 100;

//===========================================================================================
//  main renderer interface
//===========================================================================================
class gv_renderer_d3d9_data;
class gv_renderer_d3d9 : public gv_renderer
{
public:
	friend class gv_debug_renderer_d3d_imp;
	friend class gvp_memory_renderer_temp;
	//===========================================================================================
	GVM_DCL_CLASS(gv_renderer_d3d9, gv_renderer);
	gv_renderer_d3d9();
	~gv_renderer_d3d9();
	//===========================================================================================
	bool tick(gv_float dt);
	gv_int on_event(gv_object_event* pevent);
	bool do_synchronization();
	//===========================================================================================
	void init_d3d9();
	void destroy_d3d9();
	bool do_render(gv_float dt);
	void do_render_pass(gv_float dt, gv_int pass);
	void do_render_post_effect(gv_float dt);
	//===========================================================================================
	bool precache_resource(gv_resource* resource);
	bool precache_static_mesh(gv_static_mesh*);
	bool precache_skeletal_mesh(gv_skeletal_mesh*);
	bool precache_texture(gv_texture*);
	bool precache_index_buffer(gv_index_buffer*);
	bool precache_vertex_buffer(gv_vertex_buffer*);
	bool precache_effect(gv_effect*);
	bool precache_shader(gv_shader*);
	bool precache_material(gv_material*);
	//===========================================================================================
	void* get_param_from_semantric(gve_effect_semantic e);
	void* get_param_from_material(const gv_id& param_name,
								  gve_shader_data_type type);
	gv_texture* get_texture_from_material(const gv_id& tex_name);
	gv_texture* get_tex_base()
	{
		return m_tex_base;
	}
	gv_texture* get_tex_normal()
	{
		return m_tex_normal;
	}
	gv_texture* get_tex_env()
	{
		return m_tex_env;
	}
	gv_com_camera* get_main_camera()
	{
		return m_main_camera;
	}
	void set_tex_base(gv_texture* p)
	{
		m_tex_base = p;
	}
	void set_tex_normal(gv_texture* p)
	{
		m_tex_normal = p;
	}
	void set_tex_env(gv_texture* p)
	{
		m_tex_env = p;
	}
	void upload_skin_matrices();
	void add_shader_macro(const char* macro);
	void clear_shader_macro();
	gv_uint set_pass_effect(gv_effect* effect, bool do_skin, gv_material* pmat,
							gv_int pass = 0);
	void reset_effect_cache()
	{
		m_last_pass_effect = NULL;
	}
	void set_texture_to_d3d(gv_texture* texture, gv_int stage);
	gv_texture_d3d* get_shared_texture(const gv_string_tmp& file_name,
									   gv_texture* tex);
	//===========================================================================================
	HWND get_d3d_window()
	{
		return m_window_handle;
	}
	//===========================================================================================
	gv_render_target_mgr_d3d* get_render_target_mgr()
	{
		return m_render_target_mgr;
	}
	gv_post_effect_mgr_d3d* get_post_effect_mgr()
	{
		return m_post_effect_mgr;
	}
	static gv_renderer_d3d9* static_get();
	gv_component_renderer* get_com_renderer(const gv_id& id);
	gv_component_renderer* get_batch_renderer();
	gv_component_renderer* get_effect_renderer();
	gv_component_renderer* get_terrain_renderer();
	gv_texture* get_default_texture();
	gvi_ui_manager* get_ui_mgr();
	//===========================================================================================
	gv_texture* get_color_buffer_tex();
	gv_texture* get_depth_buffer_tex();
	gv_texture* get_shadow_map_tex(int idx = 0);
	gv_texture* get_shadow_map_depth_buf();
	gv_texture* get_full_size_buffer(int idx = 0);
	void set_common_vertex_decl(gve_vertex_decl_d3d decl);
	void kick_textured_quad(gv_texture*, const gv_recti& rect);
	gv_vector2i get_screen_size();
	gv_bool is_rendering_z_pass();
	//===========================================================================================
	gv_vector3 get_sun_dir();
	gv_vector3 get_main_camera_pos();
	gv_vector3 get_main_camera_dir();
	void set_static_shader_param_for_post_process();

protected:
	void add_a_renderable(gv_com_graphic* ptr);
	void remove_a_renderable(gv_com_graphic* ptr);
	void render_a_renderable(gv_com_graphic* ptr, gv_int pass, gv_float dt);
	void precache_renderable(gv_com_graphic* ptr);
	void uncache_renderable(gv_com_graphic* ptr);
	void build_main_visibility();
	bool test_a_renderable_for_visibility(gv_com_graphic* ptr);

protected:
	void render_z_pass();
	void render_shadow_map();
	gv_renderer_d3d9_data* get_private_data()
	{
		return m_private_data.ptr();
	}

protected:
	gv_float m_near_clip;
	gv_float m_far_clip;
	gv_vector3 m_camera_pos;
	gv_vector3 m_camera_dir;

public:
	gvt_array_cached< gv_vector4, GV_MAX_GPU_SKIN_BONE_NUMBER * 3 > m_bone_matrices;
	gv_float m_current_unified_life_time;
	gv_float m_current_life_time;
	//===========================================================================================
protected:
	HWND m_window_handle;
	gvt_object_type_list< gv_com_graphic > m_components;
	gvt_array_cached< gv_com_graphic*, 2048 > m_visible_components;
	gvt_hash_map< gv_id, gvt_ref_ptr< gv_component_renderer > > m_renderer_map;
	gvt_array< gvt_ref_ptr< gv_component_renderer > > m_com_renderers;
	gvt_ptr< gvi_ui_manager > m_ui_mgr;
	gvt_ref_ptr< gv_com_camera > m_main_camera;
	gvt_ref_ptr< gv_texture > m_default_texture;
	gv_int m_current_render_frame;
	gvt_ref_ptr< gv_texture > m_tex_base;
	gvt_ref_ptr< gv_texture > m_tex_normal;
	gvt_ref_ptr< gv_texture > m_tex_env;
	gvt_ref_ptr< gv_com_camera > m_main_camera_copy;
	gvt_array_cached< gv_string, 32 > m_shader_precompile_macro;
	gv_ulong m_visible_test_tag;
	gvt_memory_stack< 65536 > m_mem_stack;
	gvt_ptr< gv_effect > m_last_pass_effect;
	gvt_ptr< gv_material > m_last_material;
	gvt_ref_ptr< gv_entity > m_camera_entity;
	gvt_ref_ptr< gv_render_target_mgr_d3d > m_render_target_mgr;
	gvt_ref_ptr< gv_post_effect_mgr_d3d > m_post_effect_mgr;
	gvt_ref_ptr< gv_renderer_d3d9_data > m_private_data;
	gvt_hash_map< gv_string64, gvt_ref_ptr< gv_texture_d3d > > m_texture_map;
};

inline gv_renderer_d3d9* get_renderer_d3d9()
{
	return gv_renderer_d3d9::static_get();
}

class gvp_memory_renderer_temp : public gvp_memory_default
{
public:
	gvp_memory_renderer_temp(){};

	inline void* alloc(size_t size)
	{
		return get_renderer_d3d9()->m_mem_stack.alloc(size);
	};
	inline void free(void* p)
	{
		return get_renderer_d3d9()->m_mem_stack.free(p);
	};
	inline void* realloc(void* p, size_t size)
	{
		return get_renderer_d3d9()->m_mem_stack.realloc(p, size);
	};
};
}