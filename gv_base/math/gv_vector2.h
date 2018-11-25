namespace gv
{
template < class T >
class gvt_vector2
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_vector;
	typedef gvt_vector_generic< type_of_value, 2 > type_of_generic;
	static const int vector_size = 2;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	union {
		struct
		{
			type_of_value x, y;
		};
		struct
		{
			type_of_generic v;
		};
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	inline gvt_vector2(){};
	inline gvt_vector2(type_of_value _x, type_of_value _y)
		: x(_x), y(_y)
	{
	}
	inline gvt_vector2(const type_of_vector& a)
	{
		*this = a;
	}
	inline gvt_vector2(const type_of_generic& a)
	{
		v = a;
	}
	explicit gvt_vector2(type_of_value f)
	{
		x = y = f;
	};
	inline void set(type_of_value _x, type_of_value _y)
	{
		x = _x, y = _y;
	};
	inline void get(type_of_value& _x, type_of_value& _y) const
	{
		_x = x, _y = y;
	};
	inline type_of_value cross(const type_of_vector& a) const
	{
		return x * a.y - y * a.x;
	}
	inline type_of_value length() const
	{
		float l = dot(*this);
		l = gvt_sqrt(l);
		return l;
	};
	inline type_of_value sum() const
	{
		float l = x + y;
		return l;
	};
	inline type_of_value abs_sum() const
	{
		type_of_value l = gvt_abs(x) + gvt_abs(y);
		return l;
	};
	inline type_of_value normalize()
	{
		float l = length();
		if (value_trait::is_almost_zero(l))
		{
			(*this) = get_x_axis();
			return value_trait::zero();
		}
		(*this) /= l;
		return l;
	};
	inline type_of_value dot(const type_of_vector& a) const
	{
		return x * a.x + y * a.y;
	};

	inline type_of_value length_squared() const
	{
		type_of_value l = dot(*this);
		return l;
	};
	inline bool is_almost_equal(const type_of_vector& _v) const
	{
		if (value_trait::is_almost_equal(x, _v.x) &&
			value_trait::is_almost_equal(y, _v.y))
		{
			return true;
		}
		return false;
	};
	inline bool is_normlaized()
	{
		return value_trait::is_almost_equal(length(), value_trait::one());
	}
	inline bool is_almost_zero() const
	{
		return value_trait::is_almost_zero(abs_sum());
	};
	inline void negate()
	{
		x = -x, y = -y;
	}
	//==================================
	static const type_of_vector& get_zero_vector()
	{
		static type_of_vector _v(value_trait::zero(), value_trait::zero());
		return _v;
	};
	static const type_of_vector& get_max_vector()
	{
		static type_of_vector _v(value_trait::max(), value_trait::max());
		return _v;
	};
	static const type_of_vector& get_min_vector()
	{
		static type_of_vector _v(value_trait::min(), value_trait::min());
		return _v;
	};
	static const type_of_vector& get_x_axis()
	{
		static type_of_vector _v(value_trait::one(), value_trait::zero());
		return _v;
	}
	static const type_of_vector& get_y_axis()
	{
		static type_of_vector _v(value_trait::zero(), value_trait::one());
		return _v;
	}
	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		type_of_vector _v(*this);
		_v.negate();
		return _v;
	}
	inline type_of_vector ortho() const
	{
		return type_of_vector(-y, x);
	}
	inline type_of_vector& operator=(const type_of_vector& _v)
	{
		x = _v.x, y = _v.y;
		return *this;
	}

	template < class T2 >
	inline type_of_vector& operator=(const gvt_vector2< T2 >& _v)
	{
		x = (T)_v.x;
		y = (T)_v.y;
		return *this;
	}

	inline type_of_vector& operator+=(const type_of_vector& _v)
	{
		x += _v.x, y += _v.y;
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& _v)
	{
		x -= _v.x, y -= _v.y;
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& _v)
	{
		x *= _v.x, y *= _v.y;
		return *this;
	}
	inline type_of_vector& operator/=(const type_of_vector& _v)
	{
		x /= _v.x, y /= _v.y;
		return *this;
	}
	inline type_of_vector& operator=(T t)
	{
		x = y = t;
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		x += t, y += t;
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		x -= t, y -= t;
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		x *= t, y *= t;
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		x /= t, y /= t;
		return *this;
	}
	inline bool operator==(const type_of_vector& _v) const
	{
		if (x == _v.x && y == _v.y)
			return true;
		return false;
	}
	inline bool operator<(const type_of_vector& _v) const
	{
		if (x < _v.x)
			return true;
		if (y < _v.y)
			return true;
		return false;
	}
	inline bool is_inside(const type_of_vector& _v) const
	{
		return (x < _v.x && y < _v.y);
	}
	inline type_of_value& operator[](int index)
	{
		GV_ASSERT_SLOW(index < 2);
		return ((type_of_value*)(this))[index];
	}
	inline const type_of_value& operator[](int index) const
	{
		GV_ASSERT_SLOW(index < 2);
		return ((type_of_value*)(this))[index];
	}
	inline void clamp(T _min, T _max)
	{
		x = gvt_clamp(x, _min, _max);
		y = gvt_clamp(y, _min, _max);
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
	};
	inline void epsilon_to_zero()
	{
		if (value_trait::is_almost_zero(x))
			x = value_trait::zero();
		if (value_trait::is_almost_zero(y))
			y = value_trait::zero();
	};
	inline void zero_to_epsilon()
	{
		if (value_trait::is_almost_zero(x))
			x = value_trait::epsilon();
		if (value_trait::is_almost_zero(y))
			y = value_trait::epsilon();
	};
	inline type_of_value distance_to(const type_of_vector& _v) const
	{
		return ((*this) - _v).length();
	}
	inline type_of_value distance_squared(const type_of_vector& _v) const
	{
		return ((*this) - _v).length_squared();
	}
	inline type_of_value& get_x()
	{
		return this->x;
	}
	inline const type_of_value& get_x() const
	{
		return this->x;
	}
	inline type_of_value& get_y()
	{
		return this->y;
	}
	inline const type_of_value& get_y() const
	{
		return this->y;
	}
	inline void set_x(type_of_value _x)
	{
		x = _x;
	}
	inline void set_y(type_of_value _y)
	{
		y = _y;
	}
};
typedef gvt_vector2< gv_byte > gv_vector2b;
typedef gvt_vector2< gv_ushort > gv_vector2w;
typedef gvt_vector2< gv_short > gv_vector2s;
typedef gvt_vector2< gv_int > gv_vector2i;
typedef gvt_vector2< gv_float > gv_vector2;

template < class T >
gvt_vector2< T > gvt_clamp(const gvt_vector2< T >& x, const gvt_vector2< T >& Min,
						   const gvt_vector2< T >& Max)
{
	gvt_vector2< T > r;
	r.set_x(gvt_clamp(x.get_x(), Min.get_x(), Max.get_x()));
	r.set_y(gvt_clamp(x.get_y(), Min.get_y(), Max.get_y()));
	return r;
}

template < class T >
gvt_vector2< T > gvt_floor(const gvt_vector2< T >& x)
{
	gvt_vector2< T > r;
	r.x = (gvt_floor(x.get_x()));
	r.y = (gvt_floor(x.get_y()));
	return r;
}

template < class T >
gvt_vector2< T > gvt_ceil(const gvt_vector2< T >& x)
{
	gvt_vector2< T > r;
	r.x = (gvt_ceil(x.get_x()));
	r.y = (gvt_ceil(x.get_y()));
	return r;
}
}