#pragma once
namespace gv
{

template < class Visitor >
int gvt_trace_line3d(const gv_line_segment& l, Visitor& v)
{
	// this  is a template function that trace the given line and find all the
	// voxel it across
	// whether to use dda or Bresenham's algorithm , can't find all the voxel it
	// across
	// the function is not only useful for drawing lines , but also for some
	// collision detection.
	// and return the voxel the line passed
	int nb_voxel_passed = 0;
	if ((l.start_p - l.end_p).is_almost_zero())
		return 0; // if the line's length is zero , not care it
	int dxi = gvt_sign(l.end_p.x - l.start_p.x);
	int dyi = gvt_sign(l.end_p.y - l.start_p.y);
	int dzi = gvt_sign(l.end_p.z - l.start_p.z);
	gv_vector3 rpq = l.dir();
	rpq.zero_to_epsilon();
	rpq.v.reciprocal();
	rpq.abs();
	gv_vector3 point = l.start_p;
	int ix = (int)gvt_floor(point.x);
	int iy = (int)gvt_floor(point.y);
	int iz = (int)gvt_floor(point.z);
	int ix1 = (int)gvt_floor(l.end_p.x);
	int iy1 = (int)gvt_floor(l.end_p.y);
	int iz1 = (int)gvt_floor(l.end_p.z);
	float sx =
		(dxi > 0 ? (gvt_ceil(point.x) - point.x) : point.x - gvt_floor(point.x)) *
		rpq.x; // distance to the next x=? plane
	float sy =
		(dyi > 0 ? (gvt_ceil(point.y) - point.y) : point.y - gvt_floor(point.y)) *
		rpq.y; // distance to the next y=? plane
	float sz =
		(dzi > 0 ? (gvt_ceil(point.z) - point.z) : point.z - gvt_floor(point.z)) *
		rpq.z; // distance to the next z=? plane
	if (dxi > 0 && sx == 0)
		sx = rpq.x; // reset the distance , if the point is on the edge.
					// (|x|--sx---->|x+1|)
	if (dyi > 0 && sy == 0)
		sy = rpq.y;
	if (dzi > 0 && sz == 0)
		sz = rpq.z;
	if (!v(ix, iy, iz))
		return 1;
	nb_voxel_passed++;
	while (gvt_abs(ix - ix1) >= 1 || gvt_abs(iy - iy1) >= 1 ||
		   gvt_abs(iz - iz1) >= 1)
	{
		if (sx < sy)
		{
			if (sx < sz)
			{
				sy -= sx, sz -= sx, sx = rpq.x;
				ix += dxi;
			}
			else
			{
				sx -= sz, sy -= sz, sz = rpq.z;
				iz += dzi;
			}
		}
		else
		{
			if (sy < sz)
			{
				sx -= sy, sz -= sy, sy = rpq.y;
				iy += dyi;
			}
			else
			{
				sx -= sz, sy -= sz, sz = rpq.z;
				iz += dzi;
			}
		}
		if (!v(ix, iy, iz))
			break;
		nb_voxel_passed++;
	}
	return nb_voxel_passed;
}

template < class Visitor >
int gvt_trace_line2d(gv_vector2& start_p, gv_vector2 end_p, const Visitor& v)
{
	int nb_voxel_passed = 0;
	if ((start_p - end_p).is_almost_zero())
	{
		v((int)gvt_floor(start_p.x), (int)gvt_floor(start_p.y));
		return 1;
	}; // if the line's length is zero , access the ponit
	int dxi = gvt_sign(end_p.x - start_p.x);
	int dyi = gvt_sign(end_p.y - start_p.y);
	gv_vector2 rpq = (end_p - start_p);
	rpq.normalize();
	rpq.zero_to_epsilon();
	rpq.v.reciprocal();
	rpq.abs();
	gv_vector2 point = start_p;
	int ix = (int)gvt_floor(point.x);
	int iy = (int)gvt_floor(point.y);
	int ix1 = (int)gvt_floor(end_p.x);
	int iy1 = (int)gvt_floor(end_p.y);
	float sx =
		(dxi > 0 ? (gvt_ceil(point.x) - point.x) : point.x - gvt_floor(point.x)) *
		rpq.x; // distance to the next x=? plane
	float sy =
		(dyi > 0 ? (gvt_ceil(point.y) - point.y) : point.y - gvt_floor(point.y)) *
		rpq.y; // distance to the next y=? plane
	if (dxi > 0 && sx == 0)
		sx = rpq.x;
	if (dyi > 0 && sy == 0)
		sy = rpq.y;
	if (!v(ix, iy))
	{
		return 1;
	}
	nb_voxel_passed++;
	while (gvt_abs(ix - ix1) >= 1 || gvt_abs(iy - iy1) >= 1)
	{
		if (sx == sy)
		{
			//+		pp1	{x=44.000000 y=18.000000 v={...} }
			//gv::gvt_vector2<float>
			//+		pp0	{x=42.812500 y=19.500000 v={...} }
			//gv::gvt_vector2<float>
			if (gvt_abs(ix - ix1) == 1 || gvt_abs(iy - iy1) == 1)
			{
				nb_voxel_passed++;
				if (!v(ix1, iy1))
				{
					return nb_voxel_passed;
				}
				break;
			}
			else
			{
				sy -= sx, sx = rpq.x;
				ix += dxi;
			}
		}
		else if (sx < sy)
		{
			sy -= sx, sx = rpq.x;
			ix += dxi;
		}
		else
		{
			sx -= sy, sy = rpq.y;
			iy += dyi;
		}
		if (!v(ix, iy))
		{
			break;
		}
		nb_voxel_passed++;
	}
	return nb_voxel_passed;
}

template < int dimension, class visitor, class type_of_value >
gv_int
gvt_trace_line_dda(const gvt_vector_generic< type_of_value, dimension >& v_start,
				   const gvt_vector_generic< type_of_value, dimension >& v_end,
				   const visitor& v)
{
	gvt_vector_generic< type_of_value, dimension > delta;
	delta = v_end - v_start;
	gvt_vector_generic< type_of_value, dimension > abs_d = delta;
	abs_d.abs();
	type_of_value m = abs_d.max();
	delta /= m;
	gvt_vector_generic< type_of_value, dimension > p = v_start;
	gvt_vector_generic< gv_int, dimension > p_int;
	for (int i = 0; i <= m; i++)
	{
		p_int = p;
		v(p_int);
		p += delta;
	}
	return (gv_int)m;
}

template < class visitor >
gv_bool gvt_trace_line_fixed_point(gv_int start, gv_int end, visitor& v,
								   gv_int fixed_point_one)
{
	gv_int dir = gvt_sign(end - start);
	if (dir == 0)
		return true;
	// first point
	if (!v(start))
		return false;
	start += dir * fixed_point_one;
	start = gv_floor_fixed_point(start, fixed_point_one);
	if (dir < 0)
		start += fixed_point_one - 1;
	while (gvt_sign(end - start) == dir)
	{
		if (!v(start))
			return false;
		start += dir * fixed_point_one;
	}
	if (start != end)
		return v(end);
	return true;
};

inline gv_vector2i gv_get_point_on_line_fixed(gv_int dim, gv_vector2i start,
											  gv_vector2i end, gv_int a)
{
	gv_vector2i p;
	p[dim] = a;
	gv_int dim2 = 1 - dim;
	p[dim2] =
		(a - start[dim]) * (end[dim2] - start[dim2]) / (end[dim] - start[dim]) +
		start[dim2];
	return p;
}

template < class type_of_visitor >
struct gvt_trace_visitor_proxy_2d
{
	gvt_trace_visitor_proxy_2d(const type_of_visitor& _v, gv_int _dim,
							   gv_vector2i _start, gv_vector2i _end)
		: vistor(_v)
	{
		dim = _dim;
		start = _start;
		end = _end;
		count = 0;
	}
	bool operator()(gv_int a)
	{
		gv_vector2i p = gv_get_point_on_line_fixed(dim, start, end, a);
		count++;
		return vistor(p);
	}

protected:
	const type_of_visitor& vistor;
	gv_int dim;
	gv_vector2i start;
	gv_vector2i end;

public:
	gv_int count;
};

template < class type_of_visitor >
gv_int gvt_trace_line_fixed_point_2d(const gv_vector2i& start,
									 const gv_vector2i& end,
									 const type_of_visitor& v,
									 gv_int fixed_point_one)
{
	gvt_trace_visitor_proxy_2d< type_of_visitor > v0(v, 0, start, end);
	if (!gvt_trace_line_fixed_point(start[0], end[0], v0, fixed_point_one))
		return v0.count;
	gvt_trace_visitor_proxy_2d< type_of_visitor > v1(v, 1, start, end);
	gvt_trace_line_fixed_point(start[1], end[1], v1, fixed_point_one);
	return v0.count + v1.count;
};
}