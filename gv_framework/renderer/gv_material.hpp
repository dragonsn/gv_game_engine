namespace gv
{
gv_material::gv_material()
{
	m_diffuse_color = gv_colorf::GREY_B();
	m_specular_color = gv_colorf::WHITE();
	m_ambient_color = gv_colorf::GREY_D();
	m_opacity = 1.0f;
	m_is_precached = false;
	link_class(gv_material::static_class());
}

gv_material::~gv_material()
{
}

gv_effect* gv_material::get_effect()
{
	return this->m_effect;
};

bool gv_material::precache(gv_renderer* rnd)
{
	if (this->is_precached())
		return true;
	for (int i = 0; i < this->m_material_textures.size(); i++)
	{
		gv_material_tex& tex = this->m_material_textures[i];
		if (tex.m_id.is_empty())
			continue;
		if (!tex.m_texture)
		{
			tex.m_texture = get_sandbox()->create_object< gv_texture >(tex.m_id, this);
		}
		rnd->precache_texture(tex.m_texture);
	}
	if (this->m_effect)
	{
		gv_effect* effect = this->m_effect;
		while (effect->get_base_effect())
			effect = effect->get_base_effect();
		rnd->precache_effect(effect);
	}
	for (int i = 0; i < this->m_sub_material.size(); i++)
	{
		rnd->precache_material(this->m_sub_material[i]);
	}
	m_is_precached = true;
	return true;
}

void gv_material::uncache()
{
	m_is_precached = false;
	super::uncache();
	for (int i = 0; i < this->m_sub_material.size(); i++)
	{
		this->m_sub_material[i]->uncache();
	}
	GVM_POST_EVENT(render_reload_shader_cache, render, );
};

bool gv_material::is_precached()
{
	return this->m_is_precached;
};

void gv_material::add_texture(const char* file_name, const char* id)
{
	gv_id tex_id;
	if (id)
		tex_id = id;
	else
		tex_id = get_default_texture_id(this->m_material_textures.size());
	gv_material_tex* ptex = m_material_textures.add_dummy();
	ptex->m_texture =
		this->get_sandbox()->create_object< gv_texture >(tex_id, this);
	ptex->m_texture->set_file_name(file_name);
	ptex->m_id = tex_id;
	ptex->m_use_tiling = true;
	return;
}

gv_int gv_material::get_nb_sub_material()
{
	return m_sub_material.size();
};
void gv_material::set_nb_sub_material(gv_int i)
{
	m_sub_material.resize(i);
};
void gv_material::add_sub_material(gv_material* p)
{
	m_sub_material.push_back(p);
	p->set_owner(this);
};

gv_material* gv_material::get_sub_material(gv_int idx)
{
	if (!m_sub_material.size() && idx == 0)
		return NULL;
	if (idx < m_sub_material.size())
		return m_sub_material[idx];
	else
		return NULL;
};

void gv_material::set_sub_material(gv_int idx, gv_material* mat)
{
	m_sub_material[idx] = mat;
	mat->set_owner(this);
};

gv_id gv_material::get_default_texture_id(gv_int idx)
{
	return gv_id_g_TexBase;
};

gv_material_param_float4* gv_material::get_param_float4(const gv_id& name)
{
	for (int i = 0; i < this->m_material_float_params.size(); i++)
	{
		if (m_material_float_params[i].m_id == name)
		{
			return &m_material_float_params[i];
		}
	}
	return NULL;
};

void gv_material::update_param_float4(gv_material_param_float4 param)
{
	gv_material_param_float4* ret = get_param_float4(param.m_id);
	if (!ret)
		this->m_material_float_params.push_back(param);
	else
		*ret = param;
}

gv_material_tex* gv_material::get_material_texture(const gv_id& texture)
{
	for (int i = 0; i < m_material_textures.size(); i++)
	{
		if (m_material_textures[i].m_id == texture)
			return &m_material_textures[i];
	}
	return NULL;
};

void gv_material::update_material_texture(gv_material_tex& p)
{
	gv_material_tex* tex = get_material_texture(p.m_id);
	if (tex)
	{
		*tex = p;
	}
	else
		add_material_texture(p);
};
void gv_material::set_effect(gv_effect* e)
{
	m_effect = e;
};

gv_material* gv_material::clone(gv_sandbox* sandbox)
{
	gv_material* pmat = sandbox->clone_object(this);
	for (int i = 0; i < m_sub_material.size(); i++)
	{
		//! must not have any circular reference here!!
		gv_material* sub_mat = get_sub_material(i);
		set_sub_material(i, sub_mat->clone(sandbox));
	}
	return pmat;
}

GVM_IMP_CLASS(gv_material, gv_resource)
GVM_VAR(gvt_array< gv_material_tex >, m_material_textures)
GVM_VAR(gv_colorf, m_diffuse_color)
GVM_VAR(gv_colorf, m_specular_color)
GVM_VAR(gv_colorf, m_ambient_color)
GVM_VAR(gv_float, m_opacity)
GVM_VAR(gvt_ref_ptr< gv_effect >, m_effect)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_material > >, m_sub_material)
GVM_VAR(gvt_array< gv_material_param_float4 >, m_material_float_params)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_material_tex)
GVM_VAR(gvt_ref_ptr< gv_texture >, m_texture)
GVM_VAR(gv_id, m_id)
GVM_VAR(gv_bool, m_use_tiling)
GVM_VAR(gv_bool, m_use_mipmap)
GVM_VAR_ATTRIB_SET(file_name)
GVM_VAR(gv_text, m_file_name)
GVM_VAR_ATTRIB_UNSET(file_name)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gv_bool, m_synced)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_material_param_float4)
GVM_VAR(gv_id, m_id)
GVM_VAR(gv_vector4, m_data)
GVM_VAR(gv_int, m_count)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gv_bool, m_synced)
GVM_END_STRUCT
}
