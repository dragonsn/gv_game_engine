namespace gv
{

gv_com_static_mesh::gv_com_static_mesh()
{
	link_class(gv_com_static_mesh::static_class());
};

gv_com_static_mesh::~gv_com_static_mesh(){

};

bool gv_com_static_mesh::set_resource(gv_resource* resource)
{
	gv_static_mesh* s_mesh = gvt_cast< gv_static_mesh >(resource);
	if (!s_mesh)
		return false;
	gv_com_graphic::set_resource(s_mesh);
	this->m_static_mesh = s_mesh;
	return true;
};

GVM_IMP_CLASS(gv_com_static_mesh, gv_com_graphic)
GVM_VAR(gvt_ref_ptr< gv_static_mesh >, m_static_mesh)
GVM_END_CLASS
}