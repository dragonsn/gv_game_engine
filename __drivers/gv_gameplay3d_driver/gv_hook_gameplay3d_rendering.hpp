#include "gv_framework/renderer/gv_shader.h"
namespace gv
{
bool gv_hook_gameplay3d::drawSceneNode(Node* node)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_drawSceneNode, 0);
	gv_com_graphic* com =
		gvt_cast< gv_com_graphic >((gv_object*)node->getUserPointer());
	// terrain?
	if (node->getTerrain() && !m_hide_terrain)
	{
		node->getTerrain()->draw();
		return true;
	}
	// draw the model
	Model* model = node->getModel();
	if (!model)
		return false;
	if (com)
	{
		com->set_visible(true);
		updateMaterials(com, model);
	};

	model->draw();
	return true;
}

void gv_hook_gameplay3d::updateCamera(gv_com_camera* c)
{
	static bool not_override_camera = true;
	m_current_camera = c;
	if (c && _camera && not_override_camera)
	{
		_camera->setNearPlane(c->get_near_clip());
		_camera->setFarPlane(c->get_far_clip());
		_camera->setAspectRatio(c->get_ratio());
		_camera->setFieldOfView(c->get_fov());
		gv_matrix44 m;
		gv_vector3 pos;
		m = c->get_world_rotation();
		pos = c->get_world_position();
		_cameraNode->set(Vector3(1, 1, 1), gv_gp_helper::to_gp(m),
						 gv_gp_helper::to_gp(pos));
		// gv_string_tmp s;
		// s<<"camera pos: "<<pos;
		// s<<"-----camera rotation:"<< m;
		// gv_global::debug_draw->draw_string(*s, gv_vector2i(20,140),
		// gv_color::BLUE_B());

		if (_sky)
		{
			_sky->setTranslationX(_cameraNode->getTranslationX());
			_sky->setTranslationZ(_cameraNode->getTranslationZ());
		}
	};
};

template < class type_of_vector >
void gv_gp_add_vertex(gvt_array< gv_float >& vertices, const type_of_vector& v)
{
	for (int i = 0; i < type_of_vector::vector_size; i++)
	{
		vertices.push_back((gv_float)v[i]);
	}
};

bool gv_hook_gameplay3d::updateMaterialParam(gv_material* mat_gv,
											 Material* material)
{
	// material->getParameter("u_ambientColor")->setValue(Vector3(2, 0, 0));
	gv_bool is_editor = gv_global::framework_config.is_editor;
	for (int i = 0; i < mat_gv->m_material_float_params.size(); i++)
	{
		gv_material_param_float4& param = mat_gv->m_material_float_params[i];
		if ((is_editor || !param.m_synced) && param.m_count &&
			!param.m_id.is_empty())
		{
			param.m_synced = true;
			MaterialParameter* param_gp = material->getParameter(*param.m_id);
			if (!param_gp)
			{
				GVM_WARNING("failed to find parameter " << param.m_id);
			}
			else
			{
				switch (param.m_count)
				{
				case 1:
					param_gp->setValue(param.m_data.x);
					break;
				case 2:
					param_gp->setValue(Vector2(param.m_data.x, param.m_data.y));
					break;
				case 3:
					param_gp->setValue(
						Vector3(param.m_data.x, param.m_data.y, param.m_data.z));
					break;
				case 4:
					param_gp->setValue(Vector4(param.m_data.x, param.m_data.y,
											   param.m_data.z, param.m_data.w));
					break;
				default:
					GV_ASSERT(0);
					break;
				}
			}
		}
	}
	for (int i = 0; i < mat_gv->m_material_textures.size(); i++)
	{
		Texture::Sampler* sampler;
		gv_material_tex& tex = mat_gv->m_material_textures[i];
		gv_string_tmp tex_file;
		tex_file = tex.m_file_name;
		if (tex.m_texture)
			tex_file = *tex.m_texture->get_file_name();
		if ((is_editor || !tex.m_synced) && !tex_file.is_empty_string())
		{
			tex.m_synced = true;
			MaterialParameter* param_gp = material->getParameter(*tex.m_id);
			if (!param_gp)
			{
				GVM_WARNING("failed to find sample " << tex.m_id);
			}
			else
			{
				sampler = param_gp->setValue(*getCachedTextureName(tex_file), true);
				if (tex.m_use_tiling)
					sampler->setWrapMode(Texture::REPEAT, Texture::REPEAT);
				else
					sampler->setWrapMode(Texture::CLAMP, Texture::CLAMP);
				sampler->setFilterMode(Texture::LINEAR, Texture::LINEAR);
			}
		}
	}
	return true;
};

bool gv_hook_gameplay3d::precacheMaterial(gv_material* material_gv,
										  Material*& material)
{
	// Create the material for the cube model and assign it to the first mesh
	// part.
	GV_PROFILE_EVENT(gv_hook_gameplay3d_precacheMaterial, 0);
	gv_cache_material_gameplay* cache =
		material_gv->get_hardware_cache< gv_cache_material_gameplay >();
	if (cache)
	{
		material = cache->m_material_gp;
		updateMaterialParam(material_gv, material);
		return false;
	}
	if (material_gv->get_file_name().is_empty_string())
	{
		material_gv->set_file_name(
			*gv_game_engine::static_get()->get_default_material()->get_file_name());
	}
	cache = material_gv->get_sandbox()
				->create_nameless_object< gv_cache_material_gameplay >();
	static int use_material_table = 1;
	if (use_material_table)
	{
		Material** pparent_material =
			_materialTable.find(material_gv->get_file_name());
		Material* parent_material;
		;
		if (!pparent_material)
		{
			gv_string_tmp fn = *material_gv->get_file_name();
			fn.replace_all("\\", "/");
			parent_material = Material::create(*fn);
			_materialTable.add(material_gv->get_file_name(), parent_material);
		}
		else
		{
			parent_material = *pparent_material;
		}
		// we only use the first effect.
		Effect* peffect =
			parent_material->getTechniqueByIndex(0)->getPassByIndex(0)->getEffect();
		NodeCloneContext context;
		material = parent_material->clone(context);
		// material=Material::create(peffect);
		// material->setParent(parent_material);
	}
	else
	{
		gv_string_tmp fn = *material_gv->get_file_name();
		fn.replace_all("\\", "/");
		material = Material::create(*fn);
	}
	cache->m_material_gp = material;
	material_gv->set_hardware_cache(cache);
	updateMaterialParam(material_gv, material);
	return true;
};

Mesh* gv_hook_gameplay3d::precacheStaticMesh(gv_static_mesh* mesh_gv)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_precacheStaticMesh, 0);
	gv_cache_mesh_gameplay* cache =
		gvt_cast< gv_cache_mesh_gameplay >(mesh_gv->get_hardware_cache());
	if (!cache)
		cache = mesh_gv->get_sandbox()
					->create_nameless_object< gv_cache_mesh_gameplay >();
	else
		return cache->m_mesh_gp;
	int vertex_count = mesh_gv->get_nb_vertex();
	GV_ASSERT(vertex_count && "empty mesh!!");
	GV_ASSERT(vertex_count < 65535 && "gamplay3d don't support very big mesh !!");
	gv_vertex_buffer* vb = mesh_gv->get_vb();
	gvt_array_cached< VertexFormat::Element, 32 > elements;
	gv_int vertex_size = 3;
	elements.push_back(VertexFormat::Element(VertexFormat::POSITION, 3));
	if (vb->m_raw_normal.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::NORMAL, 3));
		vertex_size += 3;
	}
	if (vb->m_raw_binormal.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::BINORMAL, 3));
		vertex_size += 3;
	}
	if (vb->m_raw_tangent.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TANGENT, 3));
		vertex_size += 3;
	}
	if (vb->m_raw_texcoord0.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD0, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord1.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD1, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord2.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD2, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord3.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD3, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord4.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD4, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord5.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD5, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord6.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD6, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_texcoord7.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::TEXCOORD7, 2));
		vertex_size += 2;
	}
	if (vb->m_raw_blend_index.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::BLENDINDICES, 4));
		vertex_size += 4;
	}
	if (vb->m_raw_blend_weight.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::BLENDWEIGHTS, 4));
		vertex_size += 4;
	}
	if (vb->m_raw_color.size())
	{
		elements.push_back(VertexFormat::Element(VertexFormat::COLOR, 3));
		vertex_size += 3;
	}
	Mesh* mesh = Mesh::createMesh(
		VertexFormat(elements.get_data(), (unsigned int)elements.size()),
		vertex_count, false);
	if (mesh == NULL)
	{
		GVM_ERROR("Failed to create mesh.");
		return NULL;
	}
	// let us set the vertex buffer !
	gvt_array< gv_float > big_vertices;
	big_vertices.reserve(vertex_size * vertex_count);
	for (int i = 0; i < vertex_count; i++)
	{
		gv_gp_add_vertex(big_vertices, vb->m_raw_pos[i]);
		if (vb->m_raw_normal.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_normal[i]);
		}
		if (vb->m_raw_binormal.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_binormal[i]);
		}
		if (vb->m_raw_tangent.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_tangent[i]);
		}
		if (vb->m_raw_texcoord0.size())
		{
			gv_vector2 v = vb->m_raw_texcoord0[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord1.size())
		{
			gv_vector2 v = vb->m_raw_texcoord1[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord2.size())
		{
			gv_vector2 v = vb->m_raw_texcoord2[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord3.size())
		{
			gv_vector2 v = vb->m_raw_texcoord3[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord4.size())
		{
			gv_vector2 v = vb->m_raw_texcoord4[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord5.size())
		{
			gv_vector2 v = vb->m_raw_texcoord5[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord6.size())
		{
			gv_vector2 v = vb->m_raw_texcoord6[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_texcoord7.size())
		{
			gv_vector2 v = vb->m_raw_texcoord7[i];
			v.y = 1.f - v.y;
			gv_gp_add_vertex(big_vertices, v);
		}
		if (vb->m_raw_blend_index.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_blend_index[i]);
		}
		if (vb->m_raw_blend_weight.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_blend_weight[i]);
		}
		if (vb->m_raw_color.size())
		{
			gv_gp_add_vertex(big_vertices, vb->m_raw_color[i]);
		}
	}
	GV_ASSERT(big_vertices.size() == vertex_size * vertex_count);
	mesh->setVertexData(big_vertices.get_data(), 0, vertex_count);
	// let's set index buffer
	gv_index_buffer* ib = mesh_gv->m_index_buffer;
	gvt_array< gv_ushort > ib_16;
	ib_16 = ib->m_raw_index_buffer;
	if (mesh_gv->get_nb_segment())
	{
		for (int i = 0; i < mesh_gv->get_nb_segment(); i++)
		{
			gv_mesh_segment* seg = mesh_gv->get_segment(i);
			MeshPart* meshPart =
				mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16,
							  seg->m_index_size /*ib_16.size()*/, false);
			meshPart->setIndexData(ib_16.get_data() + seg->m_start_index, 0,
								   seg->m_index_size);
		}
	}
	else
	{
		MeshPart* meshPart =
			mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, ib_16.size(), false);
		meshPart->setIndexData(ib_16.get_data(), 0, ib_16.size());
	}
	mesh->setBoundingBox(gv_gp_helper::to_gp(mesh_gv->get_bbox()));
	mesh->setBoundingSphere(gv_gp_helper::to_gp(mesh_gv->get_bsphere()));
	cache->m_mesh_gp = mesh;
	mesh_gv->set_hardware_cache(cache);
	return mesh;
}

gv_string_tmp gv_hook_gameplay3d::getCachedTextureName(gv_texture* tex)
{
	return getCachedTextureName(gv_text(tex->get_file_name()));
};

gv_string_tmp gv_hook_gameplay3d::getCachedTextureName(const char* file_name)
{
	gv_string_tmp name;
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
	name = *gv_framework_config::get_texture_cache_path();;
	name += sandbox->get_file_manager()->get_main_name(file_name);
	name += ".dds";
	name = sandbox->logical_resource_path_to_absolute_path(name);
	return name;
};

void gv_hook_gameplay3d::precacheTerrain(class gv_com_terrain_roam* terrain)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_precacheTerrain, 0);
	if (!terrain->build_static_mesh())
	{
		clearScene();
		if (terrain->get_resource())
		{
			// Load scene
			clearScene();
			_scene = Scene::load(*terrain->get_resource()->get_file_name());
			_terrain = _scene->findNode("terrain");
			_sky = _scene->findNode("sky");
			_terrain->setUserPointer(terrain);
			terrain->set_user_data(_terrain);
			Terrain* terrain_gp = _terrain->getTerrain();
			if (terrain_gp)
			{
				gvt_array< gv_float > a;
				HeightField* height_map = terrain_gp->_heightfield;
				a.init(height_map->getArray(),
					   height_map->getColumnCount() * height_map->getRowCount());
				a.for_each(gvf_mul_assign< gv_float >(terrain_gp->_localScale.y));
				terrain->init(a, height_map->getRowCount(),
							  height_map->getColumnCount());
			}
			createCamera();
		}
	}
}

void gv_hook_gameplay3d::addRenderable(gv_component* in_com)
{
	GV_ASSERT(in_com);
	GV_PROFILE_EVENT(gv_hook_gameplay3d_addRenderable, (gv_int_ptr)in_com);
	m_renderable_components.push_back(in_com);
	if (!in_com->get_entity()->get_world())
	{
		return;
	}
	gv_com_graphic* com = gvt_cast< gv_com_graphic >(in_com);
	gv_com_terrain_roam* terrain = gvt_cast< gv_com_terrain_roam >(in_com);
	if (terrain)
	{
		precacheTerrain(terrain);
		return;
	}
	gv_com_static_mesh* com_static_mesh = gvt_cast< gv_com_static_mesh >(com);
	gv_com_skeletal_mesh* com_skeletal_mesh = gvt_cast< gv_com_skeletal_mesh >(com);
	gv_static_mesh* mesh_gv = com->get_resource< gv_static_mesh >();
	gv_skeletal_mesh* sk_mesh_gv = com->get_resource< gv_skeletal_mesh >();
	if (!com->get_material())
	{
		return;
	}
	if (!mesh_gv && sk_mesh_gv)
		mesh_gv = sk_mesh_gv->get_t_mesh();
	if (!mesh_gv)
		return;
	// to support terrain later
	if (sk_mesh_gv)
	{
		sk_mesh_gv->optimize_bones(GP_MAX_GPU_SKIN_BONE_NUMBER);
	}
	Mesh* mesh = precacheStaticMesh(mesh_gv);
	Model* model = Model::create(mesh);
	GV_ASSERT(model);
	// to handle skeletal mesh
	if (sk_mesh_gv)
	{
		gv_skeletal* skeletal = sk_mesh_gv->get_skeletal();
		MeshSkin* skin = new MeshSkin();
		if (!sk_mesh_gv->m_bone_mapping.size())
		{
			GV_ASSERT(skeletal->get_nb_bone() <= GP_MAX_GPU_SKIN_BONE_NUMBER);
			skin->setJointCount(skeletal->get_nb_bone());
		}
		else
		{
			GV_ASSERT(sk_mesh_gv->m_nb_bone_after_map <= GP_MAX_GPU_SKIN_BONE_NUMBER);
			skin->setJointCount(sk_mesh_gv->m_nb_bone_after_map);
		}
		model->setSkin(skin);
	}
	updateMaterials(com, model);
	// create node for the entity .
	gv_string_tmp node_name;
	const gv_entity* entity = com->get_entity();
	node_name << com->get_entity()->get_name();
	Node* node = _scene->addNode(*node_name);
	node->setModel(model);
	node->setUserPointer(com);
	gv_matrix44 tm = entity->get_tm();
	gv_matrix44 m;
	gv_vector3 pos;
	tm.get_trans(pos);
	tm.get_rotation(m);
	node->set(gv_gp_helper::to_gp(entity->get_scale()), gv_gp_helper::to_gp(m),
			  gv_gp_helper::to_gp(pos));
	com->set_user_data(node);
};

bool gv_hook_gameplay3d::updateMaterials(gv_com_graphic* com, Model* model)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_updateMaterials, 0);
	Material* materialClone;
	gv_static_mesh* mesh_gv = com->get_resource< gv_static_mesh >();
	gv_skeletal_mesh* sk_mesh_gv = com->get_resource< gv_skeletal_mesh >();
	if (sk_mesh_gv)
		mesh_gv = sk_mesh_gv->get_t_mesh();
	bool updated = precacheMaterial(com->get_material(), materialClone);
	if (updated || (materialClone && model->getMaterial() != materialClone))
	{
		model->setMaterial(materialClone);
	}
	if (com->get_material() && com->get_material()->get_nb_sub_material())
	{
		for (int i = 0; i < com->get_material()->get_nb_sub_material(); ++i)
		{
			gv_material* material = com->get_sub_material(i);
			if (material)
			{
				updated = precacheMaterial(material, materialClone);
				if (updated ||
					(materialClone && model->getMaterial(i) != materialClone))
					model->setMaterial(materialClone, i);
			}
		}
	}
	else if (mesh_gv->get_nb_segment())
	{
		for (int i = 0; i < mesh_gv->get_nb_segment(); ++i)
		{
			gv_material* material = mesh_gv->get_segment(i)->m_material;
			gv_mesh_segment* seg = mesh_gv->get_segment(i);
			if (material)
			{
				updated = precacheMaterial(material, materialClone);
				if (seg->m_is_hidden)
				{
					materialClone = NULL;
					model->setMaterial(materialClone, i);
				}
				else if (updated ||
						 (materialClone && model->getMaterial(i) != materialClone))
					model->setMaterial(materialClone, i);
			}
		}
	}
	return true;
};

void gv_hook_gameplay3d::removeRenderable(gv_component* component)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_removeRenderable, (gv_int_ptr)component);
	Node* node = (Node*)component->get_user_data();
	_scene->removeNode(node);
	component->set_user_data(NULL);
	m_renderable_components.remove(component);
};

void gv_hook_gameplay3d::render(float elapsedTime)
{
	// Clear the color and depth buffers.
	GV_PROFILE_EVENT(gv_gameplay_hook__render, 0);
	clear(CLEAR_COLOR_DEPTH,
		  gv_gp_helper::to_gp(gv_colorf(gv_global::rnd_opt.m_clear_color).v),
		  1.0f, 0);
	drawScene();
	gvt_hash_map< gv_string, Form* >::iterator it;
	it = _formTable.begin();
	while (it != _formTable.end())
	{
		if (!it.is_empty())
		{
			Form* f = (*it);
			if (f->isEnabled())
				f->draw();
		}
		++it;
	}
	if (!gv_global::rnd_opt.m_no_debug_draw)
	{
		gv_debug_renderer_gameplay3d::static_get()->render();
	}
}

void gv_hook_gameplay3d::drawScene()
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d_drawScene, 0);
	if (_scene)
	{
		for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
			_renderQueues[i].clear();
		_scene->visit(this, &gv_hook_gameplay3d::buildRenderQueues);
		m_debug_string << "visible opaque:" << _renderQueues[QUEUE_OPAQUE].size();
		for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
		{
			std::vector< Node* >& queue = _renderQueues[i];

			for (size_t j = 0, ncount = queue.size(); j < ncount; ++j)
			{
				drawSceneNode(queue[j]);
			}
		}
		//_scene->visit(this, &gv_hook_gameplay3d::drawSceneNode);
		gv_debug_renderer_gameplay3d::static_get()->set_view_projection_matrix_3d(
			_camera->getViewProjectionMatrix());
	}
}

void gv_hook_gameplay3d::reloadShaderCache()
{
	Effect::clearShaderCache();
	_materialTable.clear();
}

bool gv_hook_gameplay3d::buildRenderQueues(Node* node)
{
	GV_PROFILE_EVENT(gv_hook_gameplay3d__buildRenderQueues, 0);
	gv_com_graphic* com =
		gvt_cast< gv_com_graphic >((gv_object*)node->getUserPointer());
	// update matrix
	if (com)
	{
		// make it rendered, so the animation & other controller get updated .
		const gv_entity* entity = com->get_entity();
		gv_matrix44 tm = entity->get_tm();
		gv_matrix44 m;
		gv_vector3 pos;
		tm.get_trans(pos);
		tm.get_rotation(m);
		node->set(gv_gp_helper::to_gp(entity->get_scale()), gv_gp_helper::to_gp(m),
				  gv_gp_helper::to_gp(pos));
	}
	if (node->getTerrain())
	{
		_renderQueues[QUEUE_TERRAIN].push_back(node);
		return true;
	}
	Model* model = node->getModel();
	if (model)
	{
		// Perform view-frustum culling for this node
		if (__viewFrustumCulling &&
			!node->getBoundingSphere().intersects(
				_scene->getActiveCamera()->getFrustum()))
			return true;

		// update bone matrices
		if (model->getSkin() && com)
		{
			const gv_entity* entity = com->get_entity();
			// gv_matrix44 root_tm=entity->get_tm();
			gvt_array_cached< gv_vector4, 64 > matrix;
			gv_com_skeletal_mesh* com_sk_mesh = gvt_cast< gv_com_skeletal_mesh >(
				(gv_object*)model->getNode()->getUserPointer());
			GV_ASSERT(com_sk_mesh);
			gv_skeletal_mesh* skeletal_mesh = com_sk_mesh->get_skeletal_mesh();
			gv_skeletal* skeletal = com_sk_mesh->get_render_skeletal();
			if (!skeletal_mesh->m_bone_mapping.size())
			{
				GV_ASSERT(skeletal->get_nb_bone() <= GP_MAX_GPU_SKIN_BONE_NUMBER);
				matrix.resize(skeletal->get_nb_bone() * 3);
				for (int i = 0; i < skeletal->get_nb_bone(); i++)
				{
					gv_bone& bone = skeletal->m_bones[i];
					gv_matrix44 m = bone.m_matrix_model_to_bone * bone.m_tm;
					// m.set_identity();
					// m*=gv_matrix44::get_rotation_by_z(gv_global::time->get_sec_from_start()*i);//test;
					m.transpose_self();
					matrix[i * 3 + 0] = m.av4[0];
					matrix[i * 3 + 1] = m.av4[1];
					matrix[i * 3 + 2] = m.av4[2];
				}
			}
			else
			{
				GV_ASSERT(skeletal_mesh->m_nb_bone_after_map <=
						  GP_MAX_GPU_SKIN_BONE_NUMBER);
				matrix.resize(skeletal_mesh->m_nb_bone_after_map * 3);
				for (int i = 0; i < skeletal_mesh->m_nb_bone_after_map; i++)
				{
					gv_bone& bone =
						skeletal->m_bones[skeletal_mesh->m_bone_inv_mapping[i]];
					gv_matrix44 m = bone.m_matrix_model_to_bone * bone.m_tm;
					m.transpose_self();
					matrix[i * 3 + 0] = m.av4[0];
					matrix[i * 3 + 1] = m.av4[1];
					matrix[i * 3 + 2] = m.av4[2];
				}
			}
			model->getSkin()->setMatrixPalette((gv_float*)matrix.get_data());
		}
		// Determine which render queue to insert the node into
		std::vector< Node* >* queue;
		if (node->hasTag("transparent"))
			queue = &_renderQueues[QUEUE_TRANSPARENT];
		else
			queue = &_renderQueues[QUEUE_OPAQUE];

		queue->push_back(node);
	}
	return true;
}
}