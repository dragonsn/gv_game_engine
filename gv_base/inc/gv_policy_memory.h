#pragma once

#include <memory.h>

namespace gv
{

struct gv_global_memory_info
{
	// make it easy to tranverse the info
	typedef gv_atomic_count* iterator;
	typedef const gv_atomic_count* const_iterator;
	inline iterator begin()
	{
		return &alloc_times;
	}
	inline const_iterator begin() const
	{
		return &alloc_times;
	}
	inline iterator end()
	{
		return &last_count;
	}
	inline const_iterator end() const
	{
		return &last_count;
	}

	inline gv_global_memory_info(){};
	inline gv_global_memory_info(const gv_global_memory_info& c)
	{
		*this = c;
	};
	inline void reset()
	{
		(*this) = gv_global_memory_info();
	}
	gv_global_memory_info& operator=(const gv_global_memory_info& c)
	{
		gv::gvt_copy(c.begin(), c.end(), begin());
		return *this;
	}

	gv_atomic_count alloc_times;
	gv_atomic_count free_times;
	gv_atomic_count realloc_times;
	gv_atomic_count memcpy_times;
	gv_atomic_count memmove_times;
	gv_atomic_count memset_times;
	gv_atomic_count alloc_size;
	gv_atomic_count alloc_max;
	gv_atomic_count last_count;
};

class gvp_memory_base
{
public:
	gvp_memory_base();
	~gvp_memory_base();

	void* alloc(size_t size);
	void free(void* p);
	void* realloc(void* p, size_t size);

public:
	void* memset(void* p, int c, size_t size);
	void* memmove(void* p1, const void* p2, size_t size);
	void* memcpy(void* p1, const void* p2, size_t size);

public:
	static gv_global_memory_info s_info;
};

template < class policy_memory = gvp_memory_base >
class gvt_memory_hook_null
{
public:
	inline void* alloc(size_t size)
	{
		return m.alloc(size);
	}
	inline void free(void* ptr)
	{
		return m.free(ptr);
	}
	inline void* realloc(void* ptr, size_t size)
	{
		return m.realloc(ptr, size);
	}
	inline void* memset(void* p, int c, size_t size)
	{
		return m.memset(p, c, size);
	}
	inline void* memmove(void* p1, const void* p2, size_t size)
	{
		return m.memmove(p1, p2, size);
	}
	inline void* memcpy(void* p1, const void* p2, size_t size)
	{
		return m.memcpy(p1, p2, size);
	}

protected:
	policy_memory m;
};

class gvp_memory_default : public gvp_memory_base
{
public:
	void* alloc(size_t size);
	void free(void* p);
	void* realloc(void* p, size_t size);

	static bool static_enable_tracking(bool b = true);
	static void static_dump_tracking(gv_ulong start_time = 0);
	static void static_diable_tracking();
	static void static_reset_tracking();
};

class gvp_memory_null : public gvp_memory_base
{
	// allocator with no memory !
public:
	void* alloc(size_t size)
	{
		GV_ASSERT(0);
		return 0;
	};
	void free(void* p)
	{
		GV_ASSERT(0);
		return;
	};
	void* realloc(void* p, size_t size)
	{
		GV_ASSERT(0);
		return 0;
	};
};

template < class T >
class gvt_policy_memory_default : public gvp_memory_default
{
};

template < int buffer_size >
class gvt_memory_static : public gvp_memory_default
{
public:
	const static gv_uint MAGIC_NUMBER_ALLOCATED = 0xBACECAFE;
	const static gv_uint MAGIC_NUMBER_FREED = 0xDEADDEAD;
	gvt_memory_static()
	{
		tail_tag = MAGIC_NUMBER_FREED;
		this_start = this;
	}
	~gvt_memory_static()
	{
		GV_ASSERT(this_start == this && "!!! the memory static can't be move or "
										"realloced , don't put it in dynamic "
										"array!!")
	}
	inline void* alloc(size_t size)
	{
		GV_ASSERT(size <= buffer_size);
		GV_ASSERT(tail_tag == MAGIC_NUMBER_FREED);
		tail_tag = MAGIC_NUMBER_ALLOCATED;
		return buffer;
	};
	inline void free(void* p)
	{
		GV_ASSERT(tail_tag == MAGIC_NUMBER_ALLOCATED);
		tail_tag = MAGIC_NUMBER_FREED;
	};
	inline void* realloc(void* p, size_t size)
	{
		GV_ASSERT(size <= buffer_size);
		GV_ASSERT(p == (void*)buffer || p == 0);
		tail_tag = MAGIC_NUMBER_ALLOCATED;
		return buffer;
	};

protected:
	inline char* get_buffer()
	{
		return buffer;
	};
	char buffer[buffer_size];
	gv_uint tail_tag;
	void* this_start;
};

template < int buffer_size >
class gvt_memory_cached : public gvt_memory_static< buffer_size >
{
public:
	typedef gvt_memory_static< buffer_size > super;
	gvp_memory_default default_alloc;
	int alloc_size;
	gvt_memory_cached()
	{
		alloc_size = 0;
	}

	~gvt_memory_cached()
	{
	}

	void verify_ptr(void* p)
	{
		if (!p)
			return;
		if (in_cache(p))
		{
			GV_ASSERT(alloc_size <= buffer_size);
		}
		else
		{
			GV_ASSERT(alloc_size > buffer_size);
		}
	}

	bool in_cache(void* p)
	{
		if (p >= super::buffer && p < super::buffer + buffer_size)
		{
			return true;
		}
		return false;
	}

	inline void* alloc(size_t size)
	{
		alloc_size = (gv_int)size;
		void* ptr = NULL;
		if (size <= buffer_size)
		{
			ptr = super::alloc(size);
		}
		else
		{
			ptr = default_alloc.alloc(size);
		}
		verify_ptr(ptr);
		return ptr;
	};

	inline void free(void* p)
	{
		verify_ptr(p);
		if (in_cache(p))
		{
			return super::free(p);
		};
		return default_alloc.free(p);
	};

	inline void* realloc(void* p, size_t size)
	{
		verify_ptr(p);
		if (size <= buffer_size && in_cache(p))
		{
			alloc_size = (gv_int)size;
			return super::realloc(p, size);
		}
		void* pnew;
		if (size <= buffer_size)
		{
			pnew = super::alloc(size);
		}
		else
		{
			pnew = default_alloc.alloc(size);
		}
		if (p)
		{
			memcpy(pnew, p, gvt_min(size, (size_t)alloc_size));
			this->free(p);
		}
		alloc_size = (gv_int)size;
		verify_ptr(pnew);
		return pnew;
	};
};

#define GVM_DCL_FIXED_ALLOCATOR(s)                   \
	class gvp_memory_##s : public gvp_memory_default \
	{                                                \
	public:                                          \
		gvp_memory_##s();                            \
		void* alloc(size_t size);                    \
		void free(void* p);                          \
		void* realloc(void* p, size_t size);         \
	};

GVM_DCL_FIXED_ALLOCATOR(16)
GVM_DCL_FIXED_ALLOCATOR(32)
GVM_DCL_FIXED_ALLOCATOR(256)
GVM_DCL_FIXED_ALLOCATOR(1024)
GVM_DCL_FIXED_ALLOCATOR(2048)

template < int block_size, class memory_pool = gvp_memory_base >
class gvt_memory_cached_pool : public gvp_memory_base
{
protected:
	int alloc_size;
	gvp_memory_default default_alloc;
	memory_pool pool_alloc;

public:
	gvt_memory_cached_pool()
	{
		alloc_size = 0;
	}
	bool in_cache(void* p)
	{
		return alloc_size <= block_size;
	}
	inline void* alloc(size_t size)
	{
		alloc_size = (gv_int)size;
		if (size <= block_size)
			return pool_alloc.alloc(size);
		return default_alloc.alloc(size);
	};
	inline void free(void* p)
	{
		if (in_cache(p))
		{
			pool_alloc.free(p);
		}
		else
		{
			default_alloc.free(p);
		}
		alloc_size = 0;
	};
	inline void* realloc(void* p, size_t size)
	{
		if (size <= block_size && in_cache(p))
		{
			alloc_size = (gv_int)size;
			return pool_alloc.realloc(p, size);
		}
		void* pnew;
		if (size <= block_size)
			pnew = pool_alloc.alloc(size);
		else
			pnew = default_alloc.alloc(size);
		if (p)
		{
			memcpy(pnew, p, gvt_min(size, (size_t)alloc_size));
			this->free(p);
		}
		alloc_size = (gv_int)size;
		return pnew;
	};
};
typedef gvt_memory_cached_pool< 16, gvp_memory_16 > gvp_mem_cached_16;
typedef gvt_memory_cached_pool< 32, gvp_memory_32 > gvp_mem_cached_32;
typedef gvt_memory_cached_pool< 256, gvp_memory_256 > gvp_mem_cached_256;
typedef gvt_memory_cached_pool< 1024, gvp_memory_1024 > gvp_mem_cached_1024;

// wrapper class of std::allocator , using our memory
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvp_single_thread >
class gvt_allocator
{
public:
	typedef size_t size_type;
	typedef gv_int_ptr difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef gvt_allocator< T, policy_memory, policy_thread_mode > type_of_allocator;
	template < class Other >
	struct rebind
	{
		typedef gvt_allocator< Other, policy_memory, policy_thread_mode > other;
	};
	inline pointer address(reference val) const
	{
		return (&val);
	}
	inline const_pointer address(const_reference val) const
	{
		return (&val);
	}
	inline gvt_allocator()
	{
	}
	inline gvt_allocator(const type_of_allocator&)
	{
	}
	template < class Other >
	inline gvt_allocator(
		const gvt_allocator< Other, policy_memory, policy_thread_mode >&)
	{
	}
	template < class Other >
	inline type_of_allocator&
	operator=(const gvt_allocator< Other, policy_memory, policy_thread_mode >&)
	{
		return (*this);
	}
	inline pointer allocate(size_type count, const void* phint)
	{
		return static_cast< pointer >(this_policy_memory.alloc(count * sizeof(T)));
	}
	inline pointer allocate(size_type count)
	{
		return allocate(count, NULL);
	}
	inline void deallocate(pointer ptr, size_type)
	{
		this_policy_memory.free(ptr);
	}
	inline void construct(pointer ptr, const_reference val)
	{
		new (static_cast< void* >(ptr)) T(val);
	}
	inline void destroy(pointer ptr)
	{
		gvt_destroy(ptr);
		;
	}
	inline size_t max_size() const
	{
		size_t count = static_cast< size_t >(-1) / sizeof(T);
		return (0 < count ? count : 1);
	}

protected:
	policy_memory this_policy_memory;
};

} // namespace gv

#define GVM_NEW_OPERATOR_NO_TRACK     \
	void* operator new(size_t size)   \
	{                                 \
		gv::gvp_memory_base m;        \
		return m.alloc(size);         \
	}                                 \
	void* operator new[](size_t size) \
	{                                 \
		gv::gvp_memory_base m;        \
		return m.alloc(size);         \
	}                                 \
	void operator delete(void* p)     \
	{                                 \
		gv::gvp_memory_base m;        \
		m.free(p);                    \
	}                                 \
	void operator delete[](void* p)   \
	{                                 \
		gv::gvp_memory_base m;        \
		m.free(p);                    \
	}

#if defined(WIN32)
void* operator new(size_t size);

void operator delete(void* p);

void* operator new[](size_t size);

void operator delete[](void* p);
#endif

/*
template<class policy_memory>
inline  void* operator new( unsigned int size ,policy_memory & memory)
{
        return memory.alloc(size);
}
template<class policy_memory>
inline void operator delete( void* p ,policy_memory & memory)
{
        memory.free(p);
}
template<class policy_memory>
inline void* operator new[]( unsigned int size,policy_memory & memory )
{
        return memory.alloc(size);
}
template<class policy_memory>
inline void operator delete[]( void* p,policy_memory & memory )
{
        memory.free(p);
}*/
