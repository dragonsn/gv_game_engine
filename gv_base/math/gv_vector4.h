#pragma once

namespace gv
{

template < class T >
class gvt_vector4 : public gvt_vector3< T >
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector4< T > type_of_vector;
	typedef gvt_vector3< T > super;
	typedef gvt_vector_generic< type_of_value, 4 > type_of_generic;
	static const int vector_size = 4;

public:
	T w;

public:
	inline gvt_vector4(){};
	inline gvt_vector4(type_of_value _x, type_of_value _y, type_of_value _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = value_trait::one();
	}
	inline gvt_vector4(type_of_value _x, type_of_value _y, type_of_value _z,
					   type_of_value _w)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;
	}
	inline gvt_vector4(const type_of_vector& a)
	{
		*this = a;
	}
	inline gvt_vector4(const super& a)
	{
		*this = a;
	}
	inline gvt_vector4(const super& a, type_of_value _w)
	{
		*this = a;
		w = _w;
	}
	explicit gvt_vector4(type_of_value f)
	{
		this->x = this->y = this->z = this->w = f;
	};
	inline void set(type_of_value _x, type_of_value _y, type_of_value _z,
					type_of_value _w)
	{
		this->x = _x, this->y = _y, this->z = _z, this->w = _w;
	};
	inline void get(type_of_value& _x, type_of_value& _y, type_of_value& _z,
					type_of_value& _w) const
	{
		_x = this->x, _y = this->y, _z = this->z, _w = this->w;
	};
	inline type_of_generic& v4()
	{
		return *((type_of_generic*)this);
	}
	inline const type_of_generic& v4() const
	{
		return *((type_of_generic*)this);
	}
	inline void set_w(type_of_value _w)
	{
		w = _w;
	}
	inline type_of_value get_w() const
	{
		return w;
	}
	inline void negate()
	{
		this->x = -this->x, this->y = -this->y, this->z = -this->z;
		this->w = -this->w;
	}

	template < class T2 >
	inline type_of_vector& operator=(const gvt_vector4< T2 >& _v)
	{
		this->x = (T)_v.x;
		this->y = (T)_v.y;
		this->z = (T)_v.z;
		this->w = (T)_v.w;
		return *this;
	}
	//--------------------------------type conversion
	inline super& get_super()
	{
		return *this;
	}
	inline const super& get_super() const
	{
		return *this;
	}
	inline super& get_v3()
	{
		return *this;
	}
	inline const super& get_v3() const
	{
		return *this;
	}
	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		type_of_vector _v;
		_v.get_super() = -get_super();
		_v.w = -w;
		return _v;
	}
	inline type_of_vector& operator=(const type_of_vector& _v)
	{
		get_super() = _v.get_super();
		w = _v.w;
		return *this;
	}
	inline type_of_vector& operator=(const super& _v)
	{
		get_super() = _v;
		w = value_trait::one();
		return *this;
	}
	inline type_of_vector& operator+=(const type_of_vector& _v)
	{
		get_super() += _v.get_super();
		w += _v.w;
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& _v)
	{
		get_super() -= _v.get_super();
		w -= _v.w;
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& _v)
	{
		get_super() *= _v.get_super();
		w *= _v.w;
		return *this;
	}
	inline type_of_vector& operator/=(const type_of_vector& _v)
	{
		get_super() /= _v.get_super();
		w /= _v.w;
		return *this;
	}

	inline type_of_vector& operator=(const T& t)
	{
		get_super() = t;
		w = t;
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		get_super() += t;
		w += t;
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		get_super() -= t;
		w -= t;
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		get_super() *= t;
		w *= t;
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		get_super() /= t;
		w /= t;
		return *this;
	}

	inline void load(const type_of_value* p)
	{
		memcpy(this, p, sizeof(*this));
	}
	inline void save(type_of_value* p) const
	{
		memcpy(p, this, sizeof(*this));
	}

	inline bool operator==(const type_of_vector& _v) const
	{
		if (get_super() == _v.get_super() && w == _v.w)
			return true;
		return false;
	}
	inline bool operator<(const type_of_vector& _v) const
	{
		if (get_super() < _v.get_super())
			return true;
		if (w < _v.w)
			return true;
		return false;
	}
	inline T dot(const type_of_vector& _v) const
	{
		T ret = get_super().dot(_v.get_super());
		ret += w * _v.w;
		return ret;
	}
	inline type_of_value& operator[](int index)
	{
		GV_ASSERT_SLOW(index < 4);
		return ((type_of_value*)(this))[index];
	}
	inline const type_of_value& operator[](int index) const
	{
		GV_ASSERT_SLOW(index < 4);
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

	inline void clamp(T _min, T _max)
	{
		this->x = gvt_clamp(this->x, _min, _max);
		this->y = gvt_clamp(this->y, _min, _max);
		this->z = gvt_clamp(this->z, _min, _max);
		this->w = gvt_clamp(this->w, _min, _max);
	}
};

typedef gvt_vector4< gv_ushort > gv_vector4w;
typedef gvt_vector4< gv_float > gv_vector4;
typedef gvt_vector4< gv_int > gv_vector4i;
typedef gvt_vector4< gv_byte > gv_vector4b;

template < class T >
gvt_vector4< T > gvt_clamp(const gvt_vector4< T >& x, const gvt_vector4< T >& Min,
						   const gvt_vector4< T >& Max)
{
	gvt_vector4< T > r;
	r.set_x(gvt_clamp(x.get_x(), Min.get_x(), Max.get_x()));
	r.set_y(gvt_clamp(x.get_y(), Min.get_y(), Max.get_y()));
	r.set_z(gvt_clamp(x.get_z(), Min.get_z(), Max.get_z()));
	r.set_w(gvt_clamp(x.get_w(), Min.get_w(), Max.get_w()));
	return r;
}

template < class T >
gvt_vector4< T > gvt_floor(const gvt_vector4< T >& x)
{
	gvt_vector4< T > r;
	r.set_x(gvt_floor(x.get_x()));
	r.set_y(gvt_floor(x.get_y()));
	r.set_z(gvt_floor(x.get_z()));
	r.set_w(gvt_floor(x.get_w()));
	return r;
}

template < class T >
gvt_vector4< T > gvt_ceil(const gvt_vector4< T >& x)
{
	gvt_vector4< T > r;
	r.set_x(gvt_ceil(x.get_x()));
	r.set_y(gvt_ceil(x.get_y()));
	r.set_z(gvt_ceil(x.get_z()));
	r.set_w(gvt_ceil(x.get_w()));
	return r;
}

inline gv_vector4b gv_to_normalized_byte_v4(const gv_vector3& v)
{
	float x = gvt_clamp< float >(v.x * 127 + 128, 0, 255);
	float y = gvt_clamp< float >(v.y * 127 + 128, 0, 255);
	float z = gvt_clamp< float >(v.z * 127 + 128, 0, 255);
	return gv_vector4b((gv_byte)x, (gv_byte)y, (gv_byte)z, 0);
}

inline gv_vector3 gv_from_normalized_byte_v4(const gv_vector4b& v)
{
	float x = ((float)(v.x) - 128.f) / 127.f;
	float y = ((float)(v.y) - 128.f) / 127.f;
	float z = ((float)(v.z) - 128.f) / 127.f;
	return gv_vector3(x, y, z);
}
}