namespace gv
{
gv_com_graphic::gv_com_graphic()
{
	m_has_visible_callback = false;
	m_always_visible = false;
	m_is_visible = false;
	m_segment_hide_mask = 0;
	m_visible_test_tag = 0;
	m_visible_node.object = this;
	m_visible_tag_enabled = false;
	m_is_background = false;
	m_is_component_tm_enabled = false;
	m_is_post_effect = false;
	m_render_priority = 0;
	m_is_particle = false;
	m_component_matrix.set_identity();
	m_is_in_z_pass = 0;
	link_class(gv_com_graphic::static_class());
}
gv_com_graphic::~gv_com_graphic()
{
}
void gv_com_graphic::set_material(gv_material* material)
{
	this->m_material = material;
}
gv_material* gv_com_graphic::get_material()
{
	return this->m_material;
};
void gv_com_graphic::hide_segment(gv_int seg)
{
	this->m_segment_hide_mask |= (1 << seg);
};
void gv_com_graphic::unhide_segment(gv_int seg)
{
	this->m_segment_hide_mask &= ~(1 << seg);
};
bool gv_com_graphic::is_segment_hided(gv_int seg)
{
	return (this->m_segment_hide_mask & (1 << seg)) != 0;
};
void gv_com_graphic::unhide_all_segment()
{
	this->m_segment_hide_mask = 0xffffffff;
};
void gv_com_graphic::hide_all_segment()
{
	this->m_segment_hide_mask = 0;
};

gv_component* gv_com_graphic::clone(gv_sandbox* sandbox)
{
	gv_component* pcom = super::clone(sandbox);
	// clone material if necessary!!
	gv_com_graphic* pg = gvt_cast< gv_com_graphic >(pcom);
	gv_material* pmat = pg->get_material();
	if (pmat)
	{
		pmat = pmat->clone(sandbox);
		pmat->set_owner(pg);
		pg->set_material(pmat);
	}
	return pcom;
};

void gv_com_graphic::create_sub_material_from_resource()
{
	gv_material* mat = get_material();
	if (!mat)
	{
		mat = m_sandbox->create_object< gv_material >();
		set_material(mat);
	}
	gv_static_mesh* pmesh = this->get_static_mesh();
	if (!pmesh)
		return;
	mat->set_nb_sub_material(pmesh->get_nb_segment());
	for (int i = 0; i < mat->get_nb_sub_material(); i++)
	{
		gv_material* sub_m = pmesh->get_segment(i)->m_material;
		if (sub_m)
		{
			mat->set_sub_material(i, sub_m->clone(get_sandbox()));
		}
	}
};

gv_material* gv_com_graphic::get_sub_material(int idx)
{
	gv_material* material = m_material->get_sub_material(idx);
	if (material)
		return material;
	gv_static_mesh* pmesh = this->get_static_mesh();
	if (!pmesh)
		return NULL;
	if (pmesh->get_segment(idx))
		return pmesh->get_segment(idx)->m_material;
	return NULL;
}

gv_box gv_com_graphic::get_local_bbox()
{
	gv_static_mesh* s_mesh = get_static_mesh();
	if (s_mesh)
	{
		return s_mesh->get_bbox();
	}
	else
		return gv_box();
};

gv_sphere gv_com_graphic::get_local_bsphere()
{
	gv_static_mesh* s_mesh = get_static_mesh();
	if (s_mesh)
	{
		return s_mesh->get_bsphere();
	}
	else
		return gv_sphere();
};
//============================================================================================
//								:
//============================================================================================
GV_STATIC_ASSERT(5 == gve_render_pass_max);
GVM_IMP_CLASS(gv_com_graphic, gv_component)
GVM_STATIC_ARRAY(gv_id, 5, m_com_render_id)
GVM_VAR(gvt_ref_ptr< gv_resource >, m_main_resource)
GVM_VAR(gvt_ref_ptr< gv_material >, m_material)
GVM_VAR(gv_uint, m_segment_hide_mask)
GVM_VAR(gv_bool, m_is_background)
GVM_VAR(gv_bool, m_is_particle)
GVM_VAR(gv_bool, m_always_visible)
GVM_VAR(gv_bool, m_is_component_tm_enabled)
GVM_VAR(gv_matrix44, m_component_matrix)
GVM_VAR(gv_uint, m_render_priority)
GVM_VAR(gv_bool, m_is_post_effect)
GVM_VAR_ATTRIB_SET(transient)
GVM_STATIC_ARRAY(gvt_ptr< gv_component_renderer >, 5, m_renderers)
GVM_END_CLASS

GVM_IMP_CLASS(gv_com_x_mesh, gv_com_graphic)
GVM_END_CLASS

GVM_BGN_FUNC(gv_com_graphic, scp_switch_wireframe)
switch_wireframe();
return 1;
GVM_END_FUNC;
}