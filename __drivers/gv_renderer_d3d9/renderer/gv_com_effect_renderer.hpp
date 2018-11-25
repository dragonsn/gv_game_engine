namespace gv
{
gv_com_effect_renderer::gv_com_effect_renderer()
{
	GVM_SET_CLASS(gv_component_renderer);
	m_batch_enabled = false;
};
gv_com_effect_renderer::~gv_com_effect_renderer(){

};

void gv_com_effect_renderer::render_batch(
	gvt_array< gvt_ref_ptr< gv_component > >& batch)
{
	GV_PROFILE_EVENT_PIX(gv_com_effect_renderer__render_batch, 0);
};

;

void gv_com_effect_renderer::render_component(gv_component* com)
{
	GV_PROFILE_EVENT_PIX(gv_com_effect_renderer__render_component, 0);
	get_renderer_d3d9()->get_world_matrix() = com->get_entity()->get_tm();
	get_renderer_d3d9()->m_current_unified_life_time =
		com->get_entity()->get_unified_life_time();
	get_renderer_d3d9()->m_current_life_time =
		com->get_entity()->get_current_life_time();
	gv_skeletal_mesh* skeletal_mesh = NULL;
	gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
	gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
	bool do_skin = false;
	if (!mesh)
	{
		skeletal_mesh = com->get_resource< gv_skeletal_mesh >();
		if (skeletal_mesh)
		{
			gv_com_skeletal_mesh* com_sk_mesh = gvt_cast< gv_com_skeletal_mesh >(com);
			mesh = skeletal_mesh->get_t_mesh();
			do_skin = true;
			// fill bone matrices!!!!
			gv_skeletal* skeletal = com_sk_mesh->get_render_skeletal();
			if (!skeletal_mesh->m_bone_mapping.size())
			{
				GV_ASSERT(skeletal->get_nb_bone() <= GV_MAX_GPU_SKIN_BONE_NUMBER);
				rnd_d3d->m_bone_matrices.resize(skeletal->get_nb_bone() * 3);
				for (int i = 0; i < skeletal->get_nb_bone(); i++)
				{
					gv_bone& bone = skeletal->m_bones[i];
					gv_matrix44 m = bone.m_matrix_model_to_bone * bone.m_tm;
					m.transpose_self();
					rnd_d3d->m_bone_matrices[i * 3 + 0] = m.av4[0];
					rnd_d3d->m_bone_matrices[i * 3 + 1] = m.av4[1];
					rnd_d3d->m_bone_matrices[i * 3 + 2] = m.av4[2];
				}
			}
			else
			{
				GV_ASSERT(skeletal_mesh->m_nb_bone_after_map <=
						  GV_MAX_GPU_SKIN_BONE_NUMBER);
				rnd_d3d->m_bone_matrices.resize(skeletal_mesh->m_nb_bone_after_map * 3);
				for (int i = 0; i < skeletal_mesh->m_nb_bone_after_map; i++)
				{
					gv_bone& bone =
						skeletal->m_bones[skeletal_mesh->m_bone_inv_mapping[i]];
					gv_matrix44 m = bone.m_matrix_model_to_bone * bone.m_tm;
					m.transpose_self();
					rnd_d3d->m_bone_matrices[i * 3 + 0] = m.av4[0];
					rnd_d3d->m_bone_matrices[i * 3 + 1] = m.av4[1];
					rnd_d3d->m_bone_matrices[i * 3 + 2] = m.av4[2];
				}
			}
			rnd_d3d->upload_skin_matrices();
		};
	}
	gv_com_graphic* graphic_com = gvt_cast< gv_com_graphic >(com);
	if (graphic_com->is_particle() &&
		get_renderer_d3d9()->is_rendering_z_pass())
	{
		return;
	}
	if (!mesh || !mesh->get_vb() || !mesh->get_ib())
		return;
	if (!mesh->get_nb_segment())
		mesh->add_segment();
	gv_material* pmaterial = graphic_com->get_material();
	if (!pmaterial)
		return;
	gv_renderer_d3d9::static_get()->precache_resource(pmaterial);
	gv_renderer_d3d9::static_get()->precache_resource(mesh);
	gv_vertex_buffer_d3d* pvb =
		mesh->m_vertex_buffer->get_hardware_cache< gv_vertex_buffer_d3d >();
	gv_index_buffer_d3d* pib =
		mesh->m_index_buffer->get_hardware_cache< gv_index_buffer_d3d >();
	gv_bool use_batch_renderer = !graphic_com->is_background() && !do_skin &&
								 !graphic_com->is_post_effect() &&
								 !get_renderer_d3d9()->is_rendering_z_pass();
	{
		GV_PROFILE_EVENT_PIX(__one_segment, 0);
		pib->set();
		gv_uint last_vertex_decl = 0;
		for (int seg = 0; seg < mesh->get_nb_segment(); seg++)
		{
			gv_mesh_segment* pseg = mesh->get_segment(seg);
			if (graphic_com->is_segment_hided(seg))
				continue;
			if (pseg->m_is_hidden)
				continue;
			gv_material* pmaterial = graphic_com->get_sub_material(seg);
			if (!pmaterial)
				pmaterial = graphic_com->get_material();
			if (!pmaterial)
				return;
			gv_material* mesh_material = pmaterial;
			if (pmaterial->m_material_textures.size())
			{
				rnd_d3d->set_tex_base(pmaterial->m_material_textures[0].m_texture);
			}
			else
			{
				// use default texture in the material
				rnd_d3d->set_tex_base(NULL);
			}
			gv_id my_effect_name = gv_id_any;
			if (!pmaterial->get_effect())
			{
				pmaterial = graphic_com->get_material();
			}
			if (!pmaterial->get_effect())
				return;
			gv_effect* peffect =
				pmaterial->get_effect()->get_renderable_effect(my_effect_name);
			if (!peffect)
				return;
			for (int i = 0; i < peffect->get_nb_pass(); i++)
			{
				// TODO need test with current render pass
				if (!peffect->get_pass(i)->m_enabled)
					continue;
				gv_effect* pass_effect = peffect->get_pass(i)->m_pass_effect;
				if (!pass_effect)
					continue;
				if (use_batch_renderer)
				{
					get_renderer_d3d9()->get_batch_renderer()->add_a_instance(
						mesh, seg, graphic_com, mesh_material, pass_effect, i);
					continue;
				}
				if (pass_effect->is_transparent() &&
					get_renderer_d3d9()->is_rendering_z_pass())
				{
					continue;
				}
				gv_uint vertex_decl =
					rnd_d3d->set_pass_effect(pass_effect, do_skin, pmaterial, i);
				if (!vertex_decl)
					continue;
				if (last_vertex_decl != vertex_decl)
				{
					last_vertex_decl = vertex_decl;
					if (!pvb->set(vertex_decl))
					{
						GVM_WARNING("failed to set stream decl for mesh"
									<< mesh->get_name() << "for effect "
									<< pass_effect->get_name() << gv_endl);
						return;
					}
				};
				if (graphic_com->is_post_effect())
					get_renderer_d3d9()->set_static_shader_param_for_post_process();
				get_device_d3d9()->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST, 0, 0, pvb->get_vertex_count(),
					pseg->m_start_index, pseg->m_index_size / 3);
			};
		}
	} // if seg
}

void gv_com_effect_renderer::precache_component(gv_com_graphic* com,
												gv_int pass)
{
	gv_material* pmaterial = com->get_material();
	if (!pmaterial)
		return;
	bool ret = gv_renderer_d3d9::static_get()->precache_resource(pmaterial);
	if (!ret)
	{
		GVM_WARNING("failed to cache " << pmaterial->get_name_id()
									   << "for gv_com_effect_renderer");
	}
	gv_effect* effect = pmaterial->get_effect();
	if (!effect)
		return;
	if (com->is_post_effect() && effect->get_nb_pass() > 1)
	{
#pragma GV_REMINDER( \
	"[MEMO][RENDER]post effect will  disable all the pass not started with pp")
		for (int pass = 0; pass < effect->get_nb_pass(); pass++)
		{
			gv_effect_pass* p = effect->get_pass(pass);
			if (!p->m_name.string().has_prefix("pp"))
			{
				p->m_enabled = false;
			}
		}
	}
	gv_uint vertex_decl =
		effect->get_renderable_effect(gv_id_any)->get_current_stream_decl();
	gv_resource* presource = com->get_resource();
	gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
	if (mesh)
	{
		// if needed build vertex stream.
		mesh->prepare_vertex_stream(vertex_decl);
	}
	gv_skeletal_mesh* sk_mesh = com->get_resource< gv_skeletal_mesh >();
	if (sk_mesh)
	{
		sk_mesh->optimize_bones(GV_MAX_GPU_SKIN_BONE_NUMBER);
		sk_mesh->get_t_mesh()->prepare_vertex_stream(
			vertex_decl | e_vtx_with_blend_index | e_vtx_with_blend_weight);
		mesh = sk_mesh->get_t_mesh();
	}
	if (!mesh)
		return;
	if (!mesh->get_nb_segment())
	{
		mesh->add_segment();
	}
	for (int i = 0; i < mesh->get_nb_segment(); i++)
	{
		if (mesh->get_segment(i)->m_material)
			mesh->get_segment(i)->m_material->precache(get_renderer_d3d9());
	}
	ret = gv_renderer_d3d9::static_get()->precache_resource(presource);
	if (!ret)
	{
		GVM_WARNING("failed to cache " << com->get_name_id()
									   << "for gv_com_effect_renderer");
	};
}

void gv_com_effect_renderer::post_render_pass(gv_int pass)
{
}
GVM_IMP_CLASS(gv_com_effect_renderer, gv_component_renderer)
GVM_END_CLASS
};