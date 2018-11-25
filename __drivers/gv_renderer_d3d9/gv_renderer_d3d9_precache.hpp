namespace gv
{
#define GVM_PRECACHE(type)                         \
	if (resource->is_a(gv_##type::static_class())) \
		return this->precache_##type(gvt_cast< gv_##type >(resource));

bool gv_renderer_d3d9::precache_resource(gv_resource* resource)
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

bool gv_renderer_d3d9::precache_index_buffer(gv_index_buffer* index_buffer)
{
	if (index_buffer->get_hardware_cache())
		return true;
	gv_index_buffer_d3d* pib =
		this->get_sandbox()->create_nameless_object< gv_index_buffer_d3d >();
	pib->set_origin(index_buffer);
	index_buffer->set_hardware_cache(pib);
	return pib->precache(NULL);
};

bool gv_renderer_d3d9::precache_vertex_buffer(gv_vertex_buffer* vertex_buffer)
{
	if (!vertex_buffer)
		return false;
	if (vertex_buffer->get_hardware_cache())
		return true;
	gv_vertex_buffer_d3d* pvb =
		this->get_sandbox()->create_nameless_object< gv_vertex_buffer_d3d >();
	pvb->set_origin(vertex_buffer);
	vertex_buffer->set_hardware_cache(pvb);
	return pvb->precache(vertex_buffer->get_biggest_vertex_format());
};

bool gv_renderer_d3d9::precache_static_mesh(gv_static_mesh* mesh)
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

bool gv_renderer_d3d9::precache_skeletal_mesh(gv_skeletal_mesh* mesh)
{
	precache_static_mesh(mesh->get_t_mesh());
	return true;
}

bool gv_renderer_d3d9::precache_texture(gv_texture* tex)
{
	if (tex->get_hardware_cache())
		return true;
	if (tex->is_file_in_memory())
	{
		gv_texture_d3d* tex_d3d =
			this->get_sandbox()->create_nameless_object< gv_texture_d3d >();
		tex_d3d->set_origin(tex);
		tex->set_hardware_cache(tex_d3d);
		return tex_d3d->precache(NULL);
	}
	else
	{
		tex->set_hardware_cache(get_shared_texture(*tex->get_file_name(), tex));
	}
	return true;
}

gv_texture_d3d*
gv_renderer_d3d9::get_shared_texture(const gv_string_tmp& full_name,
									 gv_texture* tex)
{
	gv_string_tmp file_name;
	file_name = gv_global::fm->get_filename(*full_name);
	gvt_ref_ptr< gv_texture_d3d >& ptr = m_texture_map[*file_name];
	if (ptr)
	{
		return ptr;
	}
	else
	{
		gv_texture_d3d* tex_d3d =
			this->get_sandbox()->create_nameless_object< gv_texture_d3d >();
		tex_d3d->set_origin(tex);
		tex_d3d->precache(NULL);
		ptr = tex_d3d;
		return tex_d3d;
	}
};

bool gv_renderer_d3d9::precache_effect(gv_effect* effect)
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
		for (int i = 0; i < nb_rt; i++)
		{
			gv_effect_renderable_texture* ptexture =
				effect->get_nth_renderable_target(i);
			get_render_target_mgr()->precache(ptexture);
		}
		gv_int nb_state = effect->get_nb_render_state();
		for (int j = 0; j < nb_state; j++)
		{
			gv_effect_render_state* pstate = effect->get_render_state(j);
			if (pstate->m_state == D3DRS_ALPHABLENDENABLE)
			{
				effect->set_is_transparent(pstate->m_value != 0);
			}
			if (pstate->m_state == D3DRS_ZENABLE)
			{
				effect->set_is_no_depth_writing(pstate->m_value == 0);
			}
		}
	}
	return true;
};
bool gv_renderer_d3d9::precache_shader(gv_shader* shader)
{
	if (shader->get_hardware_cache())
		return true;
	gv_shader_d3d* shader_d3d =
		this->get_sandbox()->create_nameless_object< gv_shader_d3d >();
	shader_d3d->set_origin(shader);
	shader->set_hardware_cache(shader_d3d);
	return shader_d3d->precache(NULL);
};
bool gv_renderer_d3d9::precache_material(gv_material* material)
{
	material->precache(this);
	return true;
};

void gv_renderer_d3d9::precache_renderable(gv_com_graphic* ptr)
{
	for (int i = 0; i < m_render_order.size(); i++)
	{
		gv_int pass = m_render_order[i];
		ptr->set_renderer(pass, get_com_renderer(ptr->get_renderer_id(pass)));
		if (ptr->get_renderer(pass))
			ptr->get_renderer(pass)->precache_component(ptr, i);
	};
}
void gv_renderer_d3d9::uncache_renderable(gv_com_graphic* ptr)
{
	gv_resource* pres = ptr->get_resource();
	if (pres)
		pres->set_hardware_cache(NULL);
}
}