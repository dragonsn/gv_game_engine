namespace gv
{

gv_com_terrain_grass::gv_com_terrain_grass()
{
	GVM_SET_CLASS(gv_com_terrain_grass)
}
gv_com_terrain_grass::~gv_com_terrain_grass()
{
}
gv_material* gv_com_terrain_grass::get_grass_material()
{
	return gvt_cast< gv_material >(get_resource());
}

GVM_DCL_CLASS(gv_com_terrain_grass, gv_com_graphic);
GVM_VAR(gv_float, m_grass_width)
GVM_VAR(gv_float, m_grass_height)
GVM_VAR(gv_float, m_visible_radius)
GVM_VAR(gv_int, m_grass_number)
GVM_VAR(gv_bool, m_use_look_at)
GVM_END_CLASS
}