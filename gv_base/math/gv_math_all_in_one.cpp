#include "gv_base_internal.h"
#include "gvt_array.h"
#include "gv_math.h"
#include "gv_math_fixed.hpp"

namespace gv
{

//============================================================================================
//								:
//============================================================================================
template <>
bool gv_geom::intersect_ray_circle(const gv_vector2& line_point,
								   const gv_vector2& line_vector,
								   const gv_vector2& circle_center,
								   gv_float circle_radius, gv_float& s,
								   gv_float& t)
{
	gv_float a, b, c, d;
	const gv_vector2 _vv(line_point - circle_center);
	a = line_vector.dot(line_vector);
	b = line_vector.dot(_vv) * 2.0f;
	c = _vv.dot(_vv) - (circle_radius * circle_radius);
	d = (b * b) - (a * c * 4.0f);
	if (d < 0.0f)
		return false;
	gv_float root = sqrtf(d);
	s = (-b + root) / (a * 2.0f);
	t = (-b - root) / (a * 2.0f);
	if (s > t)
	{
		gvt_swap(s, t);
	}
	return true;
}
//============================================================================================
//								:
//============================================================================================
template <>
bool gv_geom::sweep_circle(const gv_circle& mover,
						   const gv_circle& being_tested, gv_vector2 moving_dir,
						   gv_float moving_speed, sweep_result& result,
						   gv_float skin)
{
	gv_float s, t;
	if (moving_speed < 0)
	{
		moving_speed = -moving_speed;
		moving_dir = -moving_dir;
	}
	if (gvt_is_almost_zero(moving_speed))
		return false;
	if (intersect_ray_circle(
			mover.get_center(), moving_dir, being_tested.get_center(),
			being_tested.get_radius() + mover.get_radius(), s, t))
	{
		if (s > -skin && s < moving_speed)
		{
			result.time_of_impact = s / moving_speed;
			result.normal = (mover.get_center() + moving_dir * s) -
							being_tested.get_center(); // normalize it by app
			return true;
		}
	}
	return false;
}
//============================================================================================
//								:
//============================================================================================
template <>
bool gv_geom::is_overlap_aabb_sphere(const gv_sphere& sphere,
									 const gv_box& aabb)
{
	gv_float s, d = 0;
	gv_vector3 normal = gv_vector3::get_zero_vector();
	for (int i = 0; i < 3; i++)
	{
		if (sphere.pos[i] < aabb.min_p[i])
		{
			s = sphere.pos[i] - aabb.min_p[i];
			d += s * s;
			normal[i] = -1.0f;
		}
		else if (sphere.pos[i] > aabb.max_p[i])
		{
			s = sphere.pos[i] - aabb.max_p[i];
			d += s * s;
			normal[i] = 1.0f;
		}
	}
	if (d > sphere.r * sphere.r)
	{
		return false;
	}
	return true;
};
}