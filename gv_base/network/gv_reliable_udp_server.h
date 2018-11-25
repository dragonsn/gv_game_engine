#pragma once
namespace gv
{
class gv_reliable_udp_server : public gvi_server_session_layer
{
	friend class gv_rudp_peer;
	friend class gv_rudp_host;

public:
	gv_reliable_udp_server();
	virtual ~gv_reliable_udp_server();
	virtual bool create(gv_ushort port, int work_thread_nb = 0,
						class gvi_server_transport_layer_udp* transport = NULL);
	// thread safe -----can be called outside the service thread
	virtual gv_packet* create_packet(gve_packet_type);
	virtual void destroy_packet(gv_packet* p);
	virtual void dump_packet(gv_packet*, gv_string_tmp& string);
	virtual gv_session_event* create_event();
	// post a event to request queue to command server , return unique service id.
	virtual gv_uint post_request(const gvt_ref_ptr< gv_session_event >& event);
	virtual gv_uint post_connect(const gv_socket_address& peer_address,
								 gv_uint host_id, gv_uint remote_host,
								 gv_ushort channel_nb);
	virtual gv_uint post_broadcast(gv_uint host_id, gv_ushort channel,
								   gv_packet* packet);
	virtual gv_uint post_send_unreliable(gv_uint host_id, gv_uint peer_id,
										 gv_ushort channel, gv_packet* packet);
	virtual gv_uint post_send_reliable(gv_uint host_id, gv_uint peer_id,
									   gv_ushort channel, gv_packet* packet);
	virtual gv_uint post_ping(gv_uint host_id, gv_uint peer_id);
	virtual gv_uint post_disconnect(gv_uint host_id, gv_uint peer_id);
	virtual gv_uint post_disconnect_now(gv_uint host_id, gv_uint peer_id);
	virtual gv_uint post_throttle_configure(gv_uint host_id, gv_uint peer_id);
	virtual gv_uint post_host_create(gv_uint max_peer_count,
									 gv_uint incoming_limit,
									 gv_uint outgoing_limit);
	virtual gv_uint post_host_destroy(gv_uint host_id);
	// not thread safe---only can be called in the service thread.
	virtual gv_uint post_result(const gvt_ref_ptr< gv_session_event >& event);
	virtual gv_uint post_result(gv_uint host_id, gv_uint peer_id,
								gve_session_event type, gv_ushort channel_id = 0,
								gv_packet* packet = 0, gv_uint user_data = 0);
	virtual bool do_service();
	virtual bool peek_event(gvt_ref_ptr< gv_session_event >&);
	virtual void enable_packet_dump(bool enable, gv_log* log);
	virtual bool dump_peer(gv_uint host_id, gv_uint peer_id,
						   gv_string_tmp& string);

protected:
	virtual bool send_to(const gv_socket_address& address, gv_packet* packet);
	class gv_reliable_udp_server_impl* m_pimpl;
};
};