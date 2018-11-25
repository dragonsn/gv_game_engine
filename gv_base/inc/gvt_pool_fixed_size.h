#pragma once
namespace gv
{

//=========================================================================
//		static pool , no memory allocation.
//=========================================================================
template < class T, gv_int pool_size,
		   class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_pool_fixed_size
	: public gvt_lockable_object_level< policy_thread_mode >
{
public:
	//@1 the pool only call construct & deconstruct for each element once.
	//@2 the pool can iterator all the free item & active item
	//@3 the pool provide the get index to change pointer to index
	typedef T type_of_data;
	typedef gvt_dlist< T, policy_memory > type_of_list;
	typedef typename type_of_list::iterator iterator;
	typedef typename type_of_list::const_iterator const_iterator;
	typedef typename type_of_list::type_of_node type_of_node;

	inline gvt_pool_fixed_size()
	{
		free_list.link_all(node_array, pool_size);
	}
	~gvt_pool_fixed_size()
	{
		free_list.unlink_all();
		using_list.unlink_all();
	}
	inline gv_int get_index(const type_of_data* d) const
	{
		const type_of_node* pnode = (const type_of_node*)d;
		GV_ASSERT(pnode >= node_array && pnode < node_array + pool_size);
		return (gv_int)(pnode - node_array);
	}
	inline type_of_data& operator[](gv_int i)
	{
		GV_ASSERT(i >= 0 && i < pool_size);
		return node_array[i].data;
	}
	inline const type_of_data& operator[](gv_int i) const
	{
		GV_ASSERT(i >= 0 && i < pool_size);
		return node_array[i].data;
	}
	inline iterator begin()
	{
		return using_list.begin();
	}
	inline iterator end()
	{
		return using_list.end();
	}
	inline iterator last()
	{
		return using_list.last();
	}
	inline iterator free_begin()
	{
		return free_list.begin();
	}
	inline iterator free_end()
	{
		return free_list.end();
	}
	inline type_of_data* allocate()
	{
		iterator it = free_list.unlink_front();
		if (!it.get_node())
			return NULL;
		using_list.link_back(it);
		return &(*it);
	}
	inline void free(type_of_data* data)
	{
		type_of_node* node = (type_of_node*)data;
		iterator it(node);
		using_list.unlink(it);
		free_list.link_back(it);
	};
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			visitor(*it);
			++it;
		}
	}

	template < class type_of_visitor >
	inline void for_each(const type_of_visitor& visitor)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			visitor(*it);
			++it;
		}
	}
	iterator find(const type_of_data& d)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			if (*it == d)
			{
				break;
			}
			++it;
		}
		return it;
	}

	void sort()
	{
		using_list.sort();
	}

	type_of_node node_array[pool_size];
	type_of_list free_list;
	type_of_list using_list;
};

//=========================================================================
//		RESIZE ABLE , one memory allocation.
//=========================================================================
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_pool_resizable
	: public gvt_lockable_object_level< policy_thread_mode >
{
public:
	//@1 the pool only call construct & deconstruct for each element once.
	//@2 the pool can iterator all the free item & active item
	//@3 the pool provide the get index to change pointer to index
	typedef T type_of_data;
	typedef gvt_dlist< T, policy_memory > type_of_list;
	typedef typename type_of_list::iterator iterator;
	typedef typename type_of_list::const_iterator const_iterator;
	typedef typename type_of_list::type_of_node type_of_node;
	inline gvt_pool_resizable()
	{
		m_used_size = 0;
		m_pool_size = 0;
	}
	inline gvt_pool_resizable(gv_int size)
	{
		reset(size);
	}
	~gvt_pool_resizable()
	{
		free_list.unlink_all();
		using_list.unlink_all();
	}
	void reset(gv_int size)
	{
		m_used_size = 0;
		m_pool_size = size;
		free_list.unlink_all();
		using_list.unlink_all();
		node_array.reserve(size);
		node_array.resize(size);
		free_list.link_all(node_array.begin(), m_pool_size);
	}
	inline gv_int get_index(const type_of_data* d) const
	{
		const type_of_node* pnode = (const type_of_node*)d;
		return node_array.index(pnode);
	}
	inline type_of_data& operator[](gv_int i)
	{
		return node_array[i].data;
	}
	inline bool is_valid(gv_int index)
	{
		return node_array.is_valid(index);
	}
	inline const type_of_data& operator[](gv_int i) const
	{
		return node_array[i].data;
	}
	inline iterator begin()
	{
		return using_list.begin();
	}
	inline iterator end()
	{
		return using_list.end();
	}
	inline gv_int size()
	{
		return m_used_size;
	}
	inline type_of_data* allocate()
	{
		iterator it = free_list.unlink_front();
		if (!it.get_node())
			return NULL;
		using_list.link_back(it);
		m_used_size++;
		return &(*it);
	}
	inline void free(type_of_data* data)
	{
		type_of_node* node = (type_of_node*)data;
		iterator it(node);
		using_list.unlink(it);
		free_list.link_back(it);
		m_used_size--;
	};
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			visitor(*it);
			++it;
		}
	}
	template < class type_of_visitor >
	inline void for_each(const type_of_visitor& visitor)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			visitor(*it);
			++it;
		}
	}
	iterator find(const type_of_data& d)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			if (*it == d)
			{
				break;
			}
			++it;
		}
		return it;
	}
	gv_int get_used_size()
	{
		return this->m_used_size;
	}

protected:
	gv_int m_used_size;
	gv_int m_pool_size;
	gvt_array< type_of_node > node_array;
	type_of_list free_list;
	type_of_list using_list;
};

//=========================================================================
//		resizeable with inde
//=========================================================================
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_pool_array : public gvt_lockable_object_level< policy_thread_mode >
{
public:
	//@1 the pool only call construct & deconstruct for each element once.
	//@2 the pool can iterator all the free item & active item
	//@3 the pool provide the get index to change pointer to index
	typedef T type_of_data;
	typedef type_of_data* iterator;
	typedef const type_of_data* const_iterator;
	inline gvt_pool_array()
	{
		m_used_size = 0;
	}
	inline gvt_pool_array(gv_int size)
	{
		reset(size);
	}
	~gvt_pool_array()
	{
	}
	void reset(gv_int size)
	{
		m_used_size = 0;
		node_array.reserve(size);
		node_array.resize(size);
		free_index_array.reserve(size);
		free_index_array.resize(size);
		for (gv_int i = 0; i < size; i++)
			free_index_array[i] = size - i - 1;
	}
	inline gv_int get_index(const type_of_data* d) const
	{
		return node_array.index(d);
	}
	inline type_of_data& operator[](gv_int i)
	{
		return node_array[i];
	}
	inline bool is_valid(gv_int index)
	{
		return node_array.is_valid(index);
	}
	inline const type_of_data& operator[](gv_int i) const
	{
		return node_array[i];
	}
	inline iterator begin()
	{
		return node_array.begin();
	}
	inline iterator end()
	{
		return node_array.end();
	}
	inline gv_int size()
	{
		return node_array.size();
	}
	inline type_of_data* allocate()
	{
		if (m_used_size == node_array.size())
		{
			// increase the size.....
			type_of_data d;
			node_array.push_back(d);
			free_index_array.push_back(m_used_size);
		}
		gv_int free_id = *free_index_array.last();
		free_index_array.pop_back();
		m_used_size++;
		return &node_array[free_id];
	}

	inline void free(type_of_data* data)
	{
		gv_int index = get_index(data);
		free_index_array.push_back(index);
		m_used_size--;
	};

	inline void free(gv_int index)
	{
		free_index_array.push_back(index);
		m_used_size--;
	};

	inline gv_int get_used_size()
	{
		return m_used_size;
	}

protected:
	gv_int m_used_size;
	gvt_array< gv_int, policy_memory, policy_thread_mode,
			   gvp_array_size_calculator_string >
		free_index_array;
	gvt_array< type_of_data > node_array;
};
}