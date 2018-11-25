#include "stdafx.h"
#include "gv_framework\network\gv_network_framework.h"
#include "gv_framework\inc\gv_framework_events.h"

using namespace gv;

#include "unit_network_test_chat.hpp"
#include "unit_network_test_replica.hpp"
#include "unit_network_test_lobby_with_replica.hpp"
#include "unit_network_test_send_command.hpp"
#include "unit_test_network_login.hpp"
#if WIN32 && GV_WITH_DATABASE
#include "unit_test_database.hpp"
#endif
#include "unit_network_test_lobby_mmo.hpp"
