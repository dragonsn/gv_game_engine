#pragma once
namespace gv
{
// row major matrix 4x4
template < class T >
class gvt_matrix44
{
protected:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector3;
	typedef gvt_vector4< T > type_of_vector4;
	typedef gvt_matrix_generic< T, 4, 4 > type_of_generic_matrix;
	typedef gvt_matrix_generic< T, 3, 3 > type_of_rotate_matrix;
	typedef gvt_matrix44< T > type_of_matrix;
	typedef gvt_matrix43< T > type_of_matrix43;

public:
	union {
		struct
		{
			type_of_value m[4][4];
		};
		struct
		{
			type_of_value af[16];
		};
		struct
		{
			type_of_vector4 av4[4];
		};
		struct
		{
			type_of_value _m00, _m01, _m02, _m03;
			type_of_value _m10, _m11, _m12, _m13;
			type_of_value _m20, _m21, _m22, _m23;
			type_of_value _m30, _m31, _m32, _m33;
		};
		struct
		{
			type_of_vector4 axis_x;
			type_of_vector4 axis_y;
			type_of_vector4 axis_z;
			type_of_vector4 axis_w;
		};
		struct
		{
			type_of_generic_matrix matrix44;
		};
	};

public:
	gvt_matrix44()
	{
	}

	inline gvt_matrix44(type_of_value _00, type_of_value _01, type_of_value _02,
						type_of_value _03, type_of_value _10, type_of_value _11,
						type_of_value _12, type_of_value _13, type_of_value _20,
						type_of_value _21, type_of_value _22, type_of_value _23,
						type_of_value _30, type_of_value _31, type_of_value _32,
						type_of_value _33)

	{
		_m00 = _00, _m01 = _01, _m02 = _02, _m03 = _03, _m10 = _10, _m11 = _11,
		_m12 = _12, _m13 = _13, _m20 = _20, _m21 = _21, _m22 = _22, _m23 = _23,
		_m30 = _30, _m31 = _31, _m32 = _32, _m33 = _33;
	};

	inline gvt_matrix44(const type_of_matrix& tm)
	{
		*this = tm;
	};

	inline gvt_matrix44(const type_of_matrix43& tm)
	{
		*this = tm;
	};

	explicit gvt_matrix44(const type_of_vector4& v)
	{
		(*this) = get_identity();
		axis_w = v;
	};

	explicit gvt_matrix44(const type_of_value* p)
	{
		this->load(p);
	};

	inline gvt_matrix44(const type_of_vector4& A, const type_of_vector4& B,
						const type_of_vector4& C, const type_of_vector4& D)
	{
		this->axis_x = A;
		this->axis_y = B;
		this->axis_z = C;
		this->axis_w = D;
	};

	inline void fill(type_of_matrix43& m)
	{
		m.axis_x = axis_x.get_super();
		m.axis_y = axis_y.get_super();
		m.axis_z = axis_z.get_super();
		m.axis_w = axis_w.get_super();
	}

	inline void load(const type_of_value* p)
	{
		memcpy(this, p, sizeof(*this));
	}

	inline void save(type_of_value* p) const
	{
		memcpy(p, this, sizeof(*this));
	}

	static const type_of_matrix& get_identity()
	{
		static type_of_matrix matrix(
			value_trait::one(), value_trait::zero(), value_trait::zero(),
			value_trait::zero(), value_trait::zero(), value_trait::one(),
			value_trait::zero(), value_trait::zero(), value_trait::zero(),
			value_trait::zero(), value_trait::one(), value_trait::zero(),
			value_trait::zero(), value_trait::zero(), value_trait::zero(),
			value_trait::one());
		return matrix;
	};

	inline void set_identity()
	{
		(*this) = get_identity();
	}

	inline void get_row(int i, type_of_vector4& v)
	{
		GV_ASSERT_SLOW(i < 4);
		v = this->av3[i];
	};

	inline void set_row(int i, const type_of_vector4& v)
	{
		GV_ASSERT_SLOW(i < 4);
		this->av3[i] = v;
	};

	inline void get_column(int i, type_of_vector4& v)
	{
		GV_ASSERT_SLOW(i < 4);
		v.set(m[0][i], m[1][i], m[2][i], m[3][i]);
	};

	inline void set_column(int i, const type_of_vector3& v)
	{
		GV_ASSERT_SLOW(i < 4);
		m[0][i] = v.x, m[1][i] = v.y, m[2][i] = v.z;
	};

	inline void get_scale(type_of_vector3& v)
	{
		v.set(_m00, _m11, _m22);
	};

	inline void set_scale(const type_of_vector3& v)
	{
		this->set_identity();
		_m00 = v.x, _m11 = v.y, _m22 = v.z;
	};

	inline void get_trans(type_of_vector3& v) const
	{
		v = this->axis_w.get_super();
	};

	inline type_of_vector3 get_trans() const
	{
		return this->axis_w.get_super();
	};

	inline void set_trans(const type_of_vector3& v)
	{
		this->axis_w.get_super() = v;
	};

	inline void set_rotation(const type_of_matrix& m)
	{
		axis_x = m.axis_x;
		axis_y = m.axis_y;
		axis_z = m.axis_z;
	}

	inline void get_rotation(type_of_matrix& m)
	{
		m.set_identity();
		m.axis_x = axis_x;
		m.axis_y = axis_y;
		m.axis_z = axis_z;
	}
	inline void remove_trans()
	{
		this->axis_w.get_super() = 0;
	};

	inline void remove_rotate()
	{
		type_of_vector4 v = this->axis_w;
		this->set_identity();
		this->axis_w = v;
	};

	inline type_of_vector3 remove_real_scale()
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
		matrix44 = matrix44.transpose();
	};

	inline type_of_matrix get_transpose()
	{
		type_of_matrix tm;
		tm.matrix44 = matrix44.transpose();
		return tm;
	};

	inline bool operator==(const type_of_matrix& a) const
	{
		return this->matrix44 == a.matrix44;
	};

	inline bool operator<(const type_of_matrix& a) const
	{
		return this->matrix44 < a.matrix44;
	}

	inline type_of_matrix& operator=(const type_of_matrix& tm)
	{
		this->matrix44 = tm.matrix44;
		return *this;
	};

	template < class T2 >
	inline type_of_matrix& operator=(const gvt_matrix44< T2 >& _v)
	{
		int size = gvt_array_length(af);
		for (int i = 0; i < size; i++)
			this->af[i] = m.af[i];
		return *this;
	}

	inline type_of_matrix& operator=(const type_of_matrix43& tm)
	{
		this->set_identity();
		this->axis_x.get_super() = tm.axis_x;
		this->axis_y.get_super() = tm.axis_y;
		this->axis_z.get_super() = tm.axis_z;
		this->axis_w.get_super() = tm.axis_w;
		return *this;
	};

	inline type_of_matrix& operator+=(const type_of_matrix& tm)
	{
		this->matrix44 += tm.matrix44;
		return *this;
	};

	inline type_of_matrix& operator-=(const type_of_matrix& tm)
	{
		this->matrix44 -= tm.matrix44;
		return *this;
	};

	inline type_of_matrix& operator*=(const type_of_matrix& B)
	{
		type_of_matrix temp(*this);

		temp._m00 = _m00 * B._m00 + _m01 * B._m10 + _m02 * B._m20 + _m03 * B._m30;
		temp._m01 = _m00 * B._m01 + _m01 * B._m11 + _m02 * B._m21 + _m03 * B._m31;
		temp._m02 = _m00 * B._m02 + _m01 * B._m12 + _m02 * B._m22 + _m03 * B._m32;
		temp._m03 = _m00 * B._m03 + _m01 * B._m13 + _m02 * B._m23 + _m03 * B._m33;

		temp._m10 = _m10 * B._m00 + _m11 * B._m10 + _m12 * B._m20 + _m13 * B._m30;
		temp._m11 = _m10 * B._m01 + _m11 * B._m11 + _m12 * B._m21 + _m13 * B._m31;
		temp._m12 = _m10 * B._m02 + _m11 * B._m12 + _m12 * B._m22 + _m13 * B._m32;
		temp._m13 = _m10 * B._m03 + _m11 * B._m13 + _m12 * B._m23 + _m13 * B._m33;

		temp._m20 = _m20 * B._m00 + _m21 * B._m10 + _m22 * B._m20 + _m23 * B._m30;
		temp._m21 = _m20 * B._m01 + _m21 * B._m11 + _m22 * B._m21 + _m23 * B._m31;
		temp._m22 = _m20 * B._m02 + _m21 * B._m12 + _m22 * B._m22 + _m23 * B._m32;
		temp._m23 = _m20 * B._m03 + _m21 * B._m13 + _m22 * B._m23 + _m23 * B._m33;

		temp._m30 = _m30 * B._m00 + _m31 * B._m10 + _m32 * B._m20 + _m33 * B._m30;
		temp._m31 = _m30 * B._m01 + _m31 * B._m11 + _m32 * B._m21 + _m33 * B._m31;
		temp._m32 = _m30 * B._m02 + _m31 * B._m12 + _m32 * B._m22 + _m33 * B._m32;
		temp._m33 = _m30 * B._m03 + _m31 * B._m13 + _m32 * B._m23 + _m33 * B._m33;

		(*this) = temp;
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
		matrix44 = t;
		return *this;
	}
	inline type_of_matrix& operator+=(const T& t)
	{
		matrix44 += t;
		return *this;
	}
	inline type_of_matrix& operator-=(const T& t)
	{
		matrix44 -= t;
		return *this;
	}
	inline type_of_matrix& operator*=(const T& t)
	{
		matrix44 *= t;
		return *this;
	}
	inline type_of_matrix& operator/=(const T& t)
	{
		matrix44 /= t;
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

	inline type_of_matrix operator*(const type_of_matrix& v) const
	{
		type_of_matrix ret(*this);
		ret *= v;
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

	inline type_of_vector4 mul_by(const type_of_vector4& v) const
	{
		return type_of_vector4(v.x * _m00 + v.y * _m10 + v.z * _m20 + v.w * _m30,
							   v.x * _m01 + v.y * _m11 + v.z * _m21 + v.w * _m31,
							   v.x * _m02 + v.y * _m12 + v.z * _m22 + v.w * _m32,
							   v.x * _m03 + v.y * _m13 + v.z * _m23 + v.w * _m33);
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

	inline type_of_value determinant()
	{
		type_of_matrix temp;
		T det_0, det_1, det_2, det_3;

		temp._m00 = _m22 * _m33 - _m23 * _m32;
		temp._m01 = _m12 * _m33 - _m13 * _m32;
		temp._m02 = _m12 * _m23 - _m13 * _m22;
		temp._m10 = _m22 * _m33 - _m23 * _m32;
		temp._m11 = _m02 * _m33 - _m03 * _m32;
		temp._m12 = _m02 * _m23 - _m03 * _m22;
		temp._m20 = _m12 * _m33 - _m13 * _m32;
		temp._m21 = _m02 * _m33 - _m03 * _m32;
		temp._m22 = _m02 * _m13 - _m03 * _m12;
		temp._m30 = _m12 * _m23 - _m13 * _m22;
		temp._m31 = _m02 * _m23 - _m03 * _m22;
		temp._m32 = _m02 * _m13 - _m03 * _m12;

		det_0 = _m11 * temp._m00 - _m21 * temp._m01 + _m31 * temp._m02;
		det_1 = _m01 * temp._m10 - _m21 * temp._m11 + _m31 * temp._m12;
		det_2 = _m01 * temp._m20 - _m11 * temp._m21 + _m31 * temp._m22;
		det_3 = _m01 * temp._m30 - _m11 * temp._m31 + _m21 * temp._m32;

		T determinant = _m00 * det_0 - _m10 * det_1 + _m20 * det_2 - _m30 * det_3;
		return determinant;
	}

	inline type_of_matrix get_inverse()
	{
		type_of_matrix result, temp;
		T det_0, det_1, det_2, det_3;

		temp._m00 = _m22 * _m33 - _m23 * _m32;
		temp._m01 = _m12 * _m33 - _m13 * _m32;
		temp._m02 = _m12 * _m23 - _m13 * _m22;
		temp._m10 = _m22 * _m33 - _m23 * _m32;
		temp._m11 = _m02 * _m33 - _m03 * _m32;
		temp._m12 = _m02 * _m23 - _m03 * _m22;
		temp._m20 = _m12 * _m33 - _m13 * _m32;
		temp._m21 = _m02 * _m33 - _m03 * _m32;
		temp._m22 = _m02 * _m13 - _m03 * _m12;
		temp._m30 = _m12 * _m23 - _m13 * _m22;
		temp._m31 = _m02 * _m23 - _m03 * _m22;
		temp._m32 = _m02 * _m13 - _m03 * _m12;

		det_0 = _m11 * temp._m00 - _m21 * temp._m01 + _m31 * temp._m02;
		det_1 = _m01 * temp._m10 - _m21 * temp._m11 + _m31 * temp._m12;
		det_2 = _m01 * temp._m20 - _m11 * temp._m21 + _m31 * temp._m22;
		det_3 = _m01 * temp._m30 - _m11 * temp._m31 + _m21 * temp._m32;

		T determinant = _m00 * det_0 - _m10 * det_1 + _m20 * det_2 - _m30 * det_3;
		T r_determinant = value_trait::one() / determinant;

		result._m00 = r_determinant * det_0;
		result._m01 = -r_determinant * det_1;
		result._m02 = r_determinant * det_2;
		result._m03 = -r_determinant * det_3;
		result._m10 = -r_determinant *
					  (_m10 * temp._m00 - _m20 * temp._m01 + _m30 * temp._m02);
		result._m11 = r_determinant *
					  (_m00 * temp._m10 - _m20 * temp._m11 + _m30 * temp._m12);
		result._m12 = -r_determinant *
					  (_m00 * temp._m20 - _m10 * temp._m21 + _m30 * temp._m22);
		result._m13 = r_determinant *
					  (_m00 * temp._m30 - _m10 * temp._m31 + _m20 * temp._m32);
		result._m20 = r_determinant * (_m10 * (_m21 * _m33 - _m23 * _m31) -
									   _m20 * (_m11 * _m33 - _m13 * _m31) +
									   _m30 * (_m11 * _m23 - _m13 * _m21));
		result._m21 = -r_determinant * (_m00 * (_m21 * _m33 - _m23 * _m31) -
										_m20 * (_m01 * _m33 - _m03 * _m31) +
										_m30 * (_m01 * _m23 - _m03 * _m21));
		result._m22 = r_determinant * (_m00 * (_m11 * _m33 - _m13 * _m31) -
									   _m10 * (_m01 * _m33 - _m03 * _m31) +
									   _m30 * (_m01 * _m13 - _m03 * _m11));
		result._m23 = -r_determinant * (_m00 * (_m11 * _m23 - _m13 * _m21) -
										_m10 * (_m01 * _m23 - _m03 * _m21) +
										_m20 * (_m01 * _m13 - _m03 * _m11));
		result._m30 = -r_determinant * (_m10 * (_m21 * _m32 - _m22 * _m31) -
										_m20 * (_m11 * _m32 - _m12 * _m31) +
										_m30 * (_m11 * _m22 - _m12 * _m21));
		result._m31 = r_determinant * (_m00 * (_m21 * _m32 - _m22 * _m31) -
									   _m20 * (_m01 * _m32 - _m02 * _m31) +
									   _m30 * (_m01 * _m22 - _m02 * _m21));
		result._m32 = -r_determinant * (_m00 * (_m11 * _m32 - _m12 * _m31) -
										_m10 * (_m01 * _m32 - _m02 * _m31) +
										_m30 * (_m01 * _m12 - _m02 * _m11));
		result._m33 = r_determinant * (_m00 * (_m11 * _m22 - _m12 * _m21) -
									   _m10 * (_m01 * _m22 - _m02 * _m21) +
									   _m20 * (_m01 * _m12 - _m02 * _m11));
		return result;
	}

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
	// build other 3d tranformation matrix
	inline void set_camera_ortho_2d(type_of_value left, type_of_value right,
									type_of_value bottom, type_of_value top)
	{
		type_of_value tx = -(right + left) / (right - left);
		type_of_value ty = -(top + bottom) / (bottom - top);
		set_identity();
		_m00 = value_trait::two() / (right - left);
		_m11 = value_trait::two() / (bottom - top);
		_m22 = value_trait::one();
		_m30 = tx;
		_m31 = ty;
	};

	inline void set_camera_ortho_2d(type_of_value left, type_of_value right,
									type_of_value bottom, type_of_value top,
									type_of_value znear, type_of_value zfar)
	{
		set_camera_ortho_2d(left, right, top, bottom);
	};

	inline void set_camera_set_frustum(type_of_value left, type_of_value right,
									   type_of_value bottom, type_of_value top,
									   type_of_value near_z,
									   type_of_value far_z)
	{
		type_of_value A = (left + right) / (right - left);
		type_of_value B = (top + bottom) / (top - bottom);
		type_of_value C = -(far_z + near_z) / (far_z - near_z);
		type_of_value D = -(value_trait::two() * far_z * near_z) / (far_z - near_z);
		type_of_value E = value_trait::two() * near_z / (right - left);
		type_of_value F = value_trait::two() * near_z / (top - bottom);
		set_identity();
		_m00 = E;
		_m11 = F;
		_m22 = C;
		_m33 = value_trait::zero();
		_m20 = A;
		_m21 = B;
		_m32 = D;
		_m23 = -value_trait::one();
	};

	inline void set_camera_lookat(const type_of_vector3& pos,
								  const type_of_vector3& target,
								  const type_of_vector3& up)
	{
		type_of_vector3 z;
		z = pos - target;
		z.normalize();
		type_of_vector3 x = up.cross(z);
		x.normalize();
		type_of_vector3 y = z.cross(x);
		y.normalize();
		set_identity();
		axis_x.get_super() = x;
		axis_y.get_super() = y;
		axis_z.get_super() = z;
		axis_w.get_super() = pos;
		(*this) = this->get_inverse_noscale_3dtransform();
	};

	inline void set_camera_lookat_left_hand(const type_of_vector3& pos,
											const type_of_vector3& target,
											const type_of_vector3& up)
	{
		type_of_vector3 z;
		z = target - pos;
		z.normalize();
		type_of_vector3 x = up.cross(z);
		x.normalize();
		type_of_vector3 y = z.cross(x);
		y.normalize();
		set_identity();
		axis_x.get_super() = x;
		axis_y.get_super() = y;
		axis_z.get_super() = z;
		axis_w.get_super() = pos;
		(*this) = this->get_inverse_noscale_3dtransform();
	};

	inline void set_camera_set_perspective_left_hand(type_of_value fov,
													 type_of_value ratio_h_by_w,
													 type_of_value near_z,
													 type_of_value far_z)
	{
		type_of_value h =
			value_trait::one() /
			(type_of_value)gvt_tan(gvt_deg2rad(fov / value_trait::two()));
		type_of_value w = h / ratio_h_by_w;
		av4[0] = type_of_vector4(w, 0.0f, 0.0f, 0.0f);
		av4[1] = type_of_vector4(0.0f, h, 0.0f, 0.0f);
		av4[2] = type_of_vector4(0.0f, 0.0f, far_z / (-near_z + far_z), 1.0f);
		av4[3] = type_of_vector4(0.0f, 0.0f, -av4[2][2] * near_z, 0.0f);
	};

	inline void set_camera_set_perspective(type_of_value fov,
										   type_of_value ratio_h_by_w,
										   type_of_value near_z,
										   type_of_value far_z)
	{
		type_of_value h =
			1.0f / (type_of_value)gvt_tan(gvt_deg2rad(fov / value_trait::two()));
		type_of_value w = h / ratio_h_by_w;

		av4[0] = type_of_vector4(w, 0.0f, 0.0f, 0.0f);
		av4[1] = type_of_vector4(0.0f, h, 0.0f, 0.0f);
		av4[2] = type_of_vector4(0.0f, 0.0f, far_z / (near_z - far_z), -1.0f);
		av4[3] = type_of_vector4(0.0f, 0.0f, av4[2][2] * near_z, 0.0f);
	}

	inline void copy_to(type_of_matrix43& tm) const
	{
		tm.set_identity();
		tm.axis_x = this->axis_x.get_super();
		tm.axis_y = this->axis_y.get_super();
		tm.axis_z = this->axis_z.get_super();
		tm.axis_w = this->axis_w.get_super();
		return;
	};

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
};

typedef gvt_matrix44< gv_float > gv_matrix44;
}