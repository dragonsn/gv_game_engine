#pragma once
namespace gv
{
template < class T >
inline T gvt_get_zero()
{
	return (T)0;
}

template < class T >
inline T gvt_get_pi()
{
	return (T)3.1415926535897932;
}

template < class T >
inline T gvt_get_one()
{
	return (T)1;
}

template < class T >
inline T gvt_get_max()
{
	return (T)1000000000; // billion
}

template < class T >
inline T gvt_get_min()
{
	return (T)-1000000000; // neg billion
}

template < class T >
inline T gvt_get_epsilon()
{
	return (T)0.000003; // 3 / million
}

template < class T >
inline T gvt_get_half()
{
	return (T)0.5; //
}

template < class T >
inline T gvt_get_180()
{
	return (T)180; //
}
//======================================================================
template < class T >
T gvt_abs(const T& A);

#undef min
#undef max

template < class T >
class gvt_value
{
public:
	static const T& min()
	{
		static T t = gvt_get_min< T >();
		return t;
	};

	static const T& max()
	{
		static T t = gvt_get_max< T >();
		return t;
	};

	static const T& zero()
	{
		static T t = gvt_get_zero< T >();
		return t;
	};

	static const T& pi()
	{
		static T t = gvt_get_pi< T >();
		return t;
	};

	static const T& two_pi()
	{
		static T t = gvt_get_pi< T >() * two();
		return t;
	};

	static const T& one()
	{
		static T t = (T)gvt_get_one< T >();
		return t;
	};

	static const T& half()
	{
		static T t = (T)gvt_get_half< T >();
		return t;
	};

	static const T& two()
	{
		static T t = gvt_get_one< T >() + gvt_get_one< T >();
		return t;
	};

	static const T& _180()
	{
		static T t = gvt_get_180< T >();
		return t;
	};

	static const T& _360()
	{
		static T t = gvt_get_180< T >() + gvt_get_180< T >();
		return t;
	};

	static const T& epsilon()
	{
		static T t = gvt_get_epsilon< T >();
		return t;
	};

	static const T& epsilon_sq()
	{
		static T t = gvt_get_epsilon< T >() * gvt_get_epsilon< T >();
		return t;
	}

	static bool is_almost_zero(const T& t)
	{
		if (is_almost_equal(t, zero()))
			return true;
		return false;
	}

	static bool is_almost_equal(const T& t1, const T& t2)
	{
		if (gvt_abs(t1 - t2) <= epsilon())
			return true;
		return false;
	}
};

template < class T >
inline bool gvt_is_almost_equal(const T& t1, const T& t2)
{
	return gvt_value< T >::is_almost_equal(t1, t2);
}

//========================specifications===========
template <>
inline gv_byte gvt_get_min< gv_byte >()
{
	return (gv_byte)0;
}

template <>
inline gv_byte gvt_get_max< gv_byte >()
{
	return (gv_byte)255;
}

template <>
inline gv_char gvt_get_min< gv_char >()
{
	return (gv_char)-128;
}

template <>
inline gv_char gvt_get_max< gv_char >()
{
	return (gv_char)127;
}

template <>
inline gv_ushort gvt_get_min< gv_ushort >()
{
	return (gv_ushort)0;
}

template <>
inline gv_ushort gvt_get_max< gv_ushort >()
{
	return (gv_ushort)65535;
}

template <>
inline gv_short gvt_get_min< gv_short >()
{
	return (gv_short)-32768;
}

template <>
inline gv_short gvt_get_max< gv_short >()
{
	return (gv_short)32767;
}

template <>
inline gv_uint gvt_get_min< gv_uint >()
{
	return (gv_uint)0;
}

template <>
inline gv_uint gvt_get_max< gv_uint >()
{
	return (gv_uint)0xffffffff;
}

template <>
inline gv_int gvt_get_min< gv_int >()
{
	return (gv_int)(-2147483647);
}

template <>
inline gv_int gvt_get_max< gv_int >()
{
	return (gv_int)2147483647;
}

template <>
inline gv_ulong gvt_get_min< gv_ulong >()
{
	return (gv_ulong)0;
}

template <>
inline gv_ulong gvt_get_max< gv_ulong >()
{
	return (gv_ulong)18446744073709551615ULL;
}

template <>
inline gv_long gvt_get_min< gv_long >()
{
	return std::numeric_limits<
		gv_long >::min(); // (gv_long)-9223372036854775808LL;
}

template <>
inline gv_long gvt_get_max< gv_long >()
{
	return (gv_long)-9223372036854775807LL;
}

template <>
inline gv_float gvt_get_min< gv_float >()
{
	return (gv_float)-3.39E38f;
}

template <>
inline gv_float gvt_get_max< gv_float >()
{
	return (gv_float)3.39E38f;
}

template <>
inline gv_double gvt_get_min< gv_double >()
{
	return (gv_double)-3.39E38f;
}

template <>
inline gv_double gvt_get_max< gv_double >()
{
	return (gv_double)3.39E38f;
}
typedef gvt_value< gv_float > gv_float_trait;

template < class T >
inline T gvt_deg2rad(const T& a)
{
	return ((a)*gvt_value< T >::pi() / gvt_value< T >::_180());
}

template < class T >
inline T gvt_rad2deg(const T& a)
{
	return ((a) / gvt_value< T >::pi() * gvt_value< T >::_180());
}

template < class T >
inline T gvt_rad_clamp(const T& a)
{
	return a - gvt_floor(a / gvt_value< T >::two_pi()) * gvt_value< T >::two_pi();
}

template < class T >
inline T gvt_abs(const T& A)
{
	return (A >= gvt_value< T >::zero()) ? A : -A;
}

template < class T >
inline int gvt_sign(const T& A)
{
	return (A > gvt_value< T >::zero()) ? 1 : ((A < gvt_value< T >::zero()) ? -1 : 0);
}

template < class T >
inline T gvt_reciprocal(const T& x)
{
	if (gvt_value< T >::is_almost_zero(x))
	{
		return gvt_value< T >::max() * gvt_sign(x);
	}
	return gvt_value< T >::one() / x;
}

template < class T >
inline bool gvt_is_almost_zero(const T& t)
{
	return gvt_value< T >::is_almost_zero(t);
}

template < class T, class type_of_real >
inline T gvt_cos_interpolation(const T& _f1, const T& _f2,
							   const type_of_real& x)
{
	type_of_real f = (gvt_value< type_of_real >::one() -
					  std::cos(x * gvt_value< type_of_real >::pi())) *
					 gvt_value< type_of_real >::half();
	return _f1 * (gvt_value< type_of_real >::one() - f) + _f2 * f;
}

template < class T >
inline T gvt_sin(T t)
{
	return std::sin(t);
}

template < class T >
inline T gvt_cos(T t)
{
	return std::cos(t);
}

template < class T >
inline T gvt_tan(T t)
{
	return std::tan(t);
}

template < class T >
inline T gvt_asin(T t)
{
	return std::asin(t);
}

template < class T >
inline T gvt_acos(T t)
{
	return std::acos(t);
}

template < class T >
inline T gvt_atan2(T Y, T X)
{
	return std::atan2(Y, X);
}

template < class type_of_data >
inline type_of_data gvt_floor(type_of_data f)
{
	return f;
}

template <>
inline gv_float gvt_floor(gv_float f)
{
	return floorf(f);
}

template <>
inline gv_double gvt_floor(gv_double f)
{
	return floor(f);
}

template < class type_of_data >
inline type_of_data gvt_ceil(type_of_data f)
{
	return f;
}

template <>
inline gv_float gvt_ceil(gv_float f)
{
	return ceilf(f);
}

template <>
inline gv_double gvt_ceil(gv_double f)
{
	return ceil(f);
}

template < class type_of_data >
inline type_of_data gvt_frac(type_of_data f)
{
	type_of_data s = gvt_floor(f);
	return f - s;
}

template < class type_of_data >
inline type_of_data gvt_div_remain(type_of_data f, type_of_data f2)
{
	type_of_data s = gvt_frac(f / f2) * f2;
	return s;
}

template <>
inline gv_int gvt_div_remain(gv_int f, gv_int f2)
{
	gv_int s = f % f2;
	return s;
}
/*
template<class type_of_factor , class type_of_data >
inline  type_of_data gvt_interpolate( const type_of_data &v1, const type_of_data
&v2, type_of_factor r)
{
return (v1*(1 - r) + v2*r);
}

template<class type_of_factor , class type_of_data >
inline 	gvt_quaternion<type_of_factor> gvt_interpolate( const
gvt_quaternion<type_of_factor> &v1, const gvt_quaternion<type_of_factor> &v2,
type_of_factor r)
{
return v1.slerp( v2,r);
}*/
}
