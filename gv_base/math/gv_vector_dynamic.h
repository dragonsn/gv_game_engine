#pragma once
namespace gv
{
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T >,
		   class policy_size_calculator = gvt_policy_size_calculator< T > >
class gvt_vector_dynamic
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector_dynamic< T, policy_memory, policy_thread_mode,
								policy_size_calculator >
		type_of_vector;

	typedef const T* const_iterator;
	typedef T* iterator;
	inline iterator begin()
	{
		return v.begin();
	}
	inline const_iterator begin() const
	{
		return v.begin();
	}
	inline iterator end()
	{
		return v.end();
	}
	inline const_iterator end() const
	{
		return v.end();
	}
	//============================================================================================
	//								:
	//============================================================================================
	gvt_array< T, policy_memory, policy_thread_mode, policy_size_calculator > v;
	//============================================================================================
	//								:
	//============================================================================================

	inline gvt_vector_dynamic(){};
	inline gvt_vector_dynamic(const T* p, gv_int v_size)
	{
		v.init(p, v_size);
	}
	inline gvt_vector_dynamic(const type_of_vector& a)
	{
		*this = a;
	}
	explicit gvt_vector_dynamic(type_of_value f)
	{
		*this = f;
	};

	inline gv_int get_vector_size() const
	{
		return v.size();
	}
	inline void set_vector_size(gv_int s)
	{
		v.resize(s);
	}
	void build_arithmetic_progression(type_of_value start, type_of_value increase,
									  gv_int size)
	{
		set_vector_size(size);
		for (int i = 0; i < size; i++)
		{
			v[i] = start;
			start += increase;
		}
	}
	void build_geometric_progression(type_of_value start, type_of_value factor,
									 gv_int size)
	{
		set_vector_size(size);
		for (int i = 0; i < size; i++)
		{
			v[i] = start;
			start *= factor;
		}
	}
	inline type_of_value length() const
	{
		float l = dot(*this);
		l = gvt_sqrt(l);
		return l;
	};
	inline type_of_value sum() const
	{
		type_of_value l = value_trait::zero();
		type_of_value* vb = v.begin();
		type_of_value* ve = v.end();
		for (; vb != ve; vb++)
		{
			l += *vb;
		}
		return l;
	};
	inline type_of_value abs_sum() const
	{
		type_of_value l = value_trait::zero();
		type_of_value* vb = v.begin();
		type_of_value* ve = v.end();
		for (; vb != ve; vb++)
		{
			l += gvt_abs(*vb);
		}
		return l;
	};
	inline type_of_value normalize()
	{
		type_of_value l = length();
		if (!get_vector_size())
			return l;

		if (value_trait::is_almost_zero(l))
		{
			(*this) = value_trait::zero();
			v[0] = value_trait::one();
			return value_trait::zero();
		}
		(*this) /= l;
		return l;
	};
	inline type_of_value dot(const type_of_vector& a) const
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		type_of_value l = value_trait::zero();
		type_of_value* vb = v.begin();
		type_of_value* vb2 = a.v.begin();
		type_of_value* ve = v.end();
		for (; vb != ve; vb++, vb2++)
		{
			l += (*vb) * (*vb2);
		}
		return l;
	};
	inline type_of_value length_squared() const
	{
		float l = dot(*this);
		return l;
	};
	inline bool is_almost_equal(const type_of_vector& a) const
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		type_of_value* vb = v.begin();
		type_of_value* vb2 = a.v.begin();
		type_of_value* ve = v.end();
		for (; vb != ve; vb++, vb2++)
		{
			if (!(value_trait::is_almost_equal((*vb), (*vb2))))
				return false;
		}
		return true;
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
		gvt_for_each_unary_op(v, v, gvf_negate< T >());
	}
	//==================================
	static const type_of_vector& get_zero_vector()
	{
		static type_of_vector _v(value_trait::zero());
		return _v;
	};
	static const type_of_vector& get_max_vector()
	{
		static type_of_vector _v(value_trait::max());
		return _v;
	};
	static const type_of_vector& get_min_vector()
	{
		static type_of_vector _v(value_trait::min());
		return _v;
	};
	//--------------------------------recursively defined  functions
	inline type_of_vector operator-() const
	{
		type_of_vector _v(*this);
		_v.negate();
		return _v;
	}
	inline type_of_vector& operator=(const type_of_vector& a)
	{
		set_vector_size(a.get_vector_size());
		gvt_for_each_unary_op(v, a.v, gvf_assign< T >());
		return *this;
	}

	template < class T2 >
	inline type_of_vector& operator=(const gvt_vector_dynamic< T2 >& a)
	{
		set_vector_size(a.get_vector_size());
		gvt_for_each_unary_op(v, a.v, gvf_cast_and_assign< T, T2 >());
		return *this;
	}
	inline type_of_vector& operator+=(const type_of_vector& a)
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		gvt_for_each_binary_op(v, v, a.v, gvf_add< T >());
		return *this;
	}
	inline type_of_vector& operator-=(const type_of_vector& a)
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		gvt_for_each_binary_op(v, v, a.v, gvf_sub< T >());
		return *this;
	}
	inline type_of_vector& operator*=(const type_of_vector& a)
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		gvt_for_each_binary_op(v, v, a.v, gvf_mul< T >());
		return *this;
		;
	}
	inline type_of_vector& operator/=(const type_of_vector& a)
	{
		GV_ASSERT(get_vector_size() == a.get_vector_size());
		gvt_for_each_binary_op(v, v, a.v, gvf_div< T >());
		return *this;
	}
	inline type_of_vector& operator=(T t)
	{
		v.for_each(gvf_assign< T >(t));
		return *this;
	}
	inline type_of_vector& operator+=(const T& t)
	{
		v.for_each(gvf_add_assign< T >(t));
		return *this;
	}
	inline type_of_vector& operator-=(const T& t)
	{
		v.for_each(gvf_sub_assign< T >(t));
		return *this;
	}
	inline type_of_vector& operator*=(const T& t)
	{
		v.for_each(gvf_mul_assign< T >(t));
		return *this;
	}
	inline type_of_vector& operator/=(const T& t)
	{
		v.for_each(gvf_div_assign< T >(t));
		return *this;
	}
	inline bool operator==(const type_of_vector& a) const
	{
		return v == a.v;
	}
	inline bool operator<(const type_of_vector& a) const
	{
		return v < a.v;
	}
	inline type_of_value& operator[](int index)
	{
		return v[index];
	}
	inline const type_of_value& operator[](int index) const
	{
		return v[index];
	}
	inline void clamp(T _min, T _max)
	{
		v.for_each(gvf_clamp< T >(_min, _max));
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
	inline void abs()
	{
		gvt_for_each_unary_op(v, v, gvf_abs< T >());
	};
	inline void epsilon_to_zero()
	{
		for (int i = 0; i < v.size(); i++)
		{
			if (value_trait::is_almost_zero(v[i]))
				v[i] = value_trait::zero();
		}
	};
	inline void zero_to_epsilon()
	{
		for (int i = 0; i < v.size(); i++)
		{
			if (value_trait::is_almost_zero(v[i]))
				v[i] = value_trait::epsilon();
		}
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
};
template < class T >
gvt_vector_dynamic< T > gvt_clamp(const gvt_vector_dynamic< T >& x,
								  const gvt_vector_dynamic< T >& Min,
								  const gvt_vector_dynamic< T >& Max)
{
	gvt_vector_dynamic< T > r;
	r.set_vector_size(x.get_vector_size());
	gvt_for_each_ternary_op(r, x, Min, Max, gvf_clamp< T >());
	return r;
}

template < class T >
gvt_vector_dynamic< T > gvt_floor(const gvt_vector_dynamic< T >& x)
{
	gvt_vector_dynamic< T > r;
	r.set_vector_size(x.get_vector_size());
	gvt_for_each_unary_op(r, x, gvf_floor< T >());
	return r;
}

template < class T >
gvt_vector_dynamic< T > gvt_ceil(const gvt_vector_dynamic< T >& x)
{
	gvt_vector_dynamic< T > r;
	gvt_for_each_unary_op(r, x, gvf_floor< T >());
	return r;
}
}