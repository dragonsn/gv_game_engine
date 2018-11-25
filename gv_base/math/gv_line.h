#pragma once

namespace gv
{
template < class T >
class gvt_line
{
protected:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector;

public:
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	type_of_vector origin_p; // Base point of line
	type_of_vector dir_v;	// Vector direction of line
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_line(){};
	inline gvt_line(const type_of_vector& line_point,
					const type_of_vector& line_vector)
		: origin_p(line_point), dir_v(line_vector)
	{
	}
	inline bool operator==(const gvt_line& line) const
	{
		return origin_p == line.origin_p && dir_v == line.dir_v;
	}
	inline bool operator!=(const gvt_line& line) const
	{
		return !((*this) == line);
	}
	inline gvt_line& set(const type_of_vector& line_point,
						 const type_of_vector& line_vector)
	{
		origin_p = line_point;
		dir_v = line_vector;
	}
	inline type_of_vector get_point_on_line(type_of_value t) const
	{
		return origin_p + dir_v * t;
	};
	inline type_of_vector project(const type_of_vector& point, type_of_value& t)
	{
		t = (point - origin_p).dot(dir_v);
		return get_point_on_line(t);
	}
	inline type_of_value get_param(const type_of_vector& point)
	{
		type_of_value param;
		param = (point - origin_p).dot(dir_v);
		return param;
	}
};
typedef gvt_line< gv_float > gv_line;
}