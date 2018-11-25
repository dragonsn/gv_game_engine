#pragma once

namespace gv
{

template < int byte_array_size = 1, class T >
inline void
gvt_encode_zero_to_one_into_byte(const T& x,
								 gv_byte (&result)[byte_array_size])
{
	T byte_max = (T)255;
	T n = x;
	for (int i = 0; i < byte_array_size; i++)
	{
		n *= byte_max;
		result[i] = (gv_byte)(n);
		n = gvt_frac(n);
	}
}

template < int byte_array_size = 1, class T >
inline void gvt_decode_byte_to_zero_to_one(T& n,
										   gv_byte (&result)[byte_array_size])
{
	T byte_factor = ((T)1) / ((T)255);
	n = 0;
	for (int i = 0; i < byte_array_size; i++)
	{
		n += (T)result[i] * byte_factor;
		byte_factor /= (T)255;
	}
}

template < class T >
inline T gvt_normalize_value(const T& x, T min_val, T max_val)
{
	if (x <= min_val)
		return 0;
	if (x >= max_val)
		return 1;
	return (x - min_val) / (max_val - min_val);
}

template < class T >
inline T gvt_restore_normalized_value(const T& x, T min_val, T max_val)
{
	return x * (max_val - min_val) + min_val;
}

template < class T >
class gvt_math : public gvt_value< T >
{
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

	static T abs(T t)
	{
		return gvt_abs(t);
	}
	static T sqrt(T t)
	{
		return gvt_sqrt(t);
	}
	static T sin(T t)
	{
		return gvt_sin(t);
	}
	static T cos(T t)
	{
		return gvt_cos(t);
	}
	static T tan(T t)
	{
		return gvt_tan(t);
	}
	static T asin(T t)
	{
		return gvt_asin(t);
	}
	static T acos(T t)
	{
		return gvt_acos(t);
	}
	static T atan2(T Y, T X)
	{
		return gvt_atan2(Y, X);
	}

	static type_of_vector2 sincos(type_of_value angle)
	{
		type_of_vector2 r;
		r.x = sin(angle);
		r.y = cos(angle);
		return r;
	}

	static void sincos(type_of_value angle, type_of_value& _x,
					   type_of_value& _y)
	{
		_x = sin(angle);
		_y = cos(angle);
	}

public:
	// conversions
	// if not mentioned , all the matrix are row major
	// quaternion <<< matrix
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
	template < class type_of_matrix >
	static bool convert_to_column_major_matrix(type_of_quaternion& q,
											   const type_of_matrix& matrix)
	{
		T tr = matrix._m00 + matrix._m11 + matrix._m22;
		T r;
		if (tr >= value_trait::zero())
		{
			r = sqrt(tr + value_trait::one());
			q.w = value_trait::half() * r;
			r = value_trait::half() / r;
			q.x = (matrix._m21 - matrix._m12) * r;
			q.y = (matrix._m02 - matrix._m20) * r;
			q.z = (matrix._m10 - matrix._m01) * r;
		}
		else
		{
			int i = 0;
			if (matrix._m11 > matrix._m00)
				i = 1;
			if (matrix._m22 > matrix(i, i))
				i = 2;
			switch (i)
			{
			case 0:
				r = sqrt((matrix._m00 - (matrix._m11 + matrix._m22)) +
						 value_trait::one());
				q.x = value_trait::half() * r;
				r = value_trait::half() / r;
				q.y = (matrix._m01 + matrix._m10) * r;
				q.z = (matrix._m20 + matrix._m02) * r;
				q.w = (matrix._m21 - matrix._m12) * r;
				break;
			case 1:
				r = sqrt((matrix._m11 - (matrix._m22 + matrix._m00)) +
						 value_trait::one());
				q.y = value_trait::half() * r;
				r = value_trait::half() / r;
				q.z = (matrix._m12 + matrix._m21) * r;
				q.x = (matrix._m01 + matrix._m10) * r;
				q.w = (matrix._m02 - matrix._m20) * r;
				break;
			case 2:
				r = sqrt((matrix._m22 - (matrix._m00 + matrix._m11)) +
						 value_trait::one());
				q.z = value_trait::half() * r;
				r = value_trait::half() / r;
				q.x = (matrix._m20 + matrix._m02) * r;
				q.y = (matrix._m12 + matrix._m21) * r;
				q.w = (matrix._m10 - matrix._m01) * r;
				break;
			};
		}
		return true;
	}
	//
	template < class type_of_matrix >
	static bool convert(type_of_quaternion& q, const type_of_matrix& matrix)
	{
		T tr = matrix._m00 + matrix._m11 + matrix._m22;
		T r;
		if (tr >= value_trait::zero())
		{
			r = sqrt(tr + value_trait::one());
			q.w = value_trait::half() * r;
			r = value_trait::half() / r;
			q.x = (matrix._m12 - matrix._m21) * r;
			q.y = (matrix._m20 - matrix._m02) * r;
			q.z = (matrix._m01 - matrix._m10) * r;
		}
		else
		{
			int i = 0;
			if (matrix._m11 > matrix._m00)
				i = 1;
			if (matrix._m22 > matrix(i, i))
				i = 2;
			switch (i)
			{
			case 0:
				r = sqrt((matrix._m00 - (matrix._m11 + matrix._m22)) +
						 value_trait::one());
				q.x = value_trait::half() * r;
				r = value_trait::half() / r;
				q.y = (matrix._m10 + matrix._m01) * r;
				q.z = (matrix._m02 + matrix._m20) * r;
				q.w = (matrix._m12 - matrix._m21) * r;
				break;
			case 1:
				r = sqrt((matrix._m11 - (matrix._m22 + matrix._m00)) +
						 value_trait::one());
				q.y = value_trait::half() * r;
				r = value_trait::half() / r;
				q.z = (matrix._m21 + matrix._m12) * r;
				q.x = (matrix._m10 + matrix._m01) * r;
				q.w = (matrix._m20 - matrix._m02) * r;
				break;
			case 2:
				r = sqrt((matrix._m22 - (matrix._m00 + matrix._m11)) +
						 value_trait::one());
				q.z = value_trait::half() * r;
				r = value_trait::half() / r;
				q.x = (matrix._m02 + matrix._m20) * r;
				q.y = (matrix._m21 + matrix._m12) * r;
				q.w = (matrix._m01 - matrix._m10) * r;
				break;
			};
		}
		return true;
	}

	static bool convert(type_of_quaternion& q, const type_of_euler& e)
	{
		type_of_matrix44 matrix;
		convert(matrix, e);
		convert(q, matrix);
		return true;
	}

	// quaternion >>>>matrix
	template < class type_of_matrix >
	static bool convert(type_of_matrix& m, const type_of_quaternion& quat)
	{
		type_of_value s;
		type_of_value xs, ys, zs;
		type_of_value wx, wy, wz;
		type_of_value xx, xy, xz;
		type_of_value yy, yz, zz;
		const type_of_value& x = quat.x;
		const type_of_value& y = quat.y;
		const type_of_value& z = quat.z;
		const type_of_value& w = quat.w;
		// For unit Quat, just set s = 2 or set xs = x + x, etc.
		s = value_trait::two() / (x * x + y * y + z * z + w * w);

		xs = x * s;
		ys = y * s;
		zs = z * s;

		wx = w * xs;
		wy = w * ys;
		wz = w * zs;

		xx = x * xs;
		xy = x * ys;
		xz = x * zs;

		yy = y * ys;
		yz = y * zs;
		zz = z * zs;

		m.set_identity();
		m._m00 = value_trait::one() - (yy + zz);
		m._m01 = xy + wz;
		m._m02 = xz - wy;

		m._m10 = xy - wz;
		m._m11 = value_trait::one() - (xx + zz);
		m._m12 = yz + wx;

		m._m20 = xz + wy;
		m._m21 = yz - wx;
		m._m22 = value_trait::one() - (xx + yy);
		return true;
	}
	/// euler >>matrix
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
	template < class type_of_matrix >
	static bool convert(type_of_matrix& m, const type_of_euler& e)
	{
		// IMPLEMENTATION 0
		// Assuming the angles are in radians.
		// NASA COORDINATE IMPLEMENTATION with y axis point toup
		type_of_value ch; // = cos(e.yaw);
		type_of_value sh; // = sin(e.yaw);
		sincos(e.yaw, sh, ch);

		type_of_value ca; //= cos(e.pitch);
		type_of_value sa; //= sin(e.pitch);
		sincos(e.pitch, sa, ca);

		type_of_value cb; //= cos(e.roll);
		type_of_value sb; //= sin(e.roll);
		sincos(e.roll, sb, cb);

		m.set_identity();

		m._m00 = ch * ca;
		m._m01 = sa;
		m._m02 = -sh * ca;

		m._m10 = sh * sb - ch * sa * cb;
		m._m11 = ca * cb;
		m._m12 = sh * sa * cb + ch * sb;

		m._m20 = ch * sa * sb + sh * cb;
		m._m21 = -ca * sb;
		m._m22 = -sh * sa * sb + ch * cb;

		/*
    //IMPLEMENTATION 1
    //with z axis point to up
    type_of_euler sinEuler;
    type_of_euler cosEuler;

    cosEuler.yaw   = cos( euler.yaw );
    sinEuler.yaw   = sin( euler.yaw );
    cosEuler.pitch = cos( euler.pitch );
    sinEuler.pitch = sin( euler.pitch );
    cosEuler.roll  = cos( euler.roll );
    sinEuler.roll  = sin( euler.roll );
    m.set_identity();
    m._m00 = cosEuler.yaw * cosEuler.pitch;
    m._m01 = sinEuler.yaw * cosEuler.pitch;
    m._m02 = -sinEuler.pitch;

    m._m10 =
            cosEuler.yaw * sinEuler.pitch * sinEuler.roll -
            sinEuler.yaw * cosEuler.roll;
    m._m11 =
            cosEuler.yaw * cosEuler.roll +
            sinEuler.yaw * sinEuler.pitch * sinEuler.roll;
    m._m12 = cosEuler.pitch * sinEuler.roll;

    m._m20 =
            cosEuler.yaw * sinEuler.pitch * cosEuler.roll +
            sinEuler.yaw * sinEuler.roll;
    m._m21 =
            sinEuler.yaw * sinEuler.pitch * cosEuler.roll -
            cosEuler.yaw * sinEuler.roll;
    m._m22 = cosEuler.pitch * cosEuler.roll;
    */

		return true;
	}

	/// euler <<matrix
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
	template < class type_of_matrix >
	static bool convert(type_of_euler& e, const type_of_matrix& m)
	{
		// IMPLEMENT 0
		// NASA COORDINATE SYSTEM
		if (value_trait::is_almost_equal(
				m._m01, value_trait::one()))
		{ // singularity at north pole
			e.yaw = atan2(m._m20, m._m22);
			e.pitch = value_trait::pi() / value_trait::two();
			e.roll = value_trait::zero();
			return true;
		}
		if (value_trait::is_almost_equal(
				m._m01, -value_trait::one()))
		{ // singularity at south pole
			e.yaw = atan2(m._m20, m._m22);
			e.pitch = -value_trait::pi() / value_trait::two();
			e.roll = value_trait::zero();
			return true;
		}
		e.yaw = atan2(-m._m02, m._m00);
		e.roll = atan2(-m._m21, m._m11);
		e.pitch = asin(m._m01);

		/* z point to up
    type_of_euler sinEuler;
    type_of_euler cosEuler;

    sinEuler.pitch = -matrix( 0, 2 );
    cosEuler.pitch = sqrt( value_trait::one() - sinEuler.pitch * sinEuler.pitch
    );

    if (!is_almost_zero( cosEuler.pitch ))
    {
            sinEuler.roll = matrix( 1, 2 ) / cosEuler.pitch;
            cosEuler.roll = matrix( 2, 2 ) / cosEuler.pitch;
            sinEuler.yaw  = matrix( 0, 1 ) / cosEuler.pitch;
            cosEuler.yaw  = matrix( 0, 0 ) / cosEuler.pitch;
    }
    else
    {
            sinEuler.roll = -matrix( 2, 1 );
            cosEuler.roll = matrix( 1, 1 );
            sinEuler.yaw  = 0.0f;
            cosEuler.yaw  = 1.0f;
    }
    e.yaw   = atan2( sinEuler.yaw, cosEuler.yaw );
    e.pitch = atan2( sinEuler.pitch, cosEuler.pitch );
    e.roll  = atan2( sinEuler.roll, cosEuler.roll );
    */
		return true;
	}

	/// axis >>matrix
	template < class type_of_matrix >
	static bool convert(type_of_matrix& m, const type_of_vector4& a1)
	{
		type_of_value c = cos(a1.w);
		type_of_value s = sin(a1.w);
		type_of_value t = value_trait::one() - c;
		//  if axis is not already normalised then uncomment this
		// double magnitude = Math.sqrt(a1.x*a1.x + a1.y*a1.y + a1.z*a1.z);
		// if (magnitude==0) throw error;
		// a1.x /= magnitude;
		// a1.y /= magnitude;
		// a1.z /= magnitude;
		m.set_identity();
		m._m00 = c + a1.x * a1.x * t;
		m._m11 = c + a1.y * a1.y * t;
		m._m22 = c + a1.z * a1.z * t;

		type_of_value tmp1 = a1.x * a1.y * t;
		type_of_value tmp2 = a1.z * s;
		m._m01 = tmp1 + tmp2;
		m._m10 = tmp1 - tmp2;
		tmp1 = a1.x * a1.z * t;
		tmp2 = a1.y * s;
		m._m02 = tmp1 - tmp2;
		m._m20 = tmp1 + tmp2;
		tmp1 = a1.y * a1.z * t;
		tmp2 = a1.x * s;
		m._m12 = tmp1 + tmp2;
		m._m21 = tmp1 - tmp2;

		return true;
	}

	/// axis <<matrix
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm
	template < class type_of_matrix >
	static bool convert(type_of_vector4& a1, const type_of_matrix& m)
	{
		type_of_quaternion q;
		convert(q, m);
		q.normalize();
		convert(a1, q);
		return true;
	}

	/// axis <<quaternion
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
	static bool convert(type_of_vector4& axis_angle,
						const type_of_quaternion& q)
	{
		// if (q.w > value_trait::one() ) q.normalize(); // if w>1 acos and sqrt
		// will produce errors, this cant happen if quaternion is normalised
		axis_angle.w /*angle*/ = value_trait::two() * acos(q.w);
		type_of_value s =
			sqrt(value_trait::one() - q.w * q.w); // assuming quaternion normalised
												  // then w is less than 1, so term
												  // always positive.
		if (s < value_trait::epsilon())
		{   // test to avoid divide by zero, s is
			// always positive due to sqrt
			// if s close to zero then direction of axis not important
			axis_angle.x = value_trait::one(); // if it is important that axis is
											   // normalised then replace with x=1;
											   // y=z=0;
			axis_angle.y = value_trait::zero();
			axis_angle.z = value_trait::zero();
		}
		else
		{
			axis_angle.x = q.x / s; // normalize axis
			axis_angle.y = q.y / s;
			axis_angle.z = q.z / s;
		}
		return true;
	}
	/// axis >>quaternion
	static bool convert(type_of_quaternion& q,
						const type_of_vector4& axis_angle)
	{
		q = type_of_quaternion(axis_angle.get_super(), axis_angle.w);
		return true;
	}
	//
	static void decompose(const type_of_matrix44& mat, type_of_vector3& trans,
						  type_of_quaternion& rot, type_of_vector3& scale)
	{
		type_of_matrix44 m = mat;
		scale = m.remove_real_scale();
		trans = m.get_trans();
		convert(rot, m);
	}

	static void compose(type_of_matrix44& mat, const type_of_vector3& trans,
						const type_of_quaternion& rot,
						const type_of_vector3& scale)
	{
		type_of_matrix44 mat_s;
		mat_s.set_identity();
		mat_s.set_scale(scale);
		type_of_matrix44 mat_r;
		convert(mat_r, rot);
		mat = mat_s * mat_r;
		mat.set_trans(trans);
	}

	static type_of_value dir_to_angle(const type_of_vector2& v)
	{
		return atan2(v.y, v.x);
	}

	static type_of_vector2 angle_to_dir(type_of_value a)
	{
		type_of_vector2 v;
		sincos(a, v.y, v.x);
		return v;
	}

	// left 1, right 1, same 0 ,
	// input angle in rad 0-2pi
	static gv_int left_right_from_angle(type_of_value a, type_of_value target_a)
	{
		if (is_almost_equal(a, target_a))
		{
			return 0;
		}
		if (target_a > a)
		{
			if (target_a - a < value_trait::pi())
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if (a - target_a < value_trait::pi())
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
	}

}; // gvt_math;

template < class T >
inline gvt_vector3< T > operator*(const gvt_vector3< T >& v,
								  const gvt_matrix43< T >& m)
{
	return m.mul_by(v);
}

template < class T >
inline gvt_vector3< T > operator*(const gvt_vector3< T >& v,
								  const gvt_matrix44< T >& m)
{
	return m.mul_by(v);
}

template < class T >
inline gvt_vector4< T > operator*(const gvt_vector4< T >& v,
								  const gvt_matrix44< T >& m)
{
	return m.mul_by(v);
}

template < class T >
inline gvt_matrix43< T >& gvt_copy(gvt_matrix43< T >& m43,
								   const gvt_matrix44< T >& m44)
{
	m43.axis_x = m44.axis_x;
	m43.axis_y = m44.axis_y;
	m43.axis_z = m44.axis_z;
	m43.axis_w = m44.axis_w;
	return m43;
}

template < class T >
inline gvt_matrix44< T >& gvt_copy(gvt_matrix44< T >& m44,
								   const gvt_matrix43< T >& m43)
{
	m44.axis_x = gvt_vector4< T >(m43.axis_x.v3, 0);
	m44.axis_y = gvt_vector4< T >(m43.axis_y.v3, 0);
	m44.axis_z = gvt_vector4< T >(m43.axis_z.v3, 0);
	m44.axis_w = gvt_vector4< T >(m43.axis_w.v3, 1);
	return m43;
}
//==================================================================================
template < class T >
inline gvt_vector3< T >& operator*=(gvt_vector3< T >& v, const gvt_matrix43< T >& m)
{
	v = m.mul_by(v);
	return v;
}

template < class T >
inline gvt_vector3< T >& operator*=(gvt_vector3< T >& v, const gvt_matrix44< T >& m)
{
	v = m.mul_by(v);
	return v;
}

template < class T >
inline gvt_vector4< T >& operator*=(gvt_vector4< T >& v, const gvt_matrix44< T >& m)
{
	v = m.mul_by(v);
	return v;
}

//=========================================================================
template < class type_of_data >
inline type_of_data gvt_indentity()
{
	return (type_of_data)0;
}

template <>
inline gv_quat gvt_indentity()
{
	return gv_quat::get_zero_rotation();
}
inline gv_int gv_floor_fixed_point(gv_int i, gv_int fixed_one)
{
	if (i >= 0)
	{
		return ((i) / fixed_one) * fixed_one;
	}
	else
	{
		return ((i - fixed_one + 1) / fixed_one) * fixed_one;
	}
}

inline gv_int gv_floor_fixed_point_closed(gv_int i, gv_int fixed_one)
{
	if (i >= 0)
	{
		return ((i) / fixed_one) * fixed_one;
	}
	else
	{
		return ((i - fixed_one + 1) / fixed_one) * fixed_one - 1;
	}
}

template < class type_of_factor, class type_of_data >
inline type_of_data gvt_interpolate(const type_of_factor r,
									const type_of_data& v1,
									const type_of_data& v2)
{
	return (v1 * (1 - r) + v2 * r);
}

template < class type_of_factor >
inline gvt_quaternion< type_of_factor >
gvt_interpolate(const type_of_factor r,
				const gvt_quaternion< type_of_factor >& v1,
				const gvt_quaternion< type_of_factor >& v2)
{
	return v1.slerp(v2, r);
}

typedef gvt_math< gv_float > gv_math;
}
