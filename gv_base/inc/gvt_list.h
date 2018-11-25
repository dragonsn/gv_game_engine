#pragma once
#include <set>
#include <list>
namespace gv
{
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_list : public std::list< T, gvt_allocator< T, policy_memory > >,
				 public gvt_lockable_object_level< policy_thread_mode >
{
public:
	gvt_list(){};
	typedef T type_of_data;
	typedef gvt_list< type_of_data, policy_memory, policy_thread_mode >
		type_of_list;
	typedef std::list< T, gvt_allocator< T, policy_memory > > super;

public:
	typename super::iterator add_dummy()
	{
		return this->insert(super::end(), T());
	}
	typename super::iterator add(const T& t)
	{
		this->push_back(t);
	}
};

template < class T >
class gvt_dlist_node
{
public:
	gvt_dlist_node()
		: next(0), pre(0)
	{
	}
	T data;
	gvt_dlist_node* next;
	gvt_dlist_node* pre;
};

template < class T, class policy_memory = gvp_memory_default,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_dlist
{
public:
	typedef T type_of_data;
	typedef gvt_dlist< T, policy_memory, policy_thread_mode > type_of_list;
	typedef gvt_dlist_node< T > type_of_node;
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
		const_iterator& operator--()
		{ // preincrement
			GV_ASSERT(ptr_node);
			ptr_node = ptr_node->pre;
			return (*this);
		}
		const_iterator operator--(int)
		{ // postincrement
			const_iterator _tmp = *this;
			--*this;
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
		inline operator type_of_node*() const
		{
			return (type_of_node*)(const_iterator::ptr_node);
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
		iterator& operator--()
		{ // preincrement
			--(*(const_iterator*)this);
			return (*this);
		}
		iterator operator--(int)
		{ // postincrement
			iterator _tmp = *this;
			--*this;
			return (_tmp);
		}
		type_of_node* get_node() const
		{ // return node pointer
			return (type_of_node*)(const_iterator::ptr_node);
		}
	};

public:
	gvt_dlist()
		: _first(0), _last(0)
	{
		GV_ASSERT(GV_STRUCT_OFFSET(type_of_node, data) == 0);
	}
	gvt_dlist(const type_of_list& list)
		: _first(0), _last(0)
	{
		*this += list;
	}
	~gvt_dlist()
	{
		this->clear();
	}
	//================================================================
	inline int size()
	{
		int size = 0;
		iterator it = this->begin();
		while (it != this->end())
		{
			++size;
			++it;
		}
		return size;
		;
	}
	//================================================================
	inline iterator add(const T& t)
	{
		iterator it = this->add_dummy();
		(*it) = t;
		return it;
	};
	//================================================================
	inline iterator insert_sorted(const T& t)
	{
		iterator it = this->begin();
		while (it != this->end())
		{
			if (!((*it) < t))
				break;
			++it;
		}
		return insert(it, t);
	}

	//
	inline iterator insert(const iterator& position, const T& t)
	{
		iterator it = this->insert_dummy(position);
		(*it) = t;
		return it;
	}

	inline iterator link_sorted(iterator t)
	{
		iterator it = this->begin();
		iterator it_pre = this->end();
		while (it != this->end())
		{
			if (!((*it) < *t))
				break;
			it_pre = it;
			++it;
		}
		return link(t, it_pre);
	}

	inline void sort()
	{
		type_of_list list;
		while (first() != end())
		{
			iterator it = unlink_front();
			list.link_sorted(it);
		}
		// link back to old list;
		while (list.first() != list.end())
		{
			iterator it = list.unlink_front();
			link_back(it);
		}
	}
	//=============insert a empty node================================
	inline iterator insert_dummy_empty(type_of_node* pnode)
	{
		_first = _last = iterator(pnode);
		return _first;
	}
	inline iterator insert_dummy_tail(type_of_node* pnode)
	{
		_last.get_node()->next = pnode;
		pnode->pre = _last.get_node();
		_last = iterator(pnode);
		return iterator(pnode);
	}
	inline iterator insert_dummy_head(type_of_node* pnode)
	{
		_first.get_node()->pre = pnode;
		pnode->next = _first.get_node();
		_first = iterator(pnode);
		return iterator(pnode);
	}

	inline iterator insert_dummy(const iterator& position)
	{

		type_of_node* pnode = new_node();

		if (_first == iterator(NULL) && _last == iterator(NULL))
			return insert_dummy_empty(pnode);
		if (position == _first)
			return insert_dummy_head(pnode);
		if (position == end())
			return insert_dummy_tail(pnode);
		pnode->next = position.get_node();
		pnode->pre = position.get_node()->pre;
		position.get_node()->pre->next = pnode;
		position.get_node()->pre = pnode;
		return iterator(pnode);
	}
	//===========================================================
	inline type_of_node* new_node()
	{
		type_of_node* pnode =
			(type_of_node*)list_memory.alloc(sizeof(type_of_node));
		gvt_construct(pnode);
		return pnode;
	}
	//================================================================
	inline iterator add_dummy()
	{
		iterator it = this->insert_dummy(end());
		return it;
	};
	//================================================================
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
	};
	//================================================================
	inline iterator push_back(const T& t)
	{
		return this->add(t);
	};

	//================================================================
	inline iterator push_front(const T& t)
	{
		return this->insert(begin(), t);
	};
	//================================================================
	inline void pop_back()
	{
		return this->erase(_last, _last.get_node()->pre);
	};
	//================================================================
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
	//================================================================
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
	//================================================================
	inline void erase(iterator it, iterator it_pre)
	{
		unlink(it, it_pre);
		gvt_destroy(it.get_node());
		list_memory.free(it.get_node());
		// delete(list_memory, it.get_node());
		// don'twork study more!!
	}
	//================================================================
	inline void erase(iterator it)
	{
		GV_ASSERT(it.get_node());
		iterator it_pre = it;
		--it_pre;
		erase(it, it_pre);
	}
	//================================================================
	inline void pop_front()
	{
		this->erase(begin(), NULL);
	}
	//================================================================
	inline iterator begin()
	{
		return _first;
	}
	//================================================================
	inline iterator end()
	{
		return NULL;
	}
	//================================================================
	bool is_empty() const
	{
		return begin() == end();
	}
	//================================================================
	inline iterator last()
	{
		return _last;
	}
	//================================================================
	inline iterator first()
	{
		return _first;
	}
	//================================================================
	inline const_iterator begin() const
	{
		return _first;
	}
	//================================================================
	inline const_iterator end() const
	{
		return NULL;
	}
	//================================================================
	inline const_iterator last() const
	{
		return _last;
	}
	//================================================================
	inline void init(const T* p, gv_int isize)
	{
		for (int i = 0; i < isize; i++)
			this->add(*p++);
	}
	//================================================================
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
	//================================================================
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
	//================================================================
	template < class type_of_visitor >
	inline T* for_each_until(type_of_visitor& visitor)
	{
		return std::find_if(begin(), end(), visitor);
	}
	//================================================================
	inline void clear()
	{
		while (begin() != this->end())
		{
			erase(begin(), iterator());
		}
		return;
	}
	//================================================================
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
	//================================================================
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
	//================================================================
	inline type_of_list& operator=(const type_of_list& a)
	{
		if (this != &a)
		{
			this->clear();
			(*this) += a;
		}
		return *this;
	}
	//================================================================
	inline iterator unlink_all()
	{
		iterator f = _first;
		_first = _last = NULL;
		return f;
	}
	template < int array_size >
	inline void link_all(type_of_node array[array_size])
	{
		for (int i = 0; i < array_size; i++)
			link_back(iterator(&array[i]));
	}
	inline void link_all(type_of_node* array, int array_size)
	{
		for (int i = 0; i < array_size; i++)
			link_back(iterator(&array[i]));
	}
	inline iterator unlink_front()
	{
		iterator it = this->_first;
		unlink(it, end());
		return it;
	}
	inline iterator unlink_back()
	{
		iterator it = this->_last;
		if (it.get_node() == NULL)
			return it; // list is empty, don't unlink empty node..
		unlink(it, it.get_node()->pre);
		return it;
	}
	inline iterator link_front(iterator it)
	{
		link(it, end());
		return it;
	}
	inline iterator link_back(iterator it)
	{
		link(it, this->_last);
		return it;
	}
	inline iterator link(iterator it, iterator it_pre)
	{
		if (it.get_node() == NULL)
			return NULL; // don't link empty node..
		it.get_node()->pre = it_pre.get_node();
		if (it_pre == this->end())
		{
			// link_head
			if (this->_first.get_node())
				this->_first.get_node()->pre = it.get_node();
			it.get_node()->next = this->_first.get_node();
			this->_first = it;
		}
		else
		{
			if (it_pre.get_node()->next)
				it_pre.get_node()->next->pre = it.get_node();
			it.get_node()->next = it_pre.get_node()->next;
			it_pre.get_node()->next = it.get_node();
		}
		if (it_pre == _last)
			_last = it.get_node();
		return it;
	}
	inline iterator unlink(iterator it, iterator it_pre)
	{
		if (it.get_node() == NULL)
			return NULL; // don't unlink empty node..
		if (it_pre == this->end())
		{
			// unlink_head
			this->_first = it.get_node()->next;
			if (this->_first.get_node())
				this->_first.get_node()->pre = NULL;
		}
		else
		{
			it_pre.get_node()->next = it.get_node()->next;
			if (it.get_node()->next)
				it.get_node()->next->pre = it_pre.get_node();
		}
		if (it == _last)
			_last = it.get_node()->pre;
		it.get_node()->pre = it.get_node()->next = NULL;
		return it;
	}
	inline iterator unlink(iterator it)
	{
		if (it.get_node() == NULL)
			return NULL;
		iterator it_pre = it;
		--it_pre;
		return unlink(it, it_pre);
	}

protected:
	iterator _first;
	iterator _last;
	policy_memory list_memory;
};

template < typename T_node_ptr >
inline void gvt_link(T_node_ptr link, T_node_ptr& first, T_node_ptr& last)
{
	if (!first)
		first = link;
	else
		last->next = link;
	link->next = NULL;
	link->prev = last;
	last = link;
}

template < typename T_node_ptr >
inline void gvt_unlink(T_node_ptr link, T_node_ptr& first, T_node_ptr& last)
{
	if (!link->prev)
		first = link->next;
	else
		link->prev->next = link->next;
	if (!link->next)
		last = link->prev;
	else
		link->next->prev = link->prev;
}

template < typename T_node_ptr >
inline void gvt_clear_list(T_node_ptr& first)
{
	while (first)
	{
		T_node_ptr tmp = first;
		first = first->next;
		delete tmp;
	}
}

} // namespace gv