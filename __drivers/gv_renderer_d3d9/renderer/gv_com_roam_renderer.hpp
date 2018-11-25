#include "../../gv_framework/component/gv_com_terrain_roam.h"
namespace gv
{
//============================================================================================
//								:
//============================================================================================
static const D3DVERTEXELEMENT9 s_terrain_vertex_elements[] = {
	{0, 0, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_POSITION, 0},

	{1, 0, D3DDECLTYPE_FLOAT1, 0, D3DDECLUSAGE_TEXCOORD, 0},
	{1, 4, D3DDECLTYPE_UBYTE4N, 0, D3DDECLUSAGE_NORMAL, 0},
	{1, 8, D3DDECLTYPE_UBYTE4N, 0, D3DDECLUSAGE_BINORMAL, 0},
	{1, 12, D3DDECLTYPE_UBYTE4N, 0, D3DDECLUSAGE_TANGENT, 0},
	D3DDECL_END()};

const static int gvc_terrain_stream_1_components = 4;

class gv_roam_chunk_d3d : public gv_resource_cache
{
public:
	friend class gv_com_roam_d3d;
	friend class gv_com_roam_renderer;
	GVM_DCL_CLASS(gv_roam_chunk_d3d, gv_resource_cache);
	gv_roam_chunk_d3d::gv_roam_chunk_d3d()
	{
		GVM_SET_CLASS(gv_roam_chunk_d3d);
	}
	bool precache(gv_uint flag)
	{
		if (!m_chunk->m_is_cache_dirty)
			return true;
		m_y_buffer = NULL;
		int vb_size = (GV_ROAM_PATCH_SIZE + 1) * (GV_ROAM_PATCH_SIZE + 1) *
					  sizeof(float) * gvc_terrain_stream_1_components;
		GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexBuffer(
			vb_size, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &m_y_buffer.ptr(),
			NULL))
		void* pdata;
		GVM_VERIFY_D3D(m_y_buffer->Lock(0, vb_size, &pdata, 0));
		float* pbuffer = (float*)pdata;
		for (int iy = 0; iy < GV_ROAM_PATCH_SIZE + 1; iy++)
			for (int ix = 0; ix < GV_ROAM_PATCH_SIZE + 1; ix++)
			{
				//(*pbuffer++)=this->m_chunk->m_height_map[ix+iy*GV_ROAM_PATCH_SIZE+1];
				//(*pbuffer++)=*((float*)&this->m_chunk->m_normals[ix+iy*GV_ROAM_PATCH_SIZE+1]);
				//(*pbuffer++)=*((float*)&this->m_chunk->m_binormals[ix+iy*GV_ROAM_PATCH_SIZE+1]);
				//(*pbuffer++)=*((float*)&this->m_chunk->m_tangents[ix+iy*GV_ROAM_PATCH_SIZE+1]);
				gv_vector4b n, b, t;
				(*pbuffer++) = m_terrain->get_height(m_offest.x + ix, m_offest.y + iy);
				m_terrain->get_normal(m_offest.x + ix, m_offest.y + iy, n, b, t);
				(*pbuffer++) = *((float*)&n);
				(*pbuffer++) = *((float*)&b);
				(*pbuffer++) = *((float*)&t);
			}
		GVM_VERIFY_D3D(m_y_buffer->Unlock());
		m_chunk->m_is_cache_dirty = false;
		return true;
	};

	bool uncache()
	{
		m_y_buffer = NULL;
		return true;
	};
	bool recache()
	{
		uncache();
		precache(NULL);
		return true;
	};
	bool set_origin(gv_resource* res)
	{
		m_chunk = gvt_cast< gv_roam_chunk >(res);
		return true;
	}

public:
	gv_vector2i m_offest; // used to collect height of boundary.
	gvt_ptr< gv_com_terrain_roam > m_terrain;
	gvt_ptr< gv_roam_chunk > m_chunk;
	gvt_ptr_d3d< IDirect3DVertexBuffer9 > m_y_buffer;
};
GVM_IMP_CLASS(gv_roam_chunk_d3d, gv_resource_cache)
GVM_END_CLASS
//============================================================================================
//								:
//============================================================================================
class gv_com_roam_d3d : public gv_resource_cache
{
	friend class gv_com_roam_renderer;

public:
	GVM_DCL_CLASS(gv_com_roam_d3d, gv_resource_cache);
	gv_com_roam_d3d::gv_com_roam_d3d()
	{
		GVM_SET_CLASS(gv_com_roam_d3d);
	}
	bool set_origin(gv_resource* res)
	{
		gv_com_terrain_roam* pterrain = gvt_cast< gv_com_terrain_roam >(res);
		if (!pterrain)
			return false;
		m_terrain = pterrain;
		return true;
	};
	void create_static_ib()
	{
		int ib_complete_size =
			GV_ROAM_PATCH_SIZE * GV_ROAM_PATCH_SIZE * 2 * 3 * sizeof(int);
		GVM_VERIFY_D3D(get_device_d3d9()->CreateIndexBuffer(
			ib_complete_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED,
			&this->m_i_buf_static.ptr(), NULL));
		{ // fill the		static index buffer
			void* pdata;
			GVM_VERIFY_D3D(m_i_buf_static->Lock(0, ib_complete_size, &pdata, 0));
			gv_int* pindex = (gv_int*)pdata;
			for (int iy = 0; iy < GV_ROAM_PATCH_SIZE; iy++)
				for (int ix = 0; ix < GV_ROAM_PATCH_SIZE; ix++)
				{
					*pindex++ = iy * (GV_ROAM_PATCH_SIZE + 1) + ix;
					*pindex++ = iy * (GV_ROAM_PATCH_SIZE + 1) + ix + 1;
					*pindex++ = (iy + 1) * (GV_ROAM_PATCH_SIZE + 1) + ix + 1;
					*pindex++ = iy * (GV_ROAM_PATCH_SIZE + 1) + ix;
					*pindex++ = (iy + 1) * (GV_ROAM_PATCH_SIZE + 1) + ix + 1;
					*pindex++ = (iy + 1) * (GV_ROAM_PATCH_SIZE + 1) + ix;
				}
			GVM_VERIFY_D3D(m_i_buf_static->Unlock());
		}
	}
	bool precache(gv_uint flag)
	{

		int ib_size = ROAM_MAX_DISPLAY_NODE * 3 * sizeof(INT);
		GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexDeclaration(
			s_terrain_vertex_elements, &this->m_vertex_dcl.ptr()))
		GVM_VERIFY_D3D(get_device_d3d9()->CreateIndexBuffer(
			ib_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED,
			&this->m_i_buf_read.ptr(), NULL));
		GVM_VERIFY_D3D(get_device_d3d9()->CreateIndexBuffer(
			ib_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED,
			&this->m_i_buf_write.ptr(), NULL));
		create_static_ib();
		{ // create vertex buffer for x,z.
			int vb_size = (GV_ROAM_PATCH_SIZE + 1) * (GV_ROAM_PATCH_SIZE + 1) *
						  sizeof(float) * 2;
			GVM_VERIFY_D3D(get_device_d3d9()->CreateVertexBuffer(
				vb_size, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED,
				&m_xz_buffer.ptr(), NULL))
			void* pdata;
			GVM_VERIFY_D3D(m_xz_buffer->Lock(0, vb_size, &pdata, 0));
			float* pbuffer = (float*)pdata;
			for (int iy = 0; iy < GV_ROAM_PATCH_SIZE + 1; iy++)
				for (int ix = 0; ix < GV_ROAM_PATCH_SIZE + 1; ix++)
				{
					(*pbuffer++) = (float)ix;
					(*pbuffer++) = (float)iy;
				}
			GVM_VERIFY_D3D(m_xz_buffer->Unlock());
		}
		gv_int nb_chunks = 0;
		gv_vector2i patch_w_l = m_terrain->get_size_in_patch();
		for (int iy = 0; iy < patch_w_l.y; iy++)
			for (int ix = 0; ix < patch_w_l.x; ix++)
			{
				gv_roam_patch* ppatch = m_terrain->get_patch(ix, iy);
				gv_roam_chunk* pchunk = m_terrain->get_patch(ix, iy)->get_chunk();
				if (!pchunk->get_hardware_cache())
				{
					gv_roam_chunk_d3d* pchunk_d3d =
						get_sandbox()->create_nameless_object< gv_roam_chunk_d3d >();
					pchunk->set_hardware_cache(pchunk_d3d);
					pchunk_d3d->set_origin(pchunk);
					pchunk_d3d->m_terrain = m_terrain;
					pchunk_d3d->m_offest = ppatch->get_offset();
					pchunk_d3d->precache(NULL);
				}
			}
		return true;
	};

	bool uncache()
	{
		m_i_buf_read = NULL;
		m_i_buf_write = NULL;
		m_xz_buffer = NULL;
		gv_vector2i patch_w_l = m_terrain->get_size_in_patch();
		for (int iy = 0; iy < patch_w_l.y; iy++)
			for (int ix = 0; ix < patch_w_l.x; ix++)
			{
				gv_roam_chunk* pchunk = m_terrain->get_patch(ix, iy)->get_chunk();
				pchunk->set_hardware_cache(NULL);
			}
		return true;
	};

	bool recache()
	{
		this->uncache();
		this->precache(NULL);
		return true;
	};

protected:
	gv_com_terrain_roam* m_terrain;
	gvt_ptr_d3d< IDirect3DIndexBuffer9 > m_i_buf_read;
	gvt_ptr_d3d< IDirect3DIndexBuffer9 > m_i_buf_write;
	gvt_ptr_d3d< IDirect3DIndexBuffer9 > m_i_buf_static;
	gvt_array< gv_uint > m_chunck_ib_offset;
	gvt_ptr_d3d< IDirect3DVertexBuffer9 > m_xz_buffer;
	gvt_ptr_d3d< IDirect3DVertexDeclaration9 > m_vertex_dcl;
	gvt_array< gvt_ptr_d3d< IDirect3DVertexBuffer9 > > m_y_buffer_for_chunks;
};
GVM_IMP_CLASS(gv_com_roam_d3d, gv_resource_cache)
GVM_END_CLASS
//============================================================================================
//								:
//============================================================================================
class gv_com_roam_renderer : public gv_component_renderer
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_com_roam_renderer, gv_component_renderer);
	virtual bool can_render_component(gv_component* p)
	{
		if (p->is_a(gv_com_terrain_roam::static_class()))
			return true;
		return false;
	}

	void render_batch(gvt_array< gvt_ref_ptr< gv_component > >& batch)
	{
		GV_PROFILE_EVENT_PIX(gv_com_effect_renderer__render_batch, 0);
	};
	gvt_ptr< gv_component_renderer > m_instancing_renderer;
	void render_mesh_layers(gv_com_terrain_roam* terrain)
	{
		if (!m_instancing_renderer)
		{
			m_instancing_renderer = get_renderer_d3d9()->get_com_renderer(
				gv_id("gv_com_instancing_renderer"));
		}
		for (int ilayer = 0; ilayer < terrain->get_nb_mesh_layer(); ilayer++)
		{
			gv_terrain_mesh_layer_info* layer = terrain->get_mesh_layer(ilayer);
			if (!layer)
				continue;
			m_instancing_renderer->precache_component(layer->mesh, 0);
			gv_box local_aabb = layer->mesh->get_local_bbox();
			int nb_patch = terrain->get_nb_visible_patch();
			for (int ipatch = 0; ipatch < nb_patch; ipatch++)
			{
				gv_roam_patch* patch = terrain->get_nth_visible_patch(ipatch);
				for (int inst = 0; inst < patch->m_mesh_layer_instances[ilayer].size();
					 inst++)
				{
					gv_matrix43 tm;
					gv_vector4& pos = patch->m_mesh_layer_instances[ilayer][inst];
					tm.set_identity();
					tm = gv_matrix43::get_rotation_by_y(pos.get_w());
					tm.set_trans(pos);
					gv_box world_aabb = local_aabb.transform(tm);
					if (get_renderer_d3d9()
							->get_main_camera()
							->get_world_frustum()
							.intersect(world_aabb))
					{
						layer->mesh->set_component_tm_enabled(true);
						layer->mesh->set_component_tm(tm);
						m_instancing_renderer->render_component(layer->mesh);
					}
				}
			} // next
		}	 // next layer
	}

	void render_component(gv_component* com)
	{
		GV_PROFILE_EVENT_PIX(gv_com_terrain_roam_render, 0);

		gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
		gv_com_graphic* graphic_com = gvt_cast< gv_com_graphic >(com);
		gv_com_terrain_roam* pterrain = gvt_cast< gv_com_terrain_roam >(com);
		if (!pterrain)
			return;
		bool use_wireframe = pterrain->is_wireframe_mode(); // for debug
		gv_material* pmaterial = graphic_com->get_material();
		if (!pmaterial)
			return;
		gv_id my_effect_name = gv_id_any;
		if (!pmaterial->get_effect())
			return;
		gv_effect* peffect =
			pmaterial->get_effect()->get_renderable_effect(my_effect_name);
		if (!peffect)
			return;
		gv_com_roam_d3d* pcache =
			pterrain->get_resource()->get_hardware_cache< gv_com_roam_d3d >();
		if (!pcache)
			return;
		gv_int nb_patch = pterrain->get_nb_visible_patch();
		if (!nb_patch)
			return;
		gv_bool use_lod = pterrain->is_dynamic_LOD_on();
		get_renderer_d3d9()->get_world_matrix() = com->get_entity()->get_tm();

		if (use_lod)
		{ // fill the index buffer
			void* pdata;
			int ib_size = ROAM_MAX_DISPLAY_NODE * 3 * sizeof(INT);
			GVM_VERIFY_D3D(pcache->m_i_buf_write->Lock(0, ib_size, &pdata, 0));
			gv_int* pindex = (gv_int*)pdata;
			gv_int index_count;
			pterrain->fill_index(pindex, index_count, ROAM_MAX_DISPLAY_NODE * 3);
			GVM_VERIFY_D3D(pcache->m_i_buf_write->Unlock());
			get_device_d3d9()->SetIndices(pcache->m_i_buf_write);
		}
		else
		{
			get_device_d3d9()->SetIndices(pcache->m_i_buf_static);
		}
		get_device_d3d9()->SetStreamSource(0, pcache->m_xz_buffer, 0,
										   2 * sizeof(float));
		for (int i = 0; i < peffect->get_nb_pass(); i++)
		{ // render the terrain!!
			if (!peffect->get_pass(i)->m_enabled)
				continue;
			gv_effect* pass_effect = peffect->get_pass(i)->m_pass_effect;
			if (!pass_effect->get_pixel_shader())
				continue;
			if (!pass_effect->get_vertex_shader())
				continue;
			rnd_d3d->set_pass_effect(pass_effect, false, pmaterial, i);
			if (use_wireframe)
				get_device_d3d9()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			GVM_VERIFY_D3D(
				get_device_d3d9()->SetVertexDeclaration(pcache->m_vertex_dcl));

			for (int idx = 0; idx < nb_patch; idx++)
			{
				gv_roam_patch* patch = pterrain->get_nth_visible_patch(idx);
				gv_roam_chunk_d3d* pchunk_d3d =
					patch->get_chunk()->get_hardware_cache< gv_roam_chunk_d3d >();
				pchunk_d3d->precache(NULL);
				GVM_VERIFY_D3D(get_device_d3d9()->SetStreamSource(
					1, pchunk_d3d->m_y_buffer, 0,
					sizeof(float) * gvc_terrain_stream_1_components));
				gv_vector4 offest;
				offest.set((float)patch->get_offset().y, 0,
						   (float)patch->get_offset().x, 0);
				get_device_d3d9()->SetVertexShaderConstantF(0, &offest.x, 1);
				if (use_lod)
				{
					gv_int offset, size;
					pterrain->get_nth_patch_index_buffer(idx, offset, size);
					get_device_d3d9()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
															GV_ROAM_PATCH_VTX_SIZE,
															offset, size / 3);
				}
				else
				{
					get_device_d3d9()->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST, 0, 0, GV_ROAM_PATCH_VTX_SIZE, 0,
						GV_ROAM_PATCH_SIZE * GV_ROAM_PATCH_SIZE * 2);
				}
				if (patch->m_is_selected)
				{
					get_sandbox()->get_debug_renderer()->draw_box_3d(
						patch->get_world_aabb(), gv_color::YELLOW());
				}
			}
		};
		if (use_wireframe)
			get_device_d3d9()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		if (use_lod)
		{
			gvt_swap(pcache->m_i_buf_write, pcache->m_i_buf_read);
		}
		render_mesh_layers(pterrain);
		get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		get_device_d3d9()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_terrain_with_fur.push_back(pterrain);
	}

	void gv_com_roam_renderer::precache_component(gv_com_graphic* com,
												  gv_int pass)
	{
		gv_com_terrain_roam* pterrain = gvt_cast< gv_com_terrain_roam >(com);
		if (!pterrain)
			return;
		gv_material* pmaterial = com->get_material();
		if (pterrain->get_resource()->get_hardware_cache())
			return;
		bool ret = gv_renderer_d3d9::static_get()->precache_resource(pmaterial);
		if (!ret)
		{
			GVM_WARNING("failed to cache " << pmaterial->get_name_id()
										   << "for gv_com_roam_renderer");
		};
		gv_effect* effect = pmaterial->get_effect();
		if (!ret)
		{
			GVM_WARNING("failed to cache " << com->get_name_id()
										   << "for gv_com_roam_renderer");
		};
		gv_com_roam_d3d* pterrain_d3d =
			get_sandbox()->create_nameless_object< gv_com_roam_d3d >();
		pterrain_d3d->m_terrain = pterrain;
		pterrain_d3d->precache(NULL);
		pterrain->get_resource()->set_hardware_cache(pterrain_d3d);
	}
	//============================================================================================
	//								:
	//============================================================================================
	void render_terrain_with_fur(gv_component* com)
	{
		GV_PROFILE_EVENT_PIX(gv_com_terrain_roam_render, 0);
		gv_renderer_d3d9* rnd_d3d = gv_renderer_d3d9::static_get();
		gv_com_graphic* graphic_com = gvt_cast< gv_com_graphic >(com);
		gv_com_terrain_roam* pterrain = gvt_cast< gv_com_terrain_roam >(com);
		if (!pterrain)
			return;
		gv_com_roam_d3d* pcache =
			pterrain->get_resource()->get_hardware_cache< gv_com_roam_d3d >();
		int nb_patch = pterrain->get_nb_visible_patch();
		get_renderer_d3d9()->get_world_matrix() = com->get_entity()->get_tm();
		get_device_d3d9()->SetIndices(pcache->m_i_buf_static);
		get_device_d3d9()->SetStreamSource(0, pcache->m_xz_buffer, 0,
										   2 * sizeof(float));
		//============================================================================================
		//								:render fur!!!! lovely
		//fur
		//============================================================================================
		for (int layer = 0; layer < pterrain->get_nb_fur_layer(); layer++)
		{
			gv_terrain_fur_layer_info* info = pterrain->get_fur_layer(layer);
			gv_material* pmaterial = info->fur_material;
			if (!pmaterial)
				return;
			gv_id my_effect_name = gv_id_any;
			if (!pmaterial->get_effect())
				return;
			gv_effect* peffect =
				pmaterial->get_effect()->get_renderable_effect(my_effect_name);
			if (peffect->get_nb_pass() <= 2)
				return;
			gv_effect* pass_effect = peffect->get_pass(1)->m_pass_effect;
			if (!pass_effect->get_pixel_shader())
				continue;
			if (!pass_effect->get_vertex_shader())
				continue;
			for (int i = 0; i < info->fur_pass; i++)
			{
				rnd_d3d->set_pass_effect(pass_effect, false, pmaterial, i);
				GVM_VERIFY_D3D(
					get_device_d3d9()->SetVertexDeclaration(pcache->m_vertex_dcl));
				for (int idx = 0; idx < nb_patch; idx++)
				{
					gv_roam_patch* patch = pterrain->get_nth_visible_patch(idx);
					gv_roam_chunk_d3d* pchunk_d3d =
						patch->get_chunk()->get_hardware_cache< gv_roam_chunk_d3d >();
					pchunk_d3d->precache(NULL);
					GVM_VERIFY_D3D(get_device_d3d9()->SetStreamSource(
						1, pchunk_d3d->m_y_buffer, 0,
						sizeof(float) * gvc_terrain_stream_1_components));
					gv_vector4 offest;
					offest.set((float)patch->get_offset().y, 0,
							   (float)patch->get_offset().x, 0);
					get_device_d3d9()->SetVertexShaderConstantF(0, &offest.x, 1);
					get_device_d3d9()->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST, 0, 0, GV_ROAM_PATCH_VTX_SIZE, 0,
						GV_ROAM_PATCH_SIZE * GV_ROAM_PATCH_SIZE * 2);
				}
			}
		} // fur layer
	}
	//============================================================================================
	//								:
	//============================================================================================
	void after_post_process()
	{
		for (int i = 0; i < m_terrain_with_fur.size(); i++)
		{
			render_terrain_with_fur(m_terrain_with_fur[i]);
		}
		m_terrain_with_fur.clear();
	}
	//
	gvt_array< gvt_ref_ptr< gv_com_terrain_roam > > m_terrain_with_fur;
};

GVM_IMP_CLASS(gv_com_roam_renderer, gv_component_renderer)
GVM_END_CLASS
};