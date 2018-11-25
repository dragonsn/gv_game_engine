namespace gv
{

template < class T >
class gvt_geom : public gvt_math< T >
{
	// reference
	// http://www.realtimerendering.com/intersections.html
	// http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_vector2;
	typedef gvt_vector3< T > type_of_vector3;
	typedef gvt_vector4< T > type_of_vector4;
	typedef gvt_matrix_generic< T, 4, 4 > type_of_generic_matrix44;
	typedef gvt_matrix_generic< T, 3, 3 > type_of_generic_matrix33;
	typedef gvt_matrix44< T > type_of_matrix44;
	typedef gvt_matrix43< T > type_of_matrix43;
	typedef gvt_quaternion< T > type_of_quaternion;
	typedef gvt_euler< T > type_of_euler;
	typedef gvt_line< T > type_of_line;
	typedef gvt_line_segment< T > type_of_line_seg;
	typedef gvt_rect< T > type_of_rect;
	typedef gvt_plane< T > type_of_plane;
	typedef gvt_box< T > type_of_box;
	typedef gvt_sphere< T > type_of_sphere;

	//*********************************************************************************
	//
	//*********************************************************************************
	struct line_check_result
	{
		type_of_value line_param;
		type_of_vector3 pos;
		type_of_vector3 normal;
		gv_uint user_data;
	};

	struct intersect_result
	{
		type_of_vector3 pos;
		type_of_vector3 normal;
		type_of_value depth;
		gv_uint user_data;
	};

	struct sweep_result
	{
		type_of_value time_of_impact;
		type_of_vector3 normal;
		gv_uint user_data;
	};

public:
protected:
	static bool _liang_barsky_clip(type_of_value denom, type_of_value num,
								   type_of_value& t_e, type_of_value& t_l)
	{
		type_of_value t;
		bool still_not_clipped = true;
		if (denom > 0.0f)
		{
			t = num / denom;
			if (t > t_l)
				still_not_clipped = false;
			else if (t > t_e)
				t_e = t;
		}
		else if (denom < 0.0f)
		{
			t = num / denom;
			if (t < t_e)
				still_not_clipped = false;
			else if (t < t_l)
				t_l = t;
		}
		else if (num > 0.0f)
		{
			still_not_clipped = false;
		}
		return still_not_clipped;
	}

public:
	static bool clip(const type_of_line_seg& line, const type_of_rect& rect,
					 type_of_line_seg& line_result)
	{
		bool is_visible = false;
		type_of_value t_e, t_l;
		type_of_value dx, dy;
		line_result = line;
		type_of_vector3& p1 = line_result.start_p;
		type_of_vector3& p2 = line_result.end_p;
		dx = p2.x - p1.x;
		dy = p2.y - p1.y;
		if (value_trait::is_almost_zero(dx) && value_trait::is_almost_zero(dy) &&
			rect.intersect(p1.v2))
		{
			is_visible = true;
		}
		else
		{
			t_e = 0.0f;
			t_l = 1.0f;
			if (_liang_barsky_clip(dx, rect.min_p.x - p1.x, t_e, t_l))
				if (_liang_barsky_clip(-dx, p1.x - rect.max_p.x, t_e, t_l))
					if (_liang_barsky_clip(dy, rect.min_p.y - p1.y, t_e, t_l))
						if (_liang_barsky_clip(-dy, p1.y - rect.max_p.y, t_e, t_l))
						{
							is_visible = true;
							if (t_l < 1.0f)
							{
								p2.x = p1.x + (t_l * dx);
								p2.y = p1.y + (t_l * dy);
							}
							if (t_e > 0.0f)
							{
								p1.x = p1.x + (t_e * dx);
								p1.y = p1.y + (t_e * dy);
							}
						}
		}
		return is_visible;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool clip_line_against_box_3d(const gv_line_segment& line,
										 const gv_box& box,
										 gv_line_segment& line_result)
	{
		gv_vector3 dir = line.dir();
		gv_line ray(line.start_p, dir);
		gv_float max_s, min_t;
		if (!intersect_ray_aabb(ray, box, max_s, min_t))
		{
			return false;
		}
		gv_float a, b;
		if (!gvt_overlap_range< gv_float >(max_s, min_t, 0, ray.get_param(line.end_p),
										   a, b))
		{
			return false;
		}
		line_result =
			gv_line_segment(ray.get_point_on_line(a), ray.get_point_on_line(b));
		return true;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static type_of_value distance_to(const type_of_vector3& v0,
									 const type_of_vector3& v1)
	{
		return v0.distance_to(v1);
	};

	static type_of_value distance_to(const type_of_line& line,
									 const type_of_vector3& v)
	{
		type_of_vector3 v2 = line.project(v);
		return v2.distance_to(v);
	};
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool intersect(const type_of_line& line, const type_of_rect& rect,
						  type_of_value& max_s, type_of_value& min_t)
	{
		max_s = value_trait::min();
		min_t = value_trait::max();
		type_of_vector2 center = rect.center();
		for (int i = 0; i < 2; ++i)
		{
			if (value_trait::is_almost_zero(line.dir_v[i]))
			{
				if (line.origin_p[i] < rect.min_p[i] ||
					line.origin_p[i] > rect.max_p[i])
					return false;
			}
			else
			{
				type_of_value s = (rect.min_p[i] - line.origin_p[i]) / line.dir_v[i];
				type_of_value t = (rect.max_p[i] - line.origin_p[i]) / line.dir_v[i];
				if (s > t)
					gvt_swap(s, t);
				max_s = gvt_max(s, max_s);
				min_t = gvt_min(t, min_t);
				if (max_s > min_t)
					return false;
			}
		}
		return true;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool intersect(const type_of_line& line, const type_of_rect& rect,
						  line_check_result (&result)[2])
	{
		type_of_value max_s, min_t;
		max_s = value_trait::min();
		min_t = value_trait::max();
		type_of_vector2 center = rect.center();
		//
		if (value_trait::is_almost_zero(line.dir_v.x))
		{
			if (line.origin_p.x < rect.min_p.x || line.origin_p.x > rect.max_p.x)
				return false;
		}
		else
		{
			type_of_value s = (rect.min_p.x - line.origin_p.x) / line.dir_v.x;
			type_of_value t = (rect.max_p.x - line.origin_p.x) / line.dir_v.x;
			gv_vector3 n_s = -gv_vector3::get_x_axis();
			gv_vector3 n_t = gv_vector3::get_x_axis();
			if (s > t)
			{
				gvt_swap(s, t);
				gvt_swap(n_s, n_t);
			}
			max_s = s;
			min_t = t;
			result[0].line_param = s;
			result[0].normal = n_s;
			result[1].line_param = t;
			result[1].normal = n_t;
		}
		// y
		if (value_trait::is_almost_zero(line.dir_v.y))
		{
			if (line.origin_p.y < rect.min_p.y || line.origin_p.y > rect.max_p.y)
				return false;
		}
		else
		{
			gv_float s = (rect.min_p.y - line.origin_p.y) / line.dir_v.y;
			gv_float t = (rect.max_p.y - line.origin_p.y) / line.dir_v.y;
			gv_vector3 n_s = -gv_vector3::get_y_axis();
			gv_vector3 n_t = gv_vector3::get_y_axis();
			if (s > t)
			{
				gvt_swap(s, t);
				gvt_swap(n_s, n_t);
			}
			if (s > max_s)
			{
				max_s = s;
				result[0].line_param = s;
				result[0].normal = n_s;
			}
			if (t < min_t)
			{
				min_t = t;
				result[1].line_param = t;
				result[1].normal = n_t;
			}
		}
		if (max_s > min_t)
			return false;
		result[0].pos = line.get_point_on_line(result[0].line_param);
		result[1].pos = line.get_point_on_line(result[1].line_param);
		return true;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool intersect_ray_plane(const type_of_line& line,
									const type_of_plane& plane,
									type_of_vector3& intersect_p,
									type_of_value& intersect_line_param)
	{
		type_of_value denominator = line.dir_v.dot(plane.normal);
		if (denominator > value_trait::epsilon() ||
			denominator < -value_trait::epsilon())
		{
			type_of_value oneOverDenominator = 1.0f / denominator;
			type_of_value numerator =
				-(plane.offset + line.origin_p.dot(plane.normal));
			intersect_line_param = numerator * oneOverDenominator;
			intersect_p.x =
				line.origin_p.x + ((line.dir_v.x * oneOverDenominator) * numerator);
			intersect_p.y =
				line.origin_p.y + ((line.dir_v.y * oneOverDenominator) * numerator);
			intersect_p.z =
				line.origin_p.z + ((line.dir_v.z * oneOverDenominator) * numerator);
			return true;
		}
		return false;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool sweep(const type_of_rect& mover, const type_of_rect& being_tested,
					  type_of_vector2 speed, sweep_result& result,
					  type_of_value skin = 0)
	{
		type_of_value velocity = speed.normalize();
		if (value_trait::is_almost_zero(velocity))
			return false;
		type_of_rect r = being_tested;
		r.extend(mover.width() / value_trait::two(),
				 mover.height() / value_trait::two());
		line_check_result line_results[2];
		bool hit = intersect(type_of_line(mover.center(), speed), r, line_results);
		if (!hit)
			return false;
		if (line_results[0].line_param >= -skin &&
			line_results[0].line_param <= velocity)
		{
			result.time_of_impact = (line_results[0].line_param - skin) / velocity;
			result.normal = line_results[0].normal;
			return true;
		}
		return false;
	}

	//*********************************************************************************
	//
	//*********************************************************************************
	static bool sweep_rect(const type_of_rect& mover,
						   const type_of_rect& being_tested,
						   type_of_vector2 speed_dir, type_of_value velocity,
						   sweep_result& result, type_of_value skin = 0)
	{
		type_of_rect r = being_tested;
		if (velocity < 0)
		{
			speed_dir = -speed_dir;
			velocity = -velocity;
		}
		r.extend(mover.width() / value_trait::two(),
				 mover.height() / value_trait::two());
		line_check_result line_results[2];
		bool hit =
			intersect(type_of_line(mover.center(), speed_dir), r, line_results);
		if (!hit)
			return false;
		if (line_results[0].line_param >= -skin &&
			line_results[0].line_param <= velocity)
		{
			result.time_of_impact = (line_results[0].line_param - skin) / velocity;
			result.normal = line_results[0].normal;
			return true;
		}
		return false;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool intersect_ray_aabb(const gv_line& line, const gv_box& box,
								   gv_float& max_s, gv_float& min_t)
	{
		const gv_vector3& line_point = line.origin_p;
		const gv_vector3& line_vector = line.dir_v;
		max_s = -3.4e+38F;
		min_t = 3.4e+38F;
		for (int i = 0; i < 3; ++i)
		{
			// line is parallel
			if (gv_float_trait::is_almost_zero(line_vector[i]))
			{
				if (line_point[i] < box.get_min_point()[i] ||
					line_point[i] > box.get_max_point()[i])
					return false;
			}
			else
			{
				gv_float s = (box.get_min_point()[i] - line_point[i]) / line_vector[i];
				gv_float t = (box.get_max_point()[i] - line_point[i]) / line_vector[i];
				if (s > t)
				{
					gvt_swap(s, t);
				}
				if (s > max_s)
					max_s = s;
				if (t < min_t)
					min_t = t;
				if (max_s > min_t)
					return false;
			}
		}
		return true;
	}
	//*********************************************************************************
	//
	//*********************************************************************************
	template < class type_of_vector >
	static type_of_vector reflect(const type_of_vector& normal,
								  const type_of_vector& vector)
	{
		type_of_value project = vector.dot(normal);
		type_of_vector f = normal * project * 2;
		return vector - f;
	}

	template < class type_of_vector >
	static type_of_vector slide(const type_of_vector& normal,
								const type_of_vector& vector)
	{
		type_of_value project = vector.dot(normal);
		return vector - normal * project;
	}

	//*********************************************************************************
	//
	//*********************************************************************************
	static bool intersect_ray_circle(const gv_vector2& line_point,
									 const gv_vector2& line_vector,
									 const gv_vector2& circle_center,
									 gv_float circle_radius, gv_float& s,
									 gv_float& t);

	static bool intersect_ray_circle(const gv_line& line, const gv_circle& circle,
									 gv_float& s, gv_float& t)
	{
		return intersect_ray_circle(line.origin_p.v2, line.dir_v.v2, circle.pos,
									circle.r, s, t);
	};

	//*********************************************************************************
	//
	//*********************************************************************************
	static bool sweep_circle(const gv_circle& mover,
							 const gv_circle& being_tested, gv_vector2 moving_dir,
							 gv_float moving_speed, sweep_result& result,
							 gv_float skin = 0.f);
	//*********************************************************************************
	//
	//*********************************************************************************
	static bool sweep_aa_rect_object_rect(const gv_rect& mover,
										  const gv_rect& object_rect,
										  gv_float object_rotation,
										  const gv_vector2& speed_dir,
										  gv_float velocity, sweep_result& result,
										  type_of_value skin = 0);

	//*********************************************************************************
	//
	//*********************************************************************************
	static bool is_overlap_aabb_sphere(const gv_sphere& sphere,
									   const gv_box& aabb);
	//*********************************************************************************
	//
	//*********************************************************************************
	static gv_box convert_sphere_to_aabb(const gv_sphere& sphere);
	//*********************************************************************************
	//
	//*********************************************************************************
	static type_of_sphere get_bounding_sphere(const type_of_box& box)
	{
		type_of_sphere s;
		s.pos = box.get_center();
		s.r = box.get_diagonal() * value_trait::half();
		return s;
	}
};
// still use gv_float as main real type ,but it's easy to change
typedef gvt_geom< gv_float > gv_geom;
typedef gvt_geom< gv_float >::line_check_result gv_line_check_result;
typedef gvt_geom< gv_float >::intersect_result gv_intersect_result;
typedef gvt_geom< gv_float >::sweep_result gv_sweep_result;
}
