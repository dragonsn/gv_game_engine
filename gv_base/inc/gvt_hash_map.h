#pragma once
#include "gvt_slist_no_alloc.h"
namespace gv
{
template < class Key, class Data >
class gvt_hash_pair
{
public:
	typedef Key type_of_key;
	typedef Data type_of_data;
	inline gvt_hash_pair(){};
	inline gvt_hash_pair(const type_of_key& _key, gv_uint _complete_hash)
		: key(_key), complete_hash(_complete_hash)
	{
	}
	inline gvt_hash_pair(const type_of_key& _key, const type_of_data& _data,
						 int _complete_hash)
		: key(_key), data(_data), complete_hash(_complete_hash)
	{
	}
	inline gvt_hash_pair(const gvt_hash_pair& p)
	{
		(*this) = p;
	}
	inline bool operator==(const gvt_hash_pair< Key, Data >& other) const
	{
		if (complete_hash != other.complete_hash)
			return false;
		return key == other.key;
	}
	gvt_hash_pair< Key, Data >& operator=(const gvt_hash_pair< Key, Data >& other)
	{
		this->complete_hash = other.complete_hash;
		this->key = other.key;
		this->data = other.data;
		return *this;
	}

public:
	type_of_data data;
	type_of_key key;
	gv_uint complete_hash;
};

template <
	class Key, class Data, int size_of_map = 256,
	class policy_memory = gvt_policy_memory_default< gvt_hash_pair< Key, Data > >,
	class policy_thread_mode = gvt_policy_thread_mode< gvt_hash_pair< Key, Data > > >
class gvt_hash_map : public gvt_lockable_object_level< policy_thread_mode >
{
public:
	typedef Key type_of_key;
	typedef Data type_of_data;
	typedef gvt_hash_pair< Key, Data > hash_pair;
	typedef gvt_slist< hash_pair, policy_memory, policy_thread_mode > column;
	typedef typename gvt_slist< hash_pair, policy_memory,
								policy_thread_mode >::type_of_node type_of_node;
	typedef typename gvt_slist< hash_pair, policy_memory,
								policy_thread_mode >::iterator column_iterator;
	typedef typename gvt_slist< hash_pair, policy_memory,
								policy_thread_mode >::const_iterator
		const_column_iterator;
	typedef gvt_hash_map< Key, Data, size_of_map, policy_memory,
						  policy_thread_mode >
		type_of_map;

	class const_iterator
	{ // iterator for nonmutable list
	public:
		typedef Data value_type;
		typedef const Data* pointer;
		typedef const Data& reference;
		typedef const_column_iterator col_it;
		const_iterator()
			: const_it(0), hash_index(0)
		{ // construct with null node pointer
		}
		const_iterator(const type_of_map* _map, const_column_iterator _const_it,
					   gv_int _hash_index)
			: map(_map), const_it(_const_it), hash_index(_hash_index)
		{
		}

		reference operator*() const
		{ // return designated value
			GV_ASSERT(const_it.get_node());
			return const_it->data;
		}
		pointer operator->() const
		{ // return pointer to class object
			GV_ASSERT(const_it.get_node());
			return (&**this);
		}
		pointer data_ptr() const
		{ // return pointer to class object
			GV_ASSERT(const_it.get_node());
			return (&**this);
		}
		const_iterator& operator++()
		{ // preincrement
			if (!is_empty())
				++const_it;
			while (const_it == 0)
			{
				++hash_index;
				if (hash_index >= size_of_map)
					break;
				const_it = map->table[hash_index].begin();
			}
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
			return (const_it == _right.const_it && hash_index == _right.hash_index);
		}
		bool operator!=(
			const const_iterator& _right) const
		{ // test for iterator inequality
			return (!(*this == _right));
		}
		const Key& get_key() const
		{ // return node pointer
			return const_it->key;
		}
		bool is_empty()
		{
			return const_it == NULL;
		}

	protected:
		const_column_iterator const_it; // pointer to node
		gv_int hash_index;
		const type_of_map* map;
	};

	// CLASS iterator
	class iterator;
	friend class iterator;
	class iterator : public const_iterator
	{ // iterator for mutable list
	public:
		typedef Data value_type;
		typedef Data* pointer;
		typedef Data& reference;
		typedef column_iterator col_it;
		iterator()
		{ // construct with null node
		}
		iterator(const iterator& other)
		{
			(*this) = other;
		}
		iterator(type_of_map* _map, column_iterator _const_it, gv_int _hash_index)
			: const_iterator(_map, _const_it,
							 _hash_index)
		{ // construct with node pointer _pnode
		}
		reference operator*() const
		{ // return designated value
			return ((reference) * *(const_iterator*)this);
		}
		pointer data_ptr() const
		{ // return designated value
			return &((reference) * *(const_iterator*)this);
		}
		pointer operator->() const
		{ // return pointer to class object
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
		Key& get_key()
		{ // return node pointer
			return *((Key*)&this->const_it->key);
		}
	};

public:
	inline gvt_hash_map()
	{
		GV_ASSERT(GV_STRUCT_OFFSET(hash_pair, data) == 0);
	}
	inline gvt_hash_map(const type_of_map& other)
	{
		(*this) = other;
	}
	inline gv_uint hash(const type_of_key& key, gv_uint& full_hash) const
	{
		full_hash = gvt_hash(key);
		return full_hash % size_of_map;
	}
	inline type_of_data& operator[](const type_of_key& key)
	{
		type_of_data* p;
		if ((p = find(key)) != NULL)
			return *p;
		return *add(key, type_of_data());
	};
	inline type_of_data* find(const type_of_key& key)
	{
		gv_uint full_hash = 0;
		gv_uint this_hash = hash(key, full_hash);
		column_iterator it = table[this_hash].begin();
		while (it != table[this_hash].end())
		{
			if (it->key == key)
				return &it->data;
			++it;
		}
		// find(hash_pair(key,full_hash));
		return NULL;
	}
	inline const type_of_data* find(const type_of_key& key) const
	{
		gv_uint full_hash = 0;
		gv_uint this_hash = hash(key, full_hash);
		const_column_iterator it = table[this_hash].begin();
		while (it != table[this_hash].end())
		{
			if (it->key == key)
				return &it->data;
			++it;
		}
		// find(hash_pair(key,full_hash));
		return NULL;
	}
	inline bool find(const type_of_key& key, type_of_data& data)
	{
		type_of_data* pdata = this->find(key);
		if (pdata)
		{
			data = *pdata;
			return true;
		}
		return false;
	}

	inline type_of_data* add(const type_of_key& key, const type_of_data& data)
	{
		gv_uint full_hash = 0;
		gv_uint this_hash = hash(key, full_hash);
		return &table[this_hash].add(hash_pair(key, data, full_hash))->data;
	}
	inline bool erase(const type_of_key& key)
	{
		gv_uint full_hash = 0;
		gv_uint this_hash = hash(key, full_hash);
		return table[this_hash].erase(hash_pair(key, full_hash));
	}

	inline bool erase(iterator& it)
	{
		return this->erase(it.get_key());
	}

	inline void clear()
	{
		column* b = boost::begin(table);
		column* e = boost::end(table);
		while (b != e)
		{
			b->clear();
			++b;
		}
	}
	inline void reset()
	{
		clear();
	}
	inline iterator begin()
	{
		return iterator(this, table[0].begin(), 0);
	}
	inline const_iterator begin() const
	{
		return const_iterator(this, table[0].begin(), 0);
	}
	inline iterator end()
	{
		column_iterator it(0);
		return iterator(this, it, size_of_map);
	}
	inline const_iterator end() const
	{
		return const_iterator(this, NULL, size_of_map);
	}
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor)
	{
		iterator it = begin();
		while (it != end())
		{
			if (!it.is_empty())
				visitor(it.get_key(), *it);
			++it;
		}
	}
	template < class type_of_visitor >
	inline void for_each(type_of_visitor& visitor) const
	{
		const_iterator it = begin();
		while (it != end())
		{
			if (!it.is_empty())
				visitor(it.get_key(), *it);
			++it;
		}
	}

	type_of_map&
	operator=(const type_of_map& other)
	{ // test for iterator equality

		const_iterator it = other.begin();
		while (it != other.end())
		{
			if (!it.is_empty())
				add(it.get_key(), *it);
			++it;
		}
		return *this;
	}

	column& get_column(int i)
	{
		GV_ASSERT(i < size_of_map);
		return table[i];
	}

	template < class key_container, class data_container >
	gv_int query_all(key_container& keys, data_container& datas)
	{
		gv_int count = 0;
		iterator it = begin();
		while (it != end())
		{
			if (!it.is_empty())
			{
				keys.push_back(it.get_key());
				datas.push_back(*it);
				++count;
			}
			++it;
		}
		return count;
	}

private:
	column table[size_of_map];
};

} // namespace gv