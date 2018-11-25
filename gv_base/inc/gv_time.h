#pragma once
namespace gv
{

template < class T >
inline T gvt_time_difference(const T& t0, const T& t1)
{
	return t0 - t1;
}
static const gv_uint gvc_u32_time_overflow =
	864000000; // milliseconds in 10 days.
template <>
inline gv_uint gvt_time_difference< gv_uint >(const gv_uint& a,
											  const gv_uint& b)
{
	return ((a) - (b) >= gvc_u32_time_overflow ? (b) - (a) : (a) - (b));
}

// time related utility
struct gv_time_imp;
extern gv_ulong gv_get_performance_counter();
class gv_time
{
public:
	gv_time();
	gv_time(const gv_time& t);
	~gv_time();
	gv_time& operator=(const gv_time& t);
	//-----------------MICRO LEVEL, USING QPC MIGHT NOT RELIABLE ,ONLY FOR DEBUG&
	//PROFILING
	//-----------------OR USE IN SOME TESTED
	//SYSTEM----------------------------------------
	gv_ulong get_microsec();
	gv_ulong get_microsec_from_start();
	gv_ulong get_performance_counter_from_start();
	gv_ulong get_start_performance_counter();
	gv_ulong performance_counter_to_microsec(gv_ulong s);
	gv_double performance_counter_to_sec(gv_ulong s);
	gv_ulong microsec_to_performance_counter(gv_ulong s);
	gv_ulong get_performance_counter();
	//-----------------MACRO LEVEL, USING TIMEGETTIME RELIABLE BUT PRECISION IS
	//LOW,
	//-----------------CAN BE USED FOR TIME STAMP
	gv_ulong get_millisec();
	gv_double get_sec_from_start();
	gv_ulong get_performance_frequency();
	/// Returns the timestamp expressed in UTC-based
	/// time. UTC base time is midnight, October 15, 1582.
	/// Resolution is 100 nanoseconds.
	gv_ulong time_stamp_utc();
	gv_ulong local_time_stamp_utc();
	/// Returns the timestamp expressed in microseconds
	/// since the Unix epoch, midnight, January 1, 1970.
	gv_ulong time_stamp_unix();
	gv_ulong local_time_stamp_unix();

	gv_string_tmp get_local_time_string();
	gv_string_tmp time_stamp_utc_to_string(gv_ulong);
	gv_string_tmp time_stamp_unix_to_string(gv_ulong);

private:
	void init();
	void pre_init();
	void handle_overflow();
	gv_mutex overflow_handler_lock;
	gv_ulong performance_counter_frequency;
	gv_ulong start_performance_counter;
	gv_ulong start_time_stamp;
	gv_ulong last_update_time_stamp;
	gv_ulong last_update_time_in_ms;
	gv_time_imp* m_imp;
};
}