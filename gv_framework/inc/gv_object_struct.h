#pragma once
namespace gv
{
static const gv_ushort gvc_max_sand_box_nb = 4096;
static const gv_uint gvc_invalid_object_index = 0xffffffff;
class gv_object;
class gv_sandbox;

//=============================================================================>
// the object name consisted of two part to reduce id  like "gv_actor0001",
// new id is expensive.
//=============================================================================>
class gv_object_name
{
	friend gv_uint gvt_hash(const gv_object_name& name);
	friend class gv_object;

public:
	gv_object_name()
	{
		m_id = gv_id_none;
		m_postfix = 0;
	};
	gv_object_name(const gv_id& id, gv_uint _postfix)
		: m_id(id), m_postfix(_postfix)
	{
	}
	gv_object_name(const gv_id& id)
		: m_id(id), m_postfix(0)
	{
	}
	explicit gv_object_name(const char* name)
		: m_id(name), m_postfix(0)
	{
	}
	gv_object_name(const gv_object_name& _name)
	{
		m_id = _name.m_id;
		m_postfix = _name.m_postfix;
	}
	~gv_object_name(){};

	const gv_uint get_postfix() const
	{
		return m_postfix;
	}
	const gv_id& get_id() const
	{
		return m_id;
	}
	inline bool operator==(const gv_object_name& name) const
	{
		if (m_postfix != name.m_postfix)
			return false;
		if (m_id != name.m_id)
			return false;
		return true;
	}
	inline bool operator!=(const gv_object_name& name) const
	{
		return !(*this == name);
	}
	inline bool operator<(const gv_object_name& name) const
	{
		if (m_id.string() < name.m_id.string())
			return true;
		if (m_id.string() == name.m_id.string())
		{
			if (m_postfix < name.m_postfix)
				return true;
		}
		return false;
	}
	gv_object_name& operator=(const gv_object_name& name)
	{
		m_postfix = name.m_postfix;
		m_id = name.m_id;
		return *this;
	}
	bool is_any()
	{
		return m_id == gv_id_any;
	}
	void set_any()
	{
		m_id = gv_id_any;
	}

	gv_id m_id;
	gv_uint m_postfix;
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gv_object_name& name)
{
	s << name.m_id << ":" << name.m_postfix;
	return s;
}

template < class type_of_stream >
inline const type_of_stream& operator>>(type_of_stream& s,
										gv_object_name& name)
{
	s >> name.m_id >> ":" >> name.m_postfix;
	return s;
}

inline gv_uint gvt_hash(const gv_object_name& name)
{
	return gvt_hash(name.m_id) + name.m_postfix * 13;
}
//=============================================================================>
// the unique location of persistent objects
// sample1. a persistent texture object.
//		pingball.0.texture.1.level.1.seashore.0
// sample2. a temp spawned object , still useful for save game & load game
//		$sandbox.
//=============================================================================>
class gv_object_location
{
	friend class gv_object;
	friend class gv_sandbox;

public:
	gv_object_location(){};
	gv_object_location(const gv_object_location& loc)
	{
		*this = loc;
	};
	gv_object_location(const gvt_array< gv_object_name >& loc)
	{
		this->m_object_location.resize(loc.size());
		gvt_for_each_copy(this->m_object_location, loc);
	};
	~gv_object_location(){};

	void clear()
	{
		m_object_location.clear();
	}

	inline bool operator==(const gv_object_location& loc) const
	{
		if (m_object_location != loc.m_object_location)
			return false;
		return true;
	}
	inline bool operator!=(const gv_object_location& loc) const
	{
		return !(*this == loc);
	}
	inline bool operator<(const gv_object_location& loc) const
	{
		if (m_object_location.size() < loc.m_object_location.size())
			return true;
		if (m_object_location.size() > loc.m_object_location.size())
			return false;
		if (m_object_location < loc.m_object_location)
			return true;
		return false;
	}
	inline gv_object_location& operator=(const gv_object_location& loc)
	{
		m_object_location = loc.m_object_location;
		return *this;
	}
	void from_sting(const gv_string_tmp& s);
	const gv_object_name& operator[](gv_int i) const
	{
		return m_object_location[i];
	}
	gv_object_name& operator[](gv_int i)
	{
		return m_object_location[i];
	}
	gv_int size() const
	{
		return m_object_location.size();
	}
	gvt_array_cached< gv_object_name, 32 > m_object_location;
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gv_object_location& loc)
{
	s << "(" << loc.m_object_location.size() << ")";
	for (int i = 0; i < loc.m_object_location.size(); i++)
	{
		s << loc.m_object_location[i];
		if (i < loc.m_object_location.size() - 1)
			s << "/";
	}
	return s;
}

template < class type_of_stream >
inline const type_of_stream& operator>>(type_of_stream& s,
										gv_object_location& loc)
{
	gv_int depth = 0;
	s >> "(" >> depth >> ")";
	loc.m_object_location.resize(depth);
	for (int i = 0; i < loc.m_object_location.size(); i++)
	{
		s >> loc.m_object_location[i];
		if (i < loc.m_object_location.size() - 1)
			s >> "/";
	}
	return s;
}

inline gv_uint gvt_hash(const gv_object_location& loc)
{
	gv_uint hash = 0;
	const static int max_hash_loc = 3;
	gv_uint primes[max_hash_loc] = {151, 41, 1};
	if (loc.size())
	{
		gv_int steps = gvt_min(max_hash_loc, loc.size());
		for (int i = 0; i < steps; i++)
		{
			hash += gvt_hash(loc[loc.size() - i - 1]) * primes[i];
		}
		hash += loc.size();
	}
	return hash;
}

class gv_object_location_with_ptr : public gv_object_location
{
public:
	gv_object_location_with_ptr()
	{
		m_cached = false;
	}
	gv_object_location_with_ptr(const gv_object_location& loc)
		: gv_object_location(loc)
	{
		m_cached = false;
	}
	gvt_ptr< gv_object > m_ptr;
	gv_bool m_cached;
};

inline gv_uint gvt_hash(const gv_object_location_with_ptr& loc)
{
	if (loc.size())
		return gvt_hash(loc[loc.size() - 1]);
	return 0;
}
//=============================================================================>
// more safe pointer for sandbox ..
//=============================================================================>
class gv_sandbox_handle
{
	friend class gv_sandbox;
	friend class gv_sandbox_manager;

public:
	gv_sandbox_handle()
	{
		m_sandbox_id = gvc_max_sand_box_nb;
		m_sandbox_uuid = 0;
	}
	bool is_valid();
	inline bool operator==(const gv_sandbox_handle& loc) const
	{
		if (m_sandbox_id != loc.m_sandbox_id)
			return false;
		if (m_sandbox_uuid != loc.m_sandbox_uuid)
			return false;
		return true;
	}
	inline bool operator!=(const gv_sandbox_handle& loc) const
	{
		return !(*this == loc);
	}
	inline bool operator<(const gv_sandbox_handle& loc) const
	{
		if (m_sandbox_id < loc.m_sandbox_id)
			return true;
		if (m_sandbox_uuid < loc.m_sandbox_uuid)
			return true;
		return false;
	}
	void init(gv_sandbox*);
	gv_sandbox* get_sandbox();

protected:
	gv_ushort m_sandbox_id;
	gv_uint m_sandbox_uuid;
};
//=============================================================================>
// more safe pointer for object , easy to check valid or not ..
//=============================================================================>
class gv_object_handle
{
	friend class gv_object;
	friend class gv_sandbox;

public:
	gv_object_handle();
	explicit gv_object_handle(gv_object* obj);
	gv_object_handle(const gv_object_handle& hd);
	bool is_any();
	void set_any();
	bool operator==(const gv_object_handle& loc) const;
	bool operator!=(const gv_object_handle& loc) const;
	bool operator<(const gv_object_handle& loc) const;
	gv_object_handle& operator=(const gv_object_handle& hd);
	void invalidate();
	bool is_valid();
	gv_object* get_object();

protected:
	gv_object_name m_name;
	gv_uint m_index;
	gvt_ptr< gv_object > m_object;
	gv_sandbox_handle m_sandbox;
};

//=============================================================================>
class gv_enum_info : public gv_refable
{
public:
	gv_enum_info();

public:
	void add_pair(gv_int enum_val, const gv_id& name);
	gv_int get_int(const gv_id& name) const;
	gv_id get_id(gv_int) const;
	const gv_id& get_name() const;
	void set_name(const gv_id& name);
	void query_all(gvt_array< gv_id >& a1, gvt_array< gv_int >& a2);

protected:
	gv_id m_name;
	gvt_dictionary< gv_id, gv_int > m_dict;
};

#define GV_DOM_FILE "gv_func_param_structs.h"
#define GVM_DOM_DECL
#define GVM_DOM_IMP_IS_EQUAL
#define GVM_DOM_IMP_XML
#define GVM_DOM_IMP_SERIALIZE
#define GVM_DOM_STATIC_CLASS
#define GVM_DOM_IMP_COPY
#include "../inc/gv_data_model_ex.h"
}
