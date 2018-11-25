#pragma once
namespace gv
{
template < class T >
class gvt_line_segment
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector3< T > type_of_vector;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	type_of_vector start_p;
	type_of_vector end_p;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// function member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_line_segment(){};
	inline gvt_line_segment(const type_of_vector& endpoint1,
							const type_of_vector& endpoint2)
	{
		start_p = endpoint1;
		end_p = endpoint2;
	};
	inline type_of_value length() const
	{
		type_of_vector v = start_p - end_p;
		return v.length();
	}
	inline type_of_vector dir() const
	{
		type_of_vector v = end_p - start_p;
		v.normalize();
		return v;
	}
	inline type_of_vector delta() const
	{
		type_of_vector v = end_p - start_p;
		return v;
	}
	inline type_of_value length_squared() const
	{
		return (start_p - end_p).length_squared();
	}
	inline type_of_vector middle() const
	{
		return (start_p + end_p) / value_trait::two();
	}
};
typedef gvt_line_segment< gv_float > gv_line_segment;
typedef gvt_line_segment< gv_int > gv_line_segi;

template < class T >
class gvt_line_segment_2d
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_vector;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	type_of_vector start_p;
	type_of_vector end_p;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// function member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_line_segment_2d(){};
	inline gvt_line_segment_2d(const type_of_vector& endpoint1,
							   const type_of_vector& endpoint2)
	{
		start_p = endpoint1;
		end_p = endpoint2;
	};
	inline type_of_value length() const
	{
		type_of_vector v = start_p - end_p;
		return v.length();
	}
	inline type_of_vector dir() const
	{
		type_of_vector v = end_p - start_p;
		v.normalize();
		return v;
	}
	inline type_of_vector delta() const
	{
		type_of_vector v = end_p - start_p;
		return v;
	}
	inline type_of_value length_squared() const
	{
		return (start_p - end_p).length_squared();
	}
	inline type_of_vector middle() const
	{
		return (start_p + end_p) / value_trait::two();
	}
};
typedef gvt_line_segment_2d< gv_float > gv_line_segment_2d;
typedef gvt_line_segment_2d< gv_int > gv_line_seg_2di;
}