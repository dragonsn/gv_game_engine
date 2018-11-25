#pragma once

namespace gv
{
//------------------------------generic row major matrix for any type, for any
//dimention
template < class T, int num_row, int num_col >
class gvt_matrix_generic
{
protected:
	struct construct_flag_identity
	{
	};

public:
	//------------------------------data----------------------
	T m[num_row][num_col];

public:
	//------------------------------functions----------------------
	static const int num_element = num_row * num_col;
	typedef gvt_vector_generic< T, num_col > type_of_row_vector;
	typedef gvt_vector_generic< T, num_row > type_of_col_vector;
	typedef gvt_matrix_generic< T, num_row, num_col > type_of_matrix;
	typedef T type_of_value;
	typedef gvt_value< T > value_trait;
	typedef const T* const_iterator;
	typedef T* iterator;

	inline iterator begin()
	{
		return &m[0][0];
	}
	inline const_iterator begin() const
	{
		return &m[0][0];
	}
	inline iterator end()
	{
		return begin() + num_element;
	}
	inline const_iterator end() const
	{
		return begin() + num_element;
	}

	inline gvt_matrix_generic()
	{
		GV_STATIC_ASSERT(sizeof(type_of_matrix) == num_element * sizeof(T));
	}
	inline gvt_matrix_generic(const type_of_matrix& a)
	{
		*this = a;
	}
	explicit gvt_matrix_generic(const type_of_row_vector& v)
	{
		for (int i = 0; i < num_row; i++)
			get_row(i) = v;
	}
	explicit gvt_matrix_generic(const type_of_value& a)
	{
		*this = a;
	}
	inline gvt_matrix_generic(const T (&a)[num_element])
	{
		const T* p = a;
		iterator i = begin();
		while (i != end())
			*i++ = *p++;
	}
	inline gvt_matrix_generic(construct_flag_identity)
	{
		for (int i = 0; i < num_row; i++)
			for (int j = 0; j < num_col; j++)
				m[i][j] = i == j ? value_trait::one() : value_trait::zero();
	}

	inline T& operator()(int i, int j)
	{
		GV_ASSERT_SLOW(i >= 0 && i < num_row && j >= 0 && j < num_col);
		return m[i][j];
	}
	inline const T& operator()(int i, int j) const
	{
		GV_ASSERT_SLOW(i >= 0 && i < num_row && j >= 0 && j < num_col);
		return m[i][j];
	}
	type_of_row_vector& get_row(int i)
	{
		GV_ASSERT(i < num_row);
		return *(type_of_row_vector*)(&m[i][0]);
	}
	const type_of_row_vector& get_row(int i) const
	{
		GV_ASSERT(i < num_row);
		return *(type_of_row_vector*)(&m[i][0]);
	}
	inline type_of_matrix operator-() const
	{
		type_of_matrix result;
		gvt_for_each_unary_op(result, *this, std::negate< type_of_value >());
		return result;
	}
	inline type_of_matrix& operator=(const type_of_matrix& _m)
	{
		gvt_for_each_copy(*this, _m);
		return *this;
	}
	inline type_of_matrix& operator+=(const type_of_matrix& _m)
	{
		gvt_for_each_binary_op(*this, *this, _m, gvf_add< type_of_value >());
		return *this;
	}
	inline type_of_matrix& operator-=(const type_of_matrix& _m)
	{
		gvt_for_each_binary_op(*this, *this, _m, gvf_sub< type_of_value >());
		return *this;
	}
	inline type_of_matrix& operator*=(const type_of_matrix& _m)
	{
		GV_STATIC_ASSERT(num_row == num_col);
		(*this) = mul(_m);
		return *this;
	}
	inline type_of_matrix& operator/=(const type_of_matrix& _m)
	{
		GV_STATIC_ASSERT(num_row == num_col);
		*this *= _m.inverse();
		return *this;
	}
	inline type_of_matrix& operator=(const T& t)
	{
		gvt_for_each(*this, gvf_assign< T >(t));
		return *this;
	}
	inline type_of_matrix& operator+=(const T& t)
	{
		gvt_for_each(*this, gvf_add_assign< T >(t));
		return *this;
	}
	inline type_of_matrix& operator-=(const T& t)
	{
		gvt_for_each(*this, gvf_sub_assign< T >(t));
		return *this;
	}
	inline type_of_matrix& operator*=(const T& t)
	{
		gvt_for_each(*this, gvf_mul_assign< T >(t));
		return *this;
	}
	inline type_of_matrix& operator/=(const T& t)
	{
		gvt_for_each(*this, gvf_div_assign< T >(t));
		return *this;
	}
	inline bool operator==(const type_of_matrix& a) const
	{
		return gvt_is_almost_equal_container(*this, a);
	}
	inline bool operator<(const type_of_matrix& a) const
	{
		return gvt_for_each_is_less(*this, a);
	}
	//====================================derivated function
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
	//====================================
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
	//====================================
	inline const type_of_matrix& identity()
	{
		*this = get_identity();
		return *this;
	}

	static const type_of_matrix& get_identity()
	{
		static type_of_matrix mat;
		static bool inited = false;
		if (!inited)
		{
			inited = true;
			mat = type_of_matrix(construct_flag_identity());
		}
		return mat;
	}
	// http://code.wikia.com/wiki/Matrix_multiplication
	template < int num_col2 >
	inline gvt_matrix_generic< T, num_row, num_col2 >
	mul(const gvt_matrix_generic< T, num_col, num_col2 >& b) const
	{
		gvt_matrix_generic< T, num_row, num_col2 > result;
		for (int i = 0; i < num_row; i++)
		{
			for (int j = 0; j < num_col2; j++)
			{
				result(i, j) = (T)0;
				for (int k = 0; k < num_col; k++) // OR k<b.GetLength(0)
					result(i, j) = result(i, j) + (*this)(i, k) * b(k, j);
			}
		}
		return result;
	}
	inline type_of_row_vector mul_by(const type_of_col_vector& b) const
	{
		gvt_matrix_generic< T, 1, num_row > mat_b(b);
		gvt_matrix_generic< T, 1, num_col > mat_r = mat_b.mul(*this);
		return type_of_row_vector(mat_r.m[0]);
	}
	inline T sum(void) const
	{
		T s = value_trait::zero();
		for (int i = 0; i < num_row; ++i)
			for (int j = 0; j < num_col; ++j)
				s += m[i][j];
		return s;
	}

	inline T abs_sum(void) const
	{
		T s = value_trait::zero();
		for (int i = 0; i < num_row; ++i)
			for (int j = 0; j < num_col; ++j)
				s += gvt_abs(m[i][j]);
		return s;
	}

	// http://www.euclideanspace.com/maths/algebra/matrix/resources/code/index.htm
	inline void transpose_self(void)
	{
		GV_STATIC_ASSERT(num_row == num_col);
		for (int i = 0; i < num_row; ++i)
		{
			for (int j = i + 1; j < num_col; ++j)
				gvt_swap(m[i][j], m[j][i]);
		}
	}
	inline gvt_matrix_generic< T, num_col, num_row > transpose(void)
	{
		gvt_matrix_generic< T, num_col, num_row > result(*this);
		result.transpose_self();
		return result;
	}
	inline type_of_matrix inverse()
	{
		type_of_matrix b;
		for (int i = 0; i < num_row; ++i)
			for (int j = 0; j < num_col; ++j)
			{
				int sgn = ((i + j) % 2) ? -1 : 1;
				b(i, j) = sgn * matrix_cofactor(i, j).determinant();
			}
		b.transpose_self();
		b /= determinant();
		return b;
	}
	inline gvt_matrix_generic< T, num_row - 1, num_col - 1 >
	matrix_cofactor(int aI, int aJ) const
	{
		gvt_matrix_generic< T, num_row - 1, num_col - 1 > mat;
		for (int i = 0, k = 0; i < num_row; ++i)
		{
			if (i != aI)
			{
				for (int j = 0, l = 0; j < num_col; ++j)
				{
					if (j != aJ)
					{
						mat(k, l) = m[i][j];
						++l;
					}
				}
				++k;
			}
		} //-------i
		return mat;
	}
	inline T determinant(void) const
	{
		T d = 0;
		for (int i = 0; i < num_row; ++i)
		{
			int sgn = (i % 2) ? -1 : 1;
			gvt_matrix_generic< T, num_row - 1, num_col - 1 > cf =
				matrix_cofactor(i, 0);
			d += sgn * m[i][0] * cf.determinant();
		}
		return d;
	}
	gvt_matrix_generic< T, num_row - 1, num_col >& get_sub_matrix()
	{
		return *((gvt_matrix_generic< T, num_row - 1, num_col >*)this);
	}
	const gvt_matrix_generic< T, num_row - 1, num_col >& get_sub_matrix() const
	{
		return *((gvt_matrix_generic< T, num_row - 1, num_col >*)this);
	}
	void randomize(const T& low = value_trait::zero(),
				   const T& high = value_trait::one())
	{
		gvt_for_each(*this, gvf_randomize< T >(low, high));
	}
	void reciprocal()
	{
		gvt_for_each(*this, gvf_reciprocal< T >());
	}

}; //===gvt_matrix_generic

template < class T >
class gvt_matrix_generic< T, 0, 0 >
{
public:
	inline T determinant(void) const
	{
		return (T)1;
	}
	inline T& operator()(int i, int j)
	{
		static T t = 0;
		return t;
	}
};

template < class T, int num_col >
class gvt_matrix_generic< T, 0, num_col >
{
public:
	inline T determinant(void) const
	{
		return (T)1;
	}
	inline T& operator()(int i, int j)
	{
		static T t = 0;
		return t;
	}
};

template < class T, int num_row >
class gvt_matrix_generic< T, num_row, 0 >
{
public:
	inline T determinant(void) const
	{
		return (T)1;
	}
	inline T& operator()(int i, int j)
	{
		static T t = 0;
		return t;
	}
};

} /// gv