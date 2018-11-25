#include "gv_base.h"
#include <stdarg.h>

namespace gv
{

#define GVM_MAKE_ID GVM_MAKE_ID_IMP
#include "gv_id_pp.h"
#include "gv_ids.h"
#undef GVM_MAKE_ID

namespace gv_global
{
gvt_global< gv_time > time;
gvt_array< gv_string > command_line_options;
gvt_global< gv_log_manager > log;
gv_base_config config;
gv_string debug_string;
gvt_lock_free_pool< gvt_pad< 2048 >, 256 > pool_2048;
gvt_lock_free_pool< gvt_pad< 1024 >, 256 > pool_1024;
gvt_lock_free_pool< gvt_pad< 256 >, 1024 > pool_256;
gvt_lock_free_pool< gvt_pad< 16 >, 1024 > pool_16;
gvt_lock_free_pool< gvt_pad< 32 >, 1024 > pool_32;
#if GV_WITH_OS_API
gvt_global< gv_network > network;
gvt_global< gv_profiler_manager > profiler;
#endif
gv_stats stats;
}
#define GVM_IMP_MEMORY_POOL(s)                          \
	gvp_memory_##s::gvp_memory_##s(){};                 \
	void* gvp_memory_##s::alloc(size_t size)            \
	{                                                   \
		GV_ASSERT(size <= s);                           \
		return gv_global::pool_##s.allocate();          \
	};                                                  \
	void gvp_memory_##s::free(void* p)                  \
	{                                                   \
		gv_global::pool_##s.free(p);                    \
	};                                                  \
	;                                                   \
	void* gvp_memory_##s::realloc(void* p, size_t size) \
	{                                                   \
		GV_ASSERT(size <= s);                           \
		if (size == 0)                                  \
		{                                               \
			free(p);                                    \
			return NULL;                                \
		}                                               \
		if (p)                                          \
			return p;                                   \
		return gv_global::pool_##s.allocate();          \
	};

GVM_IMP_MEMORY_POOL(16);
GVM_IMP_MEMORY_POOL(32);
GVM_IMP_MEMORY_POOL(256);
GVM_IMP_MEMORY_POOL(1024);
GVM_IMP_MEMORY_POOL(2048);

gv_id::type_of_map gv_id::s_map;
extern void gv_os_init();
extern void gv_os_deinit();
void gv_sleep(long milliseconds)
{
	gv_thread::sleep(milliseconds);
}

void gv_base_init()
{
	gv_base_config* pc = &gv_global::config;
	gv_os_init();
	gv_debug_output("gv_base_init: with tag \n");
#if GV_DEBUG_VERSION
	gv_debug_output("\tdebug|\n");
#else
	gv_debug_output("\trelease|\n");
#endif
#if GV_64
	gv_debug_output("\t64bit|\n");
#else
	gv_debug_output("\t32bit|\n");
#endif
	if (pc->lock_free_zone_size)
		gv_lock_free::static_init(pc->lock_free_zone_size);
	else
		gv_lock_free::static_init();
	gv_global::time.get();
	if (pc->enable_mem_tracking)
	{
#pragma GV_REMINDER( \
	"[*bug]: some software will cause the call stack tracker to dead lock ,try close some software.")
		gvp_memory_default::static_enable_tracking();
		{
			// init the track memory
			char* c = new char[16];
			delete[] c;
		}
	}
	gv_global::fm.get();
#define GVM_MAKE_ID GVM_MAKE_ID_INIT
#include "gv_id_pp.h"
#include "gv_ids.h"
#undef GVM_MAKE_ID
	gv_global::log.get();
#if GV_WITH_OS_API
	gv_thread::current("main");
	gv_global::profiler.get();
	if (pc->enable_profiler)
	{
		if (!pc->profiler_buffer_size)
			gv_global::profiler->init();
		else
			gv_global::profiler->init(pc->profiler_buffer_size);
	}
	gv_global::network.get();
#endif
	gv_global::stats.init();
	gv_global::stats.register_category(gv_id_base, gv_color::GREEN(), true);
	gv_global::stats.register_stats("alloc_times", gv_id_base,
									gvp_memory_base::s_info.alloc_times.to_int(),
									false);
	gv_global::stats.register_stats("free_times", gv_id_base,
									gvp_memory_base::s_info.free_times.to_int(),
									false);
	gv_global::stats.register_stats(
		"realloc_times", gv_id_base,
		gvp_memory_base::s_info.realloc_times.to_int(), false);
	gv_global::stats.register_stats("alloc_size", gv_id_base,
									gvp_memory_base::s_info.alloc_size.to_int(),
									false);
	gv_global::stats.register_stats("debug_txt", gv_id_base,
									gv_global::debug_string);
	// gv_database_connection::static_init();
};

void gv_base_destroy()
{
#define GVM_MAKE_ID GVM_MAKE_ID_RELEASE
#include "gv_id_pp.h"
#include "gv_ids.h"
#undef GVM_MAKE_ID
	// gv_database_connection::static_uninit();
	gv_global::stats.destroy();
#if GV_WITH_OS_API
	gv_global::network.destroy();
	gv_global::profiler.destroy();
#endif
	gv_global::log.destroy();
	gv_global::fm.destroy();
	gv_global::time.destroy();
	gv_global::command_line_options.clear();
	gv_id::static_purge();
	gv_global::pool_2048.purge_memory();
	gv_global::pool_1024.purge_memory();
	gv_global::pool_256.purge_memory();
	gv_global::pool_32.purge_memory();
	gv_global::pool_16.purge_memory();
#if GV_WITH_OS_API
	gv_global::pool_session_event.purge_memory();
#endif
	gv_global::config.no_log_all = true;
	gv_lock_free::static_destroy();
	gv_os_deinit();
};

#define GVM_IMP_POOLED_STRUCT(s)                          \
	void* gv_pooled_struct_##s::operator new(size_t size) \
	{                                                     \
		GV_ASSERT(size < s);                              \
		return gv_global::pool_##s.allocate();            \
	};                                                    \
	void gv_pooled_struct_##s::operator delete(void* p)   \
	{                                                     \
		gv_global::pool_##s.free(p);                      \
	};

GVM_IMP_POOLED_STRUCT(16);
GVM_IMP_POOLED_STRUCT(32);
GVM_IMP_POOLED_STRUCT(256);

// Define intrinsic for InterlockedCompareExchange64
bool gv_CAS2(void* _ptr, gv_int old1, gv_int old2, gv_int new1, gv_int new2)
{
	gv_long Comperand = (static_cast< gv_long >(old1) & 0xffffffff) |
						(static_cast< gv_long >(old2) << 32);
	gv_long Exchange = (static_cast< gv_long >(new1) & 0xffffffff) |
					   (static_cast< gv_long >(new2) << 32);
	return gv_atomic_if_equal_exchange64(
			   reinterpret_cast< gv_long volatile* >(_ptr), Comperand, Exchange) ==
		   Comperand;
}

bool gv_CAS(void* _ptr, gv_int oldVal, gv_int newVal)
{
	return gv_atomic_if_equal_exchange(reinterpret_cast< gv_int volatile* >(_ptr),
									   oldVal, newVal) == oldVal;
}

int gv_get_var_args(char* Dest, int Count, const char*& Fmt, va_list& ArgPtr)
{
	int Result = vsnprintf(Dest, Count, Fmt, ArgPtr);
	va_end(ArgPtr);
	return Result;
}

gv_double gv_seconds()
{
	return gv_global::time->get_sec_from_start();
}

#if !GV_64
gvp_memory_default* s_lock_free_zone_memory = NULL;
void* gv_lock_free::alloc(size_t size)
{
	GV_ASSERT(s_lock_free_zone_memory);
	return s_lock_free_zone_memory->alloc(size);
};
void gv_lock_free::free(void* p)
{
	GV_ASSERT(s_lock_free_zone_memory);
	return s_lock_free_zone_memory->free(p);
};
void* gv_lock_free::realloc(void* p, size_t size)
{
	GV_ASSERT(s_lock_free_zone_memory);
	return s_lock_free_zone_memory->realloc(p, size);
};
void* gv_lock_free::static_to_ptr(const gv_atomic_count& a)
{
	return (void*)(a.get());
};
gv_atomic_count gv_lock_free::static_from_ptr(const void* src)
{
	gv_atomic_count tgt;
	tgt.set((gv_int)src);
	return tgt;
};
void gv_lock_free::static_init(const gv_int initial_lock_free_size)
{
	s_lock_free_zone_memory = new gvp_memory_default;
};
void gv_lock_free::static_destroy()
{
	GVM_SAFE_DELETE(s_lock_free_zone_memory);
};

#else
// gvp_memory_default * s_lock_free_zone_memory = NULL;
gv_atomic_count s_lock_free_current_pointer;
gv_int s_lock_free_zone_size = 0;
char* s_lock_free_zone = NULL;
void* gv_lock_free::alloc(size_t size)
{
	GV_ASSERT(s_lock_free_zone);
	int count = 0;
	while (1)
	{
		gv_int old_size = s_lock_free_current_pointer.current();
		if (old_size + size >= s_lock_free_zone_size)
		{
			GV_ASSERT(0 && "out of lock free memory!! ");
			return NULL;
		}
		gv_int new_size = old_size + (gv_int)size;
		if (s_lock_free_current_pointer.if_equal_exchange(old_size, new_size) ==
			old_size)
		{
			return s_lock_free_zone + old_size;
		}
		GVM_LOCK_FREE_FREEZE_CHECK;
	}
	return NULL;
};
void gv_lock_free::free(void* p){
	// do nothing here..
};
void* gv_lock_free::realloc(void* p, size_t size)
{
	void* pnew = alloc(size);
	if (p)
	{
		memcpy(pnew, p, size);
	}
	return pnew;
};
void* gv_lock_free::static_to_ptr(const gv_atomic_count& a)
{
	if (a.current() == 0)
		return NULL;
	GV_ASSERT(a.current() < s_lock_free_zone_size);
	return (void*)(s_lock_free_zone + a.current());
};
gv_atomic_count gv_lock_free::static_from_ptr(const void* src)
{
	if (!src)
		return gv_atomic_count(0);
	GV_ASSERT(src >= s_lock_free_zone &&
			  src < (s_lock_free_zone + s_lock_free_zone_size));
	gv_atomic_count tgt;
	tgt.set((gv_int)(((char*)src) - s_lock_free_zone));
	return tgt;
};
void gv_lock_free::static_init(const gv_int initial_lock_free_size)
{
	GV_ASSERT(!s_lock_free_zone);
	GV_ASSERT(initial_lock_free_size >= 16);
	s_lock_free_zone_size = initial_lock_free_size;
	s_lock_free_zone = (char*)malloc(s_lock_free_zone_size);
	;
	s_lock_free_current_pointer.add_atomic(16);
};
void gv_lock_free::static_destroy()
{
	::free(s_lock_free_zone);
	s_lock_free_zone = 0;
	s_lock_free_zone_size = 0;
	s_lock_free_current_pointer.set(0);
};
#endif
};

#pragma GV_REMINDER( \
	"[*gv_base]: provide basic api & algorithm to the game engine")
#pragma GV_REMINDER( \
	"[concepts]	: lockable(lock, unlock) , guardable (guard,unguard) ,ranged (begin, end) , hashable(hash) ,assignable (operator=)")
#pragma GV_REMINDER("[prefix]	: gvt_  : template class or template fucntion")
#pragma GV_REMINDER( \
	"			: gvf_  : template functor,PS: no value data member in functor(reference is OK),unless your override the copy constructor && assign operator")
#pragma GV_REMINDER("			: gvp_  : template policy")
#pragma GV_REMINDER("			: gvi_  : normal c++ interface ")
#pragma GV_REMINDER( \
	"			: gv_ 	: normal c++ class & type & namespace")
#pragma GV_REMINDER("[math]		: matrix:   row major matrix")
#pragma GV_REMINDER( \
	"			: coodinate: Y axis point to up, Z axis point to view , x axis point to right , http://www.euclideanspace.com/maths/standards/index.htm")
#pragma GV_REMINDER( \
	"[NOTES]	: no static memory allocation in global variable construction before gv_base_init !!!!! ")
#pragma GV_REMINDER( \
	"[NOTES]	: use explicit in constructor to avoid unnecessary type convert. ")
#pragma GV_REMINDER( \
	"[NOTES]	: clear to boost vs2008 ,C:/Documents and Settings/Administrator/Local Settings/Application Data/Microsoft/websiteCache!!1")
#pragma GV_REMINDER( \
	"[NOTES]	: remember to change the android version in bat, incorrect version will cause compile errors :call android.bat update project -t \"android-14\" -p . -s!!1")
#pragma GV_REMINDER( \
	"[NOTES]	: add APP_PLATFORM := android-9 in xml fix head file not found problem")
#pragma GV_REMINDER( \
	"[NOTES]	: there are many platform don't have 128 bit atomic instruction!!!")
