#pragma once

namespace gv
{

template < class T >
class gvt_vector3
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector;
	typedef gvt_vector2< T > type_of_vector2;
	typedef gvt_vector_generic< type_of_value, 3 > type_of_generic;
	static const int vector_size = 3;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	union {
		struct
		{
			type_of_value x, y, z;
		};
		struct
		{
			type_of_generic v;
		};
		struct
		{
			type_of_vector2 v2;
		};
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	inline gvt_vector3(){};
	inline gvt_vector3(type_of_value _x, type_of_value _y, type_of_value _z)
		: x(_x), y(_y), z(_z)
	{
	}
	inline gvt_vector3(const type_of_vector& a)
	{
		*this = a;
	}
	inline gvt_vector3(const type_of_vector2& _v2)
	{
		v2 = _v2;
		z = 0;
	}
	inline gvt_vector3(const type_of_vector2& _v2, type_of_value _z)
	{
		v2 = _v2;
		z = _z;
	}
	inline gvt_vector3(const type_of_generic& a)
	{
		v = a;
	}
	explicit inline gvt_vector3(type_of_value f)
	{
		x = y = z = f;
	};

	inline void set(type_of_value _x, type_of_value _y, type_of_value _z)
	{
		x = _x, y = _y, z = _z;
	};
	inline void get(type_of_value& _x, type_of_value& _y,
					type_of_value& _z) const
	{
		_x = x, _y = y, _z = z;
	};
	inline void set_x(type_of_value _x)
	{
		x = _x;
	}
	inline void set_y(type_of_value _y)
	{
		y = _y;
	}
	inline void set_z(type_of_value _z)
	{
		z = _z;
	}
	inline type_of_value get_x() const
	{
		return x;
	}
	inline type_of_value get_y() const
	{
		return y;
	}
	inline type_of_value get_z() const
	{
		return z;
	}
	inline type_of_vector cross(const type_of_vector& a) const
	{
		type_of_vector b;
		b.x = y * a.z - z * a.y, b.y = z * a.x - x * a.z, b.z = x * a.y - y * a.x;
		return b;
	}
	inline type_of_value length() const
	{
		type_of_value l = dot(*this);
		l = gvt_sqrt(l);
		return l;
	};
	inline type_of_value sum() const
	{
		type_of_value l = x + y + z;
		return l;
	};
	inline type_of_value abs_sum() const
	{
		type_of_value l = gvt_abs(x) + gvt_abs(y) + gvt_abs(z);
		return l;
	};
	inline type_of_value normalize()
	{
		type_of_value l = length();
		if (value_trait::is_almost_zero(l))
		{
			(*this) = get_z_axis();
			return value_trait::zero();
		}
		(*this) /= l;
		return l;
	};
	inline type_of_value dot(const type_of_vector& a) const
	{
		return x * a.x + y * a.y + z * a.z;
	};

	inline type_of_vector ortho() const
	{
		return type_of_vector(-z, 0, x);
	}

	inline type_of_value length_squared() const
	{
		type_of_value l = dot(*this);
		return l;
	};
	inline bool is_almost_equal(const type_of_vector& _v) const
	{
		if (value_trait::is_almost_equal(x, _v.x) &&
			value_trait::is_almost_equal(y, _v.y) &&
			value_trait::is_almost_equal(z, _v.z))
		{
			return true;
		}
		return false;
	};
	inline bool is_normlaized() const
	{
		return value_trait::is_almost_equal(length(), value_trait::one());
	}
	inline bool is_almost_zero() const
	{
		return value_trait::is_almost_zero(abs_sum());
	};
	inline void negate()
	{
		x = -x, y = -y, z = -z;
	}
	//==================================
	static const type_of_vector& get_zero_vector()
	{
		static type_of_vector _v(value_trait::zero(), value_trait::zero(),
								 value_trait::zero());
		return _v;
	};
	static const type_of_vector& get_max_vector()
	{
		static type_of_vector _v(value_trait::max(), value_trait::max(),
								 value_trait::max());
		return _v;
	};
	static const type_of_vector& get_min_vector()
	{
		static type_of_vector _v(value_trait::min(), value_trait::min(),
								 value_trait::min());
		return _v;
	};
	static const type_of_vector& get_x_axis()
	{
		static type_of_vector _v(value_trait::one(), value_trait::zero(),
								 value_trait::zero());
		return _v;
	}
	static const type_of_vector& get_y_axis()
	{
		static type_of_vector _v(value_trait::zero(), value_trait::one(),
								 value_trait::zero());
		return _v;
	}
	static const type_of_vector& get_z_axis()
	{
		static type_of_vector _v(value_trait::zero(), value_trait::zero(),
								 value_trait::one());
		return _v;
	}
	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		type_of_vector _v(*this);
		_v.negate();
		return _v;
	}
	inline type_of_vector& operator=(const type_of_vector& _v)
	{
		x = _v.x, y = _v.y, z = _v.z;
		return *this;
	}

	template < class T2 >
	inline type_of_vector& operator=(const gvt_vector3< T2 >& _v)
	{
		x = (T)_v.x;
		y = (T)_v.y;
		z = (T)_v.z;
		return *this;
	}

	inline type_of_vector& operator+=(const type_of_vector& _v)
	{
		x += _v.x, y += _v.y, z += _v.z;
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& _v)
	{
		x -= _v.x, y -= _v.y, z -= _v.z;
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& _v)
	{
		x *= _v.x, y *= _v.y, z *= _v.z;
		return *this;
	}
	inline type_of_vector& operator/=(const type_of_vector& _v)
	{
		x /= _v.x, y /= _v.y, z /= _v.z;
		return *this;
	}
	inline type_of_vector& operator=(const T& t)
	{
		x = y = z = t;
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		x += t, y += t, z += t;
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		x -= t, y -= t, z -= t;
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		x *= t, y *= t, z *= t;
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		x /= t, y /= t, z /= t;
		return *this;
	}
	inline bool operator==(const type_of_vector& _v) const
	{
		if (x == _v.x && y == _v.y && z == _v.z)
			return true;
		return false;
	}
	inline bool operator<(const type_of_vector& _v) const
	{
		if (x < _v.x)
			return true;
		if (y < _v.y)
			return true;
		if (z < _v.z)
			return true;
		return false;
	}
	inline type_of_value& operator[](int index)
	{
		GV_ASSERT_SLOW(index < 3);
		return ((type_of_value*)(this))[index];
	}
	inline const type_of_value& operator[](int index) const
	{
		GV_ASSERT_SLOW(index < 3);
		return ((type_of_value*)(this))[index];
	}
	////--------------------------------derivated function
	inline type_of_vector operator+(const type_of_vector& _v) const
	{
		type_of_vector ret(*this);
		ret += _v;
		return ret;
	}
	inline type_of_vector operator-(const type_of_vector& _v) const
	{
		type_of_vector ret(*this);
		ret -= _v;
		return ret;
	}
	inline type_of_vector operator/(const type_of_vector& _v) const
	{
		type_of_vector ret(*this);
		ret /= _v;
		return ret;
	}
	inline type_of_vector operator*(const type_of_vector& _v) const
	{
		type_of_vector ret(*this);
		ret *= _v;
		return ret;
	}
	inline type_of_vector operator-(T t) const
	{
		type_of_vector ret(*this);
		ret -= t;
		return ret;
	}
	inline type_of_vector operator+(T t) const
	{
		type_of_vector ret(*this);
		ret += t;
		return ret;
	}
	inline type_of_vector operator/(T t) const
	{
		type_of_vector ret(*this);
		ret /= t;
		return ret;
	}
	inline type_of_vector operator*(T t) const
	{
		type_of_vector ret(*this);
		ret *= t;
		return ret;
	}

	inline bool operator!=(const type_of_vector& _v) const
	{
		return !((*this) == _v);
	}
	inline bool operator>=(const type_of_vector& _v) const
	{
		return !((*this) < _v);
	}
	inline bool operator<=(const type_of_vector& _v) const
	{
		return (*this) < _v || (*this) == _v;
	}
	inline bool operator>(const type_of_vector& _v) const
	{
		return !((*this) <= _v);
	}
	//
	inline void abs()
	{
		x = gvt_abs(x);
		y = gvt_abs(y);
		z = gvt_abs(z);
	};
	inline void epsilon_to_zero()
	{
		if (value_trait::is_almost_zero(x))
			x = value_trait::zero();
		if (value_trait::is_almost_zero(y))
			y = value_trait::zero();
		if (value_trait::is_almost_zero(z))
			z = value_trait::zero();
	};
	inline void zero_to_epsilon()
	{
		if (value_trait::is_almost_zero(x))
			x = value_trait::epsilon();
		if (value_trait::is_almost_zero(y))
			y = value_trait::epsilon();
		if (value_trait::is_almost_zero(z))
			z = value_trait::epsilon();
	};
	inline type_of_value distance_to(const type_of_vector& _v) const
	{
		return ((*this) - _v).length();
	}
	inline type_of_value distance_squared(const type_of_vector& _v) const
	{
		return ((*this) - _v).length_squared();
	}
	inline void clamp(T _min, T _max)
	{
		x = gvt_clamp(x, _min, _max);
		y = gvt_clamp(y, _min, _max);
		z = gvt_clamp(z, _min, _max);
	}
};

template < class T >
gvt_vector3< T > gvt_clamp(const gvt_vector3< T >& x, const gvt_vector3< T >& Min,
						   const gvt_vector3< T >& Max)
{
	gvt_vector3< T > r;
	r.set_x(gvt_clamp(x.get_x(), Min.get_x(), Max.get_x()));
	r.set_y(gvt_clamp(x.get_y(), Min.get_y(), Max.get_y()));
	r.set_z(gvt_clamp(x.get_z(), Min.get_z(), Max.get_z()));
	return r;
}

template < class T >
gvt_vector3< T > gvt_floor(const gvt_vector3< T >& x)
{
	gvt_vector3< T > r;
	r.set_x(gvt_floor(x.get_x()));
	r.set_y(gvt_floor(x.get_y()));
	r.set_z(gvt_floor(x.get_z()));
	return r;
}

template < class T >
gvt_vector3< T > gvt_ceil(const gvt_vector3< T >& x)
{
	gvt_vector3< T > r;
	r.set_x(gvt_ceil(x.get_x()));
	r.set_y(gvt_ceil(x.get_y()));
	r.set_z(gvt_ceil(x.get_z()));
	return r;
}

typedef gvt_vector3< gv_byte > gv_vector3b;
typedef gvt_vector3< gv_ushort > gv_vector3w;
typedef gvt_vector3< gv_float > gv_vector3;
typedef gvt_vector3< gv_int > gv_vector3i;
} // gv
