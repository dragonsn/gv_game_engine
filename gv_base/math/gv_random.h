#pragma once
namespace gv
{
// https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor/
struct gv_random_uint
{
public:
	gv_uint m_seed;

	gv_random_uint()
	{
		m_seed = 521288629;
	}

	// Used to seed the generator.
	inline void set_seed(gv_uint s)
	{
		m_seed = s;
	}

	// fastrand routine returns one integer, similar output value range as C lib.
	inline gv_ushort get_ushort()
	{
		m_seed = (214013 * m_seed + 2531011);
		return (gv_ushort)((m_seed >> 16) & 0x7FFF);
	}

	inline int get_uint()
	{
		gv_uint l = get_ushort();
		gv_uint h = get_ushort();
		return (h << 16) + l;
	}

	inline gv_uint get_ranged(gv_uint min32, gv_uint max32)
	{
		gv_uint delta = max32 - min32 + 1;
		gv_uint d = get_uint() % delta;
		return min32 + d;
	}
};

// generate uniform random number (0-1.0f);
template < typename T >
class gvt_random : public gv_random_uint
{
protected:
	typedef T type_of_value;
	typedef gvt_random< T > self;
	typedef gvt_value< T > value_trait;
	// http://www.bobwheeler.com/statistics/Password/MarsagliaPost.txt
public:
	inline T get_uniform() // random in 0-1
	{
		gv_uint u = get_uint();
		return ((T)(u + 1)) * ((T)2.328306435454494e-10);
	}
};

template < typename T >
class gvt_random_range
{
protected:
	typedef T type_of_value;
	typedef gvt_value< T > value_trait;
	type_of_value m_lower, m_upper;
	gvt_random< gv_double >& generator()
	{
		static gvt_random< gv_double > m_generator;
		return m_generator;
	}

public:
	inline gvt_random_range()
		: m_lower(value_trait::zero()), m_upper(value_trait::one())
	{
	}
	inline gvt_random_range(T lower, T upper)
		: m_lower(lower), m_upper(upper)
	{
	}
	inline void set_seed(gv_uint u, gv_uint v = 0)
	{
		generator().set_seed(u, v);
	}

public:
	inline T operator()()
	{
		gv_double rnd = generator().get_uniform();
		return (T)(m_lower + (m_upper - m_lower) * rnd);
	}
};

template < class T >
class gvf_randomize : public std::unary_function< T, void >
{
private:
	gvt_random_range< T > m_generator;

public:
	inline gvf_randomize(const T& lower, const T& upper)
		: m_generator(lower, upper)
	{
	}
	inline void operator()(T& value)
	{
		value = m_generator();
	}
};

template < class T >
inline T gvt_rand(T& value, const T& lower = gvt_value< T >::zero(),
				  const T& upper = gvt_value< T >::one())
{
	gvf_randomize< T >(lower, upper)(value);
	return value;
}
}