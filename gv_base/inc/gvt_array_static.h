#pragma once
#include "gvt_array.h"
namespace gv
{
template < class T, int fixed_size,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_array_with_fixed_memory
	: public gvt_array< T, gvt_memory_static< fixed_size * sizeof(T) >,
						policy_thread_mode, gvp_array_size_calculator_minimun >
{
};

template < class T, int fixed_size >
class gvt_array_not_resizable : public boost::array< T, fixed_size >
{
};

template < class T, int fixed_size >
class gvt_array_static
{
public:
	typedef T type_of_data;
	typedef gvt_array_static< type_of_data, fixed_size > type_of_array;
	typedef T* iterator;
	typedef const T* const_iterator;
	const static gv_int array_size = fixed_size;
	gvt_array_static()
	{
		GV_ASSERT(GV_STRUCT_OFFSET(type_of_array, data_ptr) == 0);
	}
	inline void init(const T* p, gv_int isize)
	{
		GV_ASSERT(isize == array_size);
		for (int i = 0; i < isize; i++)
			data_ptr[i] = *p++;
	};
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		std::for_each(begin(), end(), visitor);
	}
	template < class type_of_visitor >
	inline void for_each(const type_of_visitor& visitor)
	{
		std::for_each(begin(), end(), visitor);
	}
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor) const
	{
		std::for_each(begin(), end(), visitor);
	}
	template < class type_of_visitor >
	inline const T* for_each_until(type_of_visitor& visitor) const
	{
		return std::find_if(begin(), end(), visitor);
	}
	template < class type_of_visitor >
	inline T* for_each_until(type_of_visitor& visitor)
	{
		return std::find_if(begin(), end(), visitor);
	}
	inline bool is_valid(gv_int index) const
	{
		return (index >= 0 && index < size());
	}
	inline bool is_valid_ptr(const T* p) const
	{
		if (!data_ptr)
			return false;
		return (p >= begin() && p < end());
	}
	inline gv_int index(const T* p) const
	{
		GV_ASSERT_SLOW(is_valid_ptr(p));
		return p - data_ptr;
	}
	inline const T& operator[](gv_int i) const
	{
		GV_ASSERT_SLOW(is_valid(i));
		return *(data_ptr + i);
	};
	inline T& operator[](gv_int i)
	{
		GV_ASSERT_SLOW(is_valid(i));
		return *(data_ptr + i);
	};
	inline gv_int size() const
	{
		return array_size;
	};
	inline bool is_empty() const
	{
		return array_size == 0;
	}
	inline const T* begin() const
	{
		return data_ptr;
	}
	inline T* begin()
	{
		return data_ptr;
	}
	inline const T* first() const
	{
		return data_ptr;
	}
	inline T* first()
	{
		return data_ptr;
	}
	inline const T* last() const
	{
		return size() ? data_ptr + array_size - 1 : NULL;
	}
	inline T* last()
	{
		return size() ? data_ptr + array_size - 1 : NULL;
	}
	inline const T* end() const
	{
		return data_ptr + array_size;
	}
	inline T* end()
	{
		return data_ptr + array_size;
	}
	inline bool find(const T& t)
	{
		gv_int idx;
		return find(t, idx);
	}
	inline bool find(const T& t, gv_int& idx)
	{
		T* ret = std::find(begin(), end(), t);
		if (ret != end())
		{
			idx = ret - begin();
			return true;
		}
		return false;
	}
	template < class comparator >
	inline bool find(const T& t, gv_int& idx, const comparator& c) const
	{
		const T* p = for_each_until(std::bind1st(c, t));
		if (p != end())
		{
			idx = p - begin();
			return true;
		}
		return false;
	}
	//  if the return value is negtive  then not find  and the positon is
	//  -returnvalue+1
	template < class comparator_is_less >
	inline bool find_in_sorted_array(const T& t, gv_int& idx,
									 const comparator_is_less& c) const
	{
		std::pair< const T*, const T* > range =
			std::equal_range(begin(), end(), t, c);
		if (range.second == begin()) // insert before
		{
			idx = -1;
			return false;
		}
		if (range.first == end() || range.first == range.second ||
			range.second == begin()) // insert before
		{
			idx = -(range.first - begin() + 1);
			return false;
		}
		idx = range.first - begin();
		return true;
	}
	inline bool find_in_sorted_array(const T& t, gv_int& idx) const
	{
		return find_in_sorted_array(t, idx, std::less< T >());
	}
	inline void sort()
	{
		std::sort(begin(), end());
	}
	template < class comparator_is_less >
	inline void sort(const comparator_is_less& c)
	{
		std::sort(begin(), end(), c);
	};
	inline const type_of_array& operator+=(const type_of_array& a)
	{
		add(a.begin(), a.size());
		return *this;
	}
	inline bool operator==(const type_of_array& a) const
	{
		if (a.array_size != this->array_size)
			return false;
		return gvt_for_each_is_equal(*this, a);
	}
	inline type_of_array& operator=(const type_of_array& a)
	{
		if (this != &a)
		{
			init(a.begin(), a.size());
		}
		return *this;
	}
	inline type_of_array& operator=(const type_of_data& a)
	{
		for (int i = 0; i < fixed_size; i++)
			data_ptr[i] = a;
		return *this;
	}

protected:
	T data_ptr[fixed_size];
};
// just another name
#define gvt_array_fixed gvt_array_static

} // namespace gv