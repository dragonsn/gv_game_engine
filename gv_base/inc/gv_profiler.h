#pragma once

namespace gv
{

class gv_profiler_marker
{
public:
	gv_profiler_marker()
	{
		GVM_ZERO_ME;
	};
	gv_profiler_marker(const char* name, int index);
	~gv_profiler_marker();

public:
	enum type
	{
		e_nop,
		e_profile_start,
		e_profile_end,
		e_tracking_float,
		e_tracking_int,
		e_profile_paired,
		e_profile_thread_info,
		e_profile_end_of_file = 127
	};

protected:
	const char* m_name;
	int m_index;
	int m_stack_depth;
};

class gv_profiler_manager
{
public:
	gv_profiler_manager();
	~gv_profiler_manager();
	// alloc a event to write
	void init(int ring_buffer_size = 1024 * 1024);
	//
	void reset();
	//
	class gv_time_mark* alloc(bool wait = false);
	// write all the event in ring buffer to log
	void dump_snap_shot(const char* prefix);
	// TODO automaticly write to a log file with all the information.
	void enable_async_log(bool enable = true);

public:
	class gv_profiler_manager_imp* m_imp;
};

namespace gv_global
{
extern gvt_global< gv_profiler_manager > profiler;
}

#if !RETAIL && GV_WITH_OS_API
#define GV_PROFILE_EVENT(name, index) \
	gv::gv_profiler_marker GV_MAKE_UNIQUE_ID(_marker_)(#name, index);
#define GV_PROFILE_EVENT_DYNAMIC(name, index) \
	gv::gv_profiler_marker GV_MAKE_UNIQUE_ID(_marker_)(name, index);
#else
#define GV_PROFILE_EVENT(name, index)
#define GV_PROFILE_EVENT_DYNAMIC(name, index)
#endif

#if GV_DEBUG_VERSION && GV_WITH_OS_API
#define GV_PROFILE_EVENT_SLOW(name) \
	gv::gv_profiler_marker GV_MAKE_UNIQUE_ID(_marker_)(#name, 0);
#define GV_PROFILE_EVENT_SLOW_DYNAMIC(name) \
	gv::gv_profiler_marker GV_MAKE_UNIQUE_ID(_marker_)(name, 0);
#else
#define GV_PROFILE_EVENT_SLOW(name)
#define GV_PROFILE_EVENT_SLOW_DYNAMIC(name)
#endif
};
