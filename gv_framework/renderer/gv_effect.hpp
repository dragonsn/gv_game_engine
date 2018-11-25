namespace gv
{

#define GV_DOM_FILE "../renderer/gv_effect_struct.h"
#define GVM_DOM_RTTI
#include "gv_data_model_ex.h"

#define GV_DOM_ENUM_IMP
#define GV_DOM_ENUM_NAME gve_effect_semantic
#define GV_DOM_ENUM_FILE "../renderer/gv_effect_semantic.h"
#include "../inc/gv_data_model_enum.h"
//============================================================================================
//								:
//============================================================================================
#pragma GV_REMINDER( \
	"[NOTES]use class reflect to present the parameter , pros: data driven and manipulate parameter with our script function or edit function, still we can move the texture , model , and stream map out of the structure for simplification")

//============================================================================================
//								:
//============================================================================================
gv_effect::gv_effect()
{
	m_is_precached = false;
	m_is_transparent = false;
	m_is_render_state_changed = true;
	m_is_no_depth_writing = false;
	m_is_opengl_effect = false;
	link_class(gv_effect::static_class());
};
gv_effect::~gv_effect()
{
	this->destroy_params();
}
bool gv_effect::set_param(const gv_id& name, const gv_byte* param,
						  gv_int size_of_param, gv_byte* data_source)
{
	if (!this->m_param_struct)
		return false;
	gv_class_info* pcls = this->m_param_struct;
	gv_var_info* pvar = pcls->get_var(name);
	if (!pvar)
	{
		if (this->m_base_effect)
			return this->m_base_effect->set_param(name, param, size_of_param,
												  data_source);
		return false;
	}
	if (!data_source)
	{
		data_source = this->m_param_data.begin();
		GV_ASSERT(this->m_param_data.size() >= pvar->get_end_offset());
	}
	return pvar->set_var_value(data_source, param, size_of_param);
};
bool gv_effect::get_param(const gv_id& name, gv_byte* param,
						  gv_int size_of_param, gv_byte* data_source)
{
	if (!this->m_param_struct)
		return false;
	gv_class_info* pcls = this->m_param_struct;
	gv_var_info* pvar = pcls->get_var(name);
	if (!pvar)
	{
		if (this->m_base_effect)
			return this->m_base_effect->get_param(name, param, size_of_param,
												  data_source);
		return false;
	}
	if (!data_source)
	{
		data_source = this->m_param_data.begin();
		GV_ASSERT(this->m_param_data.size() >= pvar->get_end_offset());
	}
	return pvar->get_var_value(data_source, param, size_of_param);
};

gv_byte* gv_effect::get_param(const gv_id& name)
{
	if (!this->m_param_struct)
		return NULL;
	gv_class_info* pcls = this->m_param_struct;
	gv_var_info* pvar = pcls->get_var(name);
	if (!pvar)
	{
		if (this->m_base_effect)
			return this->m_base_effect->get_param(name);
		return NULL;
	}
	GV_ASSERT(this->m_param_data.size() >= pvar->get_end_offset());
	return this->m_param_data.begin() + pvar->get_offset();
};

gv_class_info* gv_effect::get_param_struct_info()
{
	return this->m_param_struct;
};
void gv_effect::set_param_struct_info(gv_class_info* pcls)
{
	this->destroy_params();
	this->m_param_struct = pcls;
	this->create_params();
};
//============================================================================================
void gv_effect::destroy_params()
{
	if (this->m_param_struct && this->m_param_data.size())
	{
		GV_ASSERT(this->m_param_struct->get_size() == this->m_param_data.size());
		this->m_param_struct->destroy(this->m_param_data.begin());
		this->m_param_data.clear();
	}
};
void gv_effect::create_params()
{
	this->destroy_params();
	if (this->m_param_struct)
	{
		this->m_param_data.resize(this->m_param_struct->get_size());
		gv_class_info* pcls = this->m_param_struct;
		gv_byte* obj = this->m_param_data.begin();
		pcls->construct(obj);
		pcls->load_default(obj, this->m_param_data.size());
	}
};

void gv_effect::load_default_params()
{
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i])
			this->m_sub_effects[i]->load_default_params();
	}
	this->create_params();
}

void gv_effect::add_param(gv_var_info* pvar)
{
	pvar->initialize();
	this->m_param_struct->add_var(pvar);
};

int gv_effect::get_nb_pass()
{
	return this->m_passes.size();
};

gv_effect_pass* gv_effect::get_pass(int index)
{
	return &this->m_passes[index];
};

void gv_effect::add_pass(const gv_effect_pass& pass)
{
	this->m_passes.push_back(pass);
};

int gv_effect::get_nb_sub_effect()
{
	return this->m_sub_effects.size();
};

gv_effect* gv_effect::get_sub_effect(const gv_id& name)
{
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i] && this->m_sub_effects[i]->get_name_id() == name)
			return this->m_sub_effects[i];
	}
	return NULL;
};

void gv_effect::add_sub_effect(gv_effect* effect)
{
	this->m_sub_effects.push_back(effect);
};

void gv_effect::add_stream_map(const gv_effect_stream_map& stream_map)
{
	this->m_stream_maps.push_back(stream_map);
};

gv_effect_stream_map* gv_effect::get_stream_map(const gv_id& id)
{
	for (int i = 0; i < m_stream_maps.size(); i++)
	{
		if (m_stream_maps[i].m_name == id)
			return &m_stream_maps[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_stream_map(id);
	return NULL;
};
gv_uint gv_effect::get_stream_decl(gv_effect_stream_map* pmap)
{
	gv_uint stream_decl = 0;
	for (int i = 0; i < pmap->m_map.size(); i++)
	{
		gv_effect_stream_channel& ch = pmap->m_map[i];
		stream_decl |= gv_get_vertex_flag_from_usage((gve_vertex_usage)ch.m_usage,
													 ch.m_usage_index);
	}
	return stream_decl;
};
gv_uint gv_effect::get_current_stream_decl()
{
	gv_uint stream_decl = 0;
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i])
			stream_decl |= this->m_sub_effects[i]->get_current_stream_decl();
	}
	gv_effect_stream_map* stream_map = this->get_stream_map(m_active_stream_map);
	if (stream_map)
		stream_decl |= this->get_stream_decl(stream_map);
	return stream_decl;
};

void gv_effect::add_model(const gv_effect_model& model)
{
	this->m_models.push_back(model);
};

gv_effect_model* gv_effect::get_model(const gv_id& id)
{
	for (int i = 0; i < this->m_models.size(); i++)
	{
		if (m_models[i].m_name == id)
			return &m_models[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_model(id);
	return NULL;
};

void gv_effect::add_texture_sample(gv_effect_texture& texture)
{
	if (!texture.m_texture)
	{
		texture.m_file_name.replace_all("\\", "/");
		gv_string_tmp fn = *this->get_file_name();
		// texture.m_texture=
		texture.m_texture =
			get_sandbox()->create_object< gv_texture >(texture.m_name, this);
		gv_string s = *get_file_name();
		gv_string dir = get_sandbox()->get_file_manager()->get_parent_path(s);
		fn = dir;
		fn << "/" << texture.m_file_name;
		texture.m_texture->set_file_name(fn);
		gv_texture* ptex = texture.m_texture;
		ptex->set_cube_texture(texture.m_is_cubemap);
		ptex->set_volume_texture(texture.m_is_3dmap);
	}
	this->m_textures.push_back(texture);
};

gv_effect_texture* gv_effect::get_texture_sample(const gv_id& id)
{
	for (int i = 0; i < this->m_textures.size(); i++)
	{
		if (m_textures[i].m_name == id)
			return &m_textures[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_texture_sample(id);
	return NULL;
};

void gv_effect::add_texture_object(
	const gv_effect_texture_object& texture_obj)
{
	this->m_texture_objs.push_back(texture_obj);
};

gv_effect_texture_object* gv_effect::get_texture_object(const gv_id& id)
{
	for (int i = 0; i < this->m_texture_objs.size(); i++)
	{
		if (m_texture_objs[i].m_name == id)
			return &m_texture_objs[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_texture_object(id);
	return NULL;
};

void gv_effect::add_camera(const gv_effect_camera& camera)
{
	this->m_cameras.push_back(camera);
};

gv_effect_camera* gv_effect::get_camera(const gv_id& id)
{
	for (int i = 0; i < m_cameras.size(); i++)
	{
		if (m_cameras[i].m_name == id)
			return &m_cameras[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_camera(id);
	return NULL;
};

void gv_effect::add_param_info(gv_effect_param_info& info)
{
	if (!info.m_semantic.is_empty())
	{
		info.m_semantic_idx = gv_get_enum_gve_effect_semantic(*info.m_semantic);
	}
	else
	{
		info.m_semantic_idx =
			gv_get_enum_gve_effect_semantic(*info.m_name.string());
	}
	if (info.m_semantic_idx == gve_effect_semantic_last)
	{
		info.m_semantic_idx = 0;
	}
	else
	{ // DEBUG ONLY
		gve_effect_semantic e = (gve_effect_semantic)info.m_semantic_idx;
		info.m_semantic_idx = info.m_semantic_idx;
	}
	if (info.m_name.string().has_prefix("static_") && !info.m_semantic_idx)
	{
		info.m_is_static = true;
	}
	this->m_param_extra_info.push_back(info);
};

gv_effect_param_info* gv_effect::get_param_info(const gv_id& id)
{
	for (int i = 0; i < m_param_extra_info.size(); i++)
	{
		if (m_param_extra_info[i].m_name == id)
			return &m_param_extra_info[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_param_info(id);
	return NULL;
};

gv_effect_renderable_texture*
gv_effect::get_renderable_texture(const gv_id& id)
{
	for (int i = 0; i < m_render_targets.size(); i++)
	{
		if (m_render_targets[i].m_name == id)
			return &m_render_targets[i];
	}
	if (this->m_base_effect)
		return this->m_base_effect->get_renderable_texture(id);
	return NULL;
};

void gv_effect::add_renderable_texture(gv_effect_renderable_texture& texture)
{
	this->m_render_targets.push_back(texture);
};

void gv_effect::set_active_camera(const gv_id& name)
{
	this->m_active_camera = name;
};

void gv_effect::set_active_model(const gv_id& name)
{
	this->m_active_model = name;
};

void gv_effect::set_active_render_target(const gv_effect_render_target& rt)
{
	this->m_active_render_target = rt;
};

gv_shader* gv_effect::get_pixel_shader()
{
	return m_pixel_shader;
};

gv_shader* gv_effect::get_vertex_shader()
{
	return m_vertex_shader;
};

void gv_effect::set_pixel_shader(gv_shader* s)
{
	this->m_pixel_shader = s;
};

void gv_effect::set_vertex_shader(gv_shader* s)
{
	this->m_vertex_shader = s;
};

gv_effect* gv_effect::get_renderable_effect(const gv_id& name)
{
	if (this->m_passes.size() && (name == gv_id_any || get_name_id() == name))
		return this;
	gv_effect* effect = NULL;
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i])
			effect = this->m_sub_effects[i]->get_renderable_effect(name);
		if (effect)
			return effect;
	}
	return NULL;
};

bool gv_effect::precache(gv_renderer* render)
{
	if (this->m_vertex_shader)
		render->precache_shader(m_vertex_shader);
	if (this->m_pixel_shader)
		render->precache_shader(m_pixel_shader);
	if (this->is_precached())
		return true;
	for (int i = 0; i < this->m_textures.size(); i++)
	{
		render->precache_texture(this->m_textures[i].m_texture);
	}
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i])
			render->precache_effect(this->m_sub_effects[i]);
	}
	m_is_precached = true;
	return true;
}

gv_int gv_effect::get_nb_render_state()
{
	return m_render_states.size();
};
gv_effect_render_state* gv_effect::get_render_state(gv_int index)
{
	return &m_render_states[index];
};

void gv_effect::query_renderable_effect(
	gvt_array_cached< gv_effect*, 256 >& result)
{
	for (int i = 0; i < this->m_sub_effects.size(); i++)
	{
		if (this->m_sub_effects[i])
			m_sub_effects[i]->query_renderable_effect(result);
	}
	if (this->m_passes.size())
		result.push_back(this);
};

GVM_IMP_CLASS(gv_effect, gv_resource)
GVM_VAR(gvt_array< gv_effect_param_info >, m_param_extra_info)
GVM_VAR(gvt_ref_ptr< gv_class_info >, m_param_struct)
GVM_VAR(gvt_array< gv_byte >, m_param_data)
GVM_VAR(gvt_array< gv_effect_pass >, m_passes)
GVM_VAR(gvt_ptr< gv_effect_project >, m_my_project)
GVM_VAR(gvt_ptr< gv_effect >, m_base_effect)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_effect > >, m_sub_effects)
GVM_VAR(gvt_ptr< gv_effect >, m_active_sub_effect)
GVM_VAR(gvt_array< gv_effect_stream_map >, m_stream_maps)
GVM_VAR(gvt_array< gv_effect_model >, m_models)
GVM_VAR(gvt_array< gv_effect_texture >, m_textures)
GVM_VAR(gvt_array< gv_effect_texture_object >, m_texture_objs)
GVM_VAR(gvt_array< gv_effect_camera >, m_cameras)
GVM_VAR(gvt_array< gv_effect_renderable_texture >, m_render_targets)
GVM_VAR(gvt_array< gv_effect_render_state >, m_render_states)
GVM_VAR(gv_id, m_active_stream_map)
GVM_VAR(gv_id, m_active_camera)
GVM_VAR(gv_id, m_active_model)
GVM_VAR(gv_effect_render_target, m_active_render_target)
GVM_VAR(gvt_ref_ptr< gv_shader >, m_pixel_shader)
GVM_VAR(gvt_ref_ptr< gv_shader >, m_vertex_shader)
GVM_VAR(gv_bool, m_is_transparent)
GVM_VAR(gv_bool, m_is_no_depth_writing)
GVM_VAR(gv_bool, m_is_opengl_effect)
GVM_END_CLASS

//============================================================================================
//								:
//============================================================================================
gv_effect_project::gv_effect_project()
{
	GVM_SET_CLASS(gv_effect_project);
};
gv_effect_project::~gv_effect_project(){};
gv_effect* gv_effect_project::add_effect(const char* name,
										 gv_effect* base_effect)
{
	gv_object* owner = this;
	if (base_effect)
		owner = base_effect;
	gv_effect* effect =
		get_sandbox()->create_object< gv_effect >(gv_id(name), owner);
	gv_class_info* pclass = get_sandbox()->create_object< gv_class_info >(effect);
	effect->set_param_struct_info(pclass);
	effect->set_file_name(*this->get_file_name());
	if (!base_effect)
		this->m_base_effect = effect;
	else
		base_effect->add_sub_effect(effect);
	effect->set_base_effect(base_effect);
	return effect;
};

GVM_IMP_CLASS(gv_effect_project, gv_resource);
GVM_VAR(gvt_ref_ptr< gv_effect >, m_base_effect)
GVM_END_CLASS

#pragma GV_REMINDER( \
	"[PITFALL]	:rfx->set to row major matrix , change the mul direction if necessary, set the view_project semantic to world_view_projection!")
}