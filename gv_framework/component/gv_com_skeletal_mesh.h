
#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
class gv_skeletal_mesh;
class gv_com_skeletal_mesh : public gv_com_graphic
{
public:
	GVM_DCL_CLASS(gv_com_skeletal_mesh, gv_com_graphic);
	gv_com_skeletal_mesh();
	~gv_com_skeletal_mesh();
	bool is_first_frame();
	gv_skeletal* get_writable_skeletal() const;
	gv_skeletal* get_render_skeletal();
	gv_skeletal_mesh* get_skeletal_mesh();
	virtual bool set_resource(gv_resource* resource);
	virtual gv_static_mesh* get_static_mesh();
	virtual bool sync_to_entity(gv_entity*);
	virtual bool post_load();
	virtual gv_component* clone(gv_sandbox* sandbox);
	virtual void on_detach();
	virtual gv_box get_local_bbox();

protected:
	void flip_skeletal();
	void clone_skeletal();
	gvt_ref_ptr< gv_skeletal_mesh > m_skeletal_mesh;   //
	gvt_ref_ptr< gv_skeletal > m_skeletal_for_display; // for renderer!!
	gvt_ref_ptr< gv_skeletal >
		m_skeletal_for_write; // for animation or IK or other deform!!
	gv_bool m_is_first_frame;
};
}
