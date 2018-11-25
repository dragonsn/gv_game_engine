
#pragma once
namespace gv
{
template < class type_of_matrix, class T >
inline void gvt_set_rotation_by_z(type_of_matrix& tm, T angle)
{
	tm.set_identity();
	T sina, cosa;
	sina = gvt_sin(angle);
	cosa = gvt_cos(angle);
	tm.axis_x.x = cosa;
	tm.axis_x.y = sina;
	tm.axis_y.x = -sina;
	tm.axis_y.y = cosa;
}

template < class type_of_matrix, class T >
inline void gvt_set_rotation_by_y(type_of_matrix& tm, T angle)
{
	tm.set_identity();
	T sina, cosa;
	sina = gvt_sin(angle);
	cosa = gvt_cos(angle);
	tm.axis_x.x = cosa;
	tm.axis_x.z = -sina;
	tm.axis_z.x = sina;
	tm.axis_z.z = cosa;
}

template < class type_of_matrix, class T >
inline void gvt_set_rotation_by_x(type_of_matrix& tm, T angle)
{
	tm.set_identity();
	T sina, cosa;
	sina = gvt_sin(angle);
	cosa = gvt_cos(angle);
	tm.axis_y.y = cosa;
	tm.axis_y.z = sina;
	tm.axis_z.y = -sina;
	tm.axis_z.z = cosa;
}

template < class type_of_matrix >
inline void gvt_yup_2_zup_matrix(type_of_matrix& tm)
{
	tm.set_identity();
	tm._m11 = type_of_matrix::value_trait::zero();
	tm._m12 = type_of_matrix::value_trait::one();
	tm._m22 = type_of_matrix::value_trait::zero();
	tm._m21 = -type_of_matrix::value_trait::one();
}

template < class type_of_matrix, class T >
inline void gvt_yup_2_zup_matrix(type_of_matrix& tm)
{
	tm.set_identity();
	tm._m11 = type_of_matrix::value_trait::zero();
	tm._m12 = -type_of_matrix::value_trait::one();
	tm._m22 = type_of_matrix::value_trait::zero();
	tm._m21 = type_of_matrix::value_trait::one();
}

// row major matrix 4x3
template < class T >
class gvt_matrix43
{
protected:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector3;
	typedef gvt_vector4< T > type_of_vector4;
	typedef gvt_matrix_generic< T, 4, 3 > type_of_generic_matrix;
	typedef gvt_matrix_generic< T, 3, 3 > type_of_rotate_matrix;
	typedef gvt_matrix43< T > type_of_matrix;

public:
	union {
		struct
		{
			type_of_value m[4][3];
		};
		struct
		{
			type_of_value af[12];
		};
		struct
		{
			type_of_vector3 av3[4];
		};
		struct
		{
			type_of_value _m00, _m01, _m02;
			type_of_value _m10, _m11, _m12;
			type_of_value _m20, _m21, _m22;
			type_of_value _m30, _m31, _m32;
		};
		struct
		{
			type_of_vector3 axis_x;
			type_of_vector3 axis_y;
			type_of_vector3 axis_z;
			type_of_vector3 axis_w;
		};
		struct
		{
			type_of_generic_matrix matrix43;
		};
	};

public:
	gvt_matrix43()
	{
	}

	inline gvt_matrix43(type_of_value _00, type_of_value _01, type_of_value _02,
						type_of_value _10, type_of_value _11, type_of_value _12,
						type_of_value _20, type_of_value _21, type_of_value _22,
						type_of_value _30, type_of_value _31, type_of_value _32)
	{
		_m00 = _00, _m01 = _01, _m02 = _02, _m10 = _10, _m11 = _11, _m12 = _12,
		_m20 = _20, _m21 = _21, _m22 = _22, _m30 = _30, _m31 = _31, _m32 = _32;
	};

	inline gvt_matrix43(const type_of_matrix& tm)
	{
		*this = tm;
	};

	explicit gvt_matrix43(const type_of_vector3& v)
	{
		(*this) = get_identity();
		axis_w = v;
	};

	inline gvt_matrix43(const type_of_vector3& A, const type_of_vector3& B,
						const type_of_vector3& C, const type_of_vector3& D)
	{
		this->axis_x = A;
		this->axis_y = B;
		this->axis_z = C;
		this->axis_w = D;
	};

	static const type_of_matrix& get_identity()
	{
		static type_of_matrix matrix(
			value_trait::one(), value_trait::zero(), value_trait::zero(),
			value_trait::zero(), value_trait::one(), value_trait::zero(),
			value_trait::zero(), value_trait::zero(), value_trait::one(),
			value_trait::zero(), value_trait::zero(), value_trait::zero());
		return matrix;
	};

	inline void set_identity()
	{
		(*this) = get_identity();
	}

	inline void get_row(int i, type_of_vector3& v)
	{
		GV_ASSERT_SLOW(i <= 3);
		v = this->av3[i];
	};

	inline void set_row(int i, const type_of_vector3& v)
	{
		GV_ASSERT_SLOW(i <= 3);
		this->av3[i] = v;
	};

	inline void get_column(int i, type_of_vector3& v)
	{
		GV_ASSERT_SLOW(i < 3);
		v.set(m[0][i], m[1][i], m[2][i]);
	};

	inline void set_column(int i, const type_of_vector3& v)
	{
		GV_ASSERT_SLOW(i < 3);
		m[0][i] = v.x, m[1][i] = v.y, m[2][i] = v.z;
	};

	inline void get_scale(type_of_vector3& v) const
	{
		v.set(this->av3[0].length(), this->av3[1].length(), this->av3[2].length());
	};

	inline void set_scale(const type_of_vector3& v)
	{
		this->set_identity();
		_m00 = v.x, _m11 = v.y, _m22 = v.z;
	};

	inline void get_trans(type_of_vector3& v)
	{
		v = this->axis_w;
	};

	inline type_of_vector3 get_trans()
	{
		return this->axis_w;
	};

	inline void set_trans(const type_of_vector3& v)
	{
		this->axis_w = v;
	};

	inline void set_rotation(const type_of_matrix& m)
	{
		for (int i = 0; i < 9; i++)
			this->af[i] = m.af[i];
	}

	inline void get_rotation(type_of_matrix& m)
	{
		for (int i = 0; i < 9; i++)
			m.af[i] = this->af[i];
		m.af[9] = m.af[10] = m.af[11] = 0.0f;
	}
	inline void remove_trans()
	{
		this->axis_w = 0;
	};

	inline void remove_rotate()
	{
		type_of_vector3 v = this->axis_w;
		this->set_identity();
		this->axis_w = v;
	};

	inline void remove_real_scale()
	{
		type_of_vector3 v3;
		v3.x = axis_x.normalize();
		v3.y = axis_y.normalize();
		v3.z = axis_z.normalize();
		return v3;
	};

	inline void move(const type_of_vector3& v)
	{
		this->axis_w += v;
	};

	inline void scale_by(const type_of_vector3& v)
	{
		type_of_matrix tm;
		tm.set_identity();
		tm.set_scale(v);
		(*this) = tm * (*this);
	};

	inline void transpose_self()
	{
		gvt_swap(_m01, _m10);
		gvt_swap(_m02, _m20);
		gvt_swap(_m12, _m21);
	};

	inline type_of_value determinant()
	{
		return ((_m00 * (_m11 * _m22 - _m21 * _m12)) -
				(_m01 * (_m10 * _m22 - _m20 * _m12)) +
				(_m02 * (_m10 * _m21 - _m20 * _m11)));
	}

	inline bool operator==(const type_of_matrix& a) const
	{
		return this->matrix43 == a.matrix43;
	};

	inline bool operator<(const type_of_matrix& a) const
	{
		return this->matrix43 < a.matrix43;
	}

	inline type_of_matrix& operator=(const type_of_matrix& tm)
	{
		this->matrix43 = tm.matrix43;
		return *this;
	};

	template < class T2 >
	inline type_of_matrix& operator=(const gvt_matrix43< T2 >& _v)
	{
		int size = gvt_array_length(af);
		for (int i = 0; i < size; i++)
			this->af[i] = m.af[i];
		return *this;
	}

	inline type_of_matrix& operator+=(const type_of_matrix& tm)
	{
		this->matrix43 += tm.matrix43;
		return *this;
	};

	inline type_of_matrix& operator-=(const type_of_matrix& tm)
	{
		this->matrix43 -= tm.matrix43;
		return *this;
	};

	inline type_of_matrix& operator*=(const type_of_matrix& A)
	{
		type_of_matrix mtemp;
		// mtemp._mij= _mi0 * A._m0j +_mi1 * A._m1j  +_mi2 * A._m2j;
		mtemp._m00 = _m00 * A._m00 + _m01 * A._m10 + _m02 * A._m20;
		mtemp._m01 = _m00 * A._m01 + _m01 * A._m11 + _m02 * A._m21;
		mtemp._m02 = _m00 * A._m02 + _m01 * A._m12 + _m02 * A._m22;

		mtemp._m10 = _m10 * A._m00 + _m11 * A._m10 + _m12 * A._m20;
		mtemp._m11 = _m10 * A._m01 + _m11 * A._m11 + _m12 * A._m21;
		mtemp._m12 = _m10 * A._m02 + _m11 * A._m12 + _m12 * A._m22;

		mtemp._m20 = _m20 * A._m00 + _m21 * A._m10 + _m22 * A._m20;
		mtemp._m21 = _m20 * A._m01 + _m21 * A._m11 + _m22 * A._m21;
		mtemp._m22 = _m20 * A._m02 + _m21 * A._m12 + _m22 * A._m22;

		mtemp._m30 = _m30 * A._m00 + _m31 * A._m10 + _m32 * A._m20 + A._m30;
		mtemp._m31 = _m30 * A._m01 + _m31 * A._m11 + _m32 * A._m21 + A._m31;
		mtemp._m32 = _m30 * A._m02 + _m31 * A._m12 + _m32 * A._m22 + A._m32;

		(*this) = mtemp;
		return *this;
	};

	inline type_of_matrix& operator/=(const type_of_matrix& A)
	{
		type_of_matrix tm(A);
		tm.inverse();
		*this *= tm;
		return *this;
	}

	inline type_of_matrix& operator=(const T& t)
	{
		matrix43 = t;
		return *this;
	}
	inline type_of_matrix& operator+=(const T& t)
	{
		matrix43 += t;
		return *this;
	}
	inline type_of_matrix& operator-=(const T& t)
	{
		matrix43 -= t;
		return *this;
	}
	inline type_of_matrix& operator*=(const T& t)
	{
		matrix43 *= t;
		return *this;
	}
	inline type_of_matrix& operator/=(const T& t)
	{
		matrix43 /= t;
		return *this;
	}

	inline type_of_value& operator()(int row, int column)
	{
		return this->m[row][column];
	};

	inline type_of_value operator()(int row, int column) const
	{
		return m[row][column];
	};

	inline type_of_matrix operator+(const type_of_matrix& v) const
	{
		type_of_matrix ret(*this);
		ret += v;
		return ret;
	}

	inline type_of_matrix operator-(const type_of_matrix& v) const
	{
		type_of_matrix ret(*this);
		ret -= v;
		return ret;
	}

	inline type_of_matrix operator/(const type_of_matrix& v) const
	{
		type_of_matrix ret(*this);
		ret /= v;
		return ret;
	}

	inline type_of_matrix operator*(const type_of_matrix& A) const
	{
		// type_of_matrix  ret(*this); ret*=v; return ret;
		type_of_matrix ret;
		// mtemp._mij  = _mi0 * A._m0j +_mi1 * A._m1j  +_mi2 * A._m2j;
		ret._m00 = _m00 * A._m00 + _m01 * A._m10 + _m02 * A._m20;
		ret._m01 = _m00 * A._m01 + _m01 * A._m11 + _m02 * A._m21;
		ret._m02 = _m00 * A._m02 + _m01 * A._m12 + _m02 * A._m22;

		ret._m10 = _m10 * A._m00 + _m11 * A._m10 + _m12 * A._m20;
		ret._m11 = _m10 * A._m01 + _m11 * A._m11 + _m12 * A._m21;
		ret._m12 = _m10 * A._m02 + _m11 * A._m12 + _m12 * A._m22;

		ret._m20 = _m20 * A._m00 + _m21 * A._m10 + _m22 * A._m20;
		ret._m21 = _m20 * A._m01 + _m21 * A._m11 + _m22 * A._m21;
		ret._m22 = _m20 * A._m02 + _m21 * A._m12 + _m22 * A._m22;

		ret._m30 = _m30 * A._m00 + _m31 * A._m10 + _m32 * A._m20 + A._m30;
		ret._m31 = _m30 * A._m01 + _m31 * A._m11 + _m32 * A._m21 + A._m31;
		ret._m32 = _m30 * A._m02 + _m31 * A._m12 + _m32 * A._m22 + A._m32;

		return ret;
	}

	inline type_of_matrix operator-(T t) const
	{
		type_of_matrix ret(*this);
		ret -= t;
		return ret;
	}

	inline type_of_matrix operator+(T t) const
	{
		type_of_matrix ret(*this);
		ret += t;
		return ret;
	}

	inline type_of_matrix operator/(T t) const
	{
		type_of_matrix ret(*this);
		ret /= t;
		return ret;
	}

	inline type_of_matrix operator*(T t) const
	{
		type_of_matrix ret(*this);
		ret *= t;
		return ret;
	}

	inline bool operator!=(const type_of_matrix& v) const
	{
		return !((*this) == v);
	}

	inline bool operator>=(const type_of_matrix& v) const
	{
		return !((*this) < v);
	}

	inline bool operator<=(const type_of_matrix& v) const
	{
		return (*this) < v || (*this) == v;
	}

	inline bool operator>(const type_of_matrix& v) const
	{
		return !((*this) <= v);
	}

	inline type_of_vector3 mul_by(const type_of_vector3& v) const
	{
		return type_of_vector3(v.x * _m00 + v.y * _m10 + v.z * _m20 + _m30,
							   v.x * _m01 + v.y * _m11 + v.z * _m21 + _m31,
							   v.x * _m02 + v.y * _m12 + v.z * _m22 + _m32);
	};

	inline type_of_vector3 rotate(const type_of_vector3& v) const
	{
		return type_of_vector3(v.x * _m00 + v.y * _m10 + v.z * _m20,
							   v.x * _m01 + v.y * _m11 + v.z * _m21,
							   v.x * _m02 + v.y * _m12 + v.z * _m22);
	};

	static inline type_of_matrix get_rotation_by_z(T angle)
	{
		type_of_matrix tm;
		gvt_set_rotation_by_z(tm, angle);
		return tm;
	}

	static inline type_of_matrix get_rotation_by_y(T angle)
	{
		type_of_matrix tm;
		gvt_set_rotation_by_y(tm, angle);
		return tm;
	}

	static inline type_of_matrix get_rotation_by_x(T angle)
	{
		type_of_matrix tm;
		gvt_set_rotation_by_x(tm, angle);
		return tm;
	}

	inline bool is_uniform_ortho(void)
	{
		type_of_vector3 xy;
		T l;
		l = this->axis_x.length();
		if (!value_trait::is_almost_equal(l, value_trait::one()))
			return false;
		l = this->axis_y.length();
		if (!value_trait::is_almost_equal(l, value_trait::one()))
			return false;
		l = this->axis_z.length();
		if (!value_trait::is_almost_equal(l, value_trait::one()))
			return false;

		xy = axis_x.cross(axis_y) - axis_z;
		if (xy.is_almost_zero())
			return 1;
		return 0;
	};

	inline type_of_matrix get_inverse_with_scale_3dtransform()
	{
		type_of_matrix matrix(*this);
		type_of_vector3 multiplier;
		multiplier.x = matrix.axis_x.normalize();
		multiplier.y = matrix.axis_y.normalize();
		multiplier.z = matrix.axis_z.normalize();
		matrix = matrix.get_inverse_noscale_3dtransform();
		multiplier.v.reciprocal();
		type_of_matrix scale;
		scale.set_scale(multiplier);
		// matrix.axis_x*=multiplier.x;
		// matrix.axis_y*=multiplier.y;
		// matrix.axis_z*=multiplier.z;
		return matrix * scale;
	}

	inline type_of_matrix get_inverse_noscale_3dtransform(void)
	{
		type_of_matrix matrix(*this);

		matrix._m30 = (-_m00 * _m30 - _m01 * _m31 - _m02 * _m32);
		matrix._m31 = (-_m10 * _m30 - _m11 * _m31 - _m12 * _m32);
		matrix._m32 = (-_m20 * _m30 - _m21 * _m31 - _m22 * _m32);

		matrix._m10 = _m01;
		matrix._m01 = _m10;
		matrix._m20 = _m02;
		matrix._m02 = _m20;
		matrix._m21 = _m12;
		matrix._m12 = _m21;
		return matrix;
	}

	const type_of_vector3& get_axis_x() const
	{
		return axis_x;
	}
	const type_of_vector3& get_axis_y() const
	{
		return axis_y;
	}
	const type_of_vector3& get_axis_z() const
	{
		return axis_z;
	}
	const type_of_vector3& get_axis_w() const
	{
		return axis_w;
	}
	type_of_vector3& get_axis_x()
	{
		return axis_x;
	}
	type_of_vector3& get_axis_y()
	{
		return axis_y;
	}
	type_of_vector3& get_axis_z()
	{
		return axis_z;
	}
	type_of_vector3& get_axis_w()
	{
		return axis_w;
	}
}; // matrix

typedef gvt_matrix43< gv_float > gv_matrix43;
typedef gvt_matrix43< gv_int > gv_matrix43i;
}