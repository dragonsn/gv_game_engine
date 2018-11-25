#include <stdarg.h>
namespace gv
{
static int s_time_zone_bias = 0;
//============================================================

gv_int gv_get_time_zone_bias()
{
	return s_time_zone_bias;
}

gv_int gv_system_call(const char* p)
{
	return system(p);
};

void gv_throw(const char* format, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, format);
	static char s_temp_buffer[4096];
	gv_get_var_args(s_temp_buffer, gvt_array_length(s_temp_buffer), format,
					ArgPtr);
	gv_console_output(s_temp_buffer);
#if !defined(__ANDROID__)
	throw(s_temp_buffer);
#endif
	gv_halt();
}

void gv_qsort(void* base, gv_int num, gv_int width, GV_COMP_FUNC compare)
{
	if (!base || !num)
		return;
	GV_ASSERT(num > 0);
	qsort(base, (size_t)num, (size_t)width, compare);
};

void gv_debug_output(const char* s)
{
}

void gv_console_output(const char* s)
{
#if !defined(__ANDROID__)
	std::cerr << s;
#else
	__android_log_print(ANDROID_LOG_INFO, "info", "%s", s);
#endif
	printf("%s", s);
	;
}

void gv_halt()
{
	*(int*)13 = 13;
};

void gv_debug_break()
{
	gv_halt();
};

// http://athile.net/library/blog/?p=44
void gv_send_console_key_event(char input)
{
}
// http://man7.org/linux/man-pages/man3/posix_memalign.3.html
void* gv_alloc_page64k()
{
#if !defined(__ANDROID__)
	void* p;
	posix_memalign(&p, 65536, 65536);
	return p;
#else
	return memalign(65536, 65536);
#endif
};
void gv_free_page64k(void* p)
{
	free(p);
};
gv_int gv_atomic_increment(volatile gv_int* pi)
{
	return __sync_fetch_and_add(pi, 1) + 1;
};

gv_int gv_atomic_decrement(volatile gv_int* pi)
{
	return __sync_fetch_and_sub(pi, 1) - 1;
};
gv_int gv_atomic_exchange(volatile gv_int* Target, gv_int Value)
{
	return __sync_lock_test_and_set(Target, Value);
};

gv_int gv_atomic_add(volatile gv_int* Addend, gv_int Value)
{
	return __sync_fetch_and_add(Addend, Value);
};
gv_long gv_atomic_add64(volatile gv_long* Addend, gv_long Value)
{
	return __sync_fetch_and_add(Addend, Value);
};

gv_int gv_atomic_if_equal_exchange(volatile gv_int* Destination,
								   gv_int Comparand, gv_int Exchange)
{
	return __sync_val_compare_and_swap(Destination, Comparand, Exchange);
}

gv_long gv_atomic_if_equal_exchange64(gv_long volatile* Destination,
									  gv_long Comparand, gv_long Exchange)
{

//	*Destination = Exchange;
//	return Comparand;
	return __sync_val_compare_and_swap(Destination, Comparand, Exchange);

};

int gv_load_callstack(void** pbuffer, gv_uint max_depth)
{
	return 0;
};
bool gv_get_symbol_from_offset(const void* offset, char* func_name,
							   int func_name_size, char* file_name,
							   int file_name_size, int& line_no)
{
	return false;
}

//============================================================================================
//								:
//============================================================================================
gv_string_tmp gv_multi_byte_to_utf8(const char* multi_byte);
gv_string_tmp gv_utf8_to_multi_byte(const char* utf8);

gv_string_tmp gv_utf8_to_multi_byte(const char* utf8)
{
	gv_string_tmp result;
	GVM_UNDER_CONSTRUCT;
	return result;
}

gv_string_tmp gv_multi_byte_to_utf8(const char* gb2312)
{
	gv_string_tmp result;
	GVM_UNDER_CONSTRUCT;
	return result;
}

gv_string_tmp gv_get_full_path_name(const gv_string_tmp& fname)
{
	return fname;
}

void gv_create_device_key_mapping(gv_int to_d[e_key_max + 1],
								  gv_int to_g[e_key_max + 1])
{
#undef GVM_MAP_KEYS
#define GVM_MAP_KEYS(my_key, vk) \
	to_d[my_key] = vk;           \
	to_g[vk] = my_key;
#include "gv_key_mapping_ansi.h"
#undef GVM_MAP_KEYS
}

#if !GV_WITH_OS_API
void gv_os_init()
{
}
void gv_os_deinit()
{
}
#endif

} // namespace gv
