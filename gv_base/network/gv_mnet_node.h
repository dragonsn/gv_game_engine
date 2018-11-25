#pragma once
namespace gv
{
// mnet session id -----------------------------------
#define gvc_invalid_mnet_sid 0xffffffffffffffffULL
struct gv_mnet_sid
{
	gv_mnet_sid()
	{
		id64 = gvc_invalid_mnet_sid;
	}
	union {
		struct
		{
			gv_uint channel_id;
			gv_uint session_id;
		};
		gv_ulong id64;
	};
};

enum gve_mnet_session_state
{
	e_mnet_not_connected,
	e_mnet_connecting,
	e_mnet_connected,
	e_mnet_reconnecting,
	e_mnet_disconnected_pending,
	e_mnet_disconnected_dead,
	e_mnet_not_invalid,
};

struct gv_mnet_init_info
{
	gv_mnet_init_info();
	gv_short local_port;
	gv_int work_thread_count;
	gv_uint max_session;
	gv_uint tick_interval;
	gv_uint heart_beat_interval;
	gv_uint reconnect_time_out;
	gv_uint max_reconnect_time;
	gvt_ptr< class gvi_server_transport_layer_udp > server_ptr;
};
//--------------------------------------------------
//-responsible for connect & disconnect handleing
// one node-> one socket ->one io_service->1 end point ( whether server or
// client )
//--------------------------------------------------
class gv_mnet_imp;
class gv_mnet_node : gv_refable
{
public:
	gv_mnet_node();
	virtual ~gv_mnet_node();
	typedef boost::function< bool(gv_packet*, const gv_mnet_sid& sid) >
		session_event_callback;

	virtual bool start();
	virtual bool stop();
	virtual bool create(const gv_mnet_init_info&);

	virtual bool async_connect(const gv_socket_address& peer_address,
							   session_event_callback callback);
	virtual bool send(const gv_mnet_sid& session, gv_packet* packet,
					  bool reliable = true);
	virtual bool disconnect(const gv_mnet_sid& session);
	virtual bool force_disconnect(const gv_mnet_sid& session);

	const gv_mnet_init_info& get_int_info();

	// callbacks
	typedef boost::function< gv_packet*(gv_packet*) > packet_modifier_callback;
	typedef boost::function< bool(gv_packet*, const gv_mnet_sid& sid) >
		authentication_callback;

	virtual void push_reciever_modifier(packet_modifier_callback callback);
	virtual void push_sender_modifier(packet_modifier_callback callback);

	virtual void push_recieve_callback(session_event_callback callback);
	virtual void push_disconnect_callback(session_event_callback callback);
	virtual void push_net_error_callback(session_event_callback callback);
	virtual void push_authentication_callback(authentication_callback callback);

private:
	virtual void internal_tick();
	gvt_ptr< gv_mnet_imp > m_mnet_data;
};
};