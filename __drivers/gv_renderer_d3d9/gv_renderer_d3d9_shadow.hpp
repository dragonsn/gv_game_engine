namespace gv
{
gv_bool gv_renderer_d3d9::is_rendering_z_pass()
{
	return get_private_data()->m_is_rendering_z_pass;
};

void gv_renderer_d3d9::render_z_pass()
{
	if (!get_private_data()->m_depth_only_pass_material)
	{
		gv_material* mat;
		mat = get_private_data()->m_depth_only_pass_material =
			get_sandbox()->create_nameless_object< gv_material >();
		mat->m_material_textures.resize(1);
		mat->m_material_textures[0].m_texture = get_default_texture();
	}
	gv_material* mat_depth_only = get_private_data()->m_depth_only_pass_material;
	gvt_ref_ptr< gv_material > mat_tmp;
	gv_effect* engine_effect = gv_global::rnd_opt.m_depth_only_effect;
	if (!engine_effect)
		return;
	//============================================================================================
	// this->get_render_target_mgr()->begin_render_target(get_depth_buffer_tex(),NULL,false,gv_color::WHITE());
	get_render_target_mgr()->lock_render_target(true);
	gv_effect* depth_only_effect =
		engine_effect->get_sub_effect(gv_id("depth_only"));
	gv_effect* depth_only_effect_terrain =
		engine_effect->get_sub_effect(gv_id("depth_only_terrain"));
	get_private_data()->m_is_rendering_z_pass = true;
	for (int i = 0; i < this->m_visible_components.size(); i++)
	{
		gv_com_graphic* graphic = this->m_visible_components[i];
		graphic->set_is_in_z_pass(false);
		if (graphic->is_background())
			continue;
		mat_tmp = graphic->get_material();
		if (!mat_tmp)
			continue;
		gv_effect* old_effect = mat_tmp->get_effect();
		if (old_effect && old_effect->is_no_depth_writing())
		{
			continue;
		}
		graphic->set_material(mat_depth_only);
		if (graphic->is_a(gv_com_terrain_roam::static_class()))
		{
			mat_depth_only->set_effect(depth_only_effect_terrain);
			get_terrain_renderer()->render_component(graphic);
		}
		else
		{
			mat_depth_only->set_effect(depth_only_effect);
			get_effect_renderer()->render_component(graphic);
			graphic->set_is_in_z_pass(true);
		}
		graphic->set_material(mat_tmp);
	}
	get_private_data()->m_is_rendering_z_pass = false;
	get_render_target_mgr()->lock_render_target(false);
	this->get_render_target_mgr()->resolve_backbuffer(get_depth_buffer_tex());
	this->get_render_target_mgr()->set_default_render_target();
};

//============================================================================================
//
//============================================================================================

static gv_vector4 static_scale_xyz[gvc_nb_shadow_map] = {
	gv_vector4(0.01f, 0.01f, 0.00001f, 0.000004f),
	gv_vector4(0.3f, 0.3f, 0.001f, 0.0001f),
	// gv_vector4 (0.01, 0.01, 0.01, 1.0),
	// gv_vector4 (0.01, 0.01, 0.01, 1.0),
};

static gv_vector2 static_pos_offset[gvc_nb_shadow_map] = {
	gv_vector2(100, 400), gv_vector2(5, 40),
};
//============================================================================================
void gv_renderer_d3d9::render_shadow_map()
{
	if (!get_private_data()->m_shadow_pass_material)
	{
		get_private_data()->m_shadow_pass_material =
			get_sandbox()->create_nameless_object< gv_material >();
	}
	gv_material* mat_shadow_pass = get_private_data()->m_shadow_pass_material;
	gvt_ref_ptr< gv_material > mat_tmp;
	gv_effect* engine_effect = gv_global::rnd_opt.m_shadow_pass_effect;
	if (!engine_effect)
		return;
	engine_effect = engine_effect->get_sub_effect(gv_id("shadow_effects"));
	if (!engine_effect)
		return;
	gv_effect* shadow_pass_effect =
		engine_effect->get_sub_effect(gv_id("shadow_pass"));
	gv_effect* shadow_pass_effect_terrain =
		engine_effect->get_sub_effect(gv_id("shadow_pass_terrain"));
	//============================================================================================
	// set
	// mat_shadow_pass->m_material_float_params
	// float4x4 static_view_light_matrix   : register(c16);
	{
		gv_matrix44 view_light_matrix;
		view_light_matrix.set_identity();
		gv_vector3 dirZ = -get_sun_dir();
		dirZ.normalize();
		gv_vector3 up = gv_vector3(0, 1, 0);
		gv_vector3 dirX = up.cross(dirZ);
		gv_vector3 dirY = dirZ.cross(dirX);
		dirX.normalize();
		dirY.normalize();
		view_light_matrix.get_axis_x() = dirX;
		view_light_matrix.get_axis_y() = dirY;
		view_light_matrix.get_axis_z() = dirZ;
		view_light_matrix.transpose_self();
		// get_device_d3d9()->SetVertexShaderConstantF(gvc_register_static_view_light_matrix
		// ,&view_light_matrix.m[0][0],4);
		get_private_data()->m_view_light_matrix = view_light_matrix;
	}

	//============================================================================================
	gv_vector4 static_light_pos_world_bak(0, 0, 0, 0),
		static_scale_xyz_bak(0, 0, 0, 0);
	for (int shadow_map = 0; shadow_map < gvc_nb_shadow_map; shadow_map++)
	{
		this->get_render_target_mgr()->begin_render_target(
			get_private_data()->m_shadow_map_textures[shadow_map],
			get_private_data()->m_shadow_map_depth_buffer, true, gv_color::WHITE());
		// if( SUCCEEDED( get_device_d3d9()->BeginScene() ) )
		{
			get_render_target_mgr()->lock_render_target(true);
			get_private_data()->m_is_rendering_z_pass = true;
			gv_sphere camera_sphere;
			gv_vector3 static_light_pos_world = get_main_camera_pos();
			camera_sphere.pos = static_light_pos_world +=
				get_main_camera_dir() * static_pos_offset[shadow_map].x;
			static_light_pos_world += get_sun_dir() * static_pos_offset[shadow_map].y;
			camera_sphere.r = static_pos_offset[shadow_map].y;
			// get_device_d3d9()->SetVertexShaderConstantF(gvc_register_static_light_pos_world
			// ,&static_light_pos_world.x,1);
			get_private_data()->m_light_pos_world_shadow_pass =
				get_private_data()->m_shadow_light_pos_world[shadow_map] =
					static_light_pos_world;
			// get_device_d3d9()->SetVertexShaderConstantF(gvc_register_static_scale_xyz
			// ,&static_scale_xyz[shadow_map].x,1);
			get_private_data()->m_scale_xyz_shadow_pass =
				get_private_data()->m_shadow_scale_xyz[shadow_map] =
					static_scale_xyz[shadow_map];
			for (int i = 0; i < this->m_visible_components.size(); i++)
			{
				gv_com_graphic* graphic = this->m_visible_components[i];
				// simple visible test for the shadow camera!
				const gv_box& world_aabb = graphic->get_entity()->get_world_aabb();
				if (!gv_geom::is_overlap_aabb_sphere(camera_sphere, world_aabb))
					continue;
				if (graphic->is_background())
					continue;
				mat_tmp = graphic->get_material();
				gv_effect* old_effect = mat_tmp->get_effect();
				if (old_effect && old_effect->is_no_depth_writing())
					continue;
				graphic->set_material(mat_shadow_pass);
				/*
        self shadow of terrain is huge...
        if (graphic->is_a(gv_com_terrain_roam::static_class()))
        {
                mat_shadow_pass->set_effect(shadow_pass_effect_terrain);
                get_terrain_renderer()->render_component(graphic);
        }
        else*/
				{
					mat_shadow_pass->set_effect(shadow_pass_effect);
					get_effect_renderer()->render_component(graphic);
				}
				graphic->set_material(mat_tmp);
			}
			get_render_target_mgr()->lock_render_target(false);
			// get_device_d3d9()->EndScene();
		}
	}
	get_private_data()->m_is_rendering_z_pass = false;
	this->get_render_target_mgr()->set_default_render_target();
	//============================================================================================
}
};