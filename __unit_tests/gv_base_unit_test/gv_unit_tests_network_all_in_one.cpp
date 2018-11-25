#include "stdafx.h"

using namespace gv;
using namespace std;

// network
#include "unit_test_network_address.hpp"
#include "unit_test_asio.hpp"
#include "unit_test_asio_chat.hpp"
#include "unit_test_asio_udp.hpp"
#include "unit_test_network_1v1_tcp_server.hpp"
#include "unit_test_network_packet.hpp"
#include "unit_test_rudp_server.hpp"
#include "unit_test_network_1v1_udp_server.hpp"

#if defined(WIN32)
#include "unit_test_iocp_udp_server.hpp"

#if GV_WITH_DATABASE
#include "unit_test_data_base.hpp"
#include "unit_test_database_stored_proc.hpp"
#endif
#endif