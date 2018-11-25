#pragma once
namespace gv
{

template < class type_of_stream, class T, gv_int n >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gvt_vector_generic< T, n >& v)
{
	const T* it = v.begin();
	s << "(";
	while (it != v.end())
	{
		s << *it;
		++it;
		if (it != v.end())
			s << ",";
	}
	s << ")";
	return s;
}

template < class type_of_stream, class T, gv_int n >
inline type_of_stream& operator>>(type_of_stream& s,
								  gvt_vector_generic< T, n >& v)
{
	T* it = v.begin();
	s >> "(";
	while (it != v.end())
	{
		s >> *it;
		++it;
		if (it != v.end())
			s >> ",";
	}
	s >> ")";
	return s;
}

template < class type_of_stream, class T, int n, int m >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gvt_matrix_generic< T, n, m >& matrix)
{
	s << " ( ";
	/*
  const T *  it=matrix.begin();
  while ( it !=matrix.end() )
  {
          s<< *it; ++it; s<<"\t ";
  }*/
	for (int i = 0; i < n; i++)
	{
		s << matrix.get_row(i);
	}
	s << " ) ";
	return s;
}

template < class type_of_stream, class T, int n, int m >
inline type_of_stream& operator>>(type_of_stream& s,
								  gvt_matrix_generic< T, n, m >& matrix)
{
	s >> "(";

	// while ( it !=matrix.end() )
	// T *  it=matrix.begin();
	//{
	//	s>> *it; ++it;
	//}

	for (int i = 0; i < n; i++)
	{
		s >> matrix.get_row(i);
	}

	s >> ")";
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_matrix44< T >& m)
{
	return s << m.matrix44;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_matrix44< T >& m)
{
	return s >> m.matrix44;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_matrix43< T >& m)
{
	return s << m.matrix43;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_matrix43< T >& m)
{
	return s >> m.matrix43;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_vector2< T >& v)
{
	return s << v.v;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_vector2< T >& v)
{
	s >> v.v;
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_vector3< T >& v)
{
	return s << v.v;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_vector3< T >& v)
{
	s >> v.v;
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_vector4< T >& v)
{
	return s << v.v4();
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_vector4< T >& v)
{
	s >> v.v4();
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_color< T >& v)
{
	return s << v.v;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_color< T >& v)
{
	s >> v.v;
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s, const gvt_euler< T >& v)
{
	return s << v.v3;
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_euler< T >& v)
{
	s >> v.v3;
	return s;
}

template < class type_of_stream, class T >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gvt_quaternion< T >& v)
{
	return s << v.v4();
}

template < class type_of_stream, class T >
inline type_of_stream& operator>>(type_of_stream& s, gvt_quaternion< T >& v)
{
	s >> v.v4();
	return s;
}

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s, gv_bool& b)
{
	s << *((gv_byte*)(&b));
	return s;
}

template < class type_of_stream >
inline type_of_stream& operator>>(type_of_stream& s, gv_bool& b)
{
	s >> *((gv_byte*)(&b));
	return s;
}
}