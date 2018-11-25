//-----------------------------------------------------------------------------
#include "gv_base_internal.h"
#include "gv_base.h"
#include "gv_ip_address.h"
#include "gv_socket.h"
#include "gv_normal_udp_server.h"

namespace gv
{
gv_normal_udp_server::gv_normal_udp_server()
{
	m_recieve_call_back = NULL;
	m_quit = false;
};
gv_normal_udp_server::~gv_normal_udp_server(){

};
bool gv_normal_udp_server::create(gv_ushort port,
								  gv_uint numberOfConcurrentThreads,
								  gv_uint numberOfWorkerThreads,
								  gv_int maxPendingRecvs,
								  gv_int refillPendingRecvs)
{
	m_socket.init_socket(e_socket_type_datagram, e_socket_proto_udp, true);
	m_socket.opt_set_broadcast(true);
	m_socket.opt_set_receive_buffer_size(1024 * 1024);
	// m_socket.opt_set_blocking(false);
	// Bind the socket to the requested port
	gv_socket_address m_internet_addr =
		gv_socket_address(gv_ip_address_ipv4::addr_wild_card(), port);
	if (port != 0)
		m_socket.bind(m_internet_addr);

	m_recv_runnable.init(this, &gv_normal_udp_server::recv_from_thread);
	m_recv_thread.start(&m_recv_runnable);
	return true;
};

void gv_normal_udp_server::recv_from_thread()
{
	while (!m_quit)
	{
		gv_packet* packet = create_packet();
		packet->reserve_max_content_size();
		gv_socket_address address;
		int size =
			m_socket.receive_from(packet->get_network_diagram_start(),
								  packet->get_network_diagram_size(), address);
		if (size > 0)
		{
			// typedef void (GV_STDCALL * gv_callback_on_recv_complete)(class
			// gvi_server_transport_layer_udp * server, gv_socket_address & address,
			// gv_packet * );
			packet->reserve_content_size(size);
			packet->m_address = address;
			packet->m_time_stamp = gv_global::time->time_stamp_unix();
			if (m_recieve_call_back)
			{
				if (net_simulation_packet_loss_check())
				{
					if (gv_global::config.net_simulation_latency_ms > 0)
					{
						delayed_packets_list::iterator it =
							m_delayed_incoming_packets.add_dummy();
						(*it).m_socket_address = address;
						(*it).m_packet = packet;
						(*it).m_dest_action_time =
							packet->m_time_stamp +
							gv_global::config.net_simulation_latency_ms * 1000;
					}
					else
					{
						m_recieve_call_back(this, address, packet);
					}
				}
				else
				{
					destroy_packet(packet);
				}
			}
			else
			{
				destroy_packet(packet);
			}
		}
		else
		{
			destroy_packet(packet);
		}
	}
}

void gv_normal_udp_server::destroy()
{
	m_quit = true;
	m_socket.close();
	m_recv_thread.join();
};

void gv_normal_udp_server::update()
{
	if (gv_global::config.net_simulation_latency_ms > 0)
	{
		update_delayed_outgoing_packets();
		update_delayed_incoming_packets();
	}
}

bool gv_normal_udp_server::send_to(const gv_socket_address& address,
								   gv_packet* packet)
{
	if (net_simulation_packet_loss_check())
	{
		if (gv_global::config.net_simulation_latency_ms > 0)
		{
			delayed_packets_list::iterator it =
				m_delayed_outgoing_packets.add_dummy();
			(*it).m_socket_address = address;
			(*it).m_packet = packet;
			(*it).m_dest_action_time =
				packet->m_time_stamp +
				gv_global::config.net_simulation_latency_ms * 1000;
		}
		else
		{
			m_socket.send_to(packet->get_network_diagram_start(),
							 packet->get_network_diagram_size(), address);
		}
	}
	return true;
};
bool gv_normal_udp_server::send_to(const gv_socket_address& address, int count,
								   gv_byte* data)
{
	m_socket.send_to(data, count, address);
	return true;
};
bool gv_normal_udp_server::net_simulation_packet_loss_check()
{
	static gvt_random< gv_float > random;
	if (gv_global::config.net_simulation_packet_loss_rate > 0)
	{
		if (random.get_uniform() <
			gv_global::config.net_simulation_packet_loss_rate)
		{
			return false;
		}
	}
	return true;
}
void gv_normal_udp_server::update_delayed_outgoing_packets()
{
	delayed_packets_list::iterator it_pre = NULL;
	delayed_packets_list::iterator it = m_delayed_outgoing_packets.begin();
	while (it != m_delayed_outgoing_packets.end())
	{
		if (gv_global::time->time_stamp_unix() >= it->m_dest_action_time)
		{
			m_socket.send_to(it->m_packet->get_network_diagram_start(),
							 it->m_packet->get_network_diagram_size(),
							 it->m_socket_address);
			it_pre = it;
			++it;
			m_delayed_outgoing_packets.erase(it_pre);
		}
		else
		{
			++it;
		}
	}
}
void gv_normal_udp_server::update_delayed_incoming_packets()
{
	delayed_packets_list::iterator it_pre = NULL;
	delayed_packets_list::iterator it = m_delayed_incoming_packets.begin();
	while (it != m_delayed_incoming_packets.end())
	{
		if (gv_global::time->time_stamp_unix() >= it->m_dest_action_time)
		{
			it->m_packet->m_time_stamp = it->m_dest_action_time;
			m_recieve_call_back(this, it->m_socket_address, it->m_packet.ptr());
			it_pre = it;
			++it;
			m_delayed_incoming_packets.erase(it_pre);
		}
		else
		{
			++it;
		}
	}
}
}