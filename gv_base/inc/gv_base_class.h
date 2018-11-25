#pragma once
// some useful  class
#include "gv_atomic_count.h"

namespace gv
{
// call guard& unguard of specific object into scope & out of scope
template < class T >
class gvt_scope_guard
{
	T& the_guard;

public:
	gvt_scope_guard(T& t)
		: the_guard(t)
	{
		the_guard.guard();
	}
	~gvt_scope_guard()
	{
		the_guard.unguard();
	}
};

template < class T >
class gvt_scope_lock
{
	T& the_lock;
	gvt_scope_lock(const gvt_scope_lock& l){};

public:
	gvt_scope_lock(T& t)
		: the_lock(t)
	{
		the_lock.lock();
	}
	~gvt_scope_lock()
	{
		the_lock.unlock();
	}
};

//
template < class T >
class gvt_factory
{
public:
	gvt_factory(){};
	virtual ~gvt_factory()
	{
	}

public:
	virtual T* create()
	{
		return new T;
	};
	virtual void destroy(T* p)
	{
		delete p;
	};
};

//
template < class T, class Implement >
class gvt_factory_override : public gvt_factory< T >
{
public:
	gvt_factory_override(){};
	virtual ~gvt_factory_override()
	{
	}

public:
	virtual T* create()
	{
		return static_cast< T* >(new Implement);
	};
	virtual void destroy(T* p)
	{
		delete p;
	};
};

class gv_refable
{
public:
	gv_refable()
		: m_ref_count(0)
	{
	}
	virtual ~gv_refable()
	{
		GV_ASSERT(m_ref_count.get() == 0);
	}
	virtual gv_int inc_ref()
	{
		return ++m_ref_count;
	};
	virtual gv_int dec_ref()
	{
		if (--m_ref_count == 0)
		{
			delete this;
			return 0;
		}
		return m_ref_count.get();
	}
	virtual gv_int get_ref() const
	{
		return m_ref_count.get();
	}

protected:
	gv_atomic_count m_ref_count;
};

class gv_refable_with_factory : public gv_refable
{
public:
	gv_refable_with_factory()
		: m_factory(0)
	{
	}
	virtual gv_int dec_ref()
	{
		if (--m_ref_count == 0)
		{
			if (m_factory)
				m_factory->destroy(this);
			else
				delete this;
			return 0;
		}
		return m_ref_count.get();
	}
	void set_factory(gvt_factory< gv_refable_with_factory >* factory)
	{
		m_factory = factory;
	}

protected:
	gvt_factory< gv_refable_with_factory >* m_factory;
};

// our_version shared ptr , so
template < class type_of_data >
class gvt_ref_ptr
{
public:
	gvt_ref_ptr()
		: m_ptr(NULL)
	{
	}
	gvt_ref_ptr(type_of_data* in_ptr)
	{
		m_ptr = in_ptr;
		if (in_ptr)
			m_ptr->inc_ref();
	}
	gvt_ref_ptr(const gvt_ref_ptr& in_ptr)
	{
		m_ptr = in_ptr.m_ptr;
		if (m_ptr)
		{
			m_ptr->inc_ref();
		}
	}
	~gvt_ref_ptr()
	{
		if (m_ptr)
		{
			m_ptr->dec_ref();
		}
	}

	inline gvt_ref_ptr& operator=(type_of_data* in_ptr)
	{
		type_of_data* older_ptr = m_ptr;
		m_ptr = in_ptr;
		if (m_ptr)
		{
			m_ptr->inc_ref();
		}
		if (older_ptr)
		{
			older_ptr->dec_ref();
		}
		return *this;
	}

	inline gvt_ref_ptr& operator=(const gvt_ref_ptr& p)
	{
		return * this = p.m_ptr;
	}

	inline bool operator==(const gvt_ref_ptr& Other) const
	{
		return m_ptr == Other.m_ptr;
	}

	inline bool operator==(type_of_data* other) const
	{
		return m_ptr == other;
	}

	inline type_of_data* operator->() const
	{
		GV_ASSERT(m_ptr);
		return m_ptr;
	}

	inline operator type_of_data*() const
	{
		return m_ptr;
	}

	inline type_of_data* ptr()
	{
		return m_ptr;
	}

	inline const type_of_data* ptr() const
	{
		return m_ptr;
	}

	void*& void_ptr()
	{
		return *((void**)&m_ptr);
	}

private:
	type_of_data* m_ptr;
};

template < class type_of_data >
inline type_of_data* get_pointer(const gvt_ref_ptr< type_of_data >& ptr)
{
	return (type_of_data*)ptr;
}

template < class T >
class gvt_ptr
{
public:
	inline gvt_ptr()
	{
		p = NULL;
	}
	inline gvt_ptr(T* _p)
	{
		p = _p;
	}
	inline ~gvt_ptr()
	{
		;
	}
	inline operator T*() const
	{
		return p;
	}
	inline T* operator->() const
	{
		GV_ASSERT(p);
		return p;
	}
	inline bool operator!=(const gvt_ptr< T >& _p) const
	{
		return p != _p.p;
	}
	inline bool operator>(const gvt_ptr< T >& _p) const
	{
		return p > _p.p;
	}
	inline bool operator<(const gvt_ptr< T >& _p) const
	{
		return p < _p.p;
	}
	inline gvt_ptr< T >& operator=(const gvt_ptr< T >& _p)
	{
		p = _p.p;
		return *this;
	}
	inline gvt_ptr< T >& operator=(T* _p)
	{
		p = _p;
		return *this;
	}
	inline T* ptr()
	{
		return p;
	}
	inline void*& void_ptr()
	{
		return *((void**)&p);
	}

protected:
	T* p;
};

struct gv_time_stamp
{
	gv_time_stamp()
	{
		m_u64 = 0;
	};
	gv_time_stamp(gv_ulong u)
	{
		m_u64 = u;
	};
	bool operator<(const gv_time_stamp& t)
	{
		return m_u64 < t.m_u64;
	}
	bool operator==(const gv_time_stamp& t)
	{
		return m_u64 == t.m_u64;
	}
	gv_time_stamp& operator=(const gv_time_stamp& t)
	{
		m_u64 = t.m_u64;
		return *this;
	}
	gv_ulong m_u64;
};

struct gv_pooled_struct_16
{
	void* operator new(size_t size);
	void operator delete(void*);
};

struct gv_pooled_struct_32
{
	void* operator new(size_t size);
	void operator delete(void*);
};

struct gv_pooled_struct_256
{
	void* operator new(size_t size);
	void operator delete(void*);
};

struct gv_pooled_struct_1024
{
	void* operator new(size_t size);
	void operator delete(void*);
};

#define GVM_POOLED(s)                          \
	void* operator new(size_t size)            \
	{                                          \
		GV_ASSERT(size < s);                   \
		return gv_global::pool_##s.allocate(); \
	};                                         \
	void operator delete(void* p)              \
	{                                          \
		gv_global::pool_##s.free(p);           \
	};

#define GVM_OVERRIDE_NEW             \
	void* operator new(size_t size); \
	void operator delete(void* p);

#define GVM_IMP_POOLED(cls, s)                 \
	void* cls::operator new(size_t size)       \
	{                                          \
		GV_ASSERT(size < s);                   \
		return gv_global::pool_##s.allocate(); \
	};                                         \
	void cls::operator delete(void* p)         \
	{                                          \
		gv_global::pool_##s.free(p);           \
	};

class gv_spinlock
{
private:
	typedef enum { Unlocked = 0,
				   Locked = 1 } LockState;
	gv_atomic_count state_;

public:
	gv_spinlock()
	{
	}
	void lock()
	{
		while (state_.exchange(Locked) == Locked)
			;
	}
	void unlock()
	{
		state_.set(Unlocked);
	}
};
};
