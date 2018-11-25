#pragma once
#include "gv_string.h"
#include "gvt_hash_map.h"

namespace gv
{
static const int gv_id_cache_size = 64;
inline gv_string_tmp gv_filter_invalid_id(const char* id)
{
	gv_string_tmp buffer;
	if (!gvt_is_letter(id[0]) && id[0] != '_' && id[0])
	{
		buffer = "_";
	}
	int size = (int)strlen(id);
	for (int i = 0; i < size; i++)
	{
		if (!gvt_is_id_content(id[i]))
		{
			buffer << "_";
		}
		else
			buffer += id[i];
	}
	return buffer;
}
typedef gvt_string< gv_char, gvt_memory_cached< gv_id_cache_size >,
					gvt_policy_size_calculator_cached<
						gv_id_cache_size, gvp_array_size_calculator_string > >
	gv_string_id_base;

struct gv_id_flags
{
	gv_id_flags()
	{
		GVM_ZERO_ME;
	}
	gv_uint m_is_log_suppressed : 1;
};

class gv_string_id : public gv_string_id_base
{
public:
	inline gv_string_id(){};
	inline gv_string_id(const gv_char* p)
		: gv_string_id_base(p){};
	inline gv_string_id(const gv_string_id& s)
		: gv_string_id_base(*s)
	{
	}

public:
	gv_id_flags m_flags;
};

inline gv_uint gvt_hash(const gv_string_id& t)
{
	return t.hash();
}

class gv_id
{
public:
	static const int c_hash_map_size = 4096;
	typedef gvt_hash_map< gv_string_id, gv_atomic_count, c_hash_map_size,
						  gvp_memory_default, gvp_multi_thread >
		type_of_map;
	typedef gv_string_id type_of_string;
	gv_id()
	{
		ppair = NULL;
	};
	explicit gv_id(const char* name)
	{
		ppair = NULL;
		(*this) = name;
	};

	gv_id(const gv_id& id)
	{
		pdata = id.pdata;
		if (pdata)
			++(*pdata);
	};

	bool is_empty() const
	{
		return ppair == NULL;
	}

	gv_id& operator=(const gv_id& id)
	{
		if (pdata)
			--(*pdata);
		pdata = id.pdata;
		if (pdata)
			++(*pdata);
		return *this;
	}

	gv_id& operator=(const char* name)
	{
		bool is_empty_name = (!name || !strlen(name));
		if (!pdata)
		{
			if (is_empty_name)
				return *this;
		}
		gvt_scope_lock< type_of_map > lock(s_map);
		if (pdata)
			--(*pdata);
		if (!is_empty_name)
		{
			gv_string_tmp s = gv_filter_invalid_id(name);
			pdata = &s_map[gv_string_id(*s)];
			++(*pdata);
		}
		else
			pdata = NULL;
		return *this;
	}
	bool operator==(const char* name) const
	{
		return string() == name;
	}

	bool operator!=(const char* name) const
	{
		return string() != name;
	}

	bool operator==(const gv_id& id) const
	{
		return pdata == id.pdata;
	}

	bool operator!=(const gv_id& id) const
	{
		return pdata != id.pdata;
	}

	bool operator<(const gv_id& id) const
	{
		return string() < id.string();
	}

	gv_int get_refcount() const
	{
		if (ppair)
			return ppair->data.get();
		else
			return 0;
	}

	const type_of_string& string() const
	{
		static type_of_string s;
		if (ppair)
			return ppair->key;
		else
			return s;
	}

	type_of_string& string()
	{
		static type_of_string s;
		if (ppair)
			return ppair->key;
		else
			return s;
	}

	inline const gv_char* operator*() const
	{
		return *string();
	}
	/*
  inline operator	const gv_char *() const
  {
          return *string();
  };*/

	~gv_id()
	{
		release();
	};

	void release()
	{
		if (pdata)
			--(*pdata);
		pdata = NULL;
	}

	static void static_init(){

	};

	static void static_purge()
	{
		gvt_scope_lock< type_of_map > lock(s_map);
		type_of_map::iterator it = s_map.begin();
		type_of_map::iterator next;
		while (it != s_map.end())
		{
			next = it;
			++next;
			if (!it.is_empty())
			{
				if (it->get() == 0)
				{
					s_map.erase(it);
				}
			}
			it = next;
		}
	};

	static void static_destroy()
	{
		static_purge();
		type_of_map::iterator it = s_map.begin();
		++it;
		GV_ASSERT(it == s_map.end());
	};

private:
	static type_of_map s_map;

	type_of_map::iterator get_it()
	{
		GV_ASSERT(ppair);
		type_of_map::column_iterator it(pnode);
		return type_of_map::iterator(&s_map, it,
									 ppair->complete_hash % c_hash_map_size);
	}

	union {
		type_of_map::hash_pair* ppair;
		type_of_map::type_of_node* pnode;
		type_of_map::type_of_data* pdata;
	};
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s, const gv_id& id)
{
	gv_string_tmp s_tmp = *id.string();
	s << s_tmp;
	return s;
}

template < class type_of_stream >
inline const type_of_stream& operator>>(const type_of_stream& s, gv_id& id)
{
	gv_string temp;
	s >> temp;
	id = *temp;
	return s;
}

template < class type_of_stream >
inline type_of_stream& operator>>(type_of_stream& s, gv_id& id)
{
	gv_string temp;
	s >> temp;
	id = *temp;
	return s;
}

inline gv_uint gvt_hash(const gv_id& id)
{
	return (gv_uint)(size_t)(&id.string()) / 23;
}

#define GVM_MAKE_ID_DCL 1
#define GVM_MAKE_ID_IMP 2
#define GVM_MAKE_ID_INIT 3
#define GVM_MAKE_ID_RELEASE 4
}
