namespace gv
{

class gv_com_terrain_grass : public gv_com_graphic
{
	friend class gv_com_terrain_roam;

public:
	GVM_DCL_CLASS(gv_com_terrain_grass, gv_com_graphic);
	gv_com_terrain_grass();
	~gv_com_terrain_grass();
	gv_material* get_grass_material();

protected:
	gv_float m_grass_width;
	gv_float m_grass_height;
	gv_float m_visible_radius;
	gv_int m_grass_number;
	gv_bool m_use_look_at;
};
}
