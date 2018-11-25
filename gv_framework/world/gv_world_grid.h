#pragma once
namespace gv
{
class gv_cell_3d;
class gv_world_grid;

class gv_cell_3d
{
public:
	typedef gvt_array_cached< gv_cell_3d*, 256 > cache;
	gv_cell_3d()
	{
		GVM_ZERO_ME;
	}

public:
	gv_int m_type;
	gv_int m_height_offset;
	gv_obj_list m_actor_list;
	gv_int m_test_tag;
	gvt_ptr< gv_world_grid > m_owner;
};

class gv_world_grid : public gvt_grid_2d< gv_cell_3d >
{
public:
	typedef gvt_grid_2d< gv_cell_3d > super;
	gv_world_grid();
	//gv_int collect_cells(const gv_boxi& rect, gv_cell_3d::cache& result);

public:
	gv_uint m_tag_masks;
};
}