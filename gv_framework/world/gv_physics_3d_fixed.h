#pragma once
namespace gv
{

class gv_physics_fixed_setting : public gv_object
{
public:
	GVM_DCL_CLASS(gv_physics_fixed_setting, gv_object);
	gv_physics_fixed_setting();
	~gv_physics_fixed_setting();

	gv_vector3i get_gravity() const;
	void set_gravity(const gv_vector3i& v);
	gv_boxi get_world_box() const;
	void set_world_box(const gv_boxi&);

protected:
	gv_vector3i m_gravity;
	gv_boxi m_world_box;
};
};