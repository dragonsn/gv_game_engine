#pragma once
namespace gv
{

template < class T1, class T2 >
inline void gv_convert_color(T1& t, const T2& s)
{
	t = (T1)s;
};

inline void gv_convert_color(gv_byte& t, const gv_float& s)
{
	t = (gv_byte)(s * 255.f);
};

inline void gv_convert_color(float& t, const gv_byte& s)
{
	t = ((gv_float)(s)) / 255.f;
};

inline void gv_convert_color(gv_byte& t, const gv_byte& s)
{
	t = s;
};

template < class T >
inline void gv_color_clamp(T& t)
{
	t = gvt_clamp(t, gvt_value< T >::zero(), gvt_value< T >::one());
}

inline void gv_color_clamp(gv_byte& t)
{
}

#define GV_DEFINE_COLOR(name, r, g, b, a)                               \
	static const type_of_color& name()                                  \
	{                                                                   \
		static type_of_color s_c((gv_float)r, (gv_float)g, (gv_float)b, \
								 (gv_float)a);                          \
		return s_c;                                                     \
	}

template < class T >
class gvt_color
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector4< T > type_of_vector;
	typedef gvt_color< T > type_of_color;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	union {
		struct
		{
			type_of_value r, g, b, a;
		};
		struct
		{
			type_of_vector v;
		};
		gv_uint fixed32;
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_color()
	{
	}

	type_of_color ARGB() const
	{
		return gvt_color(a, r, g, b);
	}

	type_of_color BGRA() const
	{
		return gvt_color(b, g, r, a);
	}

	inline gvt_color(const T& _r, const T& _g, const T& _b, const T& _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	inline gvt_color(const gvt_color& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}

	template < class T2 >
	inline gvt_color(const T2& _r, const T2& _g, const T2& _b, const T2& _a)
	{
		gv_convert_color(r, _r);
		gv_convert_color(g, _g);
		gv_convert_color(b, _b);
		gv_convert_color(a, _a);
	}

	inline void set(const T& _r, const T& _g, const T& _b, const T& _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	template < class T2 >
	inline gvt_color(const gvt_color< T2 >& other)
	{
		*this = other;
	}

	template < class T2 >
	inline type_of_color& operator=(const gvt_color< T2 >& other)
	{
		gv_convert_color(r, other.r);
		gv_convert_color(g, other.g);
		gv_convert_color(b, other.b);
		gv_convert_color(a, other.a);
		return *this;
	}

	inline type_of_color& operator=(const type_of_color& other)
	{
		v = other.v;
		return *this;
	}

	bool operator==(const type_of_color& other) const
	{
		return v == other.v;
	}

	bool operator<(const type_of_color& other) const
	{
		return v < other.v;
	}

	inline void clamp()
	{
		gv_color_clamp(r);
		gv_color_clamp(g);
		gv_color_clamp(b);
		gv_color_clamp(a);
	}

	inline gv_float get_grey_scale() const
	{
		static const gv_float R_FACTOR = .30f;
		static const gv_float G_FACTOR = .59f;
		static const gv_float B_FACTOR = .11f;
		gvt_color< gv_float > fc = *this;
		return fc.r * R_FACTOR + fc.g * G_FACTOR + fc.b * B_FACTOR;
	}

	inline void set_to_grey()
	{
		T t;
		gv_convert_color(t, this->get_grey());
		r = g = b = a = t;
	}

	inline void swap_red_blue()
	{
		gvt_swap(r, b);
	}

	type_of_value get_r() const
	{
		return r;
	}

	type_of_value get_g() const
	{
		return g;
	}

	type_of_value get_b() const
	{
		return b;
	}

	type_of_value get_a() const
	{
		return a;
	}

	inline type_of_color operator-() const
	{
		type_of_vector _v;
		_v.get_super() = -this->get_super();
		_v.w = -this->w;
		return _v;
	}
	inline type_of_color& operator+=(const type_of_color& _v)
	{
		v += _v.v;
		return *this;
	}
	inline type_of_color& operator-=(const type_of_color& _v)
	{
		v -= _v.v;
		return *this;
	}
	inline type_of_color& operator*=(const type_of_color& _v)
	{
		v *= _v.v;
		return *this;
	}
	inline type_of_color& operator/=(const type_of_color& _v)
	{
		v /= _v.v;
		return *this;
	}

	inline type_of_color& operator=(const T& t)
	{
		v = t;
		return *this;
	}
	inline type_of_color& operator+=(const T& t)
	{
		v += t;
		return *this;
	}
	inline type_of_color& operator-=(const T& t)
	{
		v -= t;
		return *this;
	}
	inline type_of_color& operator*=(const T& t)
	{
		v *= t;
		return *this;
	}
	inline type_of_color& operator/=(const T& t)
	{
		v /= t;
		return *this;
	}

	inline type_of_color operator+(const type_of_color& _v) const
	{
		type_of_color ret(*this);
		ret += _v;
		return ret;
	}
	inline type_of_color operator-(const type_of_color& _v) const
	{
		type_of_color ret(*this);
		ret -= _v;
		return ret;
	}
	inline type_of_color operator/(const type_of_color& _v) const
	{
		type_of_color ret(*this);
		ret /= _v;
		return ret;
	}
	inline type_of_color operator*(const type_of_color& _v) const
	{
		type_of_color ret(*this);
		ret *= _v;
		return ret;
	}
	inline type_of_color operator-(T t) const
	{
		type_of_color ret(*this);
		ret -= t;
		return ret;
	}
	inline type_of_color operator+(T t) const
	{
		type_of_color ret(*this);
		ret += t;
		return ret;
	}
	inline type_of_color operator/(T t) const
	{
		type_of_color ret(*this);
		ret /= t;
		return ret;
	}
	inline type_of_color operator*(T t) const
	{
		type_of_color ret(*this);
		ret *= t;
		return ret;
	}

	GV_DEFINE_COLOR(BLACK, 0, 0, 0, 1);
	GV_DEFINE_COLOR(WHITE, 1, 1, 1, 1);
	GV_DEFINE_COLOR(GREEN, 0, 0.7, 0, 1);
	GV_DEFINE_COLOR(GREEN_B, 0.2f, 1, 0.2f, 1);
	GV_DEFINE_COLOR(GREEN_D, 0, 0.3f, 0, 1);
	GV_DEFINE_COLOR(PINK, 0.7f, 0.2f, 0.2f, 1);
	GV_DEFINE_COLOR(PINK_B, 1.0f, 0.4f, 0.4f, 1);
	GV_DEFINE_COLOR(PINK_D, 0.5f, 0.1f, 0.1f, 1);
	GV_DEFINE_COLOR(YELLOW, 0.7f, 0.7f, 0, 1);
	GV_DEFINE_COLOR(YELLOW_B, 1.0f, 1.0f, 0, 1);
	GV_DEFINE_COLOR(YELLOW_D, 0.3f, 0.3f, 0, 1);
	GV_DEFINE_COLOR(BLUE, 0, 0.0, 0.9f, 1);
	GV_DEFINE_COLOR(BLUE_B, 0.3f, 0.3f, 1.0f, 1);
	GV_DEFINE_COLOR(BLUE_D, 0, 0.0, 0.6f, 1);
	GV_DEFINE_COLOR(GREY, 0.5f, 0.5f, 0.5f, 1);
	GV_DEFINE_COLOR(GREY_B, 0.75f, 0.75f, 0.75f, 1);
	GV_DEFINE_COLOR(GREY_D, 0.25f, 0.25f, 0.25f, 1);
	GV_DEFINE_COLOR(RED, 0.7f, 0, 0, 1);
	GV_DEFINE_COLOR(RED_B, 1.f, 0, 0, 1);
	GV_DEFINE_COLOR(RED_D, 0.4f, 0, 0, 1);
	GV_DEFINE_COLOR(ORANGE, 1.0f, 0.64, 0, 1);
};

typedef gvt_color< gv_byte > gv_color;
typedef gvt_color< gv_float > gv_colorf;
}