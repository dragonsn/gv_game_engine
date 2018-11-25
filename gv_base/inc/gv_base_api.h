#pragma once

namespace gv
{
typedef int (*GV_COMP_FUNC)(const void*, const void*);
extern void gv_throw(const char* format, ...);
extern void gv_debug_output(const char* fmt);
extern void gv_console_output(const char* fmt);
extern void gv_halt();
extern void gv_debug_break();
extern void gv_send_console_key_event(char input);
extern void* gv_alloc_page64k();
extern void gv_free_page64k(void*);
extern int gv_get_var_args(char* dest, int count, const char*& fmt,
						   va_list& var);
;
extern void gv_qsort(void* base, gv_int num, gv_int width,
					 GV_COMP_FUNC compare);
extern gv_int gv_get_time_zone_bias(); // local + bias =utc
extern gv_int gv_system_call(const char*);
extern gv_int gv_mem_size(void* p);
extern gv_double gv_seconds();
};

#define GV_DEBUG_BREAK gv_debug_break();

#if defined(WIN32)
#define GV_STDCALL __stdcall
#else
#define GV_STDCALL
#define gets_s gets
#endif

#define GV_REMINDER_STR(x) #x
#define GV_REMINDER_STR2(x) GV_REMINDER_STR(x)
#define GVM_MACRO_TO_STRING(x) GV_REMINDER_STR(x)
#if GV_WITH_OS_API
#define GV_REMINDER(msg) // message( __FILE__ "(" GV_REMINDER_STR2(__LINE__) ")
						 // : (--<REMINDER>--)" msg )
#else
// disable my self mumu
#define GV_REMINDER(msg)
#endif
#define GVM_EMPTY(...)
#define GVM_ZERO_ME memset(this, 0, sizeof(*this));
#define GVM_ZERO_ME_EXCLD_SUPER \
	memset(((char*)this) + sizeof(super), 0, sizeof(*this) - sizeof(super));
#define GV_FORCE_LINK_VAR(var)                        \
	{                                                 \
		void* p = &var;                               \
		printf("force touch %s at %x \r\n", #var, p); \
	}
#define GV_MACRO_CONCATE(a, b) a##b
#define GV_MACRO_CONCATE2(a, b) GV_MACRO_CONCATE(a, b)
#define GV_MAKE_UNIQUE_ID(name) GV_MACRO_CONCATE2(name, __LINE__)
#define GVM_SAFE_DELETE(p) \
	if (p)                 \
	{                      \
		delete p;          \
		p = NULL;          \
	} // in case the deconstruct is a protected method,can't be deleted from a
// global functino
#define GVM_SAFE_DELETE_ARRAY(p) \
	if (p)                       \
	{                            \
		delete[] p;              \
		p = NULL;                \
	}
#define GVM_SAFE_RELEASE(p) \
	if (p)                  \
	{                       \
		p->release();       \
		p = NULL;           \
	}
#define GVM_NO_COPY(cls)          \
private:                          \
	cls(const cls& a);            \
	cls& operator=(const cls& a); \
                                  \
public:

#define GVM_DCL_COMPARABLE(class, member) \
	inline int operator==(const class& b) \
	{                                     \
		return member == b.member;        \
	};                                    \
	inline int operator>(const class& b)  \
	{                                     \
		return member > b.member;         \
	};                                    \
	inline int operator<(const class& b)  \
	{                                     \
		return member < b.member;         \
	};

#if !RETAIL
#define GV_ASSERT(a)                                                     \
	{                                                                    \
		if (!(a))                                                        \
			gv::gv_throw("%s(%d): GV_ASSERT :" #a "\n", __FILE__, __LINE__); \
	}
#define GV_ASSERT_WITH_MSG(a, msg)   \
	{                                \
		if (!(a))                    \
			GV_ERROR_OUT(#a << msg); \
	}
#define GV_ERROR_OUT(a)                                                  \
	{                                                                    \
		gv_string_tmp c;                                                 \
		c << __FILE__ << "(" << __LINE__ << ") : ERROR!  " << a << "\n"; \
		gv_throw(*c);                                                    \
	}
#else
#define GV_ASSERT(a) \
	{                \
	}
#define GV_ASSERT_WITH_MSG(a, msg) \
	{                              \
	}
#define GV_ERROR_OUT(a) \
	{                   \
	}
#endif

#if GV_DEBUG_VERSION
#define GV_ASSERT_SLOW(a) GV_ASSERT(a)
#define GVM_CONSOLE_OUT(a)     \
	{                          \
		gv_string_tmp c;       \
		c << a << "\n";        \
		gv_console_output(*c); \
	}
#define GVM_CONSOLE_OUT_VAR(var)      \
	{                                 \
		gv_string_tmp c;              \
		c << #var "=" << var << "\n"; \
		gv_console_output(*c);        \
	}
#define GVM_DEBUG_CONSOLE_OUT(a)                                         \
	{                                                                    \
		gv_string_tmp c;                                                 \
		c << __FILE__ << "(" << __LINE__ << ") : [debug] " << a << "\n"; \
		gv_console_output(*c);                                           \
	}
#else
#define GV_ASSERT_SLOW(a)
#define GVM_CONSOLE_OUT(a)     \
	{                          \
		gv_string_tmp c;       \
		c << a << "\n";        \
		gv_console_output(*c); \
	}
#define GVM_CONSOLE_OUT_VAR(var) \
	{                            \
	}
#define GVM_DEBUG_CONSOLE_OUT(a) \
	{                            \
	}
#endif

#define GVM_UNDER_CONSTRUCT GV_ASSERT(0 && "still under construct!!!")

#define GVM_PENDING GV_ASSERT(0 && "pending for next milestone!!!")

#define GVM_VERIFY(x)       \
	if (!(x))               \
	{                       \
		GV_ASSERT(0 && #x); \
		gv_halt();          \
	}

// http://stackoverflow.com/questions/3129916/what-is-wrong-with-this-use-of-offsetof
// will generate a warning in GCC.
#define GV_STRUCT_OFFSET(struc, member) ((size_t) & ((struc*)NULL)->member)

#define GV_ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

namespace gv
{
#include "gv_base_api_detail.h"

gv_int gv_atomic_increment(gv_int volatile* p);
gv_int gv_atomic_decrement(gv_int volatile* p);
gv_int gv_atomic_exchange(gv_int volatile* target, gv_int v);
gv_int gv_atomic_add(gv_int volatile* p, gv_int v);
gv_int gv_atomic_if_equal_exchange(gv_int volatile* target, gv_int comparand,
								   gv_int exchange);
gv_long gv_atomic_increment64(gv_long volatile* addend);
gv_long gv_atomic_decrement64(gv_long volatile* addend);
gv_long gv_atomic_exchange64(gv_long volatile* target, gv_long v);
gv_long gv_atomic_add64(gv_long volatile* addend, gv_long v);
gv_long gv_atomic_if_equal_exchange64(gv_long volatile* destination,
									  gv_long comparand, gv_long exchange);
gv_long gv_atomic_if_equal_exchange128(gv_long volatile* destination,
									   gv_long comparand, gv_long exchange);
bool gv_CAS(void* _ptr, gv_int old, gv_int new_value);
bool gv_CAS2(void* _ptr, gv_int old1, gv_int old2, gv_int new1, gv_int new2);
gv_int gv_load_callstack(void** pbuffer, gv_uint max_depth);
bool gv_get_symbol_from_offset(const void* offset, char* name,
							   int name_buffer_size, char* filename,
							   int filename_size, int& line_no);
gv_uint gv_crc32(const char* buffer, int size);
gv_ulong gv_crc64(const char* buffer, int size);
void gv_sleep(long milliseconds);

#ifdef WIN32
#include "gv_string_atoi_win32.h"
#else
#include "gv_string_atoi_ansi.h"
#endif // WIN32

inline gv_int gv_float_to_int(gv_float f)
{
	return (gv_int)f;
}
inline gv_float gv_int_to_float(gv_int f)
{
	return (gv_float)f;
}
#include "gvt_inline_functions.h"

#include "gv_key_enum.h"
extern void gv_create_device_key_mapping(gv_int to_d[e_key_max + 1],
										 gv_int to_g[e_key_max + 1]);
}