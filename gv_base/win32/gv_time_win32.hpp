
#include <windows.h>
#include <mmsystem.h>
#include "gv_base_type.h"
#include "gvt_array.h"
#include "gv_string.h"
#include "gv_time.h"
#include <boost/boost/date_time.hpp>
#include <boost/boost/date_time/gregorian/gregorian.hpp>
#include <boost/boost/date_time/posix_time/posix_time.hpp>
// use YYYY-MM-DD instead of  YYYY-mmm-DD
namespace gv
{
void gv_time::init()
{
// http://blogs.msdn.com/b/oldnewthing/archive/2008/09/08/8931563.aspx
#pragma GV_REMINDER( \
	"[MEMO]the QueryPerformanceFrequency is not reliable for timestamp!!!use the timegetime instead")

	LARGE_INTEGER freq, count;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&count);
	performance_counter_frequency = freq.QuadPart;
	start_performance_counter = count.QuadPart;

	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows
						  // NT FILETIME
	epoch.LowPart = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.LowPart = ft.dwLowDateTime;
	ts.HighPart = ft.dwHighDateTime;
	ts.QuadPart -= epoch.QuadPart;
	start_time_stamp = ts.QuadPart / 10;
	last_update_time_in_ms = timeGetTime();
	last_update_time_stamp = start_time_stamp - last_update_time_in_ms * 1000ull;
}

gv_ulong gv_get_performance_counter()
{
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return count.QuadPart;
}

gv_ulong gv_time::get_performance_counter()
{
	return gv_get_performance_counter();
}

gv_ulong gv_time::get_millisec()
{
	return timeGetTime();
};
} // namespace gv