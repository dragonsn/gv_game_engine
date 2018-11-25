#pragma once

#include "gv_base_type.h"
#include "gv_base_api.h"
#include "gvt_value.h"

#include "gv_base_functor.h"
#include "gv_base_class.h"
#include "gv_singleton.h"
#include "gv_base_policy.h"
#include "gv_base_visitor.h"
#include "gvt_array.h"
#include "gvt_ring_buf.h"
#include "gvt_stack.h"
#include "gvt_stack_static.h"
#include "gvt_set.h"
#include "gvt_list.h"
#include "gvt_array_static.h"
#include "gv_string.h"
#include "gvt_slist.h"
#include "gvt_slist_no_alloc.h"
#include "gvt_pool_fixed_size.h"
#include "gvt_hash_map.h"
#include "gvt_dictionary.h"
#include "gvt_memory_stack.h"
#include "gvt_slist_no_alloc.h"
#include "gvt_bool_array.h"
#include "gvt_lock_free_buffer.h"
#include "gvt_lock_free_list.h"
#include "gvt_lock_free_pool.h"
#include "gv_file_manager.h"
#include "gv_time.h"
#include "gv_id.h"
namespace gv
{
#define GVM_MAKE_ID GVM_MAKE_ID_DCL
#include "gv_id_pp.h"
#include "gv_ids.h"
#undef GVM_MAKE_ID
}
#include "gv_lexer.h"
#include "gv_xml_parser.h"
#include "gv_math.h"
#include "gv_thread.h"
#include "gv_profiler.h"
#include "gv_log.h"

#include "gv_command_console.h"
#include "gv_unit_test_utility.h"
#include "gv_state_machine.h"
#include "gv_stream_cached.h"

#if GV_WITH_OS_API
#include "gv_network.h"
#include "../gesture/gv_gesture.h"
#endif
#include "gv_stats.h"

#if defined(__ANDROID__)
#include "android/gv_android.h"
#endif

namespace gv
{

struct gv_base_config
{
	gv_base_config()
	{
		GVM_ZERO_ME;
	};
	bool enable_mem_tracking;
	bool enable_profiler;
	bool no_boost_pool; // pool will hide memory allocation, sometimes you need
						// the allocation info;
	int profiler_buffer_size;
	bool no_log_all;
	bool no_log_net;
	bool no_log_main;
	bool no_log_render;
	bool no_log_ai;
	bool no_log_script;
	bool no_log_physics;
	bool no_log_database;
	bool no_log_debug;
	bool no_log_object;
	bool no_log_warning;
	bool no_log_error;
	bool no_log_event;
	bool no_reset_path;
	int net_simulation_latency_ms;
	int lock_free_zone_size;
	float net_simulation_packet_loss_rate;
	const char* main_log_file_name;
	gv_string path_for_write;
	gv_string path_for_save_data;
};

namespace gv_global
{
extern gvt_global< gv_time > time;
extern gvt_global< gv_file_manager > fm;
extern gv_base_config config;
extern gvt_lock_free_pool< gvt_pad< 2048 >, 256 > pool_2048;
extern gvt_lock_free_pool< gvt_pad< 1024 >, 256 > pool_1024;
extern gvt_lock_free_pool< gvt_pad< 256 >, 1024 > pool_256;
extern gvt_lock_free_pool< gvt_pad< 16 >, 1024 > pool_16;
extern gvt_lock_free_pool< gvt_pad< 32 >, 1024 > pool_32;
extern gvt_lock_free_pool< gvt_pad< 128 >, 1024 > pool_session_event;
extern gv_string debug_string;
extern gv_stats stats;
}

void gv_base_init();
void gv_base_destroy();
}