#include "gv_base_internal.h"
#include "gvi_server_session_layer.h"
#include <boost/boost/pool/singleton_pool.hpp>
#include "gv_reliable_udp_server.h"

#define DEBUG_RUDP 0 && GV_DEBUG_VERSION

namespace gv
{
//  [9/6/2010 shennan ]#pragma GV_REMINDER("[PLAN][P2][NETWORK][TODO]add endian
//  conversion to make it more portable.")
#pragma GV_REMINDER( \
	"[PLAN][P1][NETWORK][PENDING][TODO]add the bandwidth control.")

static const int gvc_event_buffer_size = 65536;
static const int gvc_check_send_time_out = 10000000;
#if GV_DEBUG_VERSION
static const int gvc_very_long_time_out = 1000000000; // 1000 seconds ,
static const int gvc_max_resend_times = 30;
static const int gvc_min_resend_time_out = 1000000; // 1 seconds
#else
static const int gvc_very_long_time_out = 400000000; // 400 seconds ,
static const int gvc_max_resend_times = 30;
static const int gvc_min_resend_time_out = 100000; // 100ms
#endif

namespace gv_global
{
gvt_lock_free_pool< gvt_pad< 128 >, 1024 > pool_session_event;
}
GV_STATIC_ASSERT(sizeof(gv_session_event) < 128);

//-----------------------------------------------------------------------------
// EVENT FACTORY
//-----------------------------------------------------------------------------
class gv_session_event_factory : public gvt_factory< gv_refable_with_factory >
{
public:
	gv_refable_with_factory* create()
	{
		gv_session_event* p;
		// if (gv_global::config.no_boost_pool)  p=new gv_session_event;
		// else
		{
			p = reinterpret_cast< gv_session_event* >(
				gv_global::pool_session_event.allocate());
			gvt_construct(p);
		}
		p->set_factory(this);
		return p;
	};
	void destroy(gv_refable_with_factory* p)
	{
		GV_ASSERT(!p->get_ref());
		// if (gv_global::config.no_boost_pool) delete p;
		// else
		{
			gvt_destroy(p);
			gv_global::pool_session_event.free(p);
			// gv_global::pool_session_event::free(p);
		};
	};
	gv_session_event* create_event()
	{
		return (gv_session_event*)create();
	}
};
gv_session_event_factory s_session_event_factory;
//-----------------------------------------------------------------------------
// PROTOCOL HEADER START HERE
//-----------------------------------------------------------------------------
enum gve_rudp_header_flag
{
	gve_rudp_header_flag_is_compressed = 1,
	gve_rudp_header_flag_is_encrypted = 2,
	gve_rudp_header_flag_is_use_checksum = 4,
	gve_rudp_header_flag_is_not_used = 8,
};

struct gv_rudp_protocol_header
{
	gv_ushort flags : 4;	// compressed ? checksum is used ?
	gv_ushort host_id : 10; //
	gv_ushort peer_id;
	union {
		gv_uint checksum; // if flag indicate not use check sum, must be session id
		gv_uint session_id;
	};
};
GV_STATIC_ASSERT(sizeof(gv_rudp_protocol_header) == 8);
//-------------------------------------------
struct gv_rudp_protocol_command_header
{
	gv_byte command;
	gv_byte channel_id;
	gv_ushort packet_size;
	gv_uint sequence_number;
};
GV_STATIC_ASSERT(sizeof(gv_rudp_protocol_command_header) == 8);
//-------------------------------------------
struct gv_rudp_protocol_acknowledge
{
	gv_ulong received_sent_time_stamp;
	gv_ulong send_time_stamp;
	gv_uint received_reliable_sequence_number;
};
//-------------------------------------------
struct gv_rudp_protocol_connection
{
	gv_ulong send_time_stamp;
	gv_ushort outgoing_host_id;
	gv_ushort outgoing_peer_id;
	gv_ushort mtu;
	gv_ushort window_size;
	gv_uint channel_count;
	gv_uint incoming_bandwidth;
	gv_uint outgoing_bandwidth;
	gv_uint session_id;
};
//-------------------------------------------
struct gv_rudp_protocol_verify_connection
{
	gv_ulong send_time_stamp;
	gv_ulong received_sent_time_stamp;
	gv_ushort outgoing_peer_id;
	gv_ushort outgoing_host_id;
	gv_uint window_size;
	gv_uint channel_count;
	gv_uint incoming_bandwidth;
	gv_uint outgoing_bandwidth;
	gv_uint session_id;
	gv_uint received_reliable_sequence_number;
};
//-------------------------------------------
struct gv_rudp_protocol_disconnection
{
	gv_ulong send_time_stamp;
	gv_uint data;
};
//-------------------------------------------
struct gv_rudp_protocol_send_reliable
{
	gv_ulong send_time_stamp;
	gv_ushort fragment_id;
	gv_ushort total_fragment;
};
//-------------------------------------------
struct gv_rudp_protocol_send_unreliable
{
	gv_ulong send_time_stamp;
};
//-----------------------------------------------------------------------------
// RUDP CHANNEL !!!
//-----------------------------------------------------------------------------
const static int gvc_rudp_default_window_size = 8;
class gv_rudp_channel
{
	friend class gv_reliable_udp_server_impl;
	friend class gv_rudp_peer;

public:
	typedef gvt_dlist< gvt_ref_ptr< gv_packet >, gvp_memory_32 > packets_list;
	gv_rudp_channel()
	{
		m_incoming_reliable_sequence = 0;
		m_incoming_unreliable_sequence = 0;
		m_outgoing_reliable_sequence = 0;
		m_outgoing_unreliable_sequence = 0;
		m_start_server_time = gv_global::time->time_stamp_unix();
		m_last_recv_time = m_start_server_time;
		m_last_send_time = m_start_server_time;
		m_send_bytes = 0;
		m_recv_bytes = 0;
		m_max_send_window_size = gvc_rudp_default_window_size;
		m_cu_send_window_size = 0;
	}
	~gv_rudp_channel(){};

protected:
	bool remove_reliable_wait_packet(gv_uint seq)
	{
		packets_list::iterator it = m_outgoing_reliable_wait_for_ack_list.begin();
		while (it != m_outgoing_reliable_wait_for_ack_list.end())
		{
			gv_packet* packet = (*it);
			if (packet->m_sequence_number == seq)
			{
				m_outgoing_reliable_wait_for_ack_list.erase(it);
				m_cu_send_window_size--;
				return true;
			}
			++it;
		} //
		return false;
	}
	gv_uint m_incoming_reliable_sequence;
	gv_uint m_incoming_unreliable_sequence;
	gv_uint m_outgoing_reliable_sequence;
	gv_uint m_outgoing_unreliable_sequence;
	gv_ulong m_start_server_time;
	gv_ulong m_last_recv_time;
	gv_ulong m_last_send_time;
	gv_int m_send_bytes;
	gv_int m_recv_bytes;
	gv_ushort m_max_send_window_size; // max packet in the waiting for ack list;
	gv_int m_cu_send_window_size;	 // current packet the waiting list;

	//  [5/17/2011 Administrator] DONE memory pool the list node
	packets_list m_outgoing_reliable_list;
	packets_list m_outgoing_reliable_wait_for_ack_list;
	packets_list m_incoming_reliable_list;			// need to send ack
	packets_list m_incoming_reliable_fragment_list; // need to send ack
};
//-----------------------------------------------------------------------------
// RUDP PEER !!!
//-----------------------------------------------------------------------------
class gv_rudp_peer : public gv_refable
{
	friend class gv_reliable_udp_server_impl;
	friend class gv_rudp_host;

public:
	gv_rudp_peer()
	{
		reset();
	};
	virtual ~gv_rudp_peer(){};

protected:
	gv_uint generator_session_id()
	{
		static gvt_random< gv_float > the_random;
		static bool inited = false;
		if (!inited)
		{
			the_random.set_seed((gv_uint)gv_global::time->get_performance_counter());
		};
		return the_random.get_uint();
	}
	gv_ulong get_time()
	{
		return gv_global::time->time_stamp_unix();
	}
	gv_string_tmp string_name()
	{
		gv_string_tmp s;
		s << "[" << this->m_address.string() << "|" << this->m_host_id << "@"
		  << this->m_peer_id << "]--";
		return s;
	}
	void reset_channel(int nb)
	{
		m_channels.clear();
		m_channels.resize(nb);
	}
	void reset()
	{
		m_state = gve_peer_state_zombie;
		m_session_id = generator_session_id();
		m_outgoing_host_id = 0;
		m_outgoing_peer_id = 0;
		m_utc_time_difference = 0;
		m_min_round_trip_time = 10000000;
		m_max_round_trip_time = 0;
		m_last_round_trip_time = 0;
		m_average_round_trip_time = 1000000; // 1000 ms for first timeout
		m_nb_acks = 0;
		m_create_time = get_time();
		m_last_recv_time = m_last_send_time = m_create_time;
		reset_channel(0);
	}
	gv_ulong corrected_remote_time(gv_ulong remote_time)
	{
		return gvt_min(remote_time + m_utc_time_difference, get_time());
	}
	gve_peer_state get_state()
	{
		return m_state;
	};
	gv_int get_timeout_for_resend()
	{
		return gvt_max(m_average_round_trip_time * 2, gvc_min_resend_time_out);
	}
	bool update_time(gv_ulong trip_start_time, gv_ulong trip_end_time,
					 gv_ulong recieved_send_time)
	{
		int round_trip_time = (int)(trip_end_time - trip_start_time);
		if (round_trip_time < 0)
		{
			return false;
		}
		// update times
		m_nb_acks++;
		m_last_recv_time = get_time();
		m_last_round_trip_time = round_trip_time;
		m_max_round_trip_time = gvt_max(round_trip_time, m_max_round_trip_time);
		m_min_round_trip_time = gvt_min(round_trip_time, m_min_round_trip_time);
		double average_alpha = 1.0 / ((double)m_nb_acks + 5);
		gv_long utc_diff =
			trip_start_time + round_trip_time / 2 - recieved_send_time;
		m_utc_time_difference =
			gvt_lerp(m_utc_time_difference, utc_diff, average_alpha);
		m_average_round_trip_time =
			gvt_lerp(m_average_round_trip_time, round_trip_time, average_alpha);
		return true;
	}

	bool update_outgoing_packet(gv_packet* packet, gv_ushort channel,
								gve_packet_type cmd)
	{
		gv_rudp_protocol_header* head =
			packet->get_protocol_header< gv_rudp_protocol_header >(0);
		gv_rudp_protocol_command_header* command =
			packet->get_protocol_header< gv_rudp_protocol_command_header >(1);
		head->flags = 0;
		head->host_id = this->m_outgoing_host_id;
		head->peer_id = this->m_outgoing_peer_id;
		head->session_id = this->m_session_id;
		command->channel_id = (gv_byte)channel;
		command->command = (gv_byte)cmd;
		command->packet_size = (gv_ushort)packet->get_packet_content_size();
		if (cmd == gve_packet_type_acknowledge)
			command->sequence_number = 0;
		else if (cmd == gve_packet_type_send_unreliable)
			command->sequence_number =
				m_channels[channel].m_outgoing_unreliable_sequence++;
		else
			command->sequence_number =
				m_channels[channel].m_outgoing_reliable_sequence++;
		packet->m_sequence_number = command->sequence_number;
		packet->m_address = m_address;
		return true;
	}
	void post_reliable_packet(gv_int channel, gv_packet* packet)
	{
		packet->m_time_stamp = get_time();
		this->m_channels[channel].m_outgoing_reliable_list.push_back(packet);
	}
	void post_unreliable_packet(gv_int channel, gv_packet* packet)
	{
		// just send ;
		packet->m_time_stamp = get_time();
		this->m_server->send_to(this->m_address, packet);
	}
	void post_connection_packet()
	{
		gv_packet* packet = m_server->create_packet(gve_packet_type_connection);
		update_outgoing_packet(packet, 0, gve_packet_type_connection);
		gv_rudp_protocol_connection* connection =
			packet->get_protocol_header< gv_rudp_protocol_connection >(2);
		connection->incoming_bandwidth = this->m_incoming_limit;
		connection->outgoing_bandwidth = this->m_outgoing_limit;
		connection->outgoing_host_id = (gv_ushort)m_host_id;
		connection->outgoing_peer_id = (gv_ushort)m_peer_id;
		connection->send_time_stamp = get_time();
		connection->channel_count = this->m_channels.size();
		connection->session_id = this->m_session_id;
		connection->window_size = this->m_channels[0].m_max_send_window_size;
		connection->mtu = gvc_default_mtu;
		post_reliable_packet(0, packet);
		m_connection_sequence = packet->m_sequence_number;
	}
	void post_disconnection_packet()
	{
		gv_packet* packet = m_server->create_packet(gve_packet_type_disconnection);
		update_outgoing_packet(packet, 0, gve_packet_type_disconnection);
		gv_rudp_protocol_disconnection* disconnection =
			packet->get_protocol_header< gv_rudp_protocol_disconnection >(2);
		disconnection->send_time_stamp = get_time();
		disconnection->data = 0;
		post_reliable_packet(0, packet);
		m_connection_sequence = packet->m_sequence_number;
	}
	void post_connection_verify_packet(gv_rudp_protocol_connection* connection,
									   gv_uint sequence)
	{
		// connection always in channel 0
		gv_packet* packet =
			m_server->create_packet(gve_packet_type_verify_connection);
		update_outgoing_packet(packet, 0, gve_packet_type_verify_connection);
		gv_rudp_protocol_verify_connection* connection_ack =
			packet->get_protocol_header< gv_rudp_protocol_verify_connection >(2);
		connection_ack->received_reliable_sequence_number = sequence;
		connection_ack->incoming_bandwidth = this->m_incoming_limit;
		connection_ack->outgoing_bandwidth = this->m_outgoing_limit;
		connection_ack->outgoing_host_id = m_host_id;
		connection_ack->outgoing_peer_id = m_peer_id;
		if (connection)
		{
			connection_ack->received_sent_time_stamp = connection->send_time_stamp;
			connection_ack->window_size = connection->window_size;
			connection_ack->channel_count = connection->channel_count;
		}
		connection_ack->send_time_stamp = get_time();
		connection_ack->session_id = this->m_session_id;
		post_reliable_packet(0, packet);
		m_connection_sequence = packet->m_sequence_number;
	}
	void post_ack_packet(gv_ulong recieved_send_time, gv_ushort channel,
						 gv_uint sequence)
	{
		gv_packet* packet = m_server->create_packet(gve_packet_type_acknowledge);
		update_outgoing_packet(packet, channel, gve_packet_type_acknowledge);
		gv_rudp_protocol_acknowledge* ack =
			packet->get_protocol_header< gv_rudp_protocol_acknowledge >(2);
		ack->received_reliable_sequence_number = sequence;
		ack->received_sent_time_stamp = recieved_send_time;
		ack->send_time_stamp = get_time();
		post_unreliable_packet(channel, packet);
	}

	bool do_connection()
	{
		if (m_state != gve_peer_state_disconnected &&
			m_state != gve_peer_state_zombie)
		{
			GVM_DEBUG_LOG(net, string_name() << "do_connection in wrong state!!\r\n");
			return false;
		}
		m_state = gve_peer_state_connecting; // send back the verify ,and check ack
											 // to become connected
		post_connection_packet();
		return true;
	}
	bool handle_connection(gv_uint sequence, gv_socket_address address,
						   gv_rudp_protocol_connection* connection)
	{
		reset_channel(connection->channel_count);
		// set the recv time when create connection
		m_last_recv_time = get_time();
		m_outgoing_host_id = connection->outgoing_host_id;
		m_outgoing_peer_id = connection->outgoing_peer_id;
		m_outgoing_limit = connection->outgoing_bandwidth;
		m_incoming_limit = connection->incoming_bandwidth;
		m_session_id = connection->session_id;
		m_state = gve_peer_state_connecting_ack;
		m_address = address;
		if (m_channels[0].m_incoming_reliable_sequence <= sequence)
			m_channels[0].m_incoming_reliable_sequence = sequence + 1;
		post_connection_verify_packet(connection, sequence);
		return true;
	}
	bool
	handle_verify_connection(gv_uint sequence, gv_ulong recv_time_stamp,
							 gv_rudp_protocol_verify_connection* connection_ack)
	{
		if (m_state != gve_peer_state_connecting)
		{
			return false;
		}
		if (!do_ack(0, connection_ack->received_reliable_sequence_number))
			return false;
		;
		if (!update_time(connection_ack->received_sent_time_stamp, recv_time_stamp,
						 connection_ack->send_time_stamp))
			return false;
		m_outgoing_host_id = connection_ack->outgoing_host_id;
		m_outgoing_peer_id = connection_ack->outgoing_peer_id;
		if (m_channels[0].m_incoming_reliable_sequence <= sequence)
			m_channels[0].m_incoming_reliable_sequence = sequence + 1;
		m_state = gve_peer_state_connected; // send back the verify ,and check ack
											// to become connected
		GVM_DEBUG_LOG(net, string_name() << "sucess connected "
										 << "\r\n");
		post_ack_packet(connection_ack->send_time_stamp, 0, sequence);
		m_server->post_result(m_host_id, m_peer_id,
							  gve_session_event_connect_succeed);
		return true;
	}
	bool handle_disconnected(gv_uint sequence, gv_ulong rev_send_time)
	{
		this->m_state = gve_peer_state_disconnected;
		post_ack_packet(rev_send_time, 0, sequence);
		return true;
	}
	bool do_disconnect()
	{
		this->m_state = gve_peer_state_disconnecting;
		post_disconnection_packet();
		return true;
	}
	bool do_send_reliable(gv_ushort channel_id, gv_packet* packet)
	{
		update_outgoing_packet(packet, channel_id, gve_packet_type_send_reliable);
		gv_rudp_protocol_send_reliable* send =
			packet->get_protocol_header< gv_rudp_protocol_send_reliable >(2);
		m_last_send_time = send->send_time_stamp = get_time();
		send->fragment_id = 0;
		send->total_fragment = 0;
		post_reliable_packet(channel_id, packet);
		return true;
	}
	bool do_send_unreliable(gv_ushort channel_id, gv_packet* packet)
	{
		update_outgoing_packet(packet, channel_id, gve_packet_type_send_unreliable);
		gv_rudp_protocol_send_unreliable* send =
			packet->get_protocol_header< gv_rudp_protocol_send_unreliable >(2);
		m_last_send_time = send->send_time_stamp = get_time();
		post_unreliable_packet(channel_id, packet);
		return true;
	}

	bool handle_recieve_reliable(gv_packet* packet,
								 gv_rudp_protocol_command_header* command,
								 gv_rudp_protocol_send_reliable* send)
	{
		if (packet->get_packet_content_size() != command->packet_size)
		{
			GVM_DEBUG_LOG(net, string_name() << "packet size mismatch !!\r\n");
			return false;
		}
		gv_rudp_channel& channel = this->m_channels[command->channel_id];
		packet->m_sequence_number = command->sequence_number;
		m_last_recv_time = packet->m_time_stamp;
		packet->m_time_stamp = corrected_remote_time(send->send_time_stamp);
		if (channel.m_incoming_reliable_sequence >
			command->sequence_number)
		{   // already get the message before ,just ack
			// if previous is not recieve.
			post_ack_packet(send->send_time_stamp, command->channel_id,
							command->sequence_number);
			;
			return false;
		}
		else if (channel.m_incoming_reliable_sequence ==
				 command->sequence_number)
		{ // lucky we are waiting for you .
			channel.m_incoming_reliable_sequence++;
			m_server->post_result(m_host_id, m_peer_id, gve_session_event_recieve,
								  command->channel_id, packet);
		}
		else
		{
			channel.m_incoming_reliable_list.insert_sorted(packet);
		}
		post_ack_packet(send->send_time_stamp, command->channel_id,
						command->sequence_number);
		;
		return true;
	}
	bool handle_recieve_unreliable(gv_packet* packet,
								   gv_rudp_protocol_command_header* command,
								   gv_rudp_protocol_send_unreliable* send)
	{
		if (packet->get_packet_content_size() != command->packet_size)
		{
			GVM_DEBUG_LOG(net, string_name() << "packet size mismatch !!\r\n");
			return false;
		}
		gv_rudp_channel& channel = this->m_channels[command->channel_id];
		packet->m_sequence_number = command->sequence_number;
		channel.m_incoming_unreliable_sequence = gvt_max(
			command->sequence_number, channel.m_incoming_unreliable_sequence);
		m_last_recv_time = packet->m_time_stamp;
		packet->m_time_stamp = corrected_remote_time(send->send_time_stamp);
		m_server->post_result(m_host_id, m_peer_id, gve_session_event_recieve,
							  command->channel_id, packet);
		return true;
	}
	bool do_ack(gv_ushort channel, gv_uint sequence)
	{
		if (channel >= m_channels.size())
			return false;
		if (!m_channels[channel].remove_reliable_wait_packet(sequence))
		{
			return false;
		}
		return true;
	}
	bool update_connect()
	{
		if (m_state == gve_peer_state_zombie)
		{
			return false;
		}
		if (m_state == gve_peer_state_disconnected)
		{
			m_state = gve_peer_state_zombie;
			return true;
		}
		if (m_last_update_time - m_last_recv_time > gvc_very_long_time_out)
		{
			GVM_DEBUG_LOG(net, string_name() << "too many time not recv from the "
												"peer, it's must be dead!! \r\n");
			return false;
		}
		return true;
	}
	bool update_incoming_channel(gv_int channel_id)
	{
		gv_rudp_channel& channel = m_channels[channel_id];
		gv_rudp_channel::packets_list::iterator it =
			channel.m_incoming_reliable_list.begin();
		gv_rudp_channel::packets_list::iterator it_pre = NULL;
		while (it != channel.m_incoming_reliable_list.end())
		{
			gv_packet* packet = *it;
			if (m_last_update_time - packet->m_time_stamp > gvc_very_long_time_out)
			{
				GVM_DEBUG_LOG(net, string_name() << "the recieve channel " << channel_id
												 << " is blocked for too long \r\n");
				return false;
			}
			if (packet->m_sequence_number == channel.m_incoming_reliable_sequence)
			{
				m_server->post_result((gv_uint)m_host_id, (gv_uint)m_peer_id,
									  gve_session_event_recieve, (gv_ushort)channel_id,
									  packet, 0);
				channel.m_incoming_reliable_sequence++;
				it_pre = it;
				it++;
				channel.m_incoming_reliable_list.erase(it_pre);
				continue;
			}
			else if (packet->m_sequence_number <
					 channel.m_incoming_reliable_sequence)
			{
				// this will happen if the same sequence send twice!!
				it_pre = it;
				it++;
				channel.m_incoming_reliable_list.erase(it_pre);
				continue;
			}
			else
				break; // will wait for a missing sequence
		}
		return true;
	}

	bool update_outgoing_channel(gv_int channel_id)
	{
		gv_rudp_channel& channel = m_channels[channel_id];
		gv_rudp_channel::packets_list::iterator it =
			channel.m_outgoing_reliable_list.begin();
		while (!channel.m_outgoing_reliable_list.is_empty() &&
			   channel.m_cu_send_window_size < channel.m_max_send_window_size)
		{
			channel.m_cu_send_window_size++;
			it = channel.m_outgoing_reliable_list.unlink_front();
			channel.m_outgoing_reliable_wait_for_ack_list.link_back(it);
		};
		return true;
	}

	bool update_outgoing_channel_ack(gv_int channel_id)
	{
		gv_rudp_channel& channel = m_channels[channel_id];
		gv_rudp_channel::packets_list::iterator it =
			channel.m_outgoing_reliable_wait_for_ack_list.begin();
		while (it != channel.m_outgoing_reliable_wait_for_ack_list.end())
		{
			gv_packet* packet = *it;
			if (m_last_update_time - packet->m_time_stamp > gvc_very_long_time_out)
			{
				GVM_DEBUG_LOG(net, string_name() << "the sending channel " << channel_id
												 << " is blocked for too long \r\n");
				return false;
			}
			if (packet->m_resend_times > gvc_max_resend_times)
			{
				GVM_DEBUG_LOG(net, string_name() << "the sending channel " << channel_id
												 << " is blocked for too long \r\n");
				return false;
			}
			if (!(packet->m_resend_times) ||
				m_last_update_time - packet->m_time_stamp >
					get_timeout_for_resend())
			{
				// resend
				packet->m_time_stamp = get_time();
				m_server->send_to(this->m_address, packet); // posible broadcast
				packet->m_resend_times++;
			}
			it++;
		}
		return true;
	}
	bool update(gv_ulong current_time)
	{
		m_last_update_time = current_time;
		if (!update_connect())
		{
			return false;
		}
		for (int i = 0; i < m_channels.size(); i++)
		{
			if (!update_incoming_channel(i))
			{
				return false;
			}
			if (!update_outgoing_channel(i))
			{
				return false;
			}
			if (!update_outgoing_channel_ack(i))
			{
				return false;
			}
		}
		return true;
	}

	gv_ushort m_host_id;
	gv_ushort m_peer_id;
	gv_reliable_udp_server* m_server;
	gve_peer_state m_state;
	gv_socket_address m_address;
	gv_uint m_session_id;		  // one address still can have several session!!
	gv_ushort m_outgoing_host_id; // host id in send packet
	gv_ushort
		m_outgoing_peer_id; // peer id for host in send packet to the remote pc
	gv_int m_nb_acks;
	gv_long m_utc_time_difference;	// correction of timestamp for synchronization.
	gv_int m_min_round_trip_time;	 // time all in micro seconds.
	gv_int m_average_round_trip_time; // time all in micro seconds.
	gv_int m_max_round_trip_time;	 //
	gv_int m_last_round_trip_time;	//
	gv_ulong m_last_update_time;
	gv_ulong m_last_recv_time;
	gv_ulong m_last_send_time;
	gv_ulong m_create_time;
	gv_uint m_incoming_limit;
	gv_uint m_outgoing_limit;
	gv_uint m_connection_sequence;
	gvt_array< gv_rudp_channel > m_channels;
};
//-----------------------------------------------------------------------------
// RUDP HOST !!!
//-----------------------------------------------------------------------------
class gv_rudp_host : public gv_refable
{
	friend class gv_reliable_udp_server_impl;

public:
	gv_rudp_host(){};
	gv_rudp_host(gv_uint max_peer_count, gv_uint incoming_limit,
				 gv_uint outgoing_limit)
	{
		init(max_peer_count, incoming_limit, outgoing_limit);
	}
	void init(gv_uint max_peer_count, gv_uint incoming_limit,
			  gv_uint outgoing_limit)
	{
		m_peers.reset(max_peer_count);
		m_incoming_limit = incoming_limit;
		m_outgoing_limit = outgoing_limit;
		m_peer_map.reset();
	}
	virtual ~gv_rudp_host(){};
	gv_rudp_peer* create_peer()
	{
		gv_rudp_peer* peer = m_peers.allocate();
		if (!peer)
			return NULL;
		peer->reset();
		peer->m_peer_id = (gv_ushort)m_peers.get_index(peer);
		peer->m_host_id = m_host_id;
		peer->m_server = m_server;
		GVM_DEBUG_LOG(net, "[reliable_udp_server]peer:" << peer->string_name()
														<< "  created " << gv_endl);
		return peer;
	}
	void destroy_peer(gv_uint peer_id)
	{
		gv_rudp_peer* peer = get_peer(peer_id, false);
		if (!peer)
			return;
		GVM_DEBUG_LOG(net, "[reliable_udp_server]peer:"
							   << peer->string_name() << "  destroyed " << gv_endl);
		remove_peer_map(peer);
		peer->reset_channel(0);
		peer->reset();
		m_peers.free(peer);
	}
	gv_rudp_peer* get_peer(gv_uint peer_id, bool ignore_zombie = true)
	{
		if (!m_peers.is_valid(peer_id))
			return NULL;
		gv_rudp_peer* peer = &m_peers[peer_id];
		// why always skip zombie peer?
		if (ignore_zombie && peer->m_state == gve_peer_state_zombie)
			return NULL;
		return peer;
	}
	void update_peers()
	{
		gvt_pool_resizable< gv_rudp_peer >::iterator it;
		it = m_peers.begin();
		while (it != m_peers.end())
		{
			bool ret = it->update(gv_global::time->time_stamp_unix());
			if (!ret)
			{
				gvt_pool_resizable< gv_rudp_peer >::iterator it_pre = it;
				m_server->post_disconnect_now(it->m_host_id, it->m_peer_id);
				it++;
				destroy_peer(it_pre->m_peer_id);
				continue;
			}
			it++;
		}
		return;
	}
	gv_rudp_peer* find_peer(const gv_socket_address& addr)
	{
		gv_rudp_peer** pp = m_peer_map.find(addr);
		if (pp)
			return *pp;
		return NULL;
	}

	void add_peer_map(const gv_socket_address& addr, gv_rudp_peer* peer)
	{
		peer->m_address = addr;
		GVM_DEBUG_LOG(net, "[reliable_udp_server]peer:" << peer->string_name()
														<< "  set address! "
														<< gv_endl);
		m_peer_map.add(addr, peer);
	}

	void remove_peer_map(gv_rudp_peer* peer)
	{
		GVM_DEBUG_LOG(net, "[reliable_udp_server]peer:"
							   << peer->string_name()
							   << "  remove from address map! " << gv_endl);
		m_peer_map.erase(peer->m_address);
	}

	gvt_pool_resizable< gv_rudp_peer > m_peers;
	gv_uint m_incoming_limit;
	gv_uint m_outgoing_limit;
	gv_ushort m_host_id;
	gv_reliable_udp_server* m_server;
	gvt_hash_map< gv_socket_address, gv_rudp_peer* > m_peer_map;
};

//-----------------------------------------------------------------------------
// RUDP SERVER IMPL !!!
//-----------------------------------------------------------------------------
void GV_STDCALL
callback_rudp_on_low_level_recv(gvi_server_transport_layer_udp* server,
								gv_socket_address& address, gv_packet* p);
void GV_STDCALL callback_rudp_on_low_level_error(
	gvi_server_transport_layer_udp* server, gv_socket_address& address,
	gv_packet* p, gv_byte op, gv_uint err);
class gv_reliable_udp_server_impl
{
	friend class gv_reliable_udp_server;
	friend void GV_STDCALL
	callback_rudp_on_low_level_recv(gvi_server_transport_layer_udp* server,
									gv_socket_address& address, gv_packet* p);
	friend void GV_STDCALL callback_rudp_on_low_level_error(
		gvi_server_transport_layer_udp* server, gv_socket_address& address,
		gv_packet* p, gv_byte op, gv_uint err);

protected:
	gv_reliable_udp_server_impl()
	{
		m_transport_layer_server = NULL;
		m_enable_packet_dump = false;
		m_packet_log_file = NULL;
	}
	~gv_reliable_udp_server_impl()
	{
		m_transport_layer_server->destroy();
		gvt_safe_delete(m_transport_layer_server);
		// gv_global::pool_session_event::purge_memory();
	}
	bool create(gv_ushort port, int work_thread_nb,
				class gvi_server_transport_layer_udp* transport)
	{
		// m_transport_layer_server.enable_apc_call(false); //TODO can try_later

		if (!transport)
			m_transport_layer_server = new gv_normal_udp_server;
		else
			m_transport_layer_server = transport;

		m_transport_layer_server->set_user_data(this);
		m_transport_layer_server->set_callback_on_recv(
			callback_rudp_on_low_level_recv);
		m_transport_layer_server->set_callback_on_error(
			callback_rudp_on_low_level_error);
		return m_transport_layer_server->create(port, work_thread_nb,
												work_thread_nb * 2);
	}

	gv_uint host_create(gv_uint max_peer_count, gv_uint incoming_limit,
						gv_uint outgoing_limit)
	{
		gvt_ref_ptr< gv_rudp_host >* phost = m_hosts.allocate();
		*phost = new gv_rudp_host(max_peer_count, incoming_limit, outgoing_limit);
		;
		(*phost)->m_host_id = (gv_ushort)m_hosts.get_index(phost);
		(*phost)->m_server = this->m_owner;
		GVM_DEBUG_LOG(net, "[reliable_udp_server]host:"
							   << ((*phost)->m_host_id) << "  created with max "
							   << max_peer_count << " peers !" << gv_endl);
		return (*phost)->m_host_id;
	}
	void host_destroy(gv_uint host_id)
	{
		GV_ASSERT(m_hosts[host_id]);
		gvt_ref_ptr< gv_rudp_host >* phost = &m_hosts[host_id];
		// verify it is in the using list;
		GV_ASSERT_SLOW(m_hosts.find(*phost) != m_hosts.end());
		GVM_DEBUG_LOG(net, "[reliable_udp_server]host:" << ((*phost)->m_host_id)
														<< "  destroyed !"
														<< gv_endl);
		*phost = NULL;
		m_hosts.free(phost);
	}
	gv_rudp_host* get_host(gv_uint host_id)
	{
		if (host_id >= gvc_max_host_per_server)
			return NULL;
		if (!this->m_hosts[host_id])
			return NULL;
		return this->m_hosts[host_id];
	}
	bool get_host_peer(gv_uint host_id, gv_uint peer_id, gv_rudp_host*& host,
					   gv_rudp_peer*& peer, bool _ignore_zombie = true)
	{
		host = get_host(host_id);
		if (!host)
			return false;
		peer = host->get_peer(peer_id, _ignore_zombie);
		if (!peer)
			return false;
		return true;
	}
	gv_packet* create_packet(gve_packet_type type)
	{
		gv_packet* packet = m_transport_layer_server->create_packet();
		packet->push_protocol_header(gv_rudp_protocol_header(), false);
		packet->push_protocol_header(gv_rudp_protocol_command_header(), false);
		switch (type)
		{
		case gve_packet_type_acknowledge:
			packet->push_protocol_header(gv_rudp_protocol_acknowledge(), false);
			break;
		case gve_packet_type_connection:
			packet->push_protocol_header(gv_rudp_protocol_connection(), false);
			break;
		case gve_packet_type_verify_connection:
			packet->push_protocol_header(gv_rudp_protocol_verify_connection(), false);
			break;
		case gve_packet_type_disconnection:
			packet->push_protocol_header(gv_rudp_protocol_disconnection(), false);
			break;
		case gve_packet_type_ping:
			packet->push_protocol_header(gv_rudp_protocol_send_reliable(), false);
			break;
		case gve_packet_type_send_reliable:
			packet->push_protocol_header(gv_rudp_protocol_send_reliable(), false);
			break;
		case gve_packet_type_send_unreliable:
			packet->push_protocol_header(gv_rudp_protocol_send_unreliable(), false);
			break;
		default:
			GV_ASSERT(0);
		}
		gv_rudp_protocol_command_header* command =
			packet->get_protocol_header< gv_rudp_protocol_command_header >(1);
		command->command = (gv_byte)type;
		return packet;
	};
	void destroy_packet(gv_packet* p)
	{
		m_transport_layer_server->destroy_packet(p);
	};
	gv_session_event* create_event()
	{
		return s_session_event_factory.create_event();
	};
	bool send_to(const gv_socket_address& address, gv_packet* packet)
	{
		packet->inc_ref();
		bool ret = this->m_transport_layer_server->send_to(address, packet);
		if (m_enable_packet_dump && m_packet_log_file)
		{
			gv_string_tmp s;
			gvt_xml_write_element_open(s, "SEND");
			dump_packet(packet, s);
			gvt_xml_write_element_close(s, "SEND");
			(*m_packet_log_file) << s;
		}
		packet->dec_ref();
		return ret;
	};
	void report_bad_packet(gv_packet* p, const char* reason = NULL)
	{
#if !RETAIL
		gv_string_tmp s;
		s << "[bad_packet]";
		if (reason)
			s << reason;
		s << "get a bad packet size is " << p->get_network_diagram_size()
		  << " from " << p->m_address.string() << "\r\n";
		GVM_DEBUG_LOG(net, s);
#endif
	}
	bool is_protocol_head_valid(bool is_connection,
								gv_rudp_protocol_header* phead,
								const gv_socket_address& address,
								gv_rudp_host*& phost, gv_rudp_peer*& ppeer)
	{
		phost = this->get_host(phead->host_id);
		if (!phost)
			return false;
		if (is_connection)
			return true; // connection only need valid host id
		ppeer = phost->get_peer(phead->peer_id);
		if (!ppeer)
			return false;
		if (ppeer->m_address != address)
			return false;
		if (phead->flags & gve_rudp_header_flag_is_use_checksum)
		{
			// TODO verify_check_sum
		}
		else if (phead->session_id != ppeer->m_session_id)
			return false;
		return true;
	}
	bool handle_incoming_packet_type_acknowledge(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		gv_rudp_protocol_acknowledge* ack =
			packet->get_protocol_header< gv_rudp_protocol_acknowledge >(2);
		if (!ack)
			return false;
		if (peer->m_state == gve_peer_state_connecting_ack &&
			ack->received_reliable_sequence_number == peer->m_connection_sequence)
		{
			peer->m_state = gve_peer_state_connected;
			post_result(peer->m_host_id, peer->m_peer_id,
						gve_session_event_connect_succeed);
			GVM_DEBUG_LOG(net, "sucess connected " << packet->m_address.string()
												   << "as " << peer->m_host_id << "@"
												   << peer->m_peer_id << "\r\n");
		}
		else if (peer->m_state == gve_peer_state_disconnecting &&
				 ack->received_reliable_sequence_number ==
					 peer->m_connection_sequence)
		{
			peer->m_state = gve_peer_state_disconnected;
			// post_result ( peer->m_host_id, peer->m_peer_id ,
			// gve_session_event_disconnected);
			GVM_DEBUG_LOG(net, "disconnected " << packet->m_address.string() << "as "
											   << peer->m_host_id << "@"
											   << peer->m_peer_id << "\r\n");
		}
		else if (peer->m_state != gve_peer_state_connected)
		{
			return false;
		}
		if (!peer->do_ack(command->channel_id,
						  ack->received_reliable_sequence_number))
		{
			// this could happen if the packet is duplicated on net.
			return false;
		}
		if (!peer->update_time(ack->received_sent_time_stamp, packet->m_time_stamp,
							   ack->send_time_stamp))
		{
			report_bad_packet(packet, "round trip time is negative:");
			return false;
		}
		return true;
	}
	bool handle_incoming_packet_type_connection(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		gv_rudp_protocol_connection* connection =
			packet->get_protocol_header< gv_rudp_protocol_connection >(2);
		if (!connection)
			return false;
		if (!connection->channel_count)
			return false;
		peer = host->find_peer(packet->m_address);
		if (peer && peer->m_state != gve_peer_state_disconnected &&
			peer->m_state != gve_peer_state_zombie)
		{
			GVM_DEBUG_LOG(
				net, "the peer already there ,no need to connect again ,address is"
						 << packet->m_address.string() << "\r\n");
			// peer->post_connection_verify_packet (
			// connection,command->sequence_number);
			return false;
		}
		if (!peer)
			peer = host->create_peer();
		if (!peer)
		{
			this->post_result(peer->m_host_id, peer->m_peer_id,
							  gve_session_event_connect_fail);
			return false;
		}
		host->add_peer_map(packet->m_address, peer);
		peer->handle_connection(command->sequence_number, packet->m_address,
								connection);
		return true;
	}
	bool handle_incoming_packet_type_verify_connection(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		gv_rudp_protocol_verify_connection* connection_ack =
			packet->get_protocol_header< gv_rudp_protocol_verify_connection >(2);
		if (peer->m_state != gve_peer_state_connecting)
		{
			peer->post_ack_packet(connection_ack->send_time_stamp, 0,
								  command->sequence_number);
			GVM_DEBUG_LOG(net, "get connection verify in wrong state \r\n");
			return false;
		}
		if (!connection_ack)
			return false;
		return peer->handle_verify_connection(command->sequence_number,
											  packet->m_time_stamp, connection_ack);
	}
	bool handle_incoming_packet_type_disconnection(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		if (!packet->push_protocol_header(gv_rudp_protocol_disconnection(), false,
										  false))
		{
			report_bad_packet(packet);
			return false;
		}
		gv_rudp_protocol_disconnection* disconnection =
			packet->get_protocol_header< gv_rudp_protocol_disconnection >(2);
		return peer->handle_disconnected(command->sequence_number,
										 disconnection->send_time_stamp);
	}
	bool handle_incoming_packet_type_send_reliable(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		if (!packet->push_protocol_header(gv_rudp_protocol_send_reliable(), false,
										  false))
		{
			report_bad_packet(packet);
			return false;
		}
		gv_rudp_protocol_send_reliable* reliable_send =
			packet->get_protocol_header< gv_rudp_protocol_send_reliable >(2);
		peer->handle_recieve_reliable(packet, command, reliable_send);
		return true;
	}
	bool handle_incoming_packet_type_send_unreliable(
		gv_packet* packet, gv_rudp_protocol_command_header* command,
		gv_rudp_peer* peer, gv_rudp_host* host)
	{
		if (!packet->push_protocol_header(gv_rudp_protocol_send_unreliable(), false,
										  false))
		{
			report_bad_packet(packet);
			return false;
		}
		gv_rudp_protocol_send_unreliable* send =
			packet->get_protocol_header< gv_rudp_protocol_send_unreliable >(2);
		peer->handle_recieve_unreliable(packet, command, send);
		return true;
	}
	//------------------------------------------------------------------------------
	bool do_handle_incoming_packets()
	{
		gv_packet* incoming = gvt_lock_free_list_pop_all(m_incoming_packets);
		while (incoming)
		{
			gvt_ref_ptr< gv_packet > packet = incoming;
			GV_ASSERT(packet->get_ref() > 1);
			packet->dec_ref();
			incoming = incoming->next;
			if (!packet)
				continue;
			if (!packet->push_protocol_header(gv_rudp_protocol_header(), false,
											  false))
			{
				report_bad_packet(packet);
				continue;
			}
			if (!packet->push_protocol_header(gv_rudp_protocol_command_header(),
											  false, false))
			{
				report_bad_packet(packet);
				continue;
			}
			gv_rudp_protocol_header* phead =
				packet->get_protocol_header< gv_rudp_protocol_header >(0);
			gv_rudp_protocol_command_header* pcommand =
				packet->get_protocol_header< gv_rudp_protocol_command_header >(1);
			gv_rudp_host* host = NULL;
			gv_rudp_peer* peer = NULL;
			bool is_connection = pcommand->command == gve_packet_type_connection;
			if (!is_protocol_head_valid(is_connection, phead, packet->m_address, host,
										peer))
			{
				report_bad_packet(packet, "protocol head is corrupt!!");
				continue;
			}

			if (!is_connection && pcommand->channel_id >= peer->m_channels.size())
			{
				report_bad_packet(packet, "invalid channel id!!");
				continue;
			}
			switch (pcommand->command)
			{
			case gve_packet_type_acknowledge:
				handle_incoming_packet_type_acknowledge(packet, pcommand, peer, host);
				break;

			case gve_packet_type_connection:
				handle_incoming_packet_type_connection(packet, pcommand, peer, host);
				break;

			case gve_packet_type_verify_connection:
				handle_incoming_packet_type_verify_connection(packet, pcommand, peer,
															  host);
				break;

			case gve_packet_type_disconnection:
				handle_incoming_packet_type_disconnection(packet, pcommand, peer, host);
				break;

			case gve_packet_type_ping:
				// handle_incoming_packet_type_ping(packet,pcommand,peer,host);
				break;

			case gve_packet_type_send_reliable:
				handle_incoming_packet_type_send_reliable(packet, pcommand, peer, host);
				break;

			case gve_packet_type_send_unreliable:
				handle_incoming_packet_type_send_unreliable(packet, pcommand, peer,
															host);
				break;
			default:
			{
				report_bad_packet(packet, "invalid command id!!");
				continue;
			}
			} // handle
			if (m_enable_packet_dump && m_packet_log_file)
			{
				gv_string_tmp s;
				gvt_xml_write_element_open(s, "RECIEVE");
				dump_packet(packet, s);
				gvt_xml_write_element_close(s, "RECIEVE");
				(*m_packet_log_file) << s;
			}

		}; // next packet
		return true;
	}
	//------------------------------------------------------------------------------
	gv_uint post_request(const gvt_ref_ptr< gv_session_event >& event)
	{
		event->service_id = alloc_service_id();
		event->inc_ref();
		gvt_lock_free_list_push((gv_session_event*)event.ptr(),
								m_incoming_requests);
		return event->service_id;
	};
	gv_uint post_connect(const gv_socket_address& peer_address, gv_uint host_id,
						 gv_uint remote_host, gv_ushort channel_nb)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_connect;
		event->address = peer_address;
		event->host_id = host_id;
		event->channel = channel_nb;
		event->param0 = remote_host;
		return post_request(event);
	};
	gv_uint post_broadcast(gv_uint host_id, gv_ushort channel,
						   gv_packet* packet)
	{
		GV_ASSERT(0 && "under construct");
		return 0;
	};
	gv_uint post_send_unreliable(gv_uint host_id, gv_uint peer_id,
								 gv_ushort channel, gv_packet* packet)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_send_unreliable;
		event->host_id = host_id;
		event->peer_id = peer_id;
		event->channel = channel;
		event->packet = packet;
		return post_request(event);
	}
	gv_uint post_send_reliable(gv_uint host_id, gv_uint peer_id,
							   gv_ushort channel, gv_packet* packet)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_send_reliable;
		event->host_id = host_id;
		event->peer_id = peer_id;
		event->channel = channel;
		event->packet = packet;
		return post_request(event);
	}
	gv_uint post_ping(gv_uint host_id, gv_uint peer_id)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_ping;
		event->host_id = host_id;
		event->peer_id = peer_id;
		event->channel = 0;
		return post_request(event);
		;
	}
	gv_uint post_disconnect(gv_uint host_id, gv_uint peer_id)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_disconnect;
		event->host_id = host_id;
		event->peer_id = peer_id;
		return post_request(event);
	}
	gv_uint post_disconnect_now(const gv_socket_address& address)
	{
		gv_rudp_peer* peer = NULL;
		host_pool::iterator it;
		it = m_hosts.begin();
		while (it != m_hosts.end())
		{
			peer = (*it)->find_peer(address);
			if (peer)
				break;
			it++;
		}
		if (!peer)
		{
			GVM_DEBUG_LOG(net, "[RUDP]strange error come from no where!!!");
			return 0;
		}
		if (peer->get_state() == gve_peer_state_disconnected ||
			peer->get_state() == gve_peer_state_zombie)
		{
			return 0;
		}
		return post_disconnect_now(peer->m_host_id, peer->m_peer_id);
	}

	gv_uint post_disconnect_now(gv_uint host_id, gv_uint peer_id)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_disconnected;
		event->host_id = host_id;
		event->peer_id = peer_id;
		gv_rudp_peer* peer = NULL;
		gv_rudp_host* host = NULL;
		this->get_host_peer(host_id, peer_id, host, peer, false);
		if (peer)
		{
			event->address = peer->m_address;
		}
		if (peer && peer->get_state() == gve_peer_state_connecting)
		{
			event->event_type = gve_session_event_connect_fail;
		}
		return post_request(event);
	}
	gv_uint post_throttle_configure(gv_uint host_id, gv_uint peer_id)
	{
		GV_ASSERT(0 && "under construct");
		return 0;
	}
	gv_uint post_host_create(gv_uint max_peer_count, gv_uint incoming_limit,
							 gv_uint outgoing_limit)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_host_create;
		event->peer_id = max_peer_count;
		event->param0 = incoming_limit;
		event->param1 = outgoing_limit;
		return post_request(event);
	};
	gv_uint post_host_destroy(gv_uint host_id)
	{
		gvt_ref_ptr< gv_session_event > event = create_event();
		event->event_type = gve_session_event_host_destroy;
		event->host_id = host_id;
		return post_request(event);
	};
	//-----------------------------------------------------------------------------
	//===========EXEC===============================================================
	//-----------------------------------------------------------------------------
	gv_uint post_result(const gvt_ref_ptr< gv_session_event >& event)
	{
		m_results.push_back(event);
		if (event->event_type == gve_session_event_send_reliable)
		{
			if (event->packet &&
				!event->packet->get_packet_content_size())
			{   // empty packet,must be a
				// ping packet.
				event->event_type = gve_session_event_ping;
			}
		}
		return event->service_id;
	};
	gv_uint post_result(gv_uint host_id, gv_uint peer_id, gve_session_event type,
						gv_ushort channel_id = 0, gv_packet* packet = 0,
						gv_uint ret_value = 0)
	{
		gv_session_event* event = create_event();
		event->host_id = host_id;
		event->peer_id = peer_id;
		gv_rudp_peer* peer = NULL;
		gv_rudp_host* host = NULL;
		this->get_host_peer(host_id, peer_id, host, peer);
		if (peer)
		{
			event->address = peer->m_address;
		}
		event->round_trip_time = (gv_uint)(peer->m_last_round_trip_time / 1000);
		event->channel = channel_id;
		event->event_type = type;
		event->return_var = ret_value;
		event->service_id = 0;
		event->packet = packet;
		return post_result(event);
	};
	//-----------------------------------------------------------------------------
	//===========EXEC===============================================================
	//-----------------------------------------------------------------------------
	gv_uint alloc_service_id()
	{
		static gv_uint s_service_id = 0;
		return s_service_id++;
	}
	gv_uint exec_request(gvt_ref_ptr< gv_session_event >& event)
	{
		switch (event->event_type)
		{
		case gve_session_event_connect:
			return exec_connect(event);
		case gve_session_event_send_unreliable:
			return exec_send_unreliable(event);
		case gve_session_event_send_reliable:
			return exec_send_reliable(event);
		case gve_session_event_ping:
			return exec_ping(event);
		case gve_session_event_disconnect:
			return exec_disconnect(event);
		case gve_session_event_connect_fail:
			return exec_disconnect_now(event);
		case gve_session_event_disconnected:
			return exec_disconnect_now(event);
		case gve_session_event_host_create:
			return exec_host_create(event);
		case gve_session_event_host_destroy:
			return exec_host_destroy(event);
		}
		GVM_DEBUG_LOG(net, "warning !! a not handled event! type "
							   << (int)event->event_type);
		return event->service_id;
	};

	gv_uint exec_connect(gvt_ref_ptr< gv_session_event >& event)
	{
		gv_rudp_host* host = get_host(event->host_id);
		if (!host)
		{
			event->event_type = gve_session_event_connect_fail;
			return this->post_result(event);
		}
		gv_rudp_peer* peer;
		peer = host->find_peer(event->address);
		if (peer && peer->m_state != gve_peer_state_disconnected &&
			peer->m_state != gve_peer_state_zombie)
		{
			GVM_DEBUG_LOG(
				net, "the peer already there ,no need to connect again ,address is"
						 << event->address.string() << "\r\n");
			return false;
		}
		if (!peer)
			peer = host->create_peer();
		if (!peer)
		{
			event->event_type = gve_session_event_connect_fail;
			return this->post_result(event);
		}
		host->add_peer_map(event->address, peer);
		if (!event->channel)
			event->channel = 1;
		event->peer_id = peer->m_peer_id;
		peer->reset_channel(event->channel);
		peer->m_address = event->address;
		peer->m_outgoing_host_id = (gv_ushort)event->param0;
		peer->m_outgoing_peer_id = 0;
		if (!peer->do_connection())
		{
			event->event_type = gve_session_event_connect_fail;
			return this->post_result(event);
		}
		return event->service_id;
	};
	gv_uint exec_broadcast(gvt_ref_ptr< gv_session_event >& event)
	{
		GV_ASSERT(0 && "under construct");
		return 0;
	};
	gv_uint exec_send_unreliable(gvt_ref_ptr< gv_session_event >& event)
	{

		gv_rudp_host* host = NULL;
		gv_rudp_peer* peer = NULL;
		if (!get_host_peer(event->host_id, event->peer_id, host, peer))
		{
			GVM_DEBUG_LOG(net, "error in exec_send_unreliable!!\r\n");
			return event->service_id;
		}
		peer->do_send_unreliable(event->channel, event->packet);
		return event->service_id;
	}
	gv_uint exec_send_reliable(const gvt_ref_ptr< gv_session_event >& event)
	{
		gv_rudp_host* host = NULL;
		gv_rudp_peer* peer = NULL;
		if (!get_host_peer(event->host_id, event->peer_id, host, peer))
		{
			GVM_DEBUG_LOG(net, "error in exec_send_reliable!!\r\n");
			return event->service_id;
		}
		peer->do_send_reliable(event->channel, event->packet);
		return event->service_id;
	}
	gv_uint exec_ping(const gvt_ref_ptr< gv_session_event >& event)
	{
		// send a empty packet and wait for ack..
		gv_rudp_host* host = NULL;
		gv_rudp_peer* peer = NULL;
		if (!get_host_peer(event->host_id, event->peer_id, host, peer))
		{
			GVM_DEBUG_LOG(net, "error in exec_send_reliable!!\r\n");
			return event->service_id;
		}
		event->packet = create_packet(gve_packet_type_send_reliable);
		peer->do_send_reliable(event->channel, event->packet);
		event->return_var = peer->m_average_round_trip_time;
		return post_result(event);
	}
	gv_uint exec_reset(const gvt_ref_ptr< gv_session_event >& event)
	{
		gv_rudp_host* host = NULL;
		gv_rudp_peer* peer = NULL;
		if (get_host_peer(event->host_id, event->peer_id, host, peer))
		{
			peer->reset();
			host->destroy_peer(event->peer_id);
		}
		GV_ASSERT(event->event_type == gve_session_event_disconnected ||
				  event->event_type == gve_session_event_connect_fail);
		post_result(event);
		return event->service_id;
	}
	gv_uint exec_disconnect(const gvt_ref_ptr< gv_session_event >& event)
	{
		gv_rudp_host* host = NULL;
		gv_rudp_peer* peer = NULL;
		if (!get_host_peer(event->host_id, event->peer_id, host, peer))
		{
			GVM_DEBUG_LOG(net, "error in exec_reset!!\r\n");
			return event->service_id;
		}
		peer->do_disconnect();
		return event->service_id;
	}
	gv_uint exec_disconnect_now(const gvt_ref_ptr< gv_session_event >& event)
	{
		return exec_reset(event);
	}
	gv_uint exec_throttle_configure(const gvt_ref_ptr< gv_session_event >& event)
	{
		GV_ASSERT(0 && "under construct");
		return 0;
	}
	gv_uint exec_host_create(const gvt_ref_ptr< gv_session_event >& event)
	{
		event->host_id =
			this->host_create(event->peer_id, event->param0, event->param1);
		return post_result(event);
	};
	gv_uint exec_host_destroy(const gvt_ref_ptr< gv_session_event >& event)
	{
		this->host_destroy(event->param0);
		return post_result(event);
	};
	//------------------------------------------------------------------------------
	bool do_exec_user_requests()
	{
		GV_PROFILE_EVENT(gv_rudp_do_exec_user_requests, 0);
		gv_session_event* it = gvt_lock_free_list_pop_all(m_incoming_requests);
		while (it)
		{
			gvt_ref_ptr< gv_session_event > p(it);
			GV_ASSERT(it->get_ref() > 1);
			it->dec_ref();
			it = it->next;
			this->exec_request(p);
		}
		return true;
	}
	//------------------------------------------------------------------------------
	bool do_update_all_peers()
	{
		GV_PROFILE_EVENT(gv_rudp_do_process_peer_event, 0);
		host_pool::iterator it;
		it = m_hosts.begin();
		while (it != m_hosts.end())
		{
			(*it)->update_peers();
			it++;
		}
		return true;
	}
	//------------------------------------------------------------------------------
	bool do_service()
	{
		GV_PROFILE_EVENT(gv_rudp_do_service, 0);
		// clear
		m_results.clear();
		// dispatch all the incoming packets , fill the result queue
		do_handle_incoming_packets();
		// process the requests , and put processed event in the result.
		do_exec_user_requests();
		// process all the queue in channels, check timeout & other state, resend if
		// necessary, put new event in the result
		do_update_all_peers();
		// give transport layer a chance to update
		m_transport_layer_server->update();

		m_current_result_it = m_results.begin();
		return true;
	};
	//------------------------------------------------------------------------------
	bool peek_event(gvt_ref_ptr< gv_session_event >& event)
	{
		if (m_current_result_it != m_results.end())
		{
			event = *m_current_result_it;
			*m_current_result_it = NULL;
			m_current_result_it++;
			return true;
		}
		return false;
	};
#include "gv_reliable_udp_implement.hpp"
	bool dump_peer(gv_uint host_id, gv_uint peer_id, gv_string_tmp& string)
	{
		gv_rudp_peer* peer = NULL;
		gv_rudp_host* host = NULL;
		if (!this->get_host_peer(host_id, peer_id, host, peer))
		{
			return false;
		}
		return dump_peer(peer, string);
	}
	void enable_packet_dump(bool enable, gv_log* log)
	{
		m_enable_packet_dump = enable;
		m_packet_log_file = log;
	}
	//-------data---------------------------------------------------------------------
	// typedef gvt_lock_free_double_buffer< gvt_ref_ptr<
	// gv_packet>,gvc_event_buffer_size >  packet_buffer;
	// typedef gvt_lock_free_double_buffer< gvt_ref_ptr< gv_session_event>
	// ,gvc_event_buffer_size >  request_buffer;
	typedef gvt_lock_free_header< gv_packet* > packet_buffer;
	typedef gvt_lock_free_header< gv_session_event* > request_buffer;
	typedef gvt_dlist< gvt_ref_ptr< gv_session_event >, gvp_memory_32 >
		session_event_list;
	typedef gvt_pool_fixed_size< gvt_ref_ptr< gv_rudp_host >,
								 gvc_max_host_per_server >
		host_pool;
	bool m_enable_packet_dump;
	gv_log* m_packet_log_file;
	host_pool m_hosts;
	gvi_server_transport_layer_udp* m_transport_layer_server;
	gv_reliable_udp_server* m_owner;
	packet_buffer m_incoming_packets;
	request_buffer m_incoming_requests;
	session_event_list m_results;
	session_event_list::iterator m_current_result_it;
};

#include "gv_reliable_udp_server.hpp"
// Our packet processing function
void GV_STDCALL
callback_rudp_on_low_level_recv(gvi_server_transport_layer_udp* server,
								gv_socket_address& address, gv_packet* p)
{
	gv_reliable_udp_server_impl* pserver =
		(gv_reliable_udp_server_impl*)server->get_user_data();
	p->inc_ref();
	gvt_lock_free_list_push(p, pserver->m_incoming_packets);
}

void GV_STDCALL callback_rudp_on_low_level_error(
	gvi_server_transport_layer_udp* server, gv_socket_address& address,
	gv_packet* p, gv_byte operation, gv_uint error_code)
{
	gv_reliable_udp_server_impl* pserver =
		(gv_reliable_udp_server_impl*)server->get_user_data();
	pserver->post_disconnect_now(address);
}
};
