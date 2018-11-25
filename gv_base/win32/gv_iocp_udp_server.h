#pragma once

namespace gv
{
class gv_iocp_udp_server_impl;
class gv_iocp_udp_server;

class gv_iocp_udp_server : public gvi_server_transport_layer_udp
{
private:
	gv_iocp_udp_server_impl* m_pimpl;
	gv_iocp_udp_server& operator=(gv_iocp_udp_server& rhs);
	gv_iocp_udp_server(const gv_iocp_udp_server& rhs);

public:
	gv_iocp_udp_server();
	virtual ~gv_iocp_udp_server();

	virtual bool create(gv_ushort port, gv_uint numberOfConcurrentThreads = 0,
						gv_uint numberOfWorkerThreads = 0,
						gv_int maxPendingRecvs = 0,
						gv_int refillPendingRecvs = 0);
	virtual void destroy();
	virtual void set_callback_on_recv(gv_callback_on_recv_complete function);
	virtual void set_callback_on_error(gv_callback_on_io_error function);
	virtual bool send_to(const gv_socket_address& address, gv_packet* packet);
	virtual bool send_to(const gv_socket_address& address, int count,
						 gv_byte* data);
	virtual gv_packet* create_packet();
	virtual void destroy_packet(gv_packet*);
	// default is on, iocp new method, if the callback is thread safe , just can
	// skip the apc call.
	virtual void enable_apc_call(bool enable);
};
}
