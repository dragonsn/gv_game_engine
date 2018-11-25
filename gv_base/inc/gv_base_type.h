#pragma once

// put your configration here
#include "gv_config.h"

#if defined(_DEBUG)
#define GV_DEBUG_VERSION 1
#else
#define GV_DEBUG_VERSION 0
#endif

#if defined(_RETAIL)
#define GV_RETAIL_VERSION 1
#else
#define GV_RETAIL_VERSION 0
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4996) // This function or variable may be unsafe.
								// Consider using _itoa_s instead. To disable
								// deprecation, use _CRT_SECURE_NO_WARNINGS. See
								// online help for details.
#pragma warning( \
	disable : 4201)				// nonstandard extension used : nameless struct/union
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4512) // warning C4512: 'gv::gvt_scope_lock<T>' :
								// assignment operator could not be generated
#pragma warning(disable : 4913) // user defined binary operator ',' exists but
								// no overload could convert all operands,
								// default built-in binary operator ',' used
#pragma warning(disable : 4211) // 4211: nonstandard extension used : redefined
								// extern to static
#pragma warning(disable : 4456) // warning C4456 hides previous local
								// declaration
#pragma warning(disable : 4838) // conversion from 'ALint' to 'ALbyte' requires
								// a narrowing conversion
#pragma warning( \
	disable : 4189)				// local variable is initialized but not referenced
#pragma warning(disable : 4457) // declaration of 'i' hides function parameter
#pragma warning(disable : 4459) // warning C4459 : declaration of 'buf_size'
								// hides global declaration
#pragma warning( \
	disable : 4458)				// warning C4458 : declaration of 'L' hides class member
#pragma warning(disable : 4091) // warning C4091: 'typedef ': ignored on left of
								// '' when no variable is declared
#pragma warning( \
	disable : 4324)				// warning C4324 :
								// 'boost::atomics::detail::`anonymous-namespace'::padded_lock<0>':
								// structure was padded due to alignment specifier
#pragma warning(disable : 4702) // warning C4702 : unreachable code
#pragma warning(disable : 4587) // warning C4702 : unreachable code
#pragma warning( \
	disable : 4668)				/// Severity	Code	Description	Project
								/// File	Line	Suppression State	Warning
								/// C4668	'_HAS_NAMESPACE' is not defined as a
								/// preprocessor macro, replacing with '0' for '#if/#elif'
								/// OrderAndChaosVR	E :
								/// \gv\gv_external\boost\config\stdlib\dinkumware.hpp
								/// 99
#pragma warning(disable : 4264) // Severity	Code	Description	Project
								// File	Line	Suppression State
								// Warning	C4264	'bool
								// gv::gvi_stream::open(const char *)': no
								// override available for virtual member
								// function from base 'gv::gvi_stream'; function
								// is hidden	OrderAndChaosVR	e :
								// \gv\gv_game_engine\gv_base\inc\gv_stream_cached.h
								// 219
#pragma warning( \
	disable : 4263) // Severity	Code	Description	Project	File	Line	Suppression
					// State		Warning	C4263	'type_of_user_data
					// *gv::gv_packet::get_user_data(void)': member function
					// does not override any base class virtual member function
					// OrderAndChaosVR	e :
					// \gv\gv_game_engine\gv_base\network\gv_packet.h	137
#endif

#if _MSC_VER
#ifndef WINVER
#define WINVER 0x0501 // SUPPORT XP
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif

#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__) || \
	defined(__ppc64__) || defined(__arm64__)
#define GV_64 1
#endif

#if defined(__ANDROID__) && (GV_WITH_OS_API)
#include <new>
extern void* gv_malloc(int size);
extern void gv_free(void* p);
inline void* operator new(std::size_t size) /*throw (std::bad_alloc)*/
{
	return gv_malloc(size);
};
inline void* operator new[](std::size_t size) /*throw (std::bad_alloc)*/
{
	return gv_malloc(size);
};
inline void operator delete(void* p) throw()
{
	return gv_free(p);
};
inline void operator delete(void* p, std::size_t) throw()
{
	return gv_free(p);
};
inline void operator delete[](void* p) throw()
{
	return gv_free(p);
};
inline void* operator new(std::size_t size,
						  const std::nothrow_t&) /*throw()*/
{
	return gv_malloc(size);
};
inline void* operator new[](std::size_t size,
							const std::nothrow_t&) /*throw()*/
{
	return gv_malloc(size);
};
inline void operator delete(void* p, const std::nothrow_t&) /*throw()*/
{
	return gv_free(p);
};
inline void operator delete[](void* p, const std::nothrow_t&) /*throw()*/
{
	return gv_free(p);
};
#endif

#include <string.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>

#define BOOST_ALL_NO_LIB

/*//current boost not working well with these macros.., need get patch from
boost
#define BOOST_NO_RTTI 1
#define BOOST_NO_TYPEID 1
#define BOOST_EXCEPTION_DISABLE 1
#define BOOST_NO_EXCEPTIONS 1*/

#include <boost/boost/type_traits.hpp>
#include <boost/boost/static_assert.hpp>
#include <boost/boost/functional/hash.hpp>
#include <boost/boost/range.hpp>
#include <boost/boost/array.hpp>
#include <boost/boost/bind.hpp>
#include <boost/boost/function.hpp>
#include <boost/boost/operators.hpp>

#define GV_STATIC_ASSERT BOOST_STATIC_ASSERT

// some keyword, just make the declaration beautiful :)
#define pure_virtual
#define non_virtual

#if _MSC_VER
#define GV_THREAD_LOCAL_STATIC __declspec(thread)
#define GV_DISABLE_OPTIMIZATION optimize("", off)
#define GV_ENABLE_OPTIMIZATION optimize("", on)
#define GV_FORCE_INLINE __forceinline
#define GV_FORCE_NOINLINE __declspec(noinline)
#define GVM_ALIGN_VAR(n, e) __declspec(align(n)) e;
#define MS_ALIGN(n) __declspec(align(n))
#else
#define GV_THREAD_LOCAL_STATIC
#define GV_DISABLE_OPTIMIZATION
#define GV_ENABLE_OPTIMIZATION
#define GV_FORCE_INLINE inline
#define GV_FORCE_NOINLINE
#define GVM_ALIGN_VAR(n, e) e __attribute__((aligned(n)));
#define MS_ALIGN(n)
#endif

#define GV_ATOI_BUF_SIZE 256

#if defined(_MSC_VER) && _MSC_VER < 1900

#define snprintf _snprintf
#define vsnprintf _vsnprintf
/*
        #include <stdarg.h>
        #define snprintf c99_snprintf
        #define vsnprintf c99_vsnprintf
        inline int c99_vsnprintf(char *outBuf, size_t size, const char *format,
   va_list ap)
        {
                int count = -1;

                if (size != 0)
                        count = _vsnprintf_s(outBuf, size, _TRUNCATE, format,
   ap);
                if (count == -1)
                        count = _vscprintf(format, ap);

                return count;
        }
        inline int c99_snprintf(char *outBuf, size_t size, const char *format,
   ...)
        {
                int count;
                va_list ap;
                va_start(ap, format);
                count = c99_vsnprintf(outBuf, size, format, ap);
                va_end(ap);
                return count;
        }
        */
#endif

// Optimization macros (preceeded by #pragma).

namespace gv
{
typedef bool gv_bool;
typedef char gv_char;
typedef unsigned char gv_byte;
typedef signed short int gv_short;
typedef unsigned short int gv_ushort;
typedef signed int gv_int;
typedef unsigned int gv_uint;
typedef signed long long gv_long;
typedef unsigned long long gv_ulong;
typedef float gv_float;
typedef double gv_double;

#if GV_64
typedef gv_long gv_long_ptr;
typedef gv_ulong gv_ulong_ptr;
typedef gv_long gv_int_ptr;
typedef gv_ulong gv_uint_ptr;
#else
typedef long gv_long_ptr;
typedef unsigned long gv_ulong_ptr;
typedef int gv_int_ptr;
typedef unsigned int gv_uint_ptr;
#endif

struct gv_empty_struct
{
};

class gv_empty_class_with_virtual
{
public:
	gv_empty_class_with_virtual(){};
	virtual ~gv_empty_class_with_virtual(){};
};

template < int pad_size >
class gvt_pad
{
public:
	gvt_pad(){};
	char m_pad[pad_size];
};

template < typename T >
struct gvt_identity
{
	typedef T type;
};

namespace
{
GV_STATIC_ASSERT(sizeof(gv_empty_struct) == 1);
#if GV_64
GV_STATIC_ASSERT(sizeof(gv_empty_class_with_virtual) == 8);
#else
GV_STATIC_ASSERT(sizeof(gv_empty_class_with_virtual) == 4);
#endif
GV_STATIC_ASSERT(sizeof(gv_short) == 2);
GV_STATIC_ASSERT(sizeof(gv_int) == 4);
GV_STATIC_ASSERT(sizeof(gv_uint) == 4);
GV_STATIC_ASSERT(sizeof(gv_long) == 8);
GV_STATIC_ASSERT(sizeof(gv_ulong) == 8);
GV_STATIC_ASSERT(sizeof(gv_float) == 4);
GV_STATIC_ASSERT(sizeof(gv_double) == 8);
GV_STATIC_ASSERT(sizeof(gvt_pad< 256 >) == 256);
GV_STATIC_ASSERT(sizeof(gv_int_ptr) == sizeof(void*));
GV_STATIC_ASSERT(sizeof(gv_long_ptr) == sizeof(void*));
}

#ifdef _MSC_VER
#undef min
#undef max
#endif
};
