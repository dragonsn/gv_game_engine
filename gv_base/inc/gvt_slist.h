#pragma once

namespace gv
{
template < class T >
class gvt_slist_node
{
public:
	gvt_slist_node()
		: next(0)
	{
	}
	T data;
	gvt_slist_node* next;
};

template < class T, class policy_memory = gvp_memory_default,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_slist
{
public:
	typedef T type_of_data;
	typedef gvt_slist< T, policy_memory, policy_thread_mode > type_of_list;
	typedef gvt_slist_node< T > type_of_node;
	typedef const T* const_ptr_of_data;
	typedef T* ptr_of_data;
	typedef const T& const_ref_of_data;
	typedef T& ref_of_data;

public:
	class const_iterator;
	friend class const_iterator;
	class const_iterator
	{ // iterator for nonmutable list
	public:
		typedef T value_type;
		typedef const_ptr_of_data pointer;
		typedef const_ref_of_data reference;
		const_iterator()
			: ptr_node(0)
		{ // construct with null node pointer
		}
		const_iterator(const type_of_node* _pnode)
			: ptr_node(_pnode)
		{ // construct with node pointer _pnode
		}
		reference operator*() const
		{ // return designated value
			GV_ASSERT(ptr_node);
			return ptr_node->data;
		}
		pointer operator->() const
		{ // return pointer to class object
			GV_ASSERT(ptr_node);
			return (&**this);
		}
		const_iterator& operator++()
		{ // preincrement
			GV_ASSERT(ptr_node);
			ptr_node = ptr_node->next;
			return (*this);
		}
		const_iterator operator++(int)
		{ // postincrement
			const_iterator _tmp = *this;
			++*this;
			return (_tmp);
		}
		bool operator==(
			const const_iterator& _right) const
		{ // test for iterator equality
			return (ptr_node == _right.ptr_node);
		}
		bool operator!=(
			const const_iterator& _right) const
		{ // test for iterator inequality
			return (!(*this == _right));
		}
		const type_of_node* get_node() const
		{ // return node pointer
			return (ptr_node);
		}

	protected:
		const type_of_node* ptr_node; // pointer to node
	};
	// CLASS iterator
	class iterator;
	friend class iterator;
	class iterator : public const_iterator
	{ // iterator for mutable list
	public:
		typedef ptr_of_data pointer;
		typedef ref_of_data reference;
		iterator()
		{ // construct with null node
		}
		iterator(type_of_node* _pnode)
			: const_iterator(_pnode)
		{ // construct with node pointer _pnode
		}
		reference operator*() const
		{ // return designated value
			GV_ASSERT(const_iterator::ptr_node);
			return ((reference) * *(const_iterator*)this);
		}
		ptr_of_data operator->() const
		{ // return pointer to class object
			GV_ASSERT(const_iterator::ptr_node);
			return (&**this);
		}
		iterator& operator++()
		{ // preincrement
			++(*(const_iterator*)this);
			return (*this);
		}
		iterator operator++(int)
		{ // postincrement
			iterator _tmp = *this;
			++*this;
			return (_tmp);
		}
		type_of_node* get_node() const
		{ // return node pointer
			return (type_of_node*)(const_iterator::ptr_node);
		}
	};

public:
	gvt_slist()
		: first_node(0)
	{
		GV_ASSERT(GV_STRUCT_OFFSET(type_of_node, data) == 0);
	}
	gvt_slist(const type_of_list& list)
		: first_node(0)
	{
		*this += list;
	}
	~gvt_slist()
	{
		this->clear();
	}
	inline iterator add(const T& t)
	{
		iterator it = this->add_dummy();
		(*it) = t;
		return it;
	};
	inline iterator push_front(const T& t)
	{
		return add(t);
	}
	inline iterator add_dummy()
	{
		type_of_node* pnode =
			(type_of_node*)list_memory.alloc(sizeof(type_of_node));
		gvt_construct(pnode);
		link(pnode);
		return pnode;
	};
	inline iterator add_unique(const T& t)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			if ((*it) == t)
				return it;
			++it;
		}
		it = add_dummy();
		(*it) = t;
		return it;
	}
	inline iterator find(const T& t)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			if ((*it) == t)
				return it;
			++it;
		}
		return NULL;
	}
	inline bool erase(const T& t)
	{
		iterator it = begin();
		iterator it_pre;
		while (it != end())
		{
			if ((*it) == t)
			{
				erase(it, it_pre);
				return true;
			}
			it_pre = it;
			it++;
		}
		return false;
	}
	inline void erase(iterator it, iterator it_pre)
	{
		unlink(it, it_pre);
		gvt_destroy(it.get_node());
		list_memory.free(it.get_node());
		// delete(list_memory, it.get_node());
		// don'twork study more!!
	}
	inline void erase(iterator node)
	{
		iterator it = begin();
		iterator it_pre;
		while (it != end())
		{
			if (it == node)
			{
				erase(it, it_pre);
				return;
			}
			it_pre = it;
			it++;
		}
		assert(0);
	}
	inline void pop_front()
	{
		this->erase(begin(), NULL);
	}

	inline iterator begin()
	{
		return first_node;
	}
	inline iterator end()
	{
		return NULL;
	}
	inline const_iterator begin() const
	{
		return first_node;
	}
	inline const_iterator end() const
	{
		return NULL;
	}
	inline void init(const T* p, gv_int isize)
	{
		for (int i = 0; i < isize; i++)
			this->add(*p++);
	}
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		gvt_for_each(begin(), end(), visitor);
	}
	template < class type_of_visitor >
	inline T* for_each_until(type_of_visitor& visitor)
	{
		return std::find_if(begin(), end(), visitor);
	}
	inline void clear()
	{
		while (begin() != this->end())
		{
			erase(begin(), iterator());
		}
		return;
	}
	inline const type_of_list& operator+=(const type_of_list& list)
	{
		const_iterator it = list.begin();
		while (it != list.end())
		{
			this->add(*it);
			++it;
		}
		return *this;
	}
	inline bool operator==(const type_of_list& list) const
	{
		const_iterator it = list.begin();
		const_iterator it2 = begin();
		while (it != list.end() && it2 != end())
		{
			if (*it != *it2)
				return false;
			++it;
			++it2;
		}
		return true;
	}
	inline type_of_list& operator=(const type_of_list& a)
	{
		if (this != &a)
		{
			this->clear();
			(*this) += a;
		}
		return *this;
	}

protected:
	inline void unlink(const iterator& it, const iterator& it_pre)
	{
		if (it_pre == this->end())
			this->first_node = it.get_node()->next;
		else
			it_pre.get_node()->next = it.get_node()->next;
	}
	inline void link(const iterator& it)
	{
		it.get_node()->next = this->first_node;
		this->first_node = it.get_node();
	}

private:
	type_of_node* first_node;
	policy_memory list_memory;
};

} // namespace gv