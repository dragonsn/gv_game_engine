#pragma once
namespace gv
{

template < class T >
class gvt_sphere
{

	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_v2;
	typedef gvt_vector3< T > type_of_v3;
	typedef gvt_vector4< T > type_of_v4;
	typedef gvt_matrix43< T > type_of_matrix43;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:
	type_of_v3 pos;
	type_of_value r;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_sphere()
	{
		r = 0;
	};
	inline gvt_sphere(const gvt_sphere& s)
	{
		pos = s.pos;
		r = s.r;
	};
	inline gvt_sphere(const type_of_v3& _pos, type_of_value _r)
	{
		pos = _pos;
		r = _r;
	};

	inline void set(const type_of_v3& _pos, type_of_value _r)
	{
		pos = _pos;
		r = _r;
	};
	inline bool is_empty()
	{
		return value_trait::is_almost_zero(r);
	};

	inline void move(const type_of_v3& v)
	{
		this->pos += v;
	};

	inline void set_center(const type_of_v3& _p)
	{
		pos = _p;
	};
	inline type_of_v3 get_center() const
	{
		return pos;
	};
	inline void set_radius(type_of_value _r)
	{
		r = _r;
	};
	inline type_of_value get_radius() const
	{
		return r;
	};

	inline gvt_sphere transform(const type_of_matrix43& mat)
	{
		gvt_sphere s;
		s.pos = pos * mat;
		type_of_v3 v;
		mat.get_scale(v);
		s.r = r * v.v.max();
		return s;
	}

	bool operator==(const gvt_sphere& s)
	{
		return (r == s.r) && (pos == s.pos);
	}
};
typedef gvt_sphere< gv_float > gv_sphere;
}