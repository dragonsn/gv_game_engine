#pragma once
namespace gv
{
template < class type_of_cell >
class gvt_grid_2d
{
public:
	gvt_grid_2d()
	{
		GVM_ZERO_ME;
	}
	void setup(const gv_vector2i& cell_size, const gv_recti& bounding)
	{
		m_bounding = bounding;
		m_cell_size = cell_size;
		m_grid_size = m_bounding.get_size() / m_cell_size;
		m_grid_rect =
			gv_recti(0, 0, m_grid_size.get_x() - 1, m_grid_size.get_y() - 1);
		m_cells.resize(m_grid_size.get_x());
		for (int i = 0; i < m_cells.size(); i++)
		{
			m_cells[i].resize(m_grid_size.get_y());
		}
	}
	gv_vector2i to_grid_index(const gv_vector2i& position)
	{
		return (position - m_bounding.min_p) / m_cell_size;
	}
	gv_vector2i to_postion(const gv_vector2i& grid_index)
	{
		return grid_index * m_cell_size + m_bounding.min_p;
	}
	gv_recti to_grid_rect(const gv_recti& rect)
	{
		return gv_recti(to_grid_index(rect.min_p), to_grid_index(rect.max_p));
	}
	gv_recti to_rect(const gv_recti& grid_rect)
	{
		return gv_recti(to_postion(grid_rect.min_p),
						to_postion(grid_rect.max_p + gv_vector2i(1, 1)) -
							gv_vector2i(1, 1));
		;
	}
	gv_recti to_rect(const gv_vector2i& grid_index)
	{
		return to_rect(gv_recti(grid_index, grid_index));
	}
	//===========================================================================
	// fetch cells !!
	//===========================================================================
	type_of_cell* get_cell_by_index(const gv_vector2i& grid_index)
	{
		if (grid_index.is_inside(m_grid_size))
		{
			return &m_cells[grid_index.x][grid_index.y];
		}
		return NULL;
	}

	type_of_cell* get_cell(const gv_vector2i& position)
	{
		return get_cell_by_index(to_grid_index(position));
	}

	template < class type_of_cell_container >
	gv_int collect_cells_by_index(const gv_recti& check_grid,
								  type_of_cell_container& container)
	{
		gv_recti r = m_grid_rect.clip(check_grid);
		for (int i = r.left; i <= r.right; i++)
		{
			for (int j = r.top; j <= r.bottom; j++)
			{
				container.push_back(&m_cells[i][j]);
			}
		}
		return container.size();
	}

	template < class type_of_visitor>
	void visit_cells_by_index(const gv_recti& check_grid,
								type_of_visitor visitor)
	{
		gv_recti r = m_grid_rect.clip(check_grid);
		for (int i = r.left; i <= r.right; i++)
		{
			for (int j = r.top; j <= r.bottom; j++)
			{
				visitor(m_cells[i][j]);
			}
		}
	}


	template < class type_of_cell_container >
	gv_int collect_cells(const gv_recti& rect,
						 type_of_cell_container& container)
	{
		return collect_cells_by_index(to_grid_rect(rect), container);
	}

	template < class type_of_visitor >
	void visit_cells(const gv_recti& rect,
					   type_of_visitor visitor)
	{
		 visit_cells_by_index(to_grid_rect(rect), visitor);
	}

	gv_vector2i project(const gv_vector3i& v)
	{
		return gv_vector2i(v.x, v.z);
	};
	gv_recti project(const gv_boxi& b)
	{
		return gv_recti(project(b.min_p), project(b.max_p));
	};
	
	type_of_cell* get_cell(const gv_vector3i& pos)
	{
		return get_cell(project(pos));
	}

	template < class type_of_cell_container >
	gv_int collect_cells(const gv_boxi& b,
						 type_of_cell_container& container)
	{
		return collect_cells(project(b), container);
	};
public:
	gv_vector2i m_cell_size;
	gv_vector2i m_grid_size;
	gv_recti m_grid_rect;
	gv_recti m_bounding;
	gvt_array< gvt_array< type_of_cell > > m_cells;
};
}