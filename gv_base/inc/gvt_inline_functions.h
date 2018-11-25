#pragma once

template < class T >
inline T* gvt_construct(T* p)
{
	detail::do_construct(p, boost::has_trivial_constructor< T >());
	return p;
}

template < class T >
inline T* gvt_construct(T* p, const T& t)
{
	detail::do_construct_copy(p, t, boost::has_trivial_assign< T >());
	return p;
}

template < typename I1, typename I2 >
inline I2 gvt_copy(I1 first, I1 last, I2 out)
{
	typedef typename std::iterator_traits< I1 >::value_type value_type;
	return detail::copy_imp(first, last, out,
							boost::has_trivial_assign< value_type >());
}

template < class T >
inline void gvt_destroy_array(T* p1, T* p2)
{
	detail::do_destroy_array(p1, p2, ::boost::has_trivial_destructor< T >());
}

template < class T >
inline void gvt_construct_array(T* p1, T* p2)
{
	detail::do_construct_array(p1, p2, ::boost::has_trivial_constructor< T >());
}

template < class T >
inline gv_int gvt_compare_array(T* p1, T* p2, gv_int size)
{
	for (int i = 0; i < size; i++)
	{
		if (*p1 > *p2)
			return 1;
		if (*p1 < *p2)
			return -1;
		++p1;
		++p2;
	}
	return 0;
}

template < class T >
inline bool gvt_between(const T& value, const T& min, const T& max)
{
	return (value <= max && value >= min);
}

template < class T >
inline void gvt_destroy(T* p)
{
	detail::do_destroy_ptr(p, ::boost::has_trivial_destructor< T >());
}

template < class T >
inline T* gvt_strcpy(T* target, const T* source)
{
	GV_ASSERT(0);
};

template <>
inline char* gvt_strcpy< char >(char* target, const char* source)
{
	return ::strcpy(target, source);
};

template <>
inline wchar_t* gvt_strcpy< wchar_t >(wchar_t* target, const wchar_t* source)
{
	return ::wcscpy(target, source);
};

template < class T >
inline T* gvt_strcat(T* target, const T* source)
{
	GV_ASSERT(0);
};

template <>
inline char* gvt_strcat(char* target, const char* source)
{
	return ::strcat(target, source);
	;
};

template <>
inline wchar_t* gvt_strcat(wchar_t* target, const wchar_t* source)
{
	return ::wcscat(target, source);
	;
};

template < class T >
inline int gvt_strcmp(const T*, const T*)
{
	GV_ASSERT(0);
};

template <>
inline int gvt_strcmp< char >(const char* p1, const char* p2)
{
	if (!p1 && !p2)
		return 0;
	if (!p1 || !p2)
		return 1;
	return ::strcmp(p1, p2);
	;
};

template <>
inline int gvt_strcmp< wchar_t >(const wchar_t* p1, const wchar_t* p2)
{
	if (!p1 && !p2)
		return 0;
	if (!p1 || !p2)
		return 1;
	return ::wcscmp(p1, p2);
	;
};

template < class T >
inline int gvt_strlen(const T*)
{
	GV_ASSERT(0);
};

template <>
inline int gvt_strlen< char >(const char* p)
{
	if (!p)
		return 0;
	return (int)::strlen(p);
	;
};

template <>
inline int gvt_strlen< wchar_t >(const wchar_t* p)
{
	if (!p)
		return 0;
	return (int)::wcslen(p);
};

template < class T >
inline const T* gvt_strstr(const T* s, const T* substring)
{
	GV_ASSERT(0);
};

template <>
inline const char* gvt_strstr(const char* s, const char* substring)
{
	return ::strstr(s, substring);
};

template <>
inline const wchar_t* gvt_strstr(const wchar_t* s, const wchar_t* substring)
{
	return ::wcsstr(s, substring);
};

template < class T >
inline int gvt_is_space(const T& s)
{
	GV_ASSERT(0);
};

template <>
inline int gvt_is_space< char >(const char& s)
{
	return ::isspace((unsigned char)s);
};

template <>
inline int gvt_is_space< wchar_t >(const wchar_t& s)
{
	return ::iswspace((int)s);
	;
};

//////////////////////////////////////////////////////////////////////////
template < class T >
inline int gvt_is_num(const T& s)
{
	return ::isdigit((unsigned char)s);
};

template <>
inline int gvt_is_num< char >(const char& s)
{
	return ::isdigit((unsigned char)s);
};

template <>
inline int gvt_is_num< wchar_t >(const wchar_t& s)
{
	return ::iswdigit((int)s);
	;
};

//////////////////////////////////////////////////////////////////////////
template < class T >
inline int gvt_is_letter(const T& s)
{
	return ::isalpha((unsigned char)s);
};

template <>
inline int gvt_is_letter< char >(const char& s)
{
	return ::isalpha((unsigned char)s);
};

template <>
inline int gvt_is_letter< wchar_t >(const wchar_t& s)
{
	return ::iswalpha((int)s);
	;
};

//////////////////////////////////////////////////////////////////////////
template < class T >
inline int gvt_is_id_content(const T& s)
{
	return gvt_is_letter(s) || gvt_is_num(s) || s == '_';
};
//////////////////////////////////////////////////////////////////////////
template < class T >
inline gv_uint gvt_hash(const T& t)
{
	return (gv_uint)t;
}

template < class T >
inline gv_uint gvt_hash(T* t)
{
	return (gv_uint)((gv_int_ptr)t) / 13;
}

template < class T, int i >
inline int gvt_array_length(const T (&a)[i])
{
	return sizeof(a) / sizeof(a[0]);
}

template < class _Ty >
inline void
gvt_swap(_Ty& _Left,
		 _Ty& _Right)
{ // exchange values stored at _Left and _Right
	if (&_Left != &_Right)
	{ // different, worth swapping
		_Ty _Tmp = _Left;
		_Left = _Right;
		_Right = _Tmp;
	}
}

template < int size_1, int size_2 >
inline bool gvt_get_symbol_from_offset(void* offset, char (&func_name)[size_1],
									   char (&file_name)[size_2],
									   int& line_no)
{
	return gv_get_symbol_from_offset(offset, func_name, size_1, file_name, size_2,
									 line_no);
}

template < class T >
inline void gvt_safe_delete(T& t)
{
	if (t)
	{
		delete t;
		t = NULL;
	}
}

//------------------------------FOR EACHS--------------------------------

template < class T1, class T2 >
inline bool gvt_for_each_is_equal(const T1& c1, const T2& c2)
{
	typename T1::const_iterator i1 = c1.begin();
	typename T2::const_iterator i2 = c2.begin();
	while (i1 != c1.end() && i2 != c2.end())
	{
		if (*i1 != *i2)
			return false;
		i1++;
		i2++;
	}
	if (i1 == c1.end() && i2 == c2.end())
		return true;
	return false;
}

template < int array_size, class T >
inline bool gvt_for_each_is_equal(const T (&a1)[array_size],
								  const T (&a2)[array_size],
								  int count = array_size)
{
	for (int i = 0; i < count; i++)
	{
		if (a1[i] != a2[i])
			return false;
	}
	return true;
	;
}

template < class T1, class T2 >
inline bool gvt_for_each_is_less(const T1& c1, const T2& c2)
{
	typename T1::const_iterator i1 = c1.begin();
	typename T2::const_iterator i2 = c2.begin();
	while (i1 != c1.end() && i2 != c2.end())
	{
		if (*i1 < *i2)
			return true;
		if (*i1 == *i2)
		{
			i1++;
			i2++;
		}
		else
			return false;
	}
	if (i1 == c1.end() && i2 == c2.end())
		return false;
	if (i1 == c1.end())
		return true;
	return false;
}

template < class _InIt, class _Fn1 >
inline _Fn1 gvt_for_each(_InIt _First, _InIt _Last,
						 _Fn1 _Func)
{ // perform function for each element
	for (; _First != _Last; ++_First)
		_Func(*_First);
	return (_Func);
}

template < class _Container, class _Fn1 >
inline _Fn1 gvt_for_each(_Container& c1, _Fn1 _Func)
{
	typename _Container::iterator _First = c1.begin();
	typename _Container::iterator _Last = c1.end();
	for (; _First != _Last; ++_First)
		_Func(*_First);
	return (_Func);
}

template < class _Container >
inline void gvt_for_each_delete(_Container& c1)
{
	typename _Container::iterator _First = c1.begin();
	typename _Container::iterator _Last = c1.end();
	for (; _First != _Last; ++_First)
		gvt_safe_delete(*_First);
}

template < class _Container, class _Fn1 >
inline bool gvt_for_each_until_false(const _Container& c1, const _Fn1& _Func)
{
	typename _Container::iterator _First = c1.begin();
	typename _Container::iterator _Last = c1.end();
	for (; _First != _Last; ++_First)
	{
		if (!_Func(*_First))
			break;
	}
	return (_Func);
}

template < typename _Container >
inline void gvt_for_each_copy(_Container& target, const _Container& source)
{
	typename _Container::const_iterator _First = source.begin();
	typename _Container::const_iterator _Last = source.end();
	typename _Container::iterator _First2 = target.begin();
	while (_First != _Last)
	{
		*_First2 = *_First;
		++_First2;
		++_First;
	}
	GV_ASSERT(_First2 == target.end());
}

template < typename _Container0, typename _Container1 >
inline void gvt_for_each_copy(_Container0& target, const _Container1& source)
{
	typename _Container1::const_iterator _First = source.begin();
	typename _Container1::const_iterator _Last = source.end();
	typename _Container0::iterator _First2 = target.begin();
	while (_First != _Last)
	{
		*_First2 = *_First;
		++_First2;
		++_First;
	}
	GV_ASSERT(_First2 == target.end());
}

template < class T >
bool gvt_is_almost_equal(const T& t1, const T& t2);

template < class T1 >
inline bool gvt_is_almost_equal_container(const T1& c1, const T1& c2)
{
	typename T1::const_iterator i1 = c1.begin();
	typename T1::const_iterator i2 = c2.begin();
	while (i1 != c1.end() && i2 != c2.end())
	{
		if (!gvt_is_almost_equal(*i1, *i2))
			return false;
		i1++;
		i2++;
	}
	if (i1 == c1.end() && i2 == c2.end())
		return true;
	return false;
}

template < class T >
inline void gvt_sort(T first, T last)
{
	std::sort(first, last);
}

template < class T, class P >
inline void gvt_sort(T first, T last, P comparator)
{
	std::sort(first, last, comparator);
}

template < int size >
inline void gvt_swap_endian_ptr(void* p)
{
	GV_ASSERT(0);
}

template <>
inline void gvt_swap_endian_ptr< 1 >(void* p)
{
}

template <>
inline void gvt_swap_endian_ptr< 2 >(void* p)
{
	char* pc = (char*)p;
	gvt_swap(pc[0], pc[1]);
}

template <>
inline void gvt_swap_endian_ptr< 4 >(void* p)
{
	char* pc = (char*)p;
	gvt_swap(pc[0], pc[3]);
	gvt_swap(pc[1], pc[2]);
}

template <>
inline void gvt_swap_endian_ptr< 8 >(void* p)
{
	char* pc = (char*)p;
	gvt_swap(pc[0], pc[7]);
	gvt_swap(pc[1], pc[6]);
	gvt_swap(pc[2], pc[5]);
	gvt_swap(pc[3], pc[4]);
}

template < class T >
inline void gvt_swap_endian(T& t)
{
	gvt_swap_endian_ptr< sizeof(t) >((void*)&t);
}

template < class T >
inline bool gvt_is_aligned(const T& t, size_t alignment)
{
	return (((size_t)(t)) & (alignment - 1)) == 0;
}

template < class T >
struct gvt_alignment_helper
{
	char c;
	T t;
};

template < class T >
inline gv_uint gvt_get_alignment()
{
	return sizeof(gvt_alignment_helper< T >) - sizeof(T);
};

template < class T >
gv_int gvt_compare(const void* pa, const void* pb)
{
	T* a = (T*)pa;
	T* b = (T*)pb;
	if (*a < *b)
		return -1;
	if (*a == *b)
		return 0;
	return 1;
}

template < class T >
void gvt_qsort(T* first, gv_int size, gv_int(func_compare)(T*, T*))
{
	return gv_qsort(first, size, sizeof(T),
					reinterpret_cast< GV_COMP_FUNC >(func_compare));
}

template < class T >
inline void gvt_zero(T& t)
{
	memset((void*)&t, 0, sizeof(T));
}
template < class T >
inline T* gvt_strncpy(T* target, const T* source, gv_int size)
{
	GV_ASSERT(0);
};

template <>
inline char* gvt_strncpy< char >(char* target, const char* source, gv_int size)
{
	return ::strncpy(target, source, size);
};

template <>
inline wchar_t* gvt_strncpy< wchar_t >(wchar_t* target, const wchar_t* source,
									   gv_int size)
{
	return ::wcsncpy(target, source, size);
};

template < class T >
inline T gvt_max(const T& A, const T& B)
{
	return (A >= B) ? A : B;
}

template < class T >
inline T gvt_min(const T& A, const T& B)
{
	return (A <= B) ? A : B;
}

template < class T >
inline T gvt_square(const T& A)
{
	return A * A;
}

template < class T >
inline T gvt_sqrt(const T& A)
{
	return std::sqrt(A);
}

template < class T >
inline T gvt_clamp(const T& x, const T& Min, const T& Max)
{
	return x < Min ? Min : x < Max ? x : Max;
}

template < class T, class type_of_alignment >
inline T gvt_align(const T& t, const type_of_alignment& align)
{
	return (T)(((type_of_alignment)t + align - 1) & ~(align - 1));
}

template < class T, class type_of_real >
inline T gvt_lerp(const T& A, const T& B, const type_of_real& Alpha)
{
	return (T)(A + (B - A) * Alpha);
}

template < class T, class type_of_real >
inline T gvt_cubic_interpolation(const T& v0, const T& v1, const T& v2,
								 const T& v3, const type_of_real& x)
{
	T P = (v3 - v2) - (v0 - v1);
	T Q = (v0 - v1) - P;
	T R = v2 - v0;
	T S = v1;
	return P * (x * x * x) + Q * (x * x) + R * x + S;
}

template < class T >
inline bool gvt_is_overlap(const T& x0, const T& x1, const T& xx0,
						   const T& xx1)
{
	T max0 = gvt_max(x0, x1);
	T min1 = gvt_min(xx0, xx1);
	if (max0 < min1)
		return false;
	T max1 = gvt_max(xx0, xx1);
	T min0 = gvt_min(x0, x1);
	if (max1 < min0)
		return false;
	return true;
}

template < class T >
inline bool gvt_overlap_range(const T& min0, const T& max0, const T& min1,
							  const T& max1, T& min_ret, T& max_ret)
{
	min_ret = gvt_max(min0, min1);
	max_ret = gvt_min(max0, max1);
	if (max_ret < min_ret)
		return false;
	return true;
}

template < class T >
inline void gvt_merge_range(const T& min0, const T& max0, const T& min1,
							const T& max1, T& min_ret, T& max_ret)
{
	min_ret = gvt_min(min0, min1);
	max_ret = gvt_max(max0, max1);
}
