#pragma once

namespace gv
{
struct gv_delayed_packet
{
	gv_socket_address m_socket_address;
	gvt_ref_ptr< gv_packet > m_packet;
	gv_ulong m_dest_action_time;
};
class gv_normal_udp_server : public gvi_server_transport_layer_udp,
							 public gvt_factory< gv_packet >
{
private:
	gv_normal_udp_server& operator=(gv_normal_udp_server& rhs);
	gv_normal_udp_server(const gv_normal_udp_server& rhs);

public:
	gv_normal_udp_server();
	virtual ~gv_normal_udp_server();

	virtual bool create(gv_ushort port, gv_uint numberOfConcurrentThreads = 0,
						gv_uint numberOfWorkerThreads = 0,
						gv_int maxPendingRecvs = 0,
						gv_int refillPendingRecvs = 0);
	virtual void destroy();
	virtual void update();
	virtual void set_callback_on_recv(gv_callback_on_recv_complete function)
	{
		m_recieve_call_back = function;
	};
	virtual bool send_to(const gv_socket_address& address, gv_packet* packet);
	virtual bool send_to(const gv_socket_address& address, int count,
						 gv_byte* data);

	gv_packet* create()
	{
		return create_packet();
	};

	void destroy(gv_packet* p)
	{
		destroy_packet(p);
	};
	virtual gv_packet* create_packet()
	{
		gv_packet* p = new gv_packet;
		return p;
	};
	virtual void destroy_packet(gv_packet* p)
	{
		if (p->get_ref())
			p->dec_ref();
		else
			delete p;
	}

protected:
	void recv_from_thread();

private:
	bool net_simulation_packet_loss_check();
	void update_delayed_outgoing_packets();
	void update_delayed_incoming_packets();

private:
	typedef gvt_slist< gv_delayed_packet, gvp_memory_default > delayed_packets_list;
	gv_callback_on_recv_complete m_recieve_call_back;
	gv_socket m_socket;
	gvt_runnable< gv_normal_udp_server > m_recv_runnable;
	gv_thread m_recv_thread;
	gv_socket_address m_internet_addr;
	gv_bool m_quit;
	gv_mutex m_server_mutex;
	delayed_packets_list m_delayed_incoming_packets;
	delayed_packets_list m_delayed_outgoing_packets;
};
}
