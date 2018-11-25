#pragma once
namespace gv
{

template < class T >
class gvt_visitor_print : public std::unary_function< T, void >
{
public:
	inline gvt_visitor_print()
	{
	}
	inline void operator()(const T& value) const
	{
		print(value);
	}
};

//
// template<class T>
// class gvf_
}