#pragma once
//============================================================================================
// the goal is to create a fully deterministic world
//============================================================================================
namespace gv
{

class gv_math_fixed
{
public:
	gv_math_fixed();
	~gv_math_fixed();
	static gv_math_fixed* static_get();

public:
	//=====================================================================================================
	// some constants
	//=====================================================================================================
	static void set_to_fixed_factor(gv_float);
	static gv_float to_float_factor(); // in renderer  1.0=1m  ,in 2d world 1=1mm,
	static gv_float to_fixed_factor(); // in 2d 1  =1mm
	static gv_int one();
	static gv_int pi();
	static gv_int half_pi();
	static gv_int two_pi();
	//=====================================================================================================
	// convert value between deferent coordinate & number type
	// 3D x,y,z, to  2D x ,z
	//=====================================================================================================

	static gv_int to_fixed(gv_float);
	static gv_float to_float(gv_int);
	static void to_fixed(const gv_vector3& vf, gv_vector3i& vi);
	static void to_float(const gv_vector3i& vi, gv_vector3& vf);
	static gv_vector3i to_fixed(const gv_vector3& vf);
	static gv_vector3 to_float(const gv_vector3i& vi);

	static void to_float(const gv_boxi& box_i, gv_box& box_f);
	static void to_fixed(const gv_box& box_f, gv_boxi& box_i);

	static void to_fixed(const gv_euler& vf, gv_euleri& vi);
	static void to_float(const gv_euleri& vi, gv_euler& vf);

	static gv_float deg_to_rad(gv_int);
	static gv_int rad_to_deg(gv_float);
	static gv_float fixed_deg_to_float_rad(gv_int i)
	{
		return deg_to_rad(i);
	};
	static gv_int float_rad_to_fixed_deg(gv_float i)
	{
		return rad_to_deg(i);
	};

	static gv_int mul(gv_int a, gv_int b);
	static gv_int div(gv_int a, gv_int b);

	static gv_vector3i mul(const gv_vector3i& a, gv_int b);
	static gv_vector3i div(const gv_vector3i& a, gv_int b);
	//=====================================================================================================
	//
	//=====================================================================================================
	static gv_vector2i rotate_by_origin_2d(const gv_vector2i& v,
										   const gv_vector2i& o, gv_int angle);
	//=====================================================================================================
	//
	//=====================================================================================================
	static gv_vector3i rotate_by_origin(const gv_vector3i& v,
										const gv_vector3i& o, gv_vector3i angle);

	//=====================================================================================================
	//
	//=====================================================================================================
	static gv_vector2i project(const gv_vector3i& pos, gv_int project_dim);
	//=====================================================================================================
	//
	//=====================================================================================================
	static gv_recti project(const gv_boxi& box, gv_int project_dim);
	//=====================================================================================================
	// angle related computation
	//=====================================================================================================
	static gv_vector3i project_in_direction(const gv_vector3i& vec,
											const gv_vector3i& dir,
											gv_int& project_in_direction);
	static gv_int normalize_deg_angle(gv_int a);
	static gv_int delta_angle(gv_int source, gv_int target); //-pi to +pi
	//=====================================================================================================
	//  intersection  between a ray and a rect in one dimension
	//=====================================================================================================
	static bool intersect_ray(const gv_vector2i& v, gv_int dist,
							  const gv_recti& rect, gv_int& result_distance,
							  gv_int dim, gv_int skin = 0);
	//=====================================================================================================
	//  intersection  between a ray and a box in one dimension
	//=====================================================================================================
	static bool intersect_ray(const gv_vector3i& v, gv_int dist,
							  const gv_boxi& rect, gv_int& result_distance,
							  gv_int dim, gv_int skin = 0);
	//=====================================================================================================
	// a new ray intersection function will be used for box sweep box,
	// test a ray and a plan in main dimetion defined by test_dim
	// return intersection time , if <=0 , no intersection ,
	//=====================================================================================================
	static gv_int ray_intersect_plane(const gv_int* ray_vector, gv_int nb_dim,
									  gv_int test_dim, gv_int plane_d,
									  gv_int skin = 0);
	//=====================================================================================================
	// return intersection time , if <=0 , no intersection ,
	//=====================================================================================================
	static gv_int ray_intersect_box(const gv_vector3i& ray_vector,
									const gv_boxi& collider, gv_vector3i& contact,
									gv_vector3i& normal, gv_int skin = 0);
	//=====================================================================================================
	// 2d sweep a moving point against a rect in one dimension
	//=====================================================================================================
	static bool sweep(const gv_recti& mover, const gv_recti& being_tested,
					  gv_int speed_x, gv_int& distance, gv_vector2i& contact,
					  gv_vector2i& normal, gv_int dim, gv_int skin = 0);
	//=====================================================================================================
	// 2d rect sweep again a line segment (start, end) in one dimension
	//=====================================================================================================
	static bool sweep(const gv_recti& mover, gv_vector2i line_start,
					  gv_vector2i line_end, gv_int speed_x, gv_int& distance,
					  gv_vector2i& contact, gv_vector2i& normal, gv_int dim,
					  gv_int skin = 0);
	//=====================================================================================================
	// new version : 3d box sweep against b 3dox in one dimension
	////=====================================================================================================
	static bool sweep(const gv_boxi& mover, const gv_boxi& being_tested,
					  gv_int speed, gv_int& distance, gv_vector3i& contact,
					  gv_vector3i& normal, gv_int dim, gv_int skin = 0);
	////=====================================================================================================
	// test a  segment against a rect
	////=====================================================================================================
	static bool segment_cast(const gv_vector2i& start, const gv_vector2i& end,
							 gv_recti& rect);
	////=====================================================================================================
	//
	////=====================================================================================================
	static bool test_if_hit_with_distance(const gv_vector2i& normal_dir,
										  gv_int speed, gv_int& distance,
										  gv_int skin, gv_vector2i& normal);

	static gv_vector2i reflect(const gv_vector2i& normal,
							   const gv_vector2i& vector);
	static gv_vector3i reflect3d(const gv_vector3i& normal,
								 const gv_vector3i& vector);

	static gv_vector2i normalize2d(const gv_vector2i& v);
	static gv_vector3i normalize3d(const gv_vector3i& v);
	static gv_vector2i normalize_quick_and_not_precise(const gv_vector2i& v);
	static gv_vector3i normalize_quick_and_not_precise_3d(const gv_vector3i& v);

	static gv_vector2i get_random_position_in_radius(gv_random_uint& ru,
													 gv_int radius);
	static gv_bool test_random_value_in_range(gv_random_uint& ru, gv_int tv,
											  gv_int range);

	static bool is_target_within_distance(const gv_vector2i& source,
										  const gv_vector2i& target,
										  gv_int distance);

	//=====================================================================================================
	// triangluar functions need sin table
	//=====================================================================================================
	static gv_int sini(gv_int angle);
	static gv_int cosi(gv_int angle);
	static gv_int tani(gv_int angle);
	static gv_int atani(gv_int tan_value);
	static gv_vector2i angle_to_dir(gv_int angle);
	static gv_int dir_to_angle(const gv_vector2i& v);
	static gv_int length(const gv_vector2i& v);

public:
	void rebuild_sin_table();
	void init_from_array(int sin_length, gv_int* sin_table, int tan_length,
						 gv_int* tan_table);

public:
	static gvt_array< gv_int > m_sin_table;
	static gvt_array< gv_int > m_tan_table;
};
// math for fixed point
typedef gv_math_fixed gv_math_fixed;

//========================================================================
// data type for the fixed point calculation ~~~
// only use this type for calculation ;

class gv_fixed
{
public:
	gv_fixed()
	{
	}
	gv_fixed(gv_int _i)
	{
		i = _i;
	}
	gv_int get_int() const
	{
		return i;
	}
	inline operator int() const
	{
		return i;
	}
	inline gv_fixed operator-() const
	{
		gv_fixed _v(*this);
		_v.i = -i;
		return _v;
	}
	inline gv_fixed& operator=(const gv_fixed& _v)
	{
		i = _v.i;
		return *this;
	}
	inline bool operator==(const gv_fixed& a) const
	{
		if (i == a.i)
			return true;
		return false;
	}
	inline bool operator<(const gv_fixed& a) const
	{
		if (i < a.i)
			return true;
		return false;
	}
	inline gv_fixed& operator+=(const gv_fixed& a)
	{
		i += a.i;
		return *this;
	}
	inline gv_fixed& operator-=(const gv_fixed& a)
	{
		i -= a.i;
		return *this;
	}
	inline gv_fixed& operator/=(const gv_fixed& a)
	{
		i = gv_math_fixed::div(i, a.i);
		return *this;
	}
	inline gv_fixed& operator*=(const gv_fixed& a)
	{
		i = gv_math_fixed::mul(i, a.i);
		return *this;
	}
	inline gv_fixed operator+(const gv_fixed& a) const
	{
		gv_fixed ret(*this);
		ret += a;
		return ret;
	}
	inline gv_fixed operator-(const gv_fixed& a) const
	{
		gv_fixed ret(*this);
		ret -= a;
		return ret;
	}
	inline gv_fixed operator/(const gv_fixed& a) const
	{
		gv_fixed ret(*this);
		ret /= a;
		return ret;
	}
	inline gv_fixed operator*(const gv_fixed& a) const
	{
		gv_fixed ret(*this);
		ret *= a;
		return ret;
	}
	inline bool operator!=(const gv_fixed& a) const
	{
		return !((*this) == a);
	}
	inline bool operator>=(const gv_fixed& a) const
	{
		return !((*this) < a);
	}
	inline bool operator<=(const gv_fixed& a) const
	{
		return (*this) < a || (*this) == a;
	}
	inline bool operator>(const gv_fixed& a) const
	{
		return !((*this) <= a);
	}

protected:
	gv_int i;
};

//========================================================================

template <>
inline gv_fixed gvt_get_zero()
{
	return 0;
}

template <>
inline gv_fixed gvt_get_pi()
{
	return gv_math_fixed::pi();
}

template <>
inline gv_fixed gvt_get_one()
{
	return gv_math_fixed::one();
}

template <>
inline gv_fixed gvt_get_max()
{
	return (gv_fixed)1000000000; // billion
}

template <>
inline gv_fixed gvt_get_min()
{
	return (gv_fixed)-1000000000; // neg billion
}

template <>
inline gv_fixed gvt_get_epsilon()
{
	return 30; // 3 / million
}

template <>
inline gv_fixed gvt_get_half()
{
	return gv_math_fixed::one() / 2; //
}

template <>
inline gv_fixed gvt_get_180()
{
	return 180; //
}

template <>
inline gv_fixed gvt_sin(gv_fixed t)
{
	return gv_math_fixed::sini(t.get_int());
}

template <>
inline gv_fixed gvt_cos(gv_fixed t)
{
	return gv_math_fixed::cosi(t.get_int());
}

template <>
inline gv_fixed gvt_tan(gv_fixed t)
{
	return gv_math_fixed::tani(t.get_int());
}

template <>
inline gv_fixed gvt_atan2(gv_fixed y, gv_fixed x)
{
	return gv_math_fixed::atani((y / x).get_int());
}
//========================================================================
typedef gvt_value< gv_fixed > gv_value_fixed;
typedef gvt_vector2< gv_fixed > gv_vector2_fixed;
typedef gvt_vector3< gv_fixed > gv_vector3_fixed;
typedef gvt_matrix43< gv_fixed > gv_matrix_fixed;
typedef gvt_matrix44< gv_fixed > gv_matrix44_fixed;
typedef gvt_euler< gv_fixed > gv_euler_fixed;
typedef gvt_math< gv_fixed > gv_math_ex_fixed;
}