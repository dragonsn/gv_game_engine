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
	performance_counter_frequency =
		1000 * 1000 * 1000; // in nano seconds resolution
	start_performance_counter = 0;
	last_update_time_in_ms = get_millisec();
	last_update_time_stamp = start_time_stamp = time_stamp_unix();
}

gv_ulong gv_time::get_performance_counter()
{
	boost::chrono::nanoseconds ns =
		boost::chrono::high_resolution_clock::now() - m_imp->start_tick;
	gv_ulong val = ns.count();
	return val;
}

gv_ulong gv_get_performance_counter()
{
	return 0;
};

gv_ulong gv_time::get_millisec()
{
	using namespace boost::posix_time;
	ptime now = microsec_clock::local_time();
	boost::posix_time::time_duration dur;
	dur = now - m_imp->start_time;
	return dur.total_milliseconds();
};
} // namespace gv