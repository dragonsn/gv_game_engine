namespace gv
{
gv_com_skeletal_mesh::gv_com_skeletal_mesh()
{
	link_class(gv_com_skeletal_mesh::static_class());
	m_is_first_frame = true;
};

gv_com_skeletal_mesh::~gv_com_skeletal_mesh(){

};
gv_box gv_com_skeletal_mesh::get_local_bbox()
{
	if (m_skeletal_mesh)
		return m_skeletal_mesh->get_local_aabb();
	return gv_box();
};
void gv_com_skeletal_mesh::flip_skeletal()
{
	gvt_swap(m_skeletal_for_display, m_skeletal_for_write);
};

bool gv_com_skeletal_mesh::set_resource(gv_resource* resource)
{
	gv_skeletal_mesh* s_mesh = gvt_cast< gv_skeletal_mesh >(resource);
	if (!s_mesh)
		return false;
	gv_com_graphic::set_resource(s_mesh);
	this->m_skeletal_mesh = s_mesh;
	this->clone_skeletal();
	return true;
};

bool gv_com_skeletal_mesh::sync_to_entity(gv_entity*)
{
	this->flip_skeletal();
	m_skeletal_for_display->update_world_matrix();
	m_is_first_frame = false;
	return true;
};

gv_skeletal* gv_com_skeletal_mesh::get_writable_skeletal() const
{
	return (gv_skeletal*)this->m_skeletal_for_write;
};

void gv_com_skeletal_mesh::clone_skeletal()
{
	if (!m_skeletal_mesh)
		return;
	m_skeletal_mesh->get_skeletal()->update_world_matrix();
	if (this->m_skeletal_for_display)
		get_sandbox()->delete_object(this->m_skeletal_for_display);
	if (this->m_skeletal_for_write)
		get_sandbox()->delete_object(this->m_skeletal_for_write);
	this->m_skeletal_for_display =
		get_sandbox()->clone_object< gv_skeletal >(m_skeletal_mesh->get_skeletal());
	this->m_skeletal_for_write =
		get_sandbox()->clone_object< gv_skeletal >(m_skeletal_mesh->get_skeletal());
	this->m_skeletal_for_display->set_owner(this);
	this->m_skeletal_for_write->set_owner(this);
}

gv_skeletal* gv_com_skeletal_mesh::get_render_skeletal()
{
	return this->m_skeletal_for_display;
};

bool gv_com_skeletal_mesh::post_load()
{
	super::post_load();
	this->clone_skeletal();
	return true;
}

void gv_com_skeletal_mesh::on_detach()
{
	if (this->m_skeletal_for_display)
		get_sandbox()->delete_object(this->m_skeletal_for_display);
	if (this->m_skeletal_for_write)
		get_sandbox()->delete_object(this->m_skeletal_for_write);
	if (this->m_skeletal_for_display)
		m_skeletal_for_display->set_owner(NULL);
	if (this->m_skeletal_for_write)
		m_skeletal_for_write->set_owner(NULL);
};
gv_component* gv_com_skeletal_mesh::clone(gv_sandbox* sandbox)
{
	gv_com_skeletal_mesh* psk =
		gvt_cast< gv_com_skeletal_mesh >(super::clone(sandbox));
	return psk;
};

gv_skeletal_mesh* gv_com_skeletal_mesh::get_skeletal_mesh()
{
	return m_skeletal_mesh;
};

gv_static_mesh* gv_com_skeletal_mesh::get_static_mesh()
{
	if (m_skeletal_mesh)
		return m_skeletal_mesh->get_t_mesh();
	return NULL;
};

GVM_IMP_CLASS(gv_com_skeletal_mesh, gv_com_graphic)
GVM_VAR(gvt_ref_ptr< gv_skeletal_mesh >, m_skeletal_mesh)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR_ATTRIB_SET(no_clone)
GVM_VAR(gvt_ref_ptr< gv_skeletal >, m_skeletal_for_display)
GVM_VAR(gvt_ref_ptr< gv_skeletal >, m_skeletal_for_write)
GVM_VAR(gv_bool, m_is_first_frame)
GVM_END_CLASS
}