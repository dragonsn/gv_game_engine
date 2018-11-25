#pragma once

namespace gv
{
template < class T, class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_slist_no_alloc
	: public gvt_slist< T, gvt_memory_static< 1 >, policy_thread_mode >
{
public:
	// CLASS iterator
	typedef gvt_slist_no_alloc< T, policy_thread_mode > type_of_list;
	typedef gvt_slist< T, gvt_memory_static< 1 >, policy_thread_mode > super;
	typedef typename super::type_of_node type_of_node;
	typedef typename super::iterator iterator;

public:
	gvt_slist_no_alloc()
	{
	}
	~gvt_slist_no_alloc()
	{
		this->clear();
	}
	inline void add(type_of_node* p, gv_int isize)
	{
		for (int i = 0; i < isize; i++)
			this->link(p++);
	}
	inline void clear()
	{
		iterator it;
		while (this->begin() != this->end())
		{
			this->unlink(this->begin(), it);
		}
		return;
	}
	inline void add(const iterator& it)
	{
		this->link(it);
	}
	inline void erase(iterator it, iterator it_pre)
	{
		this->unlink(it, it_pre);
	}

private:
	inline iterator add_dummy()
	{
		return NULL;
	};
	inline iterator add(const T& t)
	{
		return NULL;
	};
	inline iterator add_unique(const T& t)
	{
		return NULL;
	};
	inline bool erase(const T& t)
	{
		return true;
	}
	inline bool erase(T* t)
	{
		return true;
	}
	inline const type_of_list& operator+=(const type_of_list& list)
	{
		return *this;
	}
	inline type_of_list& operator=(const type_of_list& a)
	{
		return *this;
	}
};

} // namespace gv