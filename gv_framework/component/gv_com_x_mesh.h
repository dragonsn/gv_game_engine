// kind of hack ,but very useful for some quick test and integration!
namespace gv
{
class gv_com_x_mesh : public gv_com_graphic
{
public:
	GVM_DCL_CLASS(gv_com_x_mesh, gv_com_graphic);
	gv_com_x_mesh()
	{
		link_class(gv_com_x_mesh::static_class());
		for (int i = 0; i < gve_render_pass_max; i++)
			this->m_com_render_id[i] = gv_id("gv_com_x_mesh_renderer");
	};
	~gv_com_x_mesh(){};
};
}