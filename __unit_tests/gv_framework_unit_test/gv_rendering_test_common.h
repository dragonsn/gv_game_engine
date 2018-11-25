namespace gv
{
inline void create_skybox(gv_world* my_world, bool moving_cloud = false,
						  gv_effect** atmosphere_effect = NULL)
{

	// create sky box
	gv_string_tmp file_name = FILE_3DS_SPHERE;
	gv_model* model = m_sandbox->create_object< gv_model >(my_world);
	if (!m_sandbox->import_external_format(model, file_name))
	{
		return;
	}
	gv_static_mesh* pmesh = model->get_static_mesh(0);

	gv_effect_project* effect =
		m_sandbox->create_object< gv_effect_project >(my_world);
	file_name = FILE_RFX_NEW_SKY;
	m_sandbox->import_external_format(effect, *file_name);
	gv_effect* my_effect = effect->get_base_effect();
	if (atmosphere_effect)
		*atmosphere_effect = my_effect;
	if (!moving_cloud)
	{
		my_effect = my_effect->get_renderable_effect(gv_id("SkyBox"));
	}
	else
	{
		my_effect = my_effect->get_renderable_effect(gv_id("BlueSky"));
		gv_float f = 20.f;
		my_effect->set_param(gv_id("skyScale"), f);
	}
	gv_entity* sky_entity =
		m_sandbox->create_object< gv_entity >(gv_id("sky_entity"), my_world);
	gv_material* sky_material = m_sandbox->create_object< gv_material >();
	sky_material->m_effect = my_effect;
	gv_com_static_mesh* com_static_mesh =
		get_sandbox()->create_object< gv_com_static_mesh >();
	com_static_mesh->set_material(sky_material);
	com_static_mesh->set_resource(pmesh);
	com_static_mesh->set_renderer_id(gve_render_pass_opaque,
									 gv_id("gv_com_effect_renderer"));
	com_static_mesh->set_always_visible(true);
	com_static_mesh->set_background(true);
	sky_entity->add_component(com_static_mesh);
	my_world->add_entity(sky_entity);
	sky_entity->set_ignore_line_check(true);
}

inline void create_ground(gv_world* my_world,
						  gv_vector3 min_p = gv_vector3(0, -10, 0),
						  gv_vector3 max_p = gv_vector3(3000, -1, 8000))
{
	// create ground
	gv_string_tmp file_name = FILE_3DS_CRACKED_QUAD;
	gv_model* model = m_sandbox->create_object< gv_model >(my_world);
	if (!m_sandbox->import_external_format(model, file_name))
	{
		return;
	}
	gv_static_mesh* pmesh = model->get_static_mesh(0);
	gv_vector3 scale, offset;
	gv_box target;
	target.min_p = min_p;
	target.max_p = max_p;
	// target.move_to(gv_vector3(300,-6,300));
	pmesh->get_bbox().get_tranform(target, scale, offset);
	pmesh->rebuild_normal();
	pmesh->rebuild_binormal_tangent();
	pmesh->get_vb()->m_raw_normal = gv_vector3(0, 1, 0);
	pmesh->get_vb()->m_raw_tangent = gv_vector3(-1, 0, 0);
	pmesh->get_vb()->m_raw_binormal = gv_vector3(0, 0, -1);

	gv_effect_project* effect =
		m_sandbox->create_object< gv_effect_project >(my_world);
	file_name = FILE_RFX_MATERIAL_LIB;
	m_sandbox->import_external_format(effect, *file_name);
	gv_effect* my_effect = effect->get_base_effect();
	my_effect = my_effect->get_renderable_effect(gv_id("Textured Bump"));
	// my_effect=my_effect->get_renderable_effect(gv_id("Parallax") );
	gv_vector2 v(800, 300);
	// gv_vector2 v(2,2);
	my_effect->set_param(gv_id("g_UVScale"), v);

	gv_entity* sky_entity =
		m_sandbox->create_object< gv_entity >(gv_id("ground_entity"), my_world);
	gv_material* sky_material = m_sandbox->create_object< gv_material >();
	sky_material->m_effect = my_effect;
	gv_com_static_mesh* com_static_mesh =
		get_sandbox()->create_object< gv_com_static_mesh >();
	com_static_mesh->set_material(sky_material);
	com_static_mesh->set_resource(pmesh);
	com_static_mesh->set_renderer_id(gve_render_pass_opaque,
									 gv_id("gv_com_effect_renderer"));
	sky_entity->add_component(com_static_mesh);
	sky_entity->set_position(offset);
	sky_entity->set_scale(scale);
	sky_entity->update_matrix();
	com_static_mesh->set_always_visible(true);
	my_world->add_entity(sky_entity);
	sky_entity->set_ignore_line_check(true);
}
}