#include "gv_base_internal.h"

#include "gv_base.h"
#include "../lex/gv_lexer.hpp"
#include "../src/gv_xml_parser.hpp"
#include "../src/gv_policy_memory.hpp"
#include "../src/gv_log.hpp"
#include "../src/gv_base.hpp"
#include "../src/gv_crc.hpp"
#include "../src/gv_state_machine.hpp"
#include "../src/gv_stats.hpp"

//optional module according to the gv_config.h file
#if GV_WITH_GESTURE
#include "../gesture/gv_gesture.h"
#include "../gesture/gv_gesture_all_in_one.hpp"
#endif




#if GV_WITH_OS_API
#include "../src/gv_file_manager.hpp"
#include "../src/gv_command_console.hpp"
#include "../src/gv_profiler.hpp"
#include "../network/gv_normal_udp_server.hpp"
#include "../network/gv_reliable_udp_server_detail.hpp"
#include "../network/gv_packet.hpp"
#include "gv_shared_mutex.hpp"
#endif


#if defined(_WINDOWS_)
#error  //this file should not include windows.h
#endif

#if  GV_WITH_DATABASE
#include "../database/gv_database.hpp" //BANG! windows.h change the calling conversion of this lib ..can't move to the os_all_in_one
#endif