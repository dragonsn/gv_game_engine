#pragma once
// general server interface based on reliable udp
namespace gv
{
class gv_server_session : public gv_object, public gv_state_machine
{
public:
	gv_int m_state;
	gv_ulong m_create_time;
	gv_ulong m_last_update_time;
	gv_ushort m_round_trip_time;
	gv_int m_reconnetion_time;
	gv_int m_round_trip_time;
	gv_int m_local_session_id;
	gv_int m_remote_session_id;
	gv_socket_address m_local_address;
	gv_socket_address m_remote_address;
};

struct gv_server_setting
{
	gv_int session_time_out_in_ms;
	gv_int heartbeat_interval_in_ms;
	gv_int max_package_size;
};

//<<CONCEPT>>
// strong security(encryption)
// short connection
// auto reconnect with authentication
// stability
// short latency
// flexible with listener stack
// low level implementation careless
// unified solution for game service and multilayer session
// cross platform
// simulate internet traffic(latency, packet loss)
// client also can have several session
// multi thread : session update in work threads .

class gv_server_listener;
class gv_server : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gv_server, gv_event_processor);

	virtual void start(gv_ushort port = -1, gv_int max_session = 1,
					   class gvi_server_transport_layer_udp* transport = NULL);
	// return session id
	virtual gv_int create_session(const gv_socket_address& remote);

	virtual bool destroy_session(gv_int session_id);

	virtual gv_server_session_info* get_session_info(gv_int session_id);

	virtual gv_packet* create_packet(gve_packet_type) const;

	virtual void destroy_packet(gv_packet* p) const;

	virtual void send_unreliable(gv_int session_id, gv_packet* packet);

	virtual void send_reliable(gv_int session_id, gv_packet* packet);

	virtual void send_raw_data(gv_int session_id, const gv_raw_data& data);

	virtual gv_int push_listener(gv_server_listener&);

	virtual gv_server_setting get_default_server_setting();
};

class gv_server_listener : public gv_object
{
public:
	virtual gv_packet* process_incoming_package(gv_packet* p,
												gv_server_session* session);

	virtual gv_packet* process_outgoing_package(gv_packet* p,
												gv_server_session* session);
};
}