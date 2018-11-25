#pragma once
namespace gv
{
class gv_shader;
class gv_effect_project;
class gv_model;
class gv_effect;

#define GV_DOM_ENUM_DCL
#define GV_DOM_ENUM_NAME gve_effect_semantic
#define GV_DOM_ENUM_FILE "../renderer/gv_effect_semantic.h"
#include "../inc/gv_data_model_enum.h"

#define GV_DOM_FILE "../renderer/gv_effect_struct.h"
#define GVM_DOM_DECL
#define GVM_DOM_IMP_COPY
#include "../inc/gv_data_model_ex.h"
//=======================================================================>
// reference to RenderMonkey Effect Structure.
//=======================================================================>

class gv_effect : public gv_resource
{
public:
	friend class gv_com_effect_renderer;
	GVM_DCL_CLASS(gv_effect, gv_resource);
	gv_effect();
	~gv_effect();
	bool set_param(const gv_id& name, const gv_byte* param, gv_int size_of_param,  gv_byte* data_source = NULL);
	template < class T >
	inline bool set_param(const gv_id& name, const T& t,
						  gv_byte* data_source = NULL)
	{
		return set_param(name, gvt_byte_ptr(t), sizeof(t), data_source);
	}
	bool get_param(const gv_id& name, gv_byte* param, gv_int size_of_param,
				   gv_byte* data_source = NULL);
	gv_byte* get_param(const gv_id& name);
	template < class T >
	inline bool get_param(const gv_id& name, T& t, gv_byte* data_source = NULL)
	{
		return get_param(name, gvt_byte_ptr(t), sizeof(t), data_source);
	}
	gv_class_info* get_param_struct_info();
	void set_param_struct_info(gv_class_info*);
	void load_default_params();

	int get_nb_pass();
	gv_effect_pass* get_pass(int index);
	void add_pass(const gv_effect_pass&);

	int get_nb_sub_effect();
	gv_effect* get_sub_effect(const gv_id& name);
	void add_sub_effect(gv_effect* effect);
	gv_effect* get_base_effect()
	{
		return m_base_effect;
	}

	void add_stream_map(const gv_effect_stream_map& stream_map);
	gv_effect_stream_map* get_stream_map(const gv_id& id);
	gv_uint get_stream_decl(gv_effect_stream_map*);
	gv_uint get_current_stream_decl();
	void add_model(const gv_effect_model& model);
	gv_effect_model* get_model(const gv_id& id);
	void add_texture_sample(gv_effect_texture& texture);
	gv_effect_texture* get_texture_sample(const gv_id& id);
	void add_texture_object(const gv_effect_texture_object& texture_obj);
	gv_effect_texture_object* get_texture_object(const gv_id& id);
	void add_camera(const gv_effect_camera& camera);
	gv_effect_camera* get_camera(const gv_id& id);
	void add_param_info(gv_effect_param_info& info);
	gv_effect_param_info* get_param_info(const gv_id& id);

	gv_effect_renderable_texture* get_renderable_texture(const gv_id& id);
	void add_renderable_texture(gv_effect_renderable_texture& texture);
	gv_int get_nb_renderable_texture()
	{
		return m_render_targets.size();
	}
	gv_effect_renderable_texture* get_nth_renderable_target(gv_int i)
	{
		return &m_render_targets[i];
	}

	void set_active_camera(const gv_id& name);
	void set_active_model(const gv_id& name);
	void set_active_render_target(const gv_effect_render_target& name);
	void set_active_stream_map(const gv_id& name)
	{
		m_active_stream_map = name;
	};

	gv_shader* get_pixel_shader();
	gv_shader* get_vertex_shader();
	void set_pixel_shader(gv_shader*);
	void set_vertex_shader(gv_shader*);
	void set_base_effect(gv_effect* p)
	{
		m_base_effect = p;
	}
	void add_render_state(const gv_effect_render_state& c)
	{
		m_render_states.push_back(c);
	}
	gv_int get_nb_render_state();
	gv_effect_render_state* get_render_state(gv_int index);

	gv_effect* get_renderable_effect(const gv_id& name = gv_id_any);
	bool precache(gv_renderer*);
	bool is_precached()
	{
		return m_is_precached;
	};
	void query_renderable_effect(gvt_array_cached< gv_effect*, 256 >& result);
	const gv_effect_render_target& get_active_render_target()
	{
		return m_active_render_target;
	};
	bool is_transparent() const
	{
		return m_is_transparent;
	}
	void set_is_transparent(gv_bool b)
	{
		m_is_transparent = b;
	}
	bool is_no_depth_writing() const
	{
		return m_is_no_depth_writing;
	}
	void set_is_no_depth_writing(gv_bool b)
	{
		m_is_no_depth_writing = b;
	}

	bool is_render_state_changed()
	{
		return m_is_render_state_changed;
	}
	void set_is_render_state_changed(gv_bool b)
	{
		m_is_render_state_changed = b;
	}
	bool is_opengl_effect()
	{
		return m_is_opengl_effect;
	}

protected:
	void add_param(gv_var_info* pvar);
	void destroy_params();
	void create_params();

protected:
	gvt_array< gv_effect_param_info > m_param_extra_info;
	gvt_ref_ptr< gv_class_info > m_param_struct;
	gvt_array< gv_byte > m_param_data;
	gvt_array< gv_effect_pass > m_passes;
	gvt_ptr< gv_effect_project > m_my_project;
	gvt_ptr< gv_effect > m_base_effect;
	gvt_array< gvt_ref_ptr< gv_effect > > m_sub_effects;
	gvt_ptr< gv_effect > m_active_sub_effect;

	gvt_array< gv_effect_stream_map > m_stream_maps;
	gvt_array< gv_effect_model > m_models;
	gvt_array< gv_effect_texture > m_textures;
	gvt_array< gv_effect_texture_object > m_texture_objs;
	gvt_array< gv_effect_camera > m_cameras;
	gvt_array< gv_effect_renderable_texture > m_render_targets;
	gv_id m_active_stream_map;
	gv_id m_active_camera;
	gv_id m_active_model;
	gv_effect_render_target m_active_render_target;

	gvt_ref_ptr< gv_shader > m_pixel_shader;
	gvt_ref_ptr< gv_shader > m_vertex_shader;
	gvt_array< gv_effect_render_state > m_render_states;
	gv_bool m_is_precached;
	gv_bool m_is_transparent;
	gv_bool m_is_no_depth_writing;
	gv_bool m_is_render_state_changed;
	gv_bool m_is_opengl_effect;
};

class gv_effect_project : public gv_resource
{
public:
	GVM_DCL_CLASS(gv_effect_project, gv_resource);
	gv_effect_project();
	~gv_effect_project();
	gv_effect* add_effect(const char* name, gv_effect* base_effect = NULL);
	gv_effect* get_base_effect()
	{
		return m_base_effect;
	}
protected:
	gvt_ref_ptr< gv_effect > m_base_effect; // mainly store some parameters.
};

}