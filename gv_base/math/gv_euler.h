#pragma once
namespace gv
{
// http://www.euclideanspace.com/maths/geometry/rotations/euler/index.htm
template < class T >
class gvt_euler
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector;
	typedef gvt_vector_generic< type_of_value, 3 > type_of_generic;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	union {
		struct
		{
			type_of_value yaw;   // rotate by y
			type_of_value pitch; // rotate by z
			type_of_value roll;  // rotate by x
		};
		struct
		{
			type_of_vector v3;
		};
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_euler(){};
	inline gvt_euler(const type_of_vector& _v)
	{
		v3 = _v;
	};
	inline gvt_euler(type_of_value p, type_of_value y, type_of_value r)
		: pitch(p), yaw(y), roll(r){};
	inline gvt_euler(const gvt_euler& e)
	{
		(*this) = e;
	}
	inline void set(type_of_value p, type_of_value y, type_of_value r)
	{
		pitch = p, yaw = y, roll = r;
	}
	static const gvt_euler& get_zero_rotation()
	{
		static gvt_euler< type_of_value > z(0, 0, 0);
		return z;
	}
	inline bool operator==(const gvt_euler& e) const
	{
		return v3 == e.v3;
	}
	inline gvt_euler operator+(const gvt_euler& e) const
	{
		return v3 + e.v3;
	}
	inline bool operator<(const gvt_euler& e) const
	{
		return v3 < e.v3;
	}
	inline gvt_euler& operator=(const gvt_euler& e)
	{
		v3 = e.v3;
		return *this;
	}
	inline gvt_euler& operator=(const type_of_value& e)
	{
		v3 = e;
		return *this;
	}
	template < class T2 >
	inline gvt_euler& operator=(const gvt_euler< T2 >& _v)
	{
		v3 = _v.v3;
		return *this;
	}
};
typedef gvt_euler< gv_float > gv_euler;
typedef gvt_euler< gv_int > gv_euleri;
}