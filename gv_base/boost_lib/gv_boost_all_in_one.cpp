#include "gv_base_internal.h"

#pragma GV_REMINDER("[*gv_boost]: based on boost 1.61")

#include "boost/libs/atomic/src/lockpool.cpp"
#include "boost/libs/date_time/src/gregorian/date_generators.cpp"
#include "boost/libs/date_time/src/gregorian/gregorian_types.cpp"
#include "boost/libs/date_time/src/gregorian/greg_month.cpp"
#include "boost/libs/date_time/src/gregorian/greg_weekday.cpp"
#include "boost/libs/date_time/src/posix_time/posix_time_types.cpp"
#include "boost/libs/filesystem/src/codecvt_error_category.cpp"
#include "boost/libs/filesystem/src/operations.cpp"
#include "boost/libs/filesystem/src/path.cpp"
#include "boost/libs/filesystem/src/path_traits.cpp"
#include "boost/libs/filesystem/src/portability.cpp"
#include "boost/libs/filesystem/src/unique_path.cpp"
#include "boost/libs/filesystem/src/utf8_codecvt_facet.cpp"
#include "boost/libs/filesystem/src/windows_file_codecvt.cpp"
#include "boost/libs/system/src/error_code.cpp"
#include "boost/libs/thread/src/future.cpp"
#include "boost/libs/thread/src/tss_null.cpp"
#if !defined(_WIN32)
#include "boost/libs/thread/src/pthread/once.cpp"
#include "boost/libs/thread/src/pthread/thread.cpp"
#else
#define BOOST_THREAD_BUILD_LIB
#include "boost/libs/thread/src/win32/thread.cpp"
#include "boost/libs/thread/src/win32/tss_dll.cpp"
#include "boost/libs/thread/src/win32/tss_pe.cpp"
#endif

#include "boost/libs/chrono/src/chrono.cpp"
#include "boost/libs/chrono/src/process_cpu_clocks.cpp"
#include "boost/libs/chrono/src/thread_clock.cpp"
