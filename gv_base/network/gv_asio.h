#pragma once
#define BOOST_ASIO_SEPARATE_COMPILATION 1
#include <boost/boost/asio.hpp>
namespace gv
{
namespace gv_asio
{
using namespace boost::asio::ip;
udp::endpoint to_asio(const gv_socket_address& address);
gv_socket_address from_asio(const udp::endpoint& end);
class udp_node;
}
class gv_asio_udp_server_imp;
class gv_asio_udp_server : public gvi_server_transport_layer_udp
{
public:
	gv_asio_udp_server();
	virtual ~gv_asio_udp_server();
	virtual bool create(gv_ushort port, gv_uint number_threads = 0,
						gv_uint number_workers = 0, gv_int start_recieves = 0,
						gv_int pending_recieves = 0);
	virtual void destroy();
	virtual void update();
	virtual bool send_to(const gv_socket_address& address, gv_packet* packet);

protected:
	gvt_ptr< gv_asio_udp_server_imp > m_imp;
};
}