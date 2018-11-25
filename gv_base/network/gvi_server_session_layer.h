#pragma once

namespace gv
{
static const int gvc_max_host_per_server = 1024;
static const int gvc_max_peer_per_host = 65536;
static const int gvc_max_channel_per_peer = 256;

static const int gvc_invalid_host_id = 0xffff;
static const int gvc_invalid_peer_id = 0xffff;

enum gve_packet_type
{
	//-------------------------------------------
	gve_packet_type_acknowledge,
	gve_packet_type_connection,
	gve_packet_type_verify_connection,
	gve_packet_type_disconnection,
	gve_packet_type_ping,
	gve_packet_type_send_reliable,
	gve_packet_type_send_unreliable,
};

enum gve_peer_state
{
	gve_peer_state_disconnected,
	gve_peer_state_connecting,
	gve_peer_state_connecting_ack,
	gve_peer_state_connection_pending,
	gve_peer_state_connection_succeeded,
	gve_peer_state_connected,
	gve_peer_state_disconnecting,
	gve_peer_state_disconnecting_ack,
	gve_peer_state_zombie
};

enum gve_session_event
{
	gve_session_event_none,
	gve_session_event_connect,
	gve_session_event_connect_succeed,
	gve_session_event_connect_fail,
	gve_session_event_recieve,
	gve_session_event_send_unreliable,
	gve_session_event_send_reliable,
	gve_session_event_ping,
	gve_session_event_disconnect,
	gve_session_event_disconnected,
	gve_session_event_limit_bandwidth,
	gve_session_event_host_create,
	gve_session_event_host_broadcast,
	gve_session_event_host_destroy
};

// main event used to communicate with other thread
class gv_session_event : public gv_refable_with_factory
{
public:
	gv_session_event()
	{
		user_data = 0;
		host_id = gvc_max_host_per_server;
		peer_id = gvc_max_peer_per_host;
		event_type = gve_session_event_none;
		channel = gvc_max_channel_per_peer;
		service_id = 0;
		require_return = false;
		return_var = param0 = param1 = param2 = param3 = 0;
		next = 0;
	};
	~gv_session_event()
	{
	}
	gv_socket_address address;
	gv_uint host_id;
	gv_uint peer_id;
	gv_uint round_trip_time;
	gvt_ref_ptr< gv_packet > packet;
	gve_session_event event_type;
	gv_uint user_data;
	gv_uint service_id;
	gv_uint session_id;
	gv_ushort channel;
	bool require_return;
	gv_uint return_var;
	gv_uint param0;
	gv_uint param1;
	gv_uint param2;
	gv_uint param3;

	gv_session_event* next;
};

class gvi_server_session_layer : public gv_refable
{
public:
	gvi_server_session_layer(){};
	virtual ~gvi_server_session_layer(){};
	virtual bool create(gv_ushort port, int work_thread_nb = 0,
						class gvi_server_transport_layer_udp* transport = 0) = 0;
	//[[THREAD-SAFE-START]] -----can be called outside the service thread
	virtual gv_packet* create_packet(gve_packet_type) = 0;
	virtual void destroy_packet(gv_packet*) = 0;
	virtual void dump_packet(gv_packet*, gv_string_tmp& string) = 0;
	virtual gv_session_event* create_event() = 0;
	// post a event to request queue to command server , return unique service id.
	virtual gv_uint post_request(const gvt_ref_ptr< gv_session_event >& event) = 0;
	virtual gv_uint post_connect(const gv_socket_address& peer_address,
								 gv_uint host_id, gv_uint remote_host,
								 gv_ushort channel_nb) = 0;
	virtual gv_uint post_broadcast(gv_uint host_id, gv_ushort channel,
								   gv_packet* packet) = 0;
	virtual gv_uint post_send_unreliable(gv_uint host_id, gv_uint peer_id,
										 gv_ushort channel,
										 gv_packet* packet) = 0;
	virtual gv_uint post_send_reliable(gv_uint host_id, gv_uint peer_id,
									   gv_ushort channel, gv_packet* packet) = 0;
	virtual gv_uint post_ping(gv_uint host_id, gv_uint peer_id) = 0;
	virtual gv_uint post_disconnect(gv_uint host_id, gv_uint peer_id) = 0;
	virtual gv_uint post_disconnect_now(gv_uint host_id, gv_uint peer_id) = 0;
	virtual gv_uint post_throttle_configure(gv_uint host_id, gv_uint peer_id) = 0;
	virtual gv_uint post_host_create(gv_uint max_peer_count,
									 gv_uint incoming_limit,
									 gv_uint outgoing_limit) = 0;
	virtual gv_uint post_host_destroy(gv_uint host_id) = 0;
	// post a event to the output queue , that can be fetched by the peek_event
	virtual gv_uint post_result(const gvt_ref_ptr< gv_session_event >& event) = 0;
	virtual gv_uint post_result(gv_uint host_id, gv_uint peer_id,
								gve_session_event type, gv_ushort channel_id = 0,
								gv_packet* packet = 0, gv_uint user_data = 0) = 0;
	//[[NONE-THREAD-SAFE-START]]---only can be called in the service thread.
	virtual bool do_service() = 0;
	virtual bool peek_event(gvt_ref_ptr< gv_session_event >&) = 0;
	//[[DEBUG]]
	virtual void enable_packet_dump(bool enable, gv_log* log){};
	virtual bool dump_peer(gv_uint host_id, gv_uint peer_id,
						   gv_string_tmp& string)
	{
		return false;
	}
	virtual void simulate_latency(gv_float ms,
								  gv_uint host_id = gvc_max_host_per_server,
								  gv_uint peer_id = gvc_max_peer_per_host){};
	virtual void simulate_packet_loss(gv_float ratio,
									  gv_uint host_id = gvc_max_host_per_server,
									  gv_uint peer_id = gvc_max_peer_per_host){};
};
}