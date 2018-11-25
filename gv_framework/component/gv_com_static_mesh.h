#pragma once
namespace gv
{
class gv_static_mesh;
class gv_com_static_mesh : public gv_com_graphic
{
public:
	GVM_DCL_CLASS(gv_com_static_mesh, gv_com_graphic);
	gv_com_static_mesh();
	~gv_com_static_mesh();
	virtual bool set_resource(gv_resource* resource);
	virtual gv_static_mesh* get_static_mesh()
	{
		return m_static_mesh;
	};

public:
	gvt_ref_ptr< gv_static_mesh > m_static_mesh;
};
}