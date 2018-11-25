namespace gv
{
struct instancing_id
{
	instancing_id()
	{
	}
	instancing_id(const instancing_id& a)
	{
		(*this) = a;
	}
	~instancing_id()
	{
	}
	gv_int m_seg_index;
	gvt_ref_ptr< gv_static_mesh > m_mesh;
	gvt_ref_ptr< gv_material > m_material;
	gvt_ref_ptr< gv_effect > m_effect;
	gv_int m_pass_index;

	instancing_id& operator=(const instancing_id& a)
	{
		m_effect = a.m_effect;
		m_material = a.m_material;
		m_mesh = a.m_mesh;
		m_seg_index = a.m_seg_index;
		m_pass_index = a.m_pass_index;
		return *this;
	}
	bool operator==(const instancing_id& a) const
	{
		return (m_seg_index == a.m_seg_index && m_mesh == a.m_mesh &&
				m_material == a.m_material && m_effect == a.m_effect);
	}
};
static gv_int gvc_instancing_nb = 32;
struct instancing_info
{
	gvt_ptr< gv_com_graphic > m_com;
	gv_matrix44 m_tm;
	gv_float m_life_time;
	gv_float m_lift_time_x;
};

typedef gvt_dlist< instancing_info, gvp_memory_renderer_temp > instancing_list;
typedef gvt_hash_map< instancing_id, instancing_list, 1024,
					  gvp_memory_renderer_temp >
	instancing_map;
struct instancing_sorted_it
{
	instancing_sorted_it(){};
	instancing_sorted_it(const instancing_sorted_it& i)
	{
		(*this) = i;
	}
	instancing_sorted_it& operator=(const instancing_sorted_it& i)
	{
		priority = i.priority;
		it = i.it;
		return *this;
	}
	gv_bool operator==(const instancing_sorted_it& i)
	{
		return priority == i.priority;
	}
	gv_bool operator<(const instancing_sorted_it& i)
	{
		return priority < i.priority;
	}
	gv_uint priority;
	instancing_map::iterator it;
};
inline gv_uint gvt_hash(const instancing_id& id)
{
	return gvt_hash(id.m_mesh.ptr());
}
//=============================================================================================

class gv_com_instancing_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS(gv_com_instancing_renderer, gv_component_renderer);
	gv_com_instancing_renderer()
	{
		GVM_SET_CLASS(gv_com_instancing_renderer);
	};
	~gv_com_instancing_renderer(){

	};
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_static_mesh::static_class()))
			return true;
		return false;
	}
	virtual void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch){

	};
	virtual void add_a_instance(gv_static_mesh* mesh, gv_int seg,
								gv_com_graphic* com, gv_material* material,
								gv_effect* effect, gv_int pass = 0)
	{
		GV_PROFILE_EVENT(add_a_instance, 0);
		instancing_id inst;
		inst.m_seg_index = seg;
		inst.m_mesh = mesh;
		inst.m_material = material;
		inst.m_effect = effect;
		inst.m_pass_index = pass;
		instancing_info info;
		if (!com->is_component_tm_enabled())
			info.m_com = com;
		else
			info.m_tm = com->get_component_tm();
		info.m_life_time = com->get_entity()->get_unified_life_time();
		info.m_lift_time_x = com->get_entity()->get_current_life_time();
		m_instancing_map[inst].push_back(info);
	}

	virtual void render_component(gv_component* com)
	{
		GV_PROFILE_EVENT(gv_com_instancing_renderer__render_component, 0);
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
		gv_static_mesh* mesh = com->get_resource< gv_static_mesh >();
		if (!mesh)
			return;
		if (mesh->get_instancing_copy())
			return; // already precached
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
		if (!mesh->get_nb_segment())
		{
			mesh->add_segment();
		}
		mesh->create_instancing_copy(gvc_instancing_nb);
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
										   << "for gv_com_instancing_renderer");
		};
	};
	void add_component_to_batch(gv_component* com){

	};
	gv_uint get_render_priority(gv_effect* effect, gv_material* mat)
	{
		GV_ASSERT(effect);
		if (effect->is_transparent())
		{
			return gvc_render_priority_after_bloom;
		}
		if (effect->is_no_depth_writing())
		{
			return gvc_render_priority_after_outline;
		}
		gv_effect* old_effect = mat->get_effect();
		if (old_effect && old_effect->is_no_depth_writing())
		{
			return gvc_render_priority_after_outline;
		}
		return gvc_render_priority_normal;
	}
	void post_render_pass(gv_int pass)
	{
		GV_PROFILE_EVENT(instancing_post_render_pass, 0);
		instancing_map::iterator it = m_instancing_map.begin();
		instancing_map::iterator it_end = m_instancing_map.end();
		while (it != it_end)
		{
			if (!it.is_empty())
			{
				instancing_sorted_it ii;
				ii.it = it;
				ii.priority =
					get_render_priority(it.get_key().m_effect, it.get_key().m_material);
				m_sort_array.push_back(ii);
				// do_render_a_instance_list(it.get_key(),*it);
			}
			it++;
		}
		m_sort_array.sort();
		render_priority_group(0, gvc_render_priority_post_process);
	}

	void render_priority_group(gv_uint start, gv_uint end)
	{
		GV_PROFILE_EVENT(instancing_render_priority_group, 0);
		int i = 0;
		for (i = 0; i < m_sort_array.size(); i++)
		{
			instancing_sorted_it& ii = m_sort_array[i];
			if (ii.priority >= end)
				break;
			if (ii.priority < start)
				continue;
			instancing_map::iterator it = m_sort_array[i].it;
			do_render_a_instance_list(it.get_key(), *it);
		};
		m_sort_array.erase(0, i);
		if (end == gvc_render_priority_last)
		{
			m_instancing_map.clear();
			m_sort_array.clear_and_reserve();
		}
	};

	void do_render_a_instance_list(instancing_id& id, instancing_list& list)
	{
		GV_PROFILE_EVENT_PIX(do_render_a_instance_list, 0);
		gv_material* pmaterial = id.m_material;
		gv_effect* peffect = id.m_effect;
		gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
		gv_static_mesh* mesh = id.m_mesh;
		if (!mesh->is_instancing_copy())
		{
			do_render_a_normal_list(id, list);
			return;
		}
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
			rnd_d3d->set_pass_effect(pass_effect, true, pmaterial, id.m_pass_index);
		if (!vertex_decl)
			return;
		pib->set();
		pvb->set(vertex_decl);

		rnd_d3d->m_bone_matrices.clear_and_reserve();
		instancing_list::iterator it = list.begin();
		instancing_list::iterator it_end = list.end();
		int cu_size = 0;
		int nb_index_per_copy = pseg->m_index_size / gvc_instancing_nb;
		while (it != it_end)
		{
			gv_matrix44 m = it->m_com ? it->m_com->get_entity()->get_tm() : it->m_tm;
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

	void do_render_a_normal_list(instancing_id& id, instancing_list& list)
	{
		GV_PROFILE_EVENT_PIX(do_render_a_normal_list, 0);
		gv_material* pmaterial = id.m_material;
		gv_effect* peffect = id.m_effect;
		gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
		gv_static_mesh* mesh = id.m_mesh;
		gv_vertex_buffer_d3d* pvb =
			mesh->m_vertex_buffer->get_hardware_cache< gv_vertex_buffer_d3d >();
		gv_index_buffer_d3d* pib =
			mesh->m_index_buffer->get_hardware_cache< gv_index_buffer_d3d >();
		gv_mesh_segment* pseg = mesh->get_segment(id.m_seg_index);
		gv_effect* pass_effect = peffect;
		gv_uint vertex_decl = rnd_d3d->set_pass_effect(pass_effect, false,
													   pmaterial, id.m_pass_index);
		if (!vertex_decl)
			return;
		pib->set();
		pvb->set(vertex_decl);
		instancing_list::iterator it = list.begin();
		instancing_list::iterator it_end = list.end();
		while (it != it_end)
		{
			gv_matrix44 m = it->m_com ? it->m_com->get_entity()->get_tm() : it->m_tm;
			get_renderer_d3d9()->get_world_matrix() = m;
			get_renderer_d3d9()->m_current_unified_life_time = it->m_life_time;
			get_renderer_d3d9()->m_current_life_time = it->m_lift_time_x;
			if (pmaterial->m_material_textures.size())
			{
				rnd_d3d->set_tex_base(pmaterial->m_material_textures[0].m_texture);
			}
			else
			{
				rnd_d3d->set_tex_base(NULL);
			}
			gv_uint vertex_decl = rnd_d3d->set_pass_effect(
				pass_effect, false, pmaterial, id.m_pass_index);
			if (!vertex_decl)
			{
				it++;
				continue;
			}
			if (pass_effect->is_transparent())
			{
				get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			get_device_d3d9()->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST, 0, 0, pvb->get_vertex_count(),
				pseg->m_start_index, pseg->m_index_size / 3);
			it++;
		}
	}

protected:
	gvt_array< instancing_sorted_it > m_sort_array;
	instancing_map m_instancing_map;
};

GVM_IMP_CLASS(gv_com_instancing_renderer, gv_component_renderer)
GVM_END_CLASS
}
