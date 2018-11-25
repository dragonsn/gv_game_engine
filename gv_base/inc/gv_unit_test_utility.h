#pragma once

#include "gv_time.h"

#define GV_TEST_PRINT_FUNC(x, output)       \
	{                                       \
		output << gv_endl << #x << gv_endl; \
		x;                                  \
	}
#define GV_TEST_PRINT_FUNC_AND_RESULT(x, output)          \
	{                                                     \
		output << gv_endl << #x << "=" << (x) << gv_endl; \
	}
#define GV_TEST_PRINT_FUNC_AND_VERIFY(x, ret, output)     \
	{                                                     \
		output << gv_endl << #x << "=" << (x) << gv_endl; \
		GV_ASSERT((x) == (ret));                          \
	}
#define GV_TEST_PRINT_VAR(x, output)                   \
	{                                                  \
		output << gv_endl << #x "=" << (x) << gv_endl; \
	}
#define GV_CHECK_EQUAL(x, y) \
	{                        \
		GV_ASSERT(x == y);   \
	}

namespace gv
{

// a class for complex unit tests. simple one will be just a function.
namespace gv_global
{
extern gvt_global< gv_time > time;
}

template < class T >
class gvt_scope_test_timer
{
public:
	gvt_scope_test_timer(T& _output, const char* _name,
						 bool need_memory_info = true)
		: name(_name), output_log(_output)
	{
		name.replace_all(" ", "_");
		name.replace_all(":", "_");
		name.replace_all(">", "_");
		name.replace_all("<", "_");
		(output_log) << gv_endl << "<" << name << ">" << gv_endl;
		mem_info = gvp_memory_base::s_info;
		m_start_time = gv_global::time->get_performance_counter_from_start();
		m_need_memory_info = need_memory_info;
#if GV_WITH_OS_API
		pmark = new (mark_shadow) gv_profiler_marker(_name, 0);
#endif
		// gvp_memory_default::static_reset_tracking();
	};
	~gvt_scope_test_timer()
	{
#if GV_WITH_OS_API
		gvt_destroy(pmark);
#endif
		gv_ulong micro_seconds = time.get_microsec_from_start();

		gv_int alloc_times = gvp_memory_base::s_info.alloc_times.get();
		alloc_times -= mem_info.alloc_times.get();
		gv_int free_times = gvp_memory_base::s_info.free_times.get();
		free_times -= mem_info.free_times.get();
		gv_int realloc_times = gvp_memory_base::s_info.realloc_times.get();
		realloc_times -= mem_info.realloc_times.get();

		(output_log) << gv_endl << "============!!!!benchmark of " << name
					 << " use :" << (float)micro_seconds << "  (microseconds)"
					 << gv_endl;
		;
		if (m_need_memory_info)
		{
			GV_TEST_PRINT_VAR(realloc_times, output_log);
			GV_TEST_PRINT_VAR(alloc_times, output_log);
			GV_TEST_PRINT_VAR(free_times, output_log);
			if (alloc_times != free_times)
			{
				(output_log) << "memory_leak here!!" << gv_endl;
				gvp_memory_default::static_dump_tracking(m_start_time);
			}
			(output_log) << gv_endl << "</" << name << ">" << gv_endl;
		}
	}

protected:
	gv_time time;
	gv_global_memory_info mem_info;
	gv_string_tmp name;
	gv_ulong m_start_time;
	T& output_log;
	gv_profiler_marker* pmark;
	char mark_shadow[sizeof(gv_profiler_marker)];
	gv_bool m_need_memory_info;
};

inline bool gv_find_in_command_line(const char* name, int argc, char* argv[],
									char* extra_args = NULL)
{
	for (int i = 1; i < argc; i++)
		if (strcmp(name, argv[i]) == 0)
			return true;
	if (extra_args && strstr(extra_args, name))
		return true;
	return false;
}

namespace gv_global
{
extern gvt_array< gv_string > command_line_options;
};

inline void gv_fill_command_line_arg(gvt_array< gv_string >& args, int argc,
									 char* argv[], bool update_global = true)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i] && *argv[i] == '-')
		{
			gv_string s = argv[i] + 1;
			args.add(s);
		}
		if (argv[i] && *argv[i] == '\"')
		{
			gv_string_tmp s = argv[i];
			gv_cpp_string_to_string(s, false);
			args.add(*s);
		}
	}
	if (update_global)
		gv_global::command_line_options = args;
}
}