#pragma once

namespace gv
{
//------------------------------generic vector for any type, for any dimention
template < class T, int const_size >
class gvt_vector_generic : public gvt_vector_generic< T, const_size - 1 >
{
public:
	typedef gvt_vector_generic< T, const_size - 1 > super;
	typedef gvt_vector_generic< T, const_size > type_of_vector;
	typedef T type_of_element;
	typedef gvt_value< T > value_trait;
	static const int vertex_size = const_size;

	inline gvt_vector_generic(){};
	inline gvt_vector_generic(const gvt_vector_generic& v)
	{
		*this = v;
	};
	inline gvt_vector_generic(const super& v)
	{
		this->get_super() = v;
		x = value_trait::one();
	};
	explicit gvt_vector_generic(const T& t)
	{
		*this = t;
	};
	explicit gvt_vector_generic(const T (&a)[const_size])
	{
		const T* p = a;
		iterator i = begin();
		while (i != end())
			*i++ = *p++;
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

	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		type_of_vector v;
		v.get_super() = -get_super();
		v.x = -x;
		return v;
	}
	inline type_of_vector& operator=(const type_of_vector& v)
	{
		get_super() = v.get_super();
		x = v.x;
		return *this;
	}
	inline type_of_vector& operator+=(const type_of_vector& v)
	{
		get_super() += v.get_super();
		x += v.x;
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& v)
	{
		get_super() -= v.get_super();
		x -= v.x;
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& v)
	{
		get_super() *= v.get_super();
		x *= v.x;
		return *this;
	}
	inline type_of_vector& operator/=(const type_of_vector& v)
	{
		get_super() /= v.get_super();
		x /= v.x;
		return *this;
	}

	inline type_of_vector& operator=(const T& t)
	{
		get_super() = t;
		x = t;
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		get_super() += t;
		x += t;
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		get_super() -= t;
		x -= t;
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		get_super() *= t;
		x *= t;
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		get_super() /= t;
		x /= t;
		return *this;
	}

	inline bool operator==(const type_of_vector& v) const
	{
		return gvt_is_almost_equal_container(*this, v);
	}
	inline bool operator<(const type_of_vector& v) const
	{
		if (get_super() < v.get_super())
			return true;
		if (x < v.x)
			return true;
		return false;
	}

	template < class T2 >
	inline type_of_vector&
	operator=(const gvt_vector_generic< T2, const_size >& a)
	{
		gvt_copy(a.begin(), a.end(), begin());
		return *this;
	}

	inline void clamp(T _min, T _max)
	{
		std::for_each(begin(), end(), gvf_clamp< type_of_element >(_min, _max));
	}

	inline T dot(const type_of_vector& v) const
	{
		T ret = get_super().dot(v.get_super());
		ret += x * v.x;
		return ret;
	}
	inline T sum() const
	{
		T ret = get_super().sum();
		ret += x;
		return ret;
	}
	inline T max() const
	{
		T ret = get_super().max();
		return gvt_max(x, ret);
	}
	inline T min() const
	{
		T ret = get_super().min();
		return gvt_min(x, ret);
	}
	inline void abs()
	{
		get_super().abs();
		x = gvt_abs(x);
	}
	inline void mark_index()
	{
		get_super().mark_index();
		x = (T)const_size;
	}

	////--------------------------------derivated function
	inline type_of_vector operator+(const type_of_vector& v) const
	{
		type_of_vector ret(*this);
		ret += v;
		return ret;
	}
	inline type_of_vector operator-(const type_of_vector& v) const
	{
		type_of_vector ret(*this);
		ret -= v;
		return ret;
	}
	inline type_of_vector operator/(const type_of_vector& v) const
	{
		type_of_vector ret(*this);
		ret /= v;
		return ret;
	}
	inline type_of_vector operator*(const type_of_vector& v) const
	{
		type_of_vector ret(*this);
		ret *= v;
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

	inline bool operator!=(const type_of_vector& v) const
	{
		return !((*this) == v);
	}
	inline bool operator>=(const type_of_vector& v) const
	{
		return !((*this) < v);
	}
	inline bool operator<=(const type_of_vector& v) const
	{
		return (*this) < v || (*this) == v;
	}
	inline bool operator>(const type_of_vector& v) const
	{
		return !((*this) <= v);
	}

	//////--------------------------------generic function
	typedef T* iterator;
	typedef const T* const_iterator;
	inline T* begin()
	{
		return &this->x - (const_size - 1);
	}
	inline const_iterator begin() const
	{
		return &this->x - (const_size - 1);
	}
	inline T* end()
	{
		return &this->x + 1;
	}
	inline const_iterator end() const
	{
		return &this->x + 1;
	}
	inline const T& operator[](gv_int i) const
	{
		GV_ASSERT_SLOW(i < const_size);
		return *(begin() + i);
	};
	inline T& operator[](gv_int i)
	{
		GV_ASSERT_SLOW(i < const_size);
		return *(begin() + i);
	};
	void randomize(const T& low = value_trait::zero(),
				   const T& high = value_trait::one())
	{
		gvt_for_each(*this, gvf_randomize< T >(low, high));
	}
	void reciprocal()
	{
		gvt_for_each(*this, gvf_reciprocal< T >());
	}

protected:
	T x;
};

//------------------------------specified vector for 0 dimention
template < class T >
class gvt_vector_generic< T, 0 >
{
public:
	typedef gvt_vector_generic< T, 0 > type_of_vector;
	typedef gvt_value< T > value_trait;
	inline gvt_vector_generic(){};
	inline gvt_vector_generic(const gvt_vector_generic& v){};

	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		return *this;
	}
	inline type_of_vector& operator=(const type_of_vector& v)
	{
		return *this;
	}
	inline type_of_vector& operator+=(const type_of_vector& v)
	{
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& v)
	{
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& v)
	{
		return *this;
	}
	inline type_of_vector& operator/=(const type_of_vector& v)
	{
		return *this;
	}

	inline type_of_vector& operator=(const T& t)
	{
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		return *this;
	}

	inline bool operator==(const type_of_vector& v) const
	{
		return true;
	}
	inline bool operator<(const type_of_vector& v) const
	{
		return true;
	}

	inline T dot(const type_of_vector& v) const
	{
		T ret = 0;
		return ret;
	}
	inline T sum() const
	{
		T ret = 0;
		return ret;
	}
	inline void mark_index()
	{
	}
	inline void abs()
	{
	}
	inline T max() const
	{
		return value_trait::min();
	}
	inline T min() const
	{
		return value_trait::max();
	}
};
}