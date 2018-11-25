#pragma once

namespace gv
{
//=============================================================================>
// gv_object ::
//	the base class for game objects and other object need dynamic type
//[concept] member reflection , auto serialization , auto XML porting .
//[unique location]	unique location for all the persistent object to solve
//the reference.
//[RTTI][class info] each object have a class descriptor member, which describe
//the object's type, script function interface, and data members
//[auto serialization and version conversion]
//[event handler automatic binding] automatic bind object with interest event.
//[script interface]
//=============================================================================>
class gv_class_info; // tricky , the class is also a gv_object for dynamic type
class gv_module;
class gv_object : public gv_refable
{
	friend class gv_sandbox;
	friend class gv_class_info;
	friend class gv_object_iterator;

public:
	GVM_DCL_BASE_CLASS(gv_object);

	gv_object();

	virtual ~gv_object();

	virtual gv_int inc_ref();

	virtual gv_int dec_ref();

	virtual void clear_ref_to_others(){};

	virtual bool read(gvi_stream* stream);

	virtual bool write(gvi_stream* stream);

	virtual bool import_object_xml(gv_xml_parser* parser, gv_sandbox* sandbox,
								   gv_module* module);

	virtual bool import_object_xml_in_memory(const gv_string_tmp& str);

	virtual bool export_object_xml(gv_string_tmp& str);

	virtual bool import_object_txt(gv_xml_parser* parser, gv_sandbox* sandbox,
								   gv_module* module);

	virtual bool export_object_txt(gv_string_tmp& text);

	virtual gv_int import_from_external_file(const char* file_name,
											 gv_text& error)
	{
		error = "not supported import the file to ";
		get_name_id();
		return 0;
	}

	virtual bool load_default();

	// clamp var if out of range!!
	virtual bool clamp_var();

	virtual void register_events(){};

	virtual void unregister_events(){};

	// called after load .
	virtual bool post_load()
	{
		return true;
	};

	inline gv_object_handle get_handle()
	{
		return gv_object_handle(this);
	}

	inline void set_owner(gv_object* p)
	{
		this->m_owner = p;
	}

	inline gv_object* get_owner()
	{
		return m_owner;
	}

	inline const gv_object* get_owner() const
	{
		return m_owner;
	}

	inline gv_uint get_runtime_index() const
	{
		return this->m_runtime_index;
	}

	inline gv_sandbox* get_sandbox()
	{
		return this->m_sandbox;
	}

	inline gv_sandbox* get_super_sandbox()
	{
		return this->m_sandbox ? this->m_sandbox->get_outer() : 0;
	}

	inline gv_class_info* get_class()
	{
		return m_class;
	}

	inline const gv_object_name& get_name() const
	{
		return this->m_name;
	}

	inline const gv_id& get_name_id() const
	{
		return this->m_name.get_id();
	}

	inline const gv_sandbox* get_sandbox() const
	{
		return this->m_sandbox;
	}

	inline const gv_class_info* get_class() const
	{
		return m_class;
	}

	inline void set_sandbox(gv_sandbox* p)
	{
		this->m_sandbox = p;
	}

	bool is_owned_by(gv_object* object) const;

	bool is_a(const gv_id& class_name) const;

	bool is_a(gv_class_info* cls) const;

	void link_class(gv_class_info* cls);

	gv_module* get_module();

	void rename(const gv_object_name& name, bool update_hash = true);

	void rename(const gv_id& obj_name);

	void rename(const char* obj_name);

	bool is_in_location(const gv_object_location& loc) const;

	void push_my_location(gv_object_location& l) const;

	gv_string_tmp get_location_string() const;

	gv_object_location get_location() const;

	inline bool is_deleted() const
	{
		return m_is_deleted;
	}

	inline bool is_in_sandbox() const
	{
		return m_is_in_sandbox;
	}

	inline bool is_nameless() const
	{
		return !m_is_in_sandbox;
	}

	inline bool is_native() const
	{
		return m_is_native;
	}

	static int static_compare_ptr(gv_object** p1, gv_object** p2);

	static bool static_is_valid(const gv_object* obj);

	// script here
	GVM_DCL_FUNC(scp_debug_break);
	GVM_DCL_FUNC(scp_log);
	GVM_DCL_FUNC(scp_import_file);
	GVM_DCL_FUNC(scp_rename);

protected:
	gv_object_name m_name;
	gvt_ptr< gv_class_info > m_class;
	gvt_ptr< gv_object > m_owner;
	gvt_ptr< gv_sandbox > m_sandbox;
	union {
		gv_uint m_flags;
		struct
		{
			gv_uint m_is_native : 1;
			gv_uint m_is_changed : 1;
			gv_uint m_is_deleted : 1;
			gv_uint m_is_in_sandbox : 1;
		};
	};
	gv_uint m_runtime_index;
	gvt_ptr< gv_object > m_next;
};

//=============================================================================>
template < class type_of_object >
inline type_of_object* gvt_cast(gv_object* pobj)
{
	if (pobj && pobj->is_a(type_of_object::static_class()))
		return (type_of_object*)pobj;
	return NULL;
}

template < class type_of_object >
inline const type_of_object* gvt_cast(const gv_object* pobj)
{
	if (pobj && pobj->is_a(type_of_object::static_class()))
		return (type_of_object*)pobj;
	return NULL;
}

//=============================================================================>
template < class type_of_object >
class gvt_object_type_list
{
public:
	typedef gvt_dlist< gvt_ref_ptr< type_of_object > > objects_list;
	typedef gvt_array< objects_list > objects_type_list;
	// sort the objects by its type . a utility to manager objects, to do batch
	// and job sorting.
	//---------------------------------------------------------------------
	gvt_object_type_list()
	{
	}
	~gvt_object_type_list()
	{
	}
	//---------------------------------------------------------------------
	template < class type_of_visitor >
	void for_each(type_of_visitor& vistor)
	{
		for (int i = 0; i < this->m_objects.size(); i++)
		{
			typename objects_list::iterator it = m_objects[i].begin();
			typename objects_list::iterator it_end = m_objects[i].end();
			while (it != it_end)
			{
				vistor(*it);
				++it;
			}
		}
	};

	template < class type_of_visitor >
	void for_each_delete(type_of_visitor& vistor)
	{
		for (int i = 0; i < this->m_objects.size(); i++)
		{
			typename objects_list::iterator it = m_objects[i].begin();
			typename objects_list::iterator it_end = m_objects[i].end();
			while (it != it_end)
			{
				gv_bool b = vistor(*it);
				typename objects_list::iterator it_bak = it;
				++it;
				if (!b)
				{
					m_objects[i].erase(it_bak);
					if (m_objects[i].is_empty())
					{
						this->m_objects.erase(i);
						i--;
					}
				}
			}
		}
	};

	//---------------------------------------------------------------------
	template < class type_of_visitor >
	void for_each(type_of_visitor& vistor, gv_class_info* class_info)
	{
		for (int i = 0; i < this->m_objects.size(); i++)
		{
			typename objects_list::iterator it = m_objects[i].begin();
			typename objects_list::iterator it_end = m_objects[i].end();
			if (it != it_end && it->get_class() != class_info)
				continue;
			while (it != it_end)
			{
				vistor(*it);
				++it;
			}
		}
	};
	//---------------------------------------------------------------------
	void add(type_of_object* ptr)
	{
		for (int i = 0; i < this->m_objects.size(); i++)
		{
			// TODO BATCH !!
			typename objects_list::iterator it = m_objects[i].begin();
			GV_ASSERT((*it));
			if ((*it)->get_class() == ptr->get_class())
			{
				m_objects[i].push_back(ptr);
				return;
			}
		}
		m_objects.push_back(objects_list());
		m_objects.last()->push_back(ptr);
	};
	//---------------------------------------------------------------------
	bool remove(type_of_object* ptr)
	{
		for (int i = 0; i < this->m_objects.size(); i++)
		{
			typename objects_list::iterator it = m_objects[i].begin();
			GV_ASSERT((*it)); // SHOULD NOT BE A EMPTY LIST!
			if ((*it)->get_class() == ptr->get_class())
			{
				m_objects[i].erase(ptr);
				if (m_objects[i].is_empty())
				{
					this->m_objects.erase(i);
				}
				return true;
			}
		}
		return false;
	};

protected:
	objects_type_list m_objects;
};

//=============================================================================>
typedef gvt_ref_ptr< gv_object > gv_object_ptr;
//=============================================================================>
struct gv_obj_node
{
	inline gv_obj_node(gv_object* _object)
	{
		object = _object;
	}
	inline gv_obj_node()
	{
	}
	inline gv_obj_node(const gv_obj_node& node)
	{
		*(this) = node;
	}
	inline gv_obj_node& operator=(const gv_obj_node& node)
	{
		next = node.next;
		prev = node.prev;
		object = node.object;
		return *this;
	}
	inline bool operator==(const gv_obj_node& node)
	{
		return object == node.object;
	}
	gvt_ptr< gv_obj_node > next;
	gvt_ptr< gv_obj_node > prev;
	gvt_ptr< gv_object > object;
};

typedef gvt_ptr< gv_obj_node > gv_obj_node_ptr;
struct gv_obj_list
{
	gv_obj_list(){};
	~gv_obj_list(){};
	gv_obj_node_ptr first;
	gv_obj_node_ptr last;

	class iterator
	{ // iterator for nonmutable list
	public:
		iterator()
			: ptr_node(0)
		{ // construct with null node pointer
		}
		iterator(const gv_obj_node_ptr _pnode)
			: ptr_node(_pnode)
		{ // construct with node pointer _pnode
		}
		gv_object& operator*() const
		{ // return designated value
			GV_ASSERT(ptr_node);
			return *ptr_node->object;
		}
		gv_object* operator->() const
		{ // return pointer to class object
			GV_ASSERT(ptr_node);
			return (&**this);
		}
		gv_object* ptr()
		{
			return (&**this);
		}
		iterator& operator++()
		{ // preincrement
			GV_ASSERT(ptr_node);
			ptr_node = ptr_node->next;
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
			GV_ASSERT(ptr_node);
			ptr_node = ptr_node->prev;
			return (*this);
		}
		iterator operator--(int)
		{ // postincrement
			iterator _tmp = *this;
			--*this;
			return (_tmp);
		}
		bool
		operator==(const iterator& _right) const
		{ // test for iterator equality
			return (ptr_node == _right.ptr_node);
		}
		bool
		operator!=(const iterator& _right) const
		{ // test for iterator inequality
			return (!(*this == _right));
		}
		const gv_obj_node_ptr get_node() const
		{ // return node pointer
			return (ptr_node);
		}

	protected:
		gv_obj_node_ptr ptr_node; // pointer to node
	};

	inline void unlink_all()
	{
		while (first)
			unlink(first);
	}
	inline void link(gv_obj_node_ptr node)
	{
		gvt_link(node, first, last);
	}
	inline void unlink(gv_obj_node_ptr node)
	{
		GV_ASSERT_SLOW(find(node));
		gvt_unlink(node, first, last);
	}
	inline bool find(gv_obj_node_ptr n)
	{
		gv_obj_node_ptr node = first;
		while (node)
		{
			if ((*node) == (*n))
			{
				return true;
			}
			node = node->next;
		}
		return false;
	}

	iterator begin()
	{
		return iterator(first);
	}

	iterator end()
	{
		return iterator(0);
	}

	bool is_empty()
	{
		iterator it = begin();
		return it.get_node() == NULL;
	}
};
//============================================================================================
//								:
//============================================================================================
class gv_object_iterator_safe
{
public:
	gv_object_iterator_safe(gv_sandbox* sandbox)
		: m_it(sandbox)
	{
		if (!m_it.is_empty() && m_it->is_deleted())
		{
			++(*this);
		}
	};
	bool is_empty()
	{
		return m_it.is_empty();
	};
	bool operator==(const gv_object_iterator_safe& o)
	{
		return o.m_it == m_it;
	}
	gv_object_iterator_safe& operator++()
	{
		if (is_empty())
			return *this;
		++m_it;
		while (!is_empty() && m_it->is_deleted())
		{
			++m_it;
		}
		return *this;
	};
	operator gv_object*() const
	{
		return (gv_object*)(m_it);
	}
	gv_object* operator->() const
	{
		return (gv_object*)(m_it);
	}

protected:
	gv_object_iterator m_it;
};

template < class T >
class gvt_object_iterator
{
public:
	gvt_object_iterator(gv_sandbox* sandbox)
		: m_it(sandbox)
	{
		if (!m_it.is_empty() && !m_it->is_a(T::static_class()))
		{
			++(*this);
		}
	};

	bool is_empty()
	{
		return m_it.is_empty();
	};
	bool operator==(const gvt_object_iterator& o)
	{
		return o.m_it == m_it;
	}
	gvt_object_iterator& operator++()
	{
		if (is_empty())
			return *this;
		++m_it;
		while (!is_empty() && !m_it->is_a(T::static_class()))
		{
			++m_it;
		}
		return *this;
	};
	T* ptr()
	{
		return (T*)(gv_object*)(m_it);
	}
	operator T*() const
	{
		return (T*)(gv_object*)(m_it);
	}
	T* operator->() const
	{
		return (T*)(gv_object*)(m_it);
	}

protected:
	gv_object_iterator_safe m_it;
};

template < class T >
class gvt_object_iterator_with_owner
{
public:
	gvt_object_iterator_with_owner(gv_sandbox* sandbox, gv_object* owner)
		: m_it(sandbox)
	{
		m_owner = owner;
		if (!m_it.is_empty() && !m_it->is_owned_by(m_owner))
		{
			++(*this);
		}
	};
	bool is_empty()
	{
		return m_it.is_empty();
	};
	bool operator==(const gvt_object_iterator_with_owner& o)
	{
		return o.m_it == m_it;
	}
	gvt_object_iterator_with_owner& operator++()
	{
		if (is_empty())
			return *this;
		++m_it;
		while (!is_empty() && !m_it->is_owned_by(m_owner))
		{
			++m_it;
		}
		return *this;
	};
	T* ptr()
	{
		return (T*)(gv_object*)(m_it);
	}
	operator T*() const
	{
		return (T*)(gv_object*)(m_it);
	}
	T* operator->() const
	{
		return (T*)(gv_object*)(m_it);
	}

protected:
	gvt_object_iterator< T > m_it;
	gv_object* m_owner;
};
};