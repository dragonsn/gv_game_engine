#pragma once

namespace gv
{

template < class _Container, class _Containter_Result, class _Fn1 >
inline _Fn1 gvt_for_each_unary_op(_Containter_Result& result,
								  const _Container& c1, _Fn1 _Func)
{
	typename _Container::const_iterator _First = c1.begin();
	typename _Container::const_iterator _Last = c1.end();
	typename _Container::iterator _First_R = result.begin();
	for (; _First != _Last; ++_First, ++_First_R)
		_Func(*_First_R, *_First);
	GV_ASSERT(_First_R == result.end());
	return (_Func);
}

template < class _Container, class _Containter_Result, class _Fn1 >
inline _Fn1 gvt_for_each_binary_op(_Containter_Result& result,
								   const _Container& c1, const _Container& c2,
								   _Fn1 _Func)
{
	typename _Container::const_iterator _First = c1.begin();
	typename _Container::const_iterator _Last = c1.end();
	typename _Container::const_iterator _First2 = c2.begin();
	typename _Container::iterator _First_R = result.begin();
	for (; _First != _Last; ++_First, ++_First2, ++_First_R)
		_Func(*_First_R, *_First, *_First2);
	GV_ASSERT(_First2 == c2.end() && _First_R == result.end());
	return (_Func);
}

template < class _Container, class _Containter_Result, class _Fn1 >
inline _Fn1 gvt_for_each_ternary_op(_Containter_Result& result,
									const _Container& c1, const _Container& c2,
									const _Container& c3, _Fn1 _Func)
{
	typename _Container::const_iterator _First = c1.begin();
	typename _Container::const_iterator _Last = c1.end();
	typename _Container::const_iterator _First2 = c2.begin();
	typename _Container::const_iterator _First3 = c3.begin();
	typename _Container::iterator _First_R = result.begin();
	for (; _First != _Last; ++_First, ++_First2, ++_First3, ++_First_R)
		_Func(*_First_R, *_First, *_First2, *_First3);
	GV_ASSERT(_First3 == c3.end() && _First2 == c2.end() &&
			  _First_R == result.end());
	return (_Func);
}

template < class _Container, class _Containter_Result, class _Fn1 >
inline _Fn1
gvt_for_each_quaternary_op(_Containter_Result& result, const _Container& c1,
						   const _Container& c2, const _Container& c3,
						   const _Container& c4, _Fn1 _Func)
{
	typename _Container::const_iterator _First = c1.begin();
	typename _Container::const_iterator _Last = c1.end();
	typename _Container::const_iterator _First2 = c2.begin();
	typename _Container::const_iterator _First3 = c3.begin();
	typename _Container::const_iterator _First4 = c4.begin();
	typename _Container::iterator _First_R = result.begin();
	for (; _First != _Last; ++_First, ++_First2, ++_First3, ++_First4, ++_First_R)
		_Func(*_First_R, *_First, *_First2, *_First3, *_First4);
	GV_ASSERT(_First4 == c4.end() && _First3 == c3.end() && _First2 == c2.end() &&
			  _First_R == result.end());
	return (_Func);
}

//---------------------------------------------------------------------
template < class T >
class gvf_replace
{
private:
	const T& old_value;
	const T& new_value;

public:
	inline gvf_replace(const T& o, const T& n)
		: old_value(o), new_value(n)
	{
	}
	inline void operator()(T& value) const
	{
		if (value == old_value)
			value = new_value;
	}
};

template < class T >
class gvf_is_almost_equal
{
public:
	bool operator()(const T& a, const T& b)
	{
		return gvt_value< T >::is_almost_equal(a, b);
	}
};

template < class T >
class gvf_reciprocal
{
public:
	inline gvf_reciprocal()
	{
	}
	inline void operator()(T& s)
	{
		s = gvt_reciprocal(s);
	}
	inline void operator()(T& target, const T& source)
	{
		target = gvt_reciprocal(source);
	}
};

template < class T >
class gvf_negate
{
public:
	inline gvf_negate()
	{
	}
	inline void operator()(T& s)
	{
		s = -s;
	}
	inline void operator()(T& target, const T& source)
	{
		target = -source;
	}
};

template < class T >
class gvf_abs
{
public:
	inline gvf_abs()
	{
	}
	inline void operator()(T& s)
	{
		s = gvt_abs(s);
	}
	inline void operator()(T& target, const T& source)
	{
		target = gvt_abs(source);
	}
};

template < class T >
class gvf_clamp
{
protected:
	const T& m_low;
	const T& m_high;

public:
	gvf_clamp(const T& low = gvt_value< T >::min(),
			  const T& high = gvt_value< T >::max())
		: m_low(low), m_high(high)
	{
	}
	void operator()(T& t)
	{
		t = gvt_clamp(t, m_low, m_high);
	}
	void operator()(T& result, const T& src, const T& low, const T& high)
	{
		result = gvt_clamp(src, low, high);
	}
};

template < class T >
class gvf_floor
{
public:
	inline gvf_floor()
	{
	}
	inline void operator()(T& s)
	{
		s = gvt_floor(s);
	}
	inline void operator()(T& target, const T& source)
	{
		target = gvt_floor(source);
	}
};

template < class T >
class gvf_ceil
{
public:
	inline gvf_ceil()
	{
	}
	inline void operator()(T& s)
	{
		s = gvt_ceil(s);
	}
	inline void operator()(T& target, const T& source)
	{
		target = gvt_ceil(source);
	}
};

#define GV_MAKE_UNARY_OP_FUNCTOR(name, op)               \
	template < class T >                                 \
	class gvf_##name                                     \
	{                                                    \
	private:                                             \
		const T& value;                                  \
                                                         \
	public:                                              \
		inline gvf_##name() : value(*((T*)(0)))          \
		{                                                \
		}                                                \
		inline gvf_##name(const T& t) : value(t)         \
		{                                                \
		}                                                \
		inline void operator()(T& _value) const          \
		{                                                \
			_value op value;                             \
		}                                                \
		inline void operator()(T& ret, const T& a) const \
		{                                                \
			ret op a;                                    \
		}                                                \
	};

GV_MAKE_UNARY_OP_FUNCTOR(assign, =);
GV_MAKE_UNARY_OP_FUNCTOR(add_assign, +=);
GV_MAKE_UNARY_OP_FUNCTOR(sub_assign, -=);
GV_MAKE_UNARY_OP_FUNCTOR(mul_assign, *=);
GV_MAKE_UNARY_OP_FUNCTOR(div_assign, /=);

#define GV_MAKE_BINARY_OP_FUNCTOR(name, op)                          \
	template < class T >                                             \
	class gvf_##name                                                 \
	{                                                                \
	private:                                                         \
	public:                                                          \
		inline gvf_##name()                                          \
		{                                                            \
		}                                                            \
		inline void operator()(T& ret, const T& a, const T& b) const \
		{                                                            \
			ret = a op b;                                            \
		}                                                            \
	};

GV_MAKE_BINARY_OP_FUNCTOR(add, +)
GV_MAKE_BINARY_OP_FUNCTOR(sub, -)
GV_MAKE_BINARY_OP_FUNCTOR(mul, *)
GV_MAKE_BINARY_OP_FUNCTOR(div, /)
GV_MAKE_BINARY_OP_FUNCTOR(mod, %)

template < class T >
class gvf_add_and_round
{
public:
	gvf_add_and_round(const T& _step, const T& _max)
	{
		m_max = _max;
		m_step = _step;
	}
	T operator()(const T& a)
	{
		T b = a + m_step;
		if (b >= m_max)
			b = b % m_max;
		return b;
	}

protected:
	T m_max, m_step;
};

template < class T1, class T2 >
class gvf_cast_and_assign
{
public:
	gvf_cast_and_assign()
	{
	}
	T1 operator()(const T2& a)
	{
		return (T1)a;
	}
};

template < class T >
class gvf_alpha_blend
{
public:
	gvf_alpha_blend()
	{
	}
	void operator()(T& result, const T& a, const T& b, const T& alpha)
	{
		result = gvt_lerp(a, b, alpha);
	}
};
}
