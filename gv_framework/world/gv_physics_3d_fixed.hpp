namespace gv
{

gv_physics_fixed_setting::gv_physics_fixed_setting()
{
	GVM_SET_CLASS(gv_physics_fixed_setting);
	m_gravity = gv_vector3i(0, -9800, 0); // set to be real G
};

gv_physics_fixed_setting::~gv_physics_fixed_setting(){

};

gv_vector3i gv_physics_fixed_setting::get_gravity() const
{
	return m_gravity;
};

void gv_physics_fixed_setting::set_gravity(const gv_vector3i& v)
{
	m_gravity = v;
};

gv_boxi gv_physics_fixed_setting::get_world_box() const
{
	return m_world_box;
};

void gv_physics_fixed_setting::set_world_box(const gv_boxi& b)
{
	m_world_box = b;
};

GVM_IMP_CLASS(gv_physics_fixed_setting, gv_object)
gv_vector3i m_gravity;
gv_boxi m_world_box;
GVM_END_CLASS
};