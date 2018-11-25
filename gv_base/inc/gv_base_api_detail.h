namespace detail
{
template < typename I1, typename I2, bool b >
inline I2 copy_imp(I1 first, I1 last, I2 out,
				   const boost::integral_constant< bool, b >&)
{
	while (first != last)
	{
		*out = (typename boost::remove_pointer< I2 >::type) * first;
		++out;
		++first;
	}
	return out;
}

template < typename T >
inline T* copy_imp(const T* first, const T* last, T* out,
				   const boost::true_type&)
{
	memcpy(out, first, (last - first) * sizeof(T));
	return out + (last - first);
}
}

namespace detail
{

template < class T >
inline void do_destroy_array(T* first, T* last, const boost::false_type&)
{
	while (first != last)
	{
		first->~T();
		++first;
	}
}

template < class T >
inline void do_destroy_array(T* first, T* last, const boost::true_type&)
{
}

} // namespace detail

namespace detail
{



} // namespace detail

namespace detail
{

template < class T >
inline void do_construct(T* p, const boost::false_type&)
{
	::new ((void*)p) T;
}

template < class T >
inline void do_construct(T* p, const boost::true_type&)
{
}

template < class T >
inline void do_construct_array(T* first, T* last, const boost::false_type& f)
{
	while (first != last)
	{
		do_construct(first, f);
		++first;
	}
}


template < class T >
inline void do_construct_array(T* first, T* last, const boost::true_type&)
{
}


} // namespace detail

namespace detail
{

template < class T >
inline void do_construct_copy(T* p, const T& t, const boost::false_type&)
{
	::new ((void*)p) T(t);
}

template < class T >
inline void do_construct_copy(T* p, const T& t, const boost::true_type&)
{
}

} // namespace detail

namespace detail
{

template < class T >
inline void do_destroy_ptr(T* ptr, const boost::false_type&)
{
	ptr->~T();
}

template < class T >
inline void do_destroy_ptr(T* ptr, const boost::true_type&)
{
}

} // namespace detail
