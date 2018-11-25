#include "gv_base_type.h"
#include "gvt_array.h"
#include "gv_string.h"
#include "gv_time.h"
#include <boost/boost/date_time.hpp>
#include <boost/boost/date_time/gregorian/gregorian.hpp>
#include <boost/boost/date_time/posix_time/posix_time.hpp>
#include <boost/boost/chrono.hpp>
// use YYYY-MM-DD instead of  YYYY-mmm-DD
namespace gv
{
struct gv_time_imp
{
	boost::posix_time::ptime start_time;
	boost::chrono::high_resolution_clock::time_point start_tick;
};
gv_time::gv_time()
{
	pre_init();
	init();
}
gv_time::gv_time(const gv_time& t)
{
	pre_init();
	init();
	(*this) = t;
}
gv_time::~gv_time()
{
	GVM_SAFE_DELETE(m_imp);
}
void gv_time::pre_init()
{
	using namespace boost::posix_time;
	m_imp = new gv_time_imp;
	m_imp->start_time = microsec_clock::local_time();
	m_imp->start_tick = boost::chrono::high_resolution_clock::now();
}
gv_time& gv_time::operator=(const gv_time& t)
{
	this->performance_counter_frequency = t.performance_counter_frequency;
	this->start_performance_counter = t.start_performance_counter;
	return (*this);
}
//-----------------MICRO LEVEL, USING QPC MIGHT NOT RELIABLE ,ONLY FOR DEBUG&
//PROFILING
//-----------------OR USE IN SOME TESTED
//SYSTEM----------------------------------------
gv_ulong gv_time::get_microsec()
{
	gv_ulong r = performance_counter_to_microsec(get_performance_counter());
	return r;
}
gv_ulong gv_time::get_microsec_from_start()
{
	return performance_counter_to_microsec(get_performance_counter_from_start());
}
gv_ulong gv_time::get_performance_counter_from_start()
{
	gv_ulong s = get_performance_counter() - get_start_performance_counter();
	return s;
}
gv_ulong gv_time::get_start_performance_counter()
{
	return start_performance_counter;
}
gv_ulong gv_time::performance_counter_to_microsec(gv_ulong s)
{
	return (s * 1000) / (performance_counter_frequency / 1000);
}
gv_double gv_time::performance_counter_to_sec(gv_ulong s)
{
	return performance_counter_to_microsec(s) / 1000000.f;
}
gv_ulong gv_time::microsec_to_performance_counter(gv_ulong s)
{
	return (s / 1000) * (performance_counter_frequency / 1000);
}
//-----------------MACRO LEVEL, USING TIMEGETTIME RELIABLE BUT PRECISION IS LOW,
//-----------------CAN BE USED FOR TIME STAMP
gv_double gv_time::get_sec_from_start()
{
	return (time_stamp_unix() - start_time_stamp) / 1000000.0;
}
gv_ulong gv_time::get_performance_frequency()
{
	return performance_counter_frequency;
}
gv_ulong gv_time::local_time_stamp_utc()
{
	return time_stamp_utc() - gv_get_time_zone_bias() * (10000000ull * 3600ull);
};
/// Returns the timestamp expressed in microseconds
/// since the Unix epoch, midnight, January 1, 1970.
gv_ulong gv_time::local_time_stamp_unix()
{
	return time_stamp_unix() - gv_get_time_zone_bias() * (1000000ull * 3600ull);
};
gv_string_tmp gv_time::get_local_time_string()
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	ptime t2 = second_clock::local_time();
	gv_string_tmp s = to_iso_extended_string(t2).c_str();
	s.replace_char('T', ' ');
	return s;
};

gv_string_tmp gv_time::time_stamp_utc_to_string(gv_ulong ts)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	date d(1582, 10, 15);
	ts /= 10;
	hours h((long)(gv_long(ts / (gv_ulong)(1000000ull * 3600ull))));
	ts %= (gv_ulong)(1000000ull * 3600ull);
	seconds s(long(ts / 1000000));
	ts %= (gv_ulong)1000000;
	microseconds s2(ts);
	ptime t1(d, h + s + s2);
	gv_string_tmp ss = to_iso_extended_string(t1).c_str();
	ss.replace_char('T', ' ');
	return ss;
};

gv_string_tmp gv_time::time_stamp_unix_to_string(gv_ulong ts)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	time_t t;
	t = ts / (1000 * 1000);
	ptime t1 = from_time_t(t);
	/*
  date d(1970,1,1);
  //midnight, January 1, 1970.
  hours h(long(gv_long(ts/(gv_ulong) (1000000ull*3600ull) )));
  ts%=(gv_ulong)(1000000ull*3600ull);
  seconds s(long(ts/1000000));
  ts%=(gv_ulong)1000000;
  microseconds s2(ts);
  ptime t1(d, h+s+s2);*/
	gv_string_tmp ss = to_iso_extended_string(t1).c_str();
	ss.replace_char('T', ' ');
	return ss;
};
gv_ulong gv_time::time_stamp_utc()
{
	return time_stamp_unix() * 10 + (((gv_ulong)(0x01b21dd2ull)) << 32) +
		   0x13814000;
}

void gv_time::handle_overflow()
{
	// overflow!! it will happen once every 50 days
	gv_thread_lock lock(overflow_handler_lock);
	gv_ulong now = this->get_millisec();
	if (now < last_update_time_in_ms) // test again , avoid race condition
	{
		last_update_time_stamp += 0x100000000ull * 1000ull;
		last_update_time_in_ms = get_millisec();
	}
};

gv_ulong gv_time::time_stamp_unix()
{
	gv_ulong now = get_millisec();
	if (now < last_update_time_in_ms)
	{
		handle_overflow();
	};
	last_update_time_in_ms = now;
	return last_update_time_stamp + last_update_time_in_ms * 1000;
}

void gv_time_stamp_to_string(const gv_time_stamp& t, gv_string_tmp& s)
{
	s = gv_global::time->time_stamp_unix_to_string(t.m_u64);
};

void gv_time_stamp_from_string(gv_time_stamp& t, const gv_string_tmp& s)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	t.m_u64 = 0;
	gvt_array< gv_string_tmp > result;
	s.split(result, " :-_");
	if (result.size() >= 6)
	{
		tm tt;
		gvt_zero(tt);
		result[0] >> tt.tm_year;
		tt.tm_year -= 1900;
		result[1] >> tt.tm_mon;
		result[2] >> tt.tm_mday;
		result[3] >> tt.tm_hour;
		result[4] >> tt.tm_min;
		result[5] >> tt.tm_sec;
		ptime pt(date(1970, Jan, 1), time_duration(0, 0, 0));
		try
		{
			pt = ptime_from_tm(tt);
		}
		catch (...)
		{
			// out of unix time stamp range. well just use the epoch time
		}

		tt = to_tm(pt);
		time_t t_t = mktime(&tt);
		if (t_t == -1)
		{
			t.m_u64 = 0;
		}
		else
		{
			t.m_u64 = t_t * 1000 * 1000;
		}
	}
};
}