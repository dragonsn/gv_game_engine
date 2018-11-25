#pragma once

namespace gv
{
template < class T >
class gvt_quaternion : public gvt_vector4< T >
{
protected:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector4< T > type_of_v4;
	typedef gvt_vector4< T > super;
	typedef gvt_vector3< T > type_of_v3;
	typedef gvt_quaternion< T > type_of_quaternion;

public:
	inline gvt_quaternion(){};
	inline gvt_quaternion(type_of_value _x, type_of_value _y, type_of_value _z,
						  type_of_value _w)
		: super(_x, _y, _z, _w){};
	inline gvt_quaternion(const type_of_quaternion& q)
	{
		*this = q;
	}

	inline gvt_quaternion(const type_of_v3& axis, type_of_value angle)
	{
		type_of_value cos_half_a = gvt_cos(angle / value_trait::two());
		type_of_value sin_half_a = gvt_sin(angle / value_trait::two());
		this->w = cos_half_a;
		this->get_super() = axis * sin_half_a;
	}

	inline type_of_v4& v4()
	{
		return (*this);
	}

	inline const type_of_v4& v4() const
	{
		return (*this);
	}

	inline type_of_quaternion slerp(const type_of_quaternion& q,
									type_of_value alpha) const
	{
		type_of_quaternion copy(*this);
		type_of_value cos_omega = copy.dot(q);
		type_of_value start_scale;
		type_of_value end_scale;
		if (cos_omega < value_trait::zero())
		{
			cos_omega *= -value_trait::one();
			copy.negate();
		}
		if (value_trait::one() + cos_omega > value_trait::epsilon())
		{
			if (value_trait::one() - cos_omega >
				value_trait::epsilon())
			{ // Usual case
				type_of_value omega = gvt_acos(cos_omega);
				type_of_value sin_omega = gvt_sin(omega);
				start_scale = gvt_sin((value_trait::one() - alpha) * omega) / sin_omega;
				end_scale = gvt_sin(alpha * omega) / sin_omega;
			}
			else
			{ // Ends very close
				start_scale = value_trait::one() - alpha;
				end_scale = alpha;
			}
			type_of_quaternion result;
			result.v4() = copy.v4() * start_scale + q.v4() * end_scale;
			return result;
		}
		else
		{
			// Ends nearly opposite
			type_of_quaternion result(-copy.y, copy.x, -copy.w, copy.z);
			start_scale = gvt_sin((value_trait::half() - alpha) * value_trait::pi());
			end_scale = gvt_sin(alpha * value_trait::pi());
			result.v4() = copy.v4() * start_scale + result.v4() * end_scale;
			return result;
		}
	}

	inline type_of_quaternion lerp(const type_of_quaternion& q,
								   type_of_value alpha) const
	{
		type_of_quaternion copy(*this);
		type_of_value cos_omega = copy.dot(q);
		if (cos_omega < value_trait::zero())
		{
			copy.negate();
		}
		return gvt_lerp(copy, q, alpha);
	}

	inline void inverse()
	{
		type_of_value normalize_factor = value_trait::one() / dot(*this);
		this->v *= -normalize_factor;
		this->w *= normalize_factor;
	}

	inline type_of_quaternion get_inverse()
	{
		type_of_quaternion q(*this);
		q.inverse();
		return q;
	}

	inline void rotate(type_of_v3& vout, const type_of_v3& vin) const
	{
		type_of_value A, B, C, D, twoE, twoF, twoG, twoH, twoI, twoJ,
			normalizeFactor, normX, normY, normZ;
		type_of_value two = value_trait::two();
		type_of_value one = value_trait::one();
		A = this->x * this->x;
		B = this->y * this->y;
		C = this->z * this->z;
		D = this->w * this->w;
		twoE = two * this->x * this->y;
		twoF = two * this->x * this->z;
		twoG = two * this->y * this->z;
		twoH = two * this->z * this->w;
		twoI = two * this->y * this->w;
		twoJ = two * this->x * this->w;
		normalizeFactor = one / (A + B + C + D);
		normX = normalizeFactor * vin.x;
		normY = normalizeFactor * vin.y;
		normZ = normalizeFactor * vin.z;
		vout.set((normX * (A + D - C - B) + normY * (twoE - twoH) +
				  normZ * (twoI + twoF)),
				 (normY * (D + B - A - C) + normX * (twoE + twoH) +
				  normZ * (twoG - twoJ)),
				 (normZ * (D + C - B - A) + normX * (twoF - twoI) +
				  normY * (twoG + twoJ)));
	}

	inline type_of_v3 rotate(const type_of_v3& vin) const
	{
		type_of_v3 vout;
		rotate(vout, vin);
		return vout;
	}
	inline type_of_quaternion& operator/=(const T& t)
	{
		this->get_super() /= t;
		this->w /= t;
		return *this;
	}
	inline void normalize()
	{
		type_of_value f = gvt_sqrt(this->x * this->x + this->y * this->y +
								   this->z * this->z + this->w * this->w);
		(*this) /= f;
	}

	inline void conjugate()
	{
		this->v = -this->v;
	}

	inline type_of_quaternion& operator*=(const type_of_quaternion& q)
	{
		type_of_value Dx =
			q.x * this->w + q.y * this->z - q.z * this->y + q.w * this->x;
		type_of_value Dy =
			-q.x * this->z + q.y * this->w + q.z * this->x + q.w * this->y;
		type_of_value Dz =
			q.x * this->y - q.y * this->x + q.z * this->w + q.w * this->z;
		type_of_value Dw =
			-q.x * this->x - q.y * this->y - q.z * this->z + q.w * this->w;
		set(Dx, Dy, Dz, Dw);
		return *this;
	}

	inline type_of_quaternion& operator/=(const type_of_quaternion& q)
	{
		type_of_quaternion bak(q);
		bak.inverse();
		(*this) *= bak;
		return *this;
	}

	inline type_of_quaternion operator/(const type_of_quaternion& _v) const
	{
		type_of_quaternion ret(*this);
		ret /= _v;
		return ret;
	}

	inline type_of_quaternion operator*(const type_of_quaternion& _v) const
	{
		type_of_quaternion ret(*this);
		ret *= _v;
		return ret;
	}

	void set_identity()
	{
		(*this) = get_zero_rotation();
	}

	static const gvt_quaternion& get_zero_rotation()
	{
		static gvt_quaternion q(value_trait::zero(), value_trait::zero(),
								value_trait::zero(), value_trait::one());
		return q;
	}

	static gvt_quaternion get_rotation_by_x(const T& rad)
	{
		gvt_quaternion q(super::get_x_axis(), rad);
		return q;
	}

	static gvt_quaternion get_rotation_by_y(const T& rad)
	{
		gvt_quaternion q(super::get_y_axis(), rad);
		return q;
	}

	static gvt_quaternion get_rotation_by_z(const T& rad)
	{
		gvt_quaternion q(super::get_z_axis(), rad);
		return q;
	}
};

typedef gvt_quaternion< gv_float > gv_quat;
}
