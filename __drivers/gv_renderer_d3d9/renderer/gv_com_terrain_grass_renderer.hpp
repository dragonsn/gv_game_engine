
namespace gv
{

//=============================================================================================

class gv_com_terrain_grass_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS(gv_com_terrain_grass_renderer, gv_component_renderer);
	gv_com_terrain_grass_renderer()
	{
		GVM_SET_CLASS(gv_com_terrain_grass_renderer);
	};
	~gv_com_terrain_grass_renderer(){

	};
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_terrain_grass::static_class()))
			return true;
		return false;
	}

	virtual void render_component(gv_component* com)
	{
		GV_PROFILE_EVENT(gv_com_terrain_grass__render_component, 0);
		gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
		GV_ASSERT_SLOW(mesh);
		if (!mesh)
			return;
		mesh = mesh->get_instancing_copy();
		GV_ASSERT(mesh);
		for (int seg = 0; seg < mesh->get_nb_segment(); seg++)
		{
			gv_com_graphic* graphic_com = gvt_cast< gv_com_graphic >(com);
			gv_mesh_segment* pseg = mesh->get_segment(seg);
			if (graphic_com->is_segment_hided(seg))
				continue;
			gv_material* pmaterial = graphic_com->get_sub_material(seg);
			if (!pmaterial)
				pmaterial = graphic_com->get_material();
			gv_effect* peffect = pmaterial->get_effect();
			if (!peffect)
				peffect =
					graphic_com->get_material()->get_effect()->get_renderable_effect();
			if (!peffect)
				return;
			peffect = peffect->get_pass(0)->m_pass_effect;
			this->add_a_instance(mesh, seg, graphic_com, pmaterial, peffect);
		}
	};
	virtual void precache_component(gv_com_graphic* com, gv_int pass)
	{
		gv_material* pmaterial = com->get_material();
		bool ret = gv_renderer_d3d9::static_get()->precache_resource(pmaterial);
		if (!ret)
		{
			GVM_WARNING("failed to cache " << pmaterial->get_name_id()
										   << "for gv_com_effect_renderer");
		};
		gv_effect* effect = pmaterial->get_effect();
		gv_uint vertex_decl =
			effect->get_renderable_effect(gv_id_any)->get_current_stream_decl();
		GV_ASSERT(effect);
		gv_resource* presource = com->get_resource();
		gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
		GV_ASSERT(mesh);
		if (!mesh->get_nb_segment())
		{
			mesh->add_segment();
		}
		if (!mesh->get_instancing_copy())
		{
			mesh->create_instancing_copy(gvc_instancing_nb);
		}
		mesh->get_instancing_copy()->prepare_vertex_stream(
			vertex_decl | e_vtx_with_blend_index | e_vtx_with_blend_weight);
		for (int i = 0; i < mesh->get_nb_segment(); i++)
		{
			if (mesh->get_segment(i)->m_material)
				mesh->get_segment(i)->m_material->precache(get_renderer_d3d9());
		}
		ret = gv_renderer_d3d9::static_get()->precache_resource(
			mesh->get_instancing_copy());
		if (!ret)
		{
			GVM_WARNING("failed to cache " << com->get_name_id()
										   << "for gv_com_terrain_grass_renderer");
		};
	};

	void add_component_to_batch(gv_component* com){

	};
	void post_render_pass(gv_int pass)
	{
		GV_PROFILE_EVENT(instancing_post_render_pass, 0);
		instancing_map::iterator it = m_instancing_map.begin();
		instancing_map::iterator it_end = m_instancing_map.end();
		while (it != it_end)
		{
			if (!it.is_empty())
			{
				do_render_a_instance_list(it.get_key(), *it);
			}
			it++;
		}
		m_instancing_map.clear();
	}

	void do_render_a_instance_list(instancing_id& id, instancing_list& list)
	{
		GV_PROFILE_EVENT_PIX(do_render_a_instance_list, 0);
		gv_material* pmaterial = id.m_material;
		gv_effect* peffect = id.m_effect;
		gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
		gv_static_mesh* mesh = id.m_mesh;
		gv_vertex_buffer_d3d* pvb =
			mesh->m_vertex_buffer->get_hardware_cache< gv_vertex_buffer_d3d >();
		gv_index_buffer_d3d* pib =
			mesh->m_index_buffer->get_hardware_cache< gv_index_buffer_d3d >();
		rnd_d3d->get_world_matrix().set_identity();
		gv_mesh_segment* pseg = mesh->get_segment(id.m_seg_index);
		if (pmaterial->m_material_textures.size())
		{
			rnd_d3d->set_tex_base(pmaterial->m_material_textures[0].m_texture);
		}
		else
		{
			// use default texture in the material
			rnd_d3d->set_tex_base(NULL);
		}
		gv_effect* pass_effect = peffect;
		gv_uint vertex_decl =
			get_renderer_d3d9()->set_pass_effect(pass_effect, false, pmaterial);
		pib->set();
		pvb->set(vertex_decl);

		rnd_d3d->m_bone_matrices.clear_and_reserve();
		instancing_list::iterator it = list.begin();
		instancing_list::iterator it_end = list.end();
		int cu_size = 0;
		int nb_index_per_copy = pseg->m_index_size / gvc_instancing_nb;
		while (it != it_end)
		{
			gv_matrix44 m = it->m_com->get_entity()->get_tm();
			m.transpose_self();
			rnd_d3d->m_bone_matrices.push_back(m.av4[0]);
			rnd_d3d->m_bone_matrices.push_back(m.av4[1]);
			rnd_d3d->m_bone_matrices.push_back(m.av4[2]);
			it++;
			cu_size++;
			if (cu_size >= gvc_instancing_nb || it == it_end)
			{
				GV_PROFILE_EVENT_PIX(gv_com_instancing_draw_primitive, 0);
				rnd_d3d->upload_skin_matrices();
				get_device_d3d9()->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST, 0, 0, pvb->get_vertex_count(),
					pseg->m_start_index, nb_index_per_copy * cu_size / 3);
				cu_size = 0;
				rnd_d3d->m_bone_matrices.clear_and_reserve();
			}
		}
	}

protected:
	instancing_map m_instancing_map;
};

GVM_IMP_CLASS(gv_com_terrain_grass_renderer, gv_component_renderer)
GVM_END_CLASS
}
