#pragma once
#include "../network/gv_ip_address.h"
#include "../network/gv_socket.h"
#include "../network/gv_packet.h"
#include "../network/gvi_server_transport_layer.h"
#include "../network/gv_normal_udp_server.h"
#include "../network/gvi_server_session_layer.h"
#include "../network/gv_reliable_udp_server.h"

namespace gv
{
class gv_network
{
public:
	gv_network();
	~gv_network();
};

namespace gv_global
{
extern gvt_global< gv_network > network;
}
};
