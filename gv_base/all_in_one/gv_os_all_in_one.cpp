#include "gv_base_internal.h"
#include "gv_base.h"
#include "../src/gv_time.hpp"

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
// why should winsock2 go first , check
// http://stackoverflow.com/questions/5971332/redefinition-errors-in-winsock2-h
#include "../win32/gv_win32_func.hpp"
#include "../win32/gv_thread_win32.hpp"
#include "../win32/gv_file_manager_win32.hpp"
#include "../win32/gv_time_win32.hpp"
#if GV_WITH_OS_API
#include "../win32/gv_network_win32.hpp"
#include "../win32/gv_iocp_udp_server.hpp"
#endif
#else
#if defined(__ANDROID__)
#include <android/log.h>
#include "../src/android/gv_base_android.hpp"
#include "../src/android/gv_jni.hpp"
#if defined(__ANDROID__)
#include <locale>
//fix android link error!
//undefined std::locale::locale(char const*)
std::locale::locale(char const*)
{
	//return 1;
}

#endif //  #if defined(__ANDROID__)
#endif
#include "../ansi/gv_base_ansi.hpp"
#include "../ansi/gv_thread_ansi.hpp"
#include "../ansi/gv_mutex_ansi.hpp"
#include "../ansi/gv_event_ansi.hpp"
#include "../ansi/gv_file_manager_ansi.hpp"
#include "../ansi/gv_time_ansi.hpp"
#if GV_WITH_OS_API
#include "../ansi/gv_network_ansi.hpp"
#endif
#endif
#if  GV_WITH_ASIO
#include "../network/gv_asio.h"
#include "../network/gv_asio.hpp"
#include "../network/gv_mnet_node.h"
#include "../network/gv_mnet_node.hpp"
#endif




