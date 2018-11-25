#include "gv_base_internal.h"
#include <memory.h>
#include <iostream>

namespace gv
{

enum
{
	E_MALLOC_RETAIL,
	E_MALLOC_WITH_COUNT,
	E_MALLOC_WITH_CALLSTACK,
	E_MALLOC_DEBUG
};

#if defined(_DEBUG) && defined(WIN32) && !(GV_64)
const static int c_config_malloc_type = E_MALLOC_WITH_CALLSTACK;
#elif defined(WIN32)
const static int c_config_malloc_type = E_MALLOC_WITH_COUNT;
#else
const static int c_config_malloc_type = E_MALLOC_RETAIL;
#endif
static int gv_get_random_padding()
{
	return (int)gv_get_performance_counter();
}

#if defined(WIN32)
const static int need_fill_random = 1;
#else
const static int need_fill_random = 0;
#endif

#if defined(__ANDROID__) && defined(_DEBUG)
const static int need_header = 1;
#else
const static int need_header = 0;
#endif

template < class policy_memory = gvp_memory_base >
class gvt_memory_hook_track_times : public gvt_memory_hook_null< policy_memory >
{
public:
	typedef gvt_memory_hook_null< policy_memory > super;
	static const gv_ulong magic_number = 0xabeadbeddead1976ull;
	inline void* alloc(size_t size)
	{
		++gvp_memory_base::s_info.alloc_times;
		gvp_memory_base::s_info.alloc_size.add_atomic((gv_int)size);
		if (need_header)
		{
			size += 16;
		}
		char* ptr = (char*)super::alloc(size);
		if (need_fill_random)
		{
			memset(ptr, gv_get_random_padding(), size);
		}
		if (need_header)
		{
			gv_ulong* pl = (gv_ulong*)ptr;
			*pl++ = magic_number;
			*pl++ = size;
			ptr += 16;
		}
		return ptr;
	}
	inline void free(void* ptr)
	{
		++gvp_memory_base::s_info.free_times;
		if (!need_header && ptr)
		{
			gvp_memory_base::s_info.alloc_size.add_atomic(-gv_mem_size(ptr));
			int size = gv_mem_size(ptr);
			if (need_fill_random)
			{
				memset(ptr, gv_get_random_padding(), size);
			}
		}
		gv_int size;
		if (need_header && is_valid(ptr, size, ptr))
		{
			gvp_memory_base::s_info.alloc_size.add_atomic(-((int)size));
		}
		return super::free(ptr);
	}
	inline void* realloc(void* ptr, size_t size)
	{
		if (ptr == NULL && size)
		{
			return alloc(size);
		}
		else if (ptr && size == 0)
		{
			free(ptr);
			return NULL;
		}
		else
		{
			gv_int size_old;
			if (need_header && is_valid(ptr, size_old, ptr))
			{
				gvp_memory_base::s_info.alloc_size.add_atomic(-size_old);
				size += 16;
			}
			int old_size = 0;
			++gvp_memory_base::s_info.realloc_times;
			gvp_memory_base::s_info.alloc_size.add_atomic((gv_int)size);
			if (ptr && !need_header)
			{
				old_size = gv_mem_size(ptr);
				gvp_memory_base::s_info.alloc_size.add_atomic(-old_size);
			}
			char* pc = (char*)super::realloc(ptr, size);
			if (need_header)
			{
				gv_ulong* pl = (gv_ulong*)pc;
				*pl++ = magic_number;
				*pl++ = size;
				pc += 16;
			}
			if (old_size < (gv_int)size && need_fill_random)
			{
				memset(pc + old_size, gv_get_random_padding(), size - old_size);
			}
			return pc;
		}
	}
	bool is_valid(void* p, gv_int& size, void*& pr)
	{
		if (!p)
			return false;
		gv_ulong* pl = (gv_ulong*)p;
		if (*(pl - 2) == magic_number)
		{
			size = (gv_int)(*(pl - 1));
			pr = pl - 2;
			return true;
		}
		return false;
	}

public:
};

template < class policy_memory = gvp_memory_base >
class gvt_memory_hook_track_call_stack
	: public gvt_memory_hook_null< policy_memory >
{
public:
	typedef gvt_memory_hook_null< policy_memory > super;
	void* alloc(size_t size);
	void free(void* ptr);
	void* realloc(void* ptr, size_t size);
};

//============================================================================>
gv_global_memory_info gvp_memory_base::s_info;
gvp_memory_base::gvp_memory_base()
{
}
gvp_memory_base::~gvp_memory_base()
{
}

#define GVM_CRASH_SAVIOR_MEM_GOD 0

#if GVM_CRASH_SAVIOR_MEM_GOD
static char* s_mem_god_pool = NULL;
static const int s_mem_god_pool_size = 1024 * 1024 * 1024;
static gv_atomic_count s_mem_god_pool_pointer;
static const gv_ulong s_mem_god_magic_number = 0xabeadbeddead1976ull;
static void setup_mem_god()
{
	if (!s_mem_god_pool)
	{
		s_mem_god_pool = (char*)::malloc(s_mem_god_pool_size);
		memset(s_mem_god_pool, 0xcddccddc, s_mem_god_pool_size);
	}
}
struct debug_memsize_info
{
	debug_memsize_info()
	{
	}
	void init(gv_int in_size)
	{
		GV_ASSERT(sizeof(debug_memsize_info) <= 16);
		size = in_size;
		magic = s_mem_god_magic_number;
		in_use = true;
	}
	char* get_ptr()
	{
		return (char*)(this) + 16;
	}
	void do_check()
	{
		GV_ASSERT(magic == s_mem_god_magic_number);
		GV_ASSERT((char*)this + 16 + size <=
				  s_mem_god_pool + s_mem_god_pool_pointer.get());
		GV_ASSERT((char*)this >= s_mem_god_pool);
	}
	gv_int size;
	gv_bool in_use;
	gv_ulong magic;
};
static debug_memsize_info* s_get_mem_god_info(void* p)
{
	return (debug_memsize_info*)((char*)p - 16);
};

gv_int gv_mem_size(void* p)
{
	if (p)
	{
		debug_memsize_info* info = s_get_mem_god_info(p);
		info->do_check();
		return info->size;
	};
	return 0;
};
#endif // GVM_CRASH_SAVIOR_MEM_GOD

void* gvp_memory_base::alloc(size_t size)
{
#if !GVM_CRASH_SAVIOR_MEM_GOD
	return ::malloc(size);
#else
	setup_mem_god();
	size = gvt_align(size, 16);
	gv_int p = s_mem_god_pool_pointer.add_atomic(size + 16);
	GV_ASSERT(s_mem_god_pool_pointer.get() < s_mem_god_pool_size);
	debug_memsize_info* pinfo = (debug_memsize_info*)(s_mem_god_pool + p);
	pinfo->init(size);
	return s_mem_god_pool + p + 16;
#endif
};

void gvp_memory_base::free(void* p)
{
#if !GVM_CRASH_SAVIOR_MEM_GOD
	return ::free(p);
#else
	GV_ASSERT(p > s_mem_god_pool && p < s_mem_god_pool + s_mem_god_pool_size);
	debug_memsize_info* pinfo = s_get_mem_god_info(p);
	pinfo->do_check();
	GV_ASSERT(pinfo->in_use);
	pinfo->in_use = false;
	return;
#endif
};

void* gvp_memory_base::realloc(void* p, size_t size)
{
#if !GVM_CRASH_SAVIOR_MEM_GOD
	return ::realloc(p, size);
#else
	if (p == NULL && size == 0)
		return NULL;
	setup_mem_god();
	if (p == NULL && size)
	{
		return alloc(size);
	}
	else if (p && size == 0)
	{
		free(p);
		return NULL;
	}
	else
	{
		debug_memsize_info* pinfo = s_get_mem_god_info(p);
		pinfo->do_check();
		if (pinfo->size > (gv_int)size)
		{
			memset(pinfo->get_ptr() + size, 0xdeaddead, pinfo->size - size);
			pinfo->size = size;
			return p;
		}
		else if (pinfo->size < (gv_int)size)
		{
			void* pnew = alloc(size);
			memcpy(pnew, p, pinfo->size);
			return pnew;
		}
	}
	return p;
#endif
};
void* gvp_memory_base::memset(void* p, int c, size_t size)
{
	return ::memset(p, c, size);
};
void* gvp_memory_base::memmove(void* p1, const void* p2, size_t size)
{
	if (size == 0)
		return p1;
	return ::memmove(p1, p2, size);
};
void* gvp_memory_base::memcpy(void* p1, const void* p2, size_t size)
{
	if (size == 0)
		return p1;
	return ::memcpy(p1, p2, size);
};

template < int malloc_type >
class gvp_memory_implementation
{
};
//============================================================================>
template <>
class gvp_memory_implementation< E_MALLOC_RETAIL > : public gvp_memory_base
{
public:
	void* operator new(size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void* operator new[](size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void operator delete(void* p)
	{
		gvp_memory_base m;
		m.free(p);
	}

	bool enable_tracking(bool b)
	{
		return false;
	};
	void dump_tracking(gv_ulong start_time = 0){

	};
	void diable_tracking(){

	};
	void reset_tracking(){

	};
};
//============================================================================>

template <>
class gvp_memory_implementation< E_MALLOC_WITH_COUNT >
	: public gvt_memory_hook_track_times< gvp_memory_base >
{
public:
	void* operator new(size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void* operator new[](size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void operator delete(void* p)
	{
		gvp_memory_base m;
		m.free(p);
	}
	bool enable_tracking(bool b)
	{
		return false;
	};
	void dump_tracking(gv_ulong start_time = 0){

	};
	void diable_tracking(){

	};
	void reset_tracking(){

	};
};

//============================================================================>
void do_process_memory_dump(const gv_string_tmp& info);
template <>
class gvp_memory_implementation< E_MALLOC_WITH_CALLSTACK >
	: public gvt_memory_hook_track_times< gvp_memory_base >
{
public:
	gvp_memory_implementation()
		: m_track_enabled(false)
	{
		m_track_dump_started = false;
	}

	~gvp_memory_implementation()
	{
		m_track_enabled = false;
	}

	void* operator new(size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void* operator new[](size_t size)
	{
		gvp_memory_base m;
		return m.alloc(size);
	}
	void operator delete(void* p)
	{
		gvp_memory_base m;
		m.free(p);
	}

	const static int c_malloc_max_callstack = 16;

	typedef gvt_memory_hook_track_times< gvp_memory_base > super;

	typedef gvp_multi_thread::mutex_holder type_of_mutex;

	inline void* alloc(size_t size)
	{
		gv_thread_lock lock(m_lock);
		void* ret = super::alloc(size);
		track_alloc(ret, size);
		return ret;
	};

	inline void free(void* p)
	{
		gv_thread_lock lock(m_lock);
		track_free(p);
		super::free(p);
	};
	inline void* realloc(void* p, size_t size)
	{
		gv_thread_lock lock(m_lock);
		void* ret = super::realloc(p, size);
		if (p == NULL && size)
		{
			track_alloc(ret, size);
		}
		else if (p && size == 0)
		{
			track_free(p);
		}
		else
		{
			track_free(p);
			track_alloc(ret, size);
		}
		return ret;
	};

	bool enable_tracking(bool b)
	{
		gv_thread_lock lock(m_lock);
		gv_global::time.get();
		bool old = m_track_enabled;
		m_track_enabled = b;
		return old;
	};

	void diable_tracking()
	{
		gv_thread_lock lock(m_lock);
		m_track_enabled = false;
	};
	void reset_tracking()
	{
		gv_thread_lock lock(m_lock);
		m_map.reset();
	};

private:
	class gv_malloc_call_stack
	{
	public:
		void* m_stack[c_malloc_max_callstack];
		int alloc_size;
		int stack_depth;
		gv_uint tag;
		gv_ulong time;
	};

	typedef gvt_hash_map< void*, gv_malloc_call_stack, 4096, gvp_memory_base,
						  gvp_multi_thread >
		callstack_map;

	type_of_mutex m_lock;

	bool m_track_enabled;
	volatile bool m_track_dump_started;
	callstack_map m_map;
	;

public:
	inline void track_alloc(void* ret, size_t size)
	{
		if (m_track_enabled && !m_track_dump_started)
		{
			m_track_dump_started = true;
			gv_malloc_call_stack stack;
			int depth = gv_load_callstack(stack.m_stack, c_malloc_max_callstack);
			stack.alloc_size = (gv_int)size;
			stack.stack_depth = depth;
			stack.time = gv_global::time->get_performance_counter_from_start();
			m_map.add(ret, stack);
			gvp_memory_base::s_info.alloc_size.add_atomic((gv_int)size);
			m_track_dump_started = false;
		}
	}
	inline void track_free(void* p)
	{
		if (m_track_enabled && !m_track_dump_started)
		{
			gv_malloc_call_stack* stack = m_map.find(p);
			if (stack)
			{
				gvp_memory_base::s_info.alloc_size.add_atomic(-stack->alloc_size);
				m_map.erase(p);
			}
			else
			{
				// might be so not tracking data!
				// might be so not tracking data!
				m_track_dump_started = true;
				gv_string_tmp log;
				log << "DOUBLE FREE!!!" << p << "\r\n";
				GVM_WARNING(log);
				m_track_dump_started = false;
			};
		}
	}

	void dump_tracking(gv_ulong start_time = 0)
	{
		if (!m_track_enabled)
			return;
		gv_thread_lock lock(m_lock);
		m_track_dump_started = true;
		{
			gv_string_tmp log;
			log << "<memory_callstacks>"
				<< "\r\n";
			callstack_map::iterator it;
			it = m_map.begin();
			while (it != m_map.end())
			{
				if (it.is_empty())
				{
					it++;
					continue;
				}
				void* p = it.get_key();
				gv_malloc_call_stack& stack = *it;
				if (stack.time > start_time)
				{
					log << "!!!"
						<< "\r\n";
					for (int i = 0; i < stack.stack_depth; i++)
					{
						char func_name[1024];
						char file_name[1024];
						int line_no;
						if (gvt_get_symbol_from_offset(stack.m_stack[i], func_name,
													   file_name, line_no))
						{
							log << file_name << "(" << line_no << "):info:" << func_name
								<< "\r\n";
						}
					}
					log << "[]"
						<< "\r\n";
					log << "size=" << stack.alloc_size << "\r\n";
					log << "ptr=" << (gv_uint)(size_t)p << "\r\n";
				}
				it++;
			}
			log << "</memory_callstacks>"
				<< "\r\n";
			do_process_memory_dump(log);
		}
		m_track_dump_started = false;
	};
};

//============================================================================>

void do_process_memory_dump(const gv_string_tmp& info)
{

	/*!!!!!!!!!!!!!>>>>Function:................
  [Frames below may be incorrect and/or missing, no symbols loaded for
  kernel32.dll]
  */

	// !TOKEN_BOOL_UNOP
	// [TOKEN_LB

	using namespace gv::gv_lang_cpp;
	{

		gvt_lexer< gv_scanner_string > lexer;
		lexer.load_string(*info);
		gv_lang_cpp::TOKEN token = (gv_lang_cpp::TOKEN)lexer.lex();
		gvt_hash_map< gv_string_tmp, int, 1024 > map;

		while (token)
		{
			if (token == gv_lang_cpp::TOKEN_BOOL_UNOP)
			{
				lexer.strip_this_line();
				gv_byte b = lexer.look_ahead();
				gv_string_tmp callstack_name;
				while (b && b != '[')
				{
					gv_string_tmp line = lexer.strip_this_line();
					callstack_name += line;
					callstack_name += "";
					b = lexer.look_ahead();
				}
				int* pint = map.find(callstack_name);
				if (!pint)
				{
					map.add(callstack_name, 1);
				}
				else
					(*pint) += 1;
			}
			else
			{
				lexer.strip_this_line();
			}
			token = (gv_lang_cpp::TOKEN)lexer.lex();
		};

		gvt_hash_map< gv_string_tmp, int, 1024 >::iterator it = map.begin();
		gv_string_tmp final;
		while (it != map.end())
		{
			if (!it.is_empty())
			{
				gv_string_tmp name = *it.get_key();
				int time = *it;
				// name.replace_all("\r", ">");
				// name.replace_all("\n", ">");
				final << "|times|" << time << "\t"
					  << "\r\n"
					  << "|callstack|" << *name << "\r\n"; //<< gv_endl;
				GVM_WARNING(final);
				final = "";
			}
			it++;
		}
	}
}

gvp_memory_implementation< c_config_malloc_type >* s_memory_imp = NULL;
static gv_char
	malloc_shadow[sizeof(gvp_memory_implementation< c_config_malloc_type >)];
static void verify_memory_imp()
{
	if (!s_memory_imp)
		s_memory_imp = gvt_construct(
			(gvp_memory_implementation< c_config_malloc_type >*)malloc_shadow);
}

typedef gvp_memory_implementation< c_config_malloc_type > type_of_mem_imp;

//============================================================================>

void* gvp_memory_default::alloc(size_t size)
{
	verify_memory_imp();
	void* p = s_memory_imp->alloc(size);
	if (size)
	{
		GV_ASSERT(p && "out of memory");
	}
	// memset(p, 0, size);
	return p;
}

void gvp_memory_default::free(void* p)
{
	verify_memory_imp();
	if (!p)
		return;
	return s_memory_imp->free(p);
};

void* gvp_memory_default::realloc(void* p, size_t size)
{
	verify_memory_imp();
	void* ret = s_memory_imp->realloc(p, size);
	if (size)
	{
		GV_ASSERT(ret && "out of memory");
	}
	return ret;
};

bool gvp_memory_default::static_enable_tracking(bool b)
{
	verify_memory_imp();
	return s_memory_imp->enable_tracking(b);
};
void gvp_memory_default::static_dump_tracking(gv_ulong start_time)
{
	verify_memory_imp();
	s_memory_imp->dump_tracking(start_time);
};
void gvp_memory_default::static_diable_tracking()
{
	verify_memory_imp();
	s_memory_imp->diable_tracking();
};
void gvp_memory_default::static_reset_tracking()
{
	verify_memory_imp();
	s_memory_imp->reset_tracking();
};
//============================================================================>

} // namespace gv

#define NO_GV_NEW 1

#if (!NO_GV_NEW) && defined(WIN32)

void* operator new(size_t size)
{
	void* p = gv::gvp_memory_default().alloc(size);
	return p;
}
void operator delete(void* p)
{
	gv::gvp_memory_default().free(p);
}
void* operator new[](size_t size)
{
	void* p = gv::gvp_memory_default().alloc(size);
	return p;
}
void operator delete[](void* p)
{
	gv::gvp_memory_default().free(p);
}

#if !GVM_CRASH_SAVIOR_MEM_GOD
namespace gv
{
gv_int gv_mem_size(void* p)
{
	return (gv_int)_msize(p);
};
}
#endif

#elif defined(__ANDROID__)

void* gv_malloc(int size)
{
	return gv::gvp_memory_default().alloc(size);
};
void gv_free(void* p)
{
	gv::gvp_memory_default().free(p);
};
/*
void * operator new(std::size_t size) throw(std::bad_alloc)
{
        return gv::gvp_memory_default().alloc(size);
}
void operator delete(void * p) throw()
{
        gv::gvp_memory_default().free(p);
}
void * operator new[](std::size_t size) throw(std::bad_alloc)
{
        return gv::gvp_memory_default().alloc(size);
}
void operator delete[](void * p) throw()
{
        gv::gvp_memory_default().free(p);
}*/
// Linux :malloc_usable_size : Windows:_msize :
extern size_t dlmalloc_usable_size(const void*);
namespace gv
{
gv_int gv_mem_size(void* p)
{

	// return dlmalloc_usable_size(p);
	return 0;
};
}
#elif defined(WIN32)
namespace gv
{
gv_int gv_mem_size(void* p)
{
	return (gv_int)_msize(p);
};
}
#else
namespace gv
{
gv_int gv_mem_size(void* p)
{
	return 0;
};
}
#endif