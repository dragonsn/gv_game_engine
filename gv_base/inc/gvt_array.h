#pragma once

namespace gv
{
class gvp_array_size_calculator_minimun
{
public:
	static bool need_change_max(gv_int old_max, gv_int new_size,
								gv_int& new_max)
	{
		if (old_max >= new_size)
			return false;
		new_max = new_size;
		return true;
	};
};
//
class gvp_array_size_calculator_string
{
public:
	static bool need_change_max(gv_int old_max, gv_int new_size,
								gv_int& new_max)
	{
		new_max = old_max;
		if (old_max >= new_size)
			return false;
		new_max = new_size * 2 + 32;
		return true;
	};
};
class gvp_array_size_calculator_dynamic
{
public:
	static bool need_change_max(gv_int old_max, gv_int new_size,
								gv_int& new_max)
	{
		new_max = old_max;
		if (old_max >= new_size)
			return false;
		new_max = new_size * 2;
		return true;
	};
};

class gvp_array_size_calculator_auto
{
public:
	static bool need_change_max(gv_int old_max, gv_int new_size,
								gv_int& new_max)
	{
		gv_int max_max = new_size + new_size;
		gv_int max_min = new_size;
		if (old_max > max_max || (old_max == 0 && new_size > 0))
		{
			new_max = max_min;
			return true;
		}
		if (old_max < new_size)
		{
			new_max = max_max;
			return true;
		}
		return false;
	};
};

template < class T >
class gvt_policy_size_calculator : public gvp_array_size_calculator_auto
{
};

template < int cache_size,
		   class super_calculator = gvp_array_size_calculator_auto >
class gvt_policy_size_calculator_cached
{
public:
	static bool need_change_max(gv_int old_max, gv_int new_size,
								gv_int& new_max)
	{
		if (old_max >= new_size)
			return false;
		if (new_size <= cache_size)
		{
			new_max = cache_size;
			return true;
		}
		return super_calculator::need_change_max(old_max, new_size, new_max);
	};
};

//@gvt_array
// replacement of std::vector
// lockable , ranged ,iterator.
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T >,
		   class policy_size_calculator = gvt_policy_size_calculator< T > >
class gvt_array
{
	friend class gv_type_array;

public:
	typedef T type_of_data;
	typedef gvt_array< type_of_data, policy_memory, policy_thread_mode,
					   policy_size_calculator >
		type_of_array;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef typename policy_thread_mode::mutex_holder type_of_mutex;

protected:
	T* data_ptr;
	gv_int array_size;
	gv_int array_max;
	type_of_mutex array_mutex;
	policy_memory array_memory;

public:
	inline gvt_array()
	{
		array_size = 0, data_ptr = 0, array_max = 0;
	}

	inline gvt_array(const type_of_array& a)
	{
		array_size = 0, data_ptr = 0, array_max = 0;
		*this = a;
	}

	inline ~gvt_array()
	{
		clear();
	};

	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		std::for_each(begin(), end(), visitor);
	}

	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor) const
	{
		std::for_each(begin(), end(), visitor);
	}

	template < class type_of_visitor >
	inline void for_each(const type_of_visitor& visitor)
	{
		std::for_each(begin(), end(), visitor);
	}

	template < class type_of_visitor >
	inline const T* for_each_until(type_of_visitor& visitor) const
	{
		return std::find_if(begin(), end(), visitor);
	}

	template < class type_of_visitor >
	inline const T* for_each_until(const type_of_visitor& visitor) const
	{
		return std::find_if(begin(), end(), visitor);
	}

	template < class type_of_visitor >
	inline T* for_each_until(type_of_visitor& visitor)
	{
		return std::find_if(begin(), end(), visitor);
	}

	inline void resize(gv_int new_size)
	{
		gv_int new_max = 0;
		GV_ASSERT(new_size >= 0);
		if (new_size < array_size)
		{
			gvt_destroy_array(data_ptr + new_size, data_ptr + array_size);
		}
		if (policy_size_calculator::need_change_max(array_max, new_size, new_max))
		{
			data_ptr = (T*)array_memory.realloc(begin(), sizeof(T) * new_max);
			array_max = new_max;
		}
		if (new_size > array_size)
		{
			gvt_construct_array(data_ptr + array_size, data_ptr + new_size);
		}
		array_size = new_size;
	};

	inline void reserve(gv_int new_max)
	{
		GV_ASSERT(new_max >= 0);
		if (new_max == array_max)
			return;
		if (new_max < array_size)
		{
			gvt_destroy_array(data_ptr + new_max, data_ptr + array_size);
			array_size = new_max;
		}
		data_ptr = (T*)array_memory.realloc(data_ptr, sizeof(T) * new_max);
		array_max = new_max;
	}

	inline bool is_valid(gv_int index) const
	{
		return (index >= 0 && index < array_size);
	}

	inline bool is_valid_ptr(const T* p) const
	{
		if (!data_ptr)
			return false;
		return (p >= data_ptr && p < data_ptr + array_size);
	}

	inline gv_int index(const T* p) const
	{
		GV_ASSERT(is_valid_ptr(p));
		return (gv_int)(p - data_ptr);
	}

	inline const T& operator[](gv_int i) const
	{
		GV_ASSERT(is_valid(i));
		return *(data_ptr + i);
	};

	inline T& operator[](gv_int i)
	{
		GV_ASSERT(is_valid(i));
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

	inline const T* get_data() const
	{
		return begin();
	}

	inline const T* first() const
	{
		return data_ptr;
	}
	inline T* first()
	{
		return data_ptr;
	}
	// one past last
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

	inline void init(const T* p, gv_int isize)
	{
		resize(0);
		insert(p, 0, isize);
	};

	template < int isize >
	inline void init(T (&p)[isize])
	{
		init(p, isize);
	}

	inline void insert(const T* p, gv_int position, gv_int n = 1)
	{
		GV_ASSERT(position >= 0 && position <= array_size);
		if (n <= 0)
			return;
		type_of_array array_backup;
		if (is_valid_ptr(p))
		{
			array_backup.init(p, n);
			p = array_backup.begin();
		}
		if (array_max < array_size + n)
		{
			gv_int new_max = 0;
			policy_size_calculator::need_change_max(array_max, array_size + n,
													new_max);
			this->reserve(new_max);
		}
		array_memory.memmove(data_ptr + position + n, data_ptr + position,
							 (array_size - position) * sizeof(T));
		gvt_construct_array(data_ptr + position, data_ptr + position + n);
		gvt_copy(p, p + n, data_ptr + position);
		array_size += n;
	}
	inline void erase(gv_int position, gv_int n = 1)
	{
		GV_ASSERT(position >= 0);
		GV_ASSERT(position + n <= array_size);
		GV_ASSERT(n >= 0);
		if (n == 0)
			return;
		gvt_destroy_array(data_ptr + position, data_ptr + position + n);
		array_memory.memmove(data_ptr + position, data_ptr + position + n,
							 (array_size - position - n) * sizeof(T));
		this->array_size -= n;
		this->resize(array_size);
	}
	inline void remove(gv_int position, gv_int n = 1)
	{
		erase(position, n);
	}
	inline void erase_fast(gv_int idx)
	{
		GV_ASSERT(is_valid(idx))
		if (idx != size() - 1)
		{
			gvt_swap((*this)[idx], *last());
		}
		resize(size() - 1);
	}

	inline void erase_fast(T* it)
	{
		erase_fast((int)(it - begin()));
	}

	inline T* push_front(const T& t)
	{
		insert(&t, 0);
		return begin();
	}
	inline T* push_back(const T& t)
	{
		// some optimizaton can be done here rather than	insert  (&t, array_size
		// );
		this->resize(size() + 1);
		T* p = this->end() - 1;
		*p = t;
		return p;
	}
	inline T* add(const T& t)
	{
		return this->push_back(t);
	}
	inline void add(const T& t, int n)
	{
		for (int i = 0; i < n; i++)
			this->push_back(t);
	}
	inline T* add_dummy()
	{
		T t;
		return this->push_back(t);
	}

	inline void pop_back()
	{
		this->erase(array_size - 1);
	}

	inline void pop_front()
	{
		this->erase(0);
	}
	inline void add(const T* pt, gv_int n = 1)
	{
		insert(pt, array_size, n);
	}
	inline void add(const type_of_array& a)
	{
		this->add(a.begin(), a.size());
	}
	inline void clear()
	{
		this->erase(0, this->size());
		if (data_ptr)
			array_memory.free(data_ptr);
		array_size = 0, data_ptr = 0, array_max = 0;
	}

	inline void clear_and_reserve()
	{
		gvt_destroy_array(begin(), end());
		array_size = 0;
	}

	inline void shrink()
	{
		this->reserve(this->array_size);
	}
	inline void lock()
	{
		array_mutex.lock();
	}
	inline void unlock()
	{
		array_mutex.unlock();
	}
	inline gv_int add_unique(const T& t)
	{
		gv_int idx = -1;
		if (!find(t, idx))
		{
			this->push_back(t);
			return size() - 1;
		}
		return idx;
	}
	inline gv_int add_unique(const T& t, bool fn_test(const T&, const T&),
							 bool replace = false)
	{
		int ret;
		int idx;
		idx = this->find(t, fn_test);
		if (idx != -1)
		{
			ret = idx;
			if (replace)
				(*this)[idx] = t;
			return ret;
		}
		this->push_back(t);
		ret = size() - 1;
		return ret;
	}

	inline bool erase_item(const T& t)
	{
		gv_int idx = -1;
		if (!find(t, idx))
		{
			return false;
		}
		erase(idx);
		return true;
	}
	inline bool remove(const T& t)
	{
		return erase_item(t);
	}
	inline bool erase_item_fast(const T& t)
	{
		gv_int idx = -1;
		if (!find(t, idx))
		{
			return false;
		}
		if (idx != size() - 1)
		{
			gvt_swap((*this)[idx], *last());
		}
		resize(size() - 1);
		return true;
	}
	inline bool remove_fast(const T& t)
	{
		return erase_item_fast(t);
	}
	inline bool find(const T& t) const
	{
		gv_int idx;
		return find(t, idx);
	}
	inline bool find(const T& t, gv_int& idx) const
	{
		T* ret = std::find((T*)begin(), (T*)end(), t);
		if (ret != end())
		{
			idx = (gv_int)(ret - begin());
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
			idx = (gv_int)(p - begin());
			return true;
		}
		return false;
	}
	inline int find(const T& t, bool fn_test(const T&, const T&)) const
	{
		int ret = -1;
		int i;
		const T* p = begin();
		for (i = 0; i < size(); i++, p++)
		{
			if (fn_test(t, *p))
			{
				ret = i;
				return ret;
			}
		}
		return ret;
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
			idx = -(int)(range.first - begin() + 1);
			return false;
		}
		idx = (int)(range.first - begin());
		return true;
	}
	inline bool find_in_sorted_array(const T& t, gv_int& idx) const
	{
		return find_in_sorted_array(t, idx, std::less< T >());
	}

	inline void insert_in_sorted_array(const T& t, bool alway_insert = false)
	{
		insert_in_sorted_array(t, std::less< T >(), alway_insert);
	}

	template < class comparator_is_less >
	inline void insert_in_sorted_array(const T& t, const comparator_is_less& c,
									   bool alway_insert = false)
	{
		gv_int index;
		if (find_in_sorted_array(t, index, c))
		{
			if (!alway_insert)
			{
				(*this)[index] = t;
			}
			else
			{
				this->insert(&t, index);
			}
		}
		else
		{
			index = -index - 1;
			this->insert(&t, index);
		}
	}

	inline void sort()
	{
		std::sort(begin(), end());
	}

	inline bool check_is_sorted(bool allow_equal = false)
	{
		for (int i = 0; i < this->size() - 1; i++)
		{
			if (!allow_equal)
			{
				if (!((*this)[i] < (*this)[i + 1]))
				{
					return false;
				}
			}
			else
			{
				if (!((*this)[i] <= (*this)[i + 1]))
				{
					return false;
				}
			}
		}
		return true;
	}

	template < class comparator_is_less >
	inline void sort(const comparator_is_less& c)
	{
		std::sort(begin(), end(), c);
	};
	gv_uint crc32() const
	{
		return gv_crc32((const char*)begin(), size() * sizeof(T));
	}
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
	inline bool operator!=(const type_of_array& a) const
	{
		return !((*this) == a);
	}
	inline bool operator<(const type_of_array& a) const
	{
		return gvt_for_each_is_less(*this, a);
	}
	inline type_of_array& operator=(const type_of_array& a)
	{
		if (this != &a)
		{
			init(a.begin(), a.size());
		}
		return *this;
	}
	inline type_of_array& operator=(const type_of_data& d)
	{
		iterator it = begin();
		while (it != end())
		{
			*it = d;
			++it;
		}
		return *this;
	}

	template < class T1 >
	inline type_of_array& operator=(const gvt_array< T1 >& a)
	{
		this->resize(a.size());
		for (int i = 0; i < a.size(); i++)
			(*this)[i] = (T)a[i];
		return *this;
	}

	template < class _T, class _policy_memory, class _policy_thread_mode,
			   class _policy_size_calculator >
	inline type_of_array&
	operator=(const gvt_array< _T, _policy_memory, _policy_thread_mode,
							   _policy_size_calculator >& a)
	{
		resize(a.size());
		gvt_copy(a.begin(), a.end(), begin());
		return *this;
	}
};
#if GV_64
GV_STATIC_ASSERT(sizeof(gvt_array< char >) == 24);
#else
GV_STATIC_ASSERT(sizeof(gvt_array< char >) == 16);
#endif

template < class T, int const_cached_size = 16,
		   class policy_thread_mode = gvt_policy_thread_mode< T >,
		   class policy_size_calculator = gvt_policy_size_calculator< T > >
class gvt_array_cached
	: public gvt_array< T, gvt_memory_cached< sizeof(T) * const_cached_size >,
						policy_thread_mode,
						gvt_policy_size_calculator_cached<
							const_cached_size, policy_size_calculator > >
{
};

} // namespace gv