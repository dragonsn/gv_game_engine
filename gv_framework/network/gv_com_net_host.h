#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
enum gve_host_state
{
	gve_host_not_created,
	gve_host_disconnected,
	gve_host_disconnecting,
	gve_host_connecting,
	gve_host_connected,
	gve_host_reconnecting,
	gve_host_zombie,
	gve_host_state_max,
};

static const int gvc_bt_tree_host_level = 0;
static const int gvc_max_client_connection = 16;
static const int gvc_net_state_connection_level = 2;
class gv_remote_call;
class gv_com_net_replica;
struct gv_net_error
{
	gv_net_error()
	{
		m_has_error = false;
		m_error_string = "";
	}
	gv_bool m_has_error;
	gv_uint m_error_code;
	gv_string_tmp m_error_string;
};

//============================================================================================
//								:
//============================================================================================
class gv_network_mgr;
class gv_com_net_host : public gv_com_tasklet
{
	friend class gv_network_mgr;
	friend class gv_network_mgr_imp;

public:
	GVM_DCL_CLASS(gv_com_net_host, gv_com_tasklet);
	gv_com_net_host();
	~gv_com_net_host();
	//============================================================================================
	//							:the component methods
	//============================================================================================
	gv_bool tick(gv_float dt);
	gv_bool set_resource(gv_resource*);
	gv_bool is_tickable();
	gv_bool sync_to_entity(gv_entity*);
	void on_attach();
	void on_detach();
	//============================================================================================
	//							:basic host  methods
	//============================================================================================
	gv_network_mgr* get_network_mgr()
	{
		return m_network;
	}
	void init_host(const gv_socket_address& address, gv_bool is_local = true,
				   gv_float send_delta_time = 0.1f,
				   gv_int remote_host_id = gvc_invalid_host_id,
				   gv_int max_connection = gvc_max_client_connection,
				   bool auto_connect = true);
	gv_string_tmp host_name();
	gv_com_net_host* get_connected_host(const gv_id& name);
	gv_int get_nb_connected_host();
	gv_com_net_host* get_connected_host(gv_int idx);
	gve_host_state get_host_state();
	void set_host_state(gve_host_state);
	void set_host_address(const gv_socket_address&);
	const gv_socket_address& get_host_address();
	gv_bool is_server();
	gv_bool is_remote()
	{
		return m_is_remote;
	}
	gv_bool is_persistent()
	{
		return m_is_persistent;
	} // don't remove the host even it's disconnected;
	void set_persistent(gv_bool b = true)
	{
		m_is_persistent = b;
	}
	void set_remote(gv_bool b = true)
	{
		m_is_remote = b;
	}
	gv_bool is_connected();
	gv_bool manual_connect();
	gv_bool manual_disconnect();
	virtual bool already_login()
	{
		return false;
	};
	template < class type_of_command >
	inline gv_bool post_net_cmd(const type_of_command& cmd,
								gv_bool is_reliable = true,
								gv_com_tasklet* sender = NULL)
	{
		gvt_lock_free_list_push(
			(gv_net_cmd_list_base*)new gvt_net_cmd_list< type_of_command >(
				cmd, is_reliable, sender),
			m_posted_net_cmd_list);
		return true;
	};
	template < class type_of_command >
	inline gv_bool post_net_cmd(type_of_command* cmd, gv_bool is_reliable = true,
								gv_com_tasklet* sender = NULL)
	{
		gvt_lock_free_list_push(
			(gv_net_cmd_list_base*)new gvt_net_cmd_list_ptr< type_of_command >(
				cmd, is_reliable, sender),
			m_posted_net_cmd_list);
		return true;
	};
	//============================================================================================
	//								:generic cmd
	//helper
	//============================================================================================
	gv_bool post_net_cmd(const gv_net_cmd_generic& cmd);
	inline gv_bool post_net_cmd(gv_ushort cmd, gv_bool reliable)
	{
		gv_net_cmd_generic c;
		c.m_generic_cmd_id = cmd;
		c.m_nb_pay_load = 0;
		post_net_cmd(c);
		return true;
	};
	template < class T0 >
	inline gv_bool post_net_cmd(gv_ushort cmd, gv_bool reliable, const T0& t0)
	{
		gv_net_cmd_generic c;
		c.m_generic_cmd_id = cmd;
		c.m_nb_pay_load = 1;
		c.m_payloads[0] = t0;
		post_net_cmd(c);
		return true;
	};
	template < class T0, class T1 >
	inline gv_bool post_net_cmd(gv_ushort cmd, gv_bool reliable, const T0& t0,
								const T1& t1)
	{
		gv_net_cmd_generic c;
		c.m_generic_cmd_id = cmd;
		c.m_nb_pay_load = 2;
		c.m_payloads[0] = t0;
		c.m_payloads[1] = t1;
		post_net_cmd(c);
		return true;
	};
	template < class T0, class T1, class T2 >
	inline gv_bool post_net_cmd(gv_ushort cmd, gv_bool reliable, const T0& t0,
								const T1& t1, const T2& t2)
	{
		gv_net_cmd_generic c;
		c.m_generic_cmd_id = cmd;
		c.m_nb_pay_load = 3;
		c.m_payloads[0] = t0;
		c.m_payloads[1] = t1;
		c.m_payloads[2] = t2;
		post_net_cmd(c);
		return true;
	};
	template < class T0, class T1, class T2, class T3 >
	inline gv_bool post_net_cmd(gv_ushort cmd, gv_bool reliable, const T0& t0,
								const T1& t1, const T2& t2, const T3& t3)
	{
		gv_net_cmd_generic c;
		c.m_generic_cmd_id = cmd;
		c.m_nb_pay_load = 4;
		c.m_payloads[0] = t0;
		c.m_payloads[1] = t1;
		c.m_payloads[2] = t2;
		c.m_payloads[3] = t3;
		post_net_cmd(c);
		return true;
	};
	// can continue
	//============================================================================================
	//								:post a  call (not maintained
	//)
	//============================================================================================
	gv_bool post_remote_call(gv_remote_call* call, bool need_return = true);

public:
	template < class type_of_command >
	inline gv_bool send_net_cmd(type_of_command& cmd,
								gv_bool is_reliable = true)
	{
		if (is_deleted())
			return false; //
		GV_ASSERT(m_is_remote);
		GV_ASSERT(gvt_is_valid_net_cmd(cmd));
		gv_int size_to_send = gvt_size_in_packet(cmd) + 2;
		gv_packet* p = get_packet_with_size(size_to_send, is_reliable);
		if (!p)
			return false;
		gv_net_cmd header;
		header.m_index = type_of_command::static_net_cmd_id();
		(*p) << (header);
		(*p) << (cmd);
		GVM_DEBUG_LOG(net, "[send_net_cmd]" << cmd.static_name()
											<< " to:" << host_name() << gv_endl);
		return true;
	};
	template < class type_of_command >
	inline gv_bool recieve_net_cmd(type_of_command& cmd)
	{
		GV_ASSERT(0 && "not handled cmd !!")
		return false;
	}
	template < class type_of_command >
	inline void broadcast_net_cmd(type_of_command& cmd,
								  gv_bool is_reliable = true)
	{
		GV_ASSERT(!m_is_remote);
		for (int i = 0; i < this->m_connected_hosts.size(); i++)
		{
			m_connected_hosts[i]->send_net_cmd(cmd, is_reliable);
		}
	};
	gv_bool send_remote_call(gv_remote_call* call);
	gv_int get_max_replica_var_one_time();
	void set_max_replica_var_one_time(gv_int);
	gv_uint get_uuid()
	{
		return m_unique_user_id;
	}
	void set_uuid(gv_uint uuid)
	{
		m_unique_user_id = uuid;
	}
	gv_com_net_replica* get_out_going_replica()
	{
		return m_out_going_replica;
	}
	void set_out_going_replica(gv_com_net_replica* rep)
	{
		m_out_going_replica = rep;
	}
	gv_com_net_replica* get_in_coming_replica()
	{
		return m_in_coming_replica;
	}
	void set_in_coming_replica(gv_com_net_replica* rep)
	{
		m_in_coming_replica = rep;
	}

protected:
	// event for local host
	virtual void on_connection(gv_com_net_host* remote){};
	virtual void on_disconnection(gv_com_net_host* remote){};

protected:
	gvt_ref_ptr< gv_packet >& get_pending_packet(gv_bool need_reliable);
	gv_packet* create_packet(gv_bool need_reliable);
	gv_packet* get_packet_with_size(gv_int size, gv_bool need_reliable);
	bool test_packet_with_size(gv_int size, gv_bool need_reliable);
	void queue_incoming_packet(gv_packet* packet);
	// when a remote host connect to server ,server will call this function.
	virtual gv_com_net_host* create_client();

	virtual void flush_out_going_packets();
	void flush_out_going_packets(bool use_reliable);
	virtual void flush_in_coming_packets();
	virtual void update_out_going_replica();
	virtual void update_in_coming_replica();
	virtual gv_bool recieve_one_packet(gv_packet& pkt);

	virtual gv_uint get_average_latency();
	virtual gv_float get_packet_loss_rate();
	gv_uint get_cu_packet_sequence_number()
	{
		return m_cu_packet_sq;
	};

	virtual gv_bool tick_local_host(gv_float dt);
	virtual gv_bool tick_remote_host(gv_float dt);
	virtual gv_bool tick_connection(gv_float dt);

	virtual gv_bool exec_net_cmd(const gv_net_cmd&, gv_packet& pkt,
								 gv_net_error& error);
	virtual gv_bool exec_remote_call(const gv_net_cmd&, gv_packet& pkt,
									 gv_net_error& error);
	virtual gv_bool exec_remote_call_ack(const gv_net_cmd&, gv_packet& pkt,
										 gv_net_error& error);
	virtual gv_bool exec_replicate(const gv_net_cmd&, gv_packet& pkt,
								   gv_net_error& error);
	virtual gv_bool exec_remote_call(const gv_remote_call& call);

	virtual gv_bool task_connect(gv_float time_out);
	virtual gv_bool task_disconnect(gv_float time_out);
	virtual gv_bool task_wait_net_cmd(const gv_net_cmd& cmd,
									  gv_float time_out = 0);
	virtual gv_bool task_remote_call(gv_remote_call* call, gv_remote_call* ret,
									 gv_float time_out = 0);

protected:
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_ping& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_sync_game_time& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_sync_system_time& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_chat& cmd);

	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_0& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_1& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_2& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_3& cmd);

	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_4& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_5& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_6& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_7& cmd);

	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_8& cmd);
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_cmd_generic_9& cmd);

	virtual gv_bool
	exec_remote_call(const class gv_remote_call_client_login& call);
	virtual gv_bool
	exec_remote_call(const class gv_remote_call_client_login_ack& call);
	//-------------------------------------------------------------------------------------------

	void fill_one_replica_update(struct gv_net_replica_update_info& info,
								 gv_packet& pkt, gv_ushort& last_replica_id,
								 gv_byte& last_array_index,
								 gv_byte& last_var_idx);
	void process_pending_call();
	void process_pending_net_cmd();

	gv_socket_address m_ip_address;
	gv_int m_max_connection;
	gv_int m_max_replica_var_one_time;
	gv_bool m_is_remote;
	gv_bool m_auto_connect;
	gv_bool m_is_persistent;
	gv_float m_send_delta_time;
	gv_float m_current_send_time;
	gv_float m_heart_beat_interval;
	gv_float m_current_heart_beat_time;
	gv_uint m_cu_packet_sq;

	gv_uint m_unique_user_id;
	gv_int m_host_id;
	gv_int m_peer_id;
	gv_int m_remote_host_id; // we share  port for each host, so when do
							 // connection to a foreign host , need to pass the
							 // target host id, ( think it as game room NO.).
	gv_bool m_connected;
	gv_bool m_is_hacker;

	gvt_array< gvt_ref_ptr< gv_com_net_host > > m_connected_hosts;
	gvt_ref_ptr< gv_packet > m_pending_reliable_pkt;
	gvt_ref_ptr< gv_packet > m_pending_unreliable_pkt;
	gvt_ref_ptr< gv_packet > m_incoming_pkt_list;
	gvt_ptr< gv_network_mgr > m_network;
	gvt_ref_ptr< gv_com_net_replica > m_out_going_replica;
	gvt_ref_ptr< gv_com_net_replica > m_in_coming_replica;
	gvt_ptr< gv_remote_call > m_call_return;
	gvt_ptr< gv_remote_call > m_waiting_call;
	gv_net_cmd m_waiting_command;

	gvt_lock_free_header< gv_net_cmd_list_base* > m_posted_net_cmd_list;
	gvt_lock_free_header< gv_remote_call* > m_posted_call_list;
};
}