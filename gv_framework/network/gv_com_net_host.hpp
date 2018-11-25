namespace gv
{
gv_com_net_host::gv_com_net_host()
{
	GVM_SET_CLASS(gv_com_net_host);
	m_send_delta_time = 0.1f;
	m_current_send_time = 0.f;
	m_unique_user_id = gvc_invalid_global_user_id;
	m_peer_id = gvc_invalid_host_id;
	m_host_id = gvc_invalid_peer_id;
	m_remote_host_id = gvc_invalid_host_id;
	m_connected = false;
	m_current_heart_beat_time = m_heart_beat_interval = 30.f;
	m_max_connection = 1;
	m_max_replica_var_one_time = 200;
	m_is_remote = true;
	m_is_persistent = false;
	m_is_hacker = false;
};
gv_com_net_host::~gv_com_net_host(){

};

void* gv_net_cmd_list_base::operator new(size_t size)
{
	GV_ASSERT(size < 256);
	return gv_global::pool_256.allocate();
};
void gv_net_cmd_list_base::operator delete(void* p)
{
	gv_global::pool_256.free(p);
};
//============================================================================================
//							:the component methods
//============================================================================================
gv_bool gv_com_net_host::tick(gv_float dt)
{
	this->flush_in_coming_packets();

	if (m_is_remote)
		tick_remote_host(dt);
	else
		tick_local_host(dt);

	this->m_current_send_time -= dt;
	if (this->m_current_send_time <= 0)
	{
		this->m_current_send_time = this->m_send_delta_time;
		this->update_out_going_replica();
		this->flush_out_going_packets();
	}
	return true;
}

gv_bool gv_com_net_host::tick_connection(gv_float dt)
{
	m_current_heart_beat_time -= dt;
	if (m_current_heart_beat_time <= 0)
	{
		gv_net_cmd_host_ping cmd;
		this->send_net_cmd(cmd, true);
		m_current_heart_beat_time = this->m_heart_beat_interval;
	}
	this->process_pending_call();
	this->process_pending_net_cmd();
	return false;
};

gv_bool gv_com_net_host::set_resource(gv_resource*)
{
	return true;
};
gv_bool gv_com_net_host::is_tickable()
{
	return false; // tick in network thread by default .
};
gv_bool gv_com_net_host::sync_to_entity(gv_entity*)
{
	return true;
};
void gv_com_net_host::on_attach()
{
	GVM_POST_EVENT(net_add_host, net, pe->host = this);
}
void gv_com_net_host::on_detach()
{
	GVM_POST_EVENT(net_remove_host, net, pe->host = this);
}

//============================================================================================
//							:basic host  methods
//============================================================================================

void gv_com_net_host::init_host(const gv_socket_address& address,
								gv_bool is_local, gv_float send_delta_time,
								gv_int remote_host_id, gv_int max_connection,
								bool auto_connect)
{
	// m_is_remote=!address.m_sin_addr.is_local_host();
	m_is_remote = !is_local; // remote host can be in the same physical machine.
	m_max_connection = max_connection;
	m_ip_address = address;
	m_auto_connect = auto_connect;
	m_send_delta_time = send_delta_time;
	m_remote_host_id = remote_host_id;
	if (!m_is_remote)
	{
		this->set_host_state(gve_host_connected); // localhost is always connected!!
	}
	else if (m_is_remote && m_auto_connect)
	{
		this->set_host_state(gve_host_connecting);
	}
	else
	{
		this->set_host_state(gve_host_disconnected);
	}
};

gv_bool gv_com_net_host::manual_connect()
{
	if (this->m_is_remote && this->get_host_state() == gve_host_disconnected)
	{
		this->set_host_state(gve_host_connecting);
		return true;
	}
	return false;
};
gv_bool gv_com_net_host::manual_disconnect()
{
	if (this->m_is_remote && this->get_host_state() == gve_host_connected)
	{
		this->set_host_state(gve_host_disconnecting);
		return true;
	}
	return false;
};
gve_host_state gv_com_net_host::get_host_state()
{
	return (gve_host_state) this->get_state(gvc_bt_tree_host_level);
};
void gv_com_net_host::set_host_state(gve_host_state s)
{
	this->goto_state((gv_bt_state_type)s, gvc_bt_tree_host_level);
};

gv_uint gv_com_net_host::get_average_latency()
{
	return 0;
};

gv_float gv_com_net_host::get_packet_loss_rate()
{
	return 0.f;
};

const gv_socket_address& gv_com_net_host::get_host_address()
{
	return this->m_ip_address;
}

void gv_com_net_host::set_host_address(const gv_socket_address& addr)
{
	this->m_ip_address = addr;
};

gv_string_tmp gv_com_net_host::host_name()
{
	gv_string_tmp s;
	s << "[" << this->get_name_id() << "|" << this->m_ip_address.string() << "|"
	  << this->m_host_id << "@" << this->m_peer_id << "]";
	return s;
}

gv_bool gv_com_net_host::is_server()
{
	return this->m_max_connection > gvc_max_client_connection;
};

gv_bool gv_com_net_host::is_connected()
{
	return this->m_connected;
};

gv_bool gv_com_net_host::tick_remote_host(gv_float dt)
{
	super::tick(dt);
	GVM_BT_START
	GVM_BT_NODE_SELECT(gve_host_disconnected)
	GVM_BT_LEAF(1, goto_state(1))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(gve_host_connecting)
	GVM_BT_LEAF_LATENT(1, task_connect(60));
	GVM_BT_LEAF_TEST(2, m_connected, goto_state(gve_host_connected, -1))
	GVM_BT_LEAF(3, goto_state(gve_host_disconnected, -1))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(gve_host_connected)
	GVM_BT_LEAF(1, tick_connection(dt));
	GVM_BT_LEAF(2, goto_state(1))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(gve_host_disconnecting)
	GVM_BT_LEAF(1, task_disconnect(0) && false);
	GVM_BT_LEAF(2, goto_state(gve_host_disconnected, -1))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(gve_host_zombie)
	GVM_BT_LEAF(1, goto_state(0))
	GVM_BT_END_NODE
	GVM_BT_END
	return true;
};

gv_bool gv_com_net_host::tick_local_host(gv_float dt)
{
	super::tick(dt);
	return true;
}
//============================================================================================
//								:
//============================================================================================
gvt_ref_ptr< gv_packet >&
gv_com_net_host::get_pending_packet(gv_bool need_reliable)
{
	if (need_reliable)
		return m_pending_reliable_pkt;
	else
		return m_pending_unreliable_pkt;
};

gv_packet* gv_com_net_host::create_packet(gv_bool need_reliable)
{
	GV_ASSERT(this->m_network);
	gv_packet* pkt = this->m_network->create_packet(need_reliable);
	GV_ASSERT(pkt);
	gvt_ref_ptr< gv_packet >& pending = get_pending_packet(need_reliable);
	if (pending)
	{
		(*pending) << gv_net_cmd_end();
		m_network->send_packet(this, pending, need_reliable);
	}
	pending = pkt;
	return pkt;
};

gv_packet* gv_com_net_host::get_packet_with_size(gv_int size,
												 gv_bool need_reliable)
{
	if (test_packet_with_size(size, need_reliable))
	{
		return get_pending_packet(need_reliable);
	}
	return create_packet(need_reliable);
};

gv_bool gv_com_net_host::test_packet_with_size(gv_int size,
											   gv_bool need_reliable)
{
	GV_ASSERT(size < gvc_default_mtu - 2);
	gvt_ref_ptr< gv_packet >& pending = get_pending_packet(need_reliable);
	if (pending)
	{
		if (pending->test_network_diagram_size(size, gvc_default_mtu - 2))
			return true;
	}
	return false;
}

void gv_com_net_host::flush_out_going_packets()
{
	flush_out_going_packets(true);
	flush_out_going_packets(false);
	return;
};

void gv_com_net_host::flush_out_going_packets(bool use_reliable)
{
	if (m_pending_reliable_pkt && use_reliable)
	{
		(*m_pending_reliable_pkt) << gv_net_cmd_end();
		m_network->send_packet(this, m_pending_reliable_pkt, true);
		m_pending_reliable_pkt = NULL;
	}
	if (m_pending_unreliable_pkt && !use_reliable)
	{
		(*m_pending_unreliable_pkt) << gv_net_cmd_end();
		m_network->send_packet(this, m_pending_unreliable_pkt, false);
		m_pending_unreliable_pkt = NULL;
	}
	return;
};

void gv_com_net_host::flush_in_coming_packets()
{
	gv_packet* pkt = this->m_incoming_pkt_list;
	while (pkt)
	{
		if (!this->recieve_one_packet(*pkt))
		{
			m_is_hacker = true;
			GVM_WARNING("[hacker]maybe a hack send a pkt with error!!!, ignore the "
						"remaining incoming packet");
			break;
		};
		gvt_ref_ptr< gv_packet > pkt_scope = pkt;
		pkt = pkt->m_next_ref;
	}
	m_incoming_pkt_list = NULL; // WARNING !!if the list is too long(>1000?) ,
								// this may cause stack overflow, need to use
								// better clear.
}

gv_bool gv_com_net_host::recieve_one_packet(gv_packet& pkt)
{
	m_cu_packet_sq = pkt.m_sequence_number;
	gv_net_cmd cmd;
	gv_net_error error;
	pkt >> cmd;
	while (cmd != gv_net_cmd_end() && !pkt.eof())
	{
		if (cmd.is_replicate())
		{
			if (!this->exec_replicate(cmd, pkt, error))
			{
				GVM_DEBUG_LOG(net, this->host_name() << "error in replica " << cmd
													 << gv_endl);
			};
		}
		else if (cmd.is_remote_call())
		{
			if (!this->exec_remote_call(cmd, pkt, error))
			{
				GVM_DEBUG_LOG(net, this->host_name() << "send not handled remote call "
													 << cmd << gv_endl);
			}
		}
		else if (cmd.is_remote_call_ack())
		{
			if (!this->exec_remote_call_ack(cmd, pkt, error))
			{
				GVM_DEBUG_LOG(net, this->host_name()
									   << "send not handled remote call ack " << cmd
									   << gv_endl);
			}
		}
		else if (!this->exec_net_cmd(cmd, pkt, error))
		{
			GVM_DEBUG_LOG(net, this->host_name() << "send not handled cmd " << cmd
												 << gv_endl);
			return false;
		}
		if (error.m_has_error)
		{
			GVM_WARNING(this->host_name()
						<< "[hacker]got an error in the package !!! msg is:"
						<< error.m_error_string);
			return false;
		}
		pkt >> cmd;
	}
	return true;
};

void gv_com_net_host::queue_incoming_packet(gv_packet* packet)
{
	packet->m_next_ref = this->m_incoming_pkt_list;
	this->m_incoming_pkt_list = packet;
};
//============================================================================================
//								:
//============================================================================================
gv_bool gv_com_net_host::exec_net_cmd(const gv_net_cmd& cmd, gv_packet& packet,
									  gv_net_error& error)
{
	gv_bool ret = false;
	switch (cmd.m_cmd_id)
	{
#define GVM_NET_CMD_IMP_RECV
#define GV_NET_CMD_INC_FILE "..\network\gv_net_cmd_basic.h"
#include "gv_net_cmd_maker.h"
	};
	return ret;
};
//==
gv_bool gv_com_net_host::exec_remote_call(const gv_remote_call& call)
{
	GV_ASSERT(0);
	return false;
};

gv_bool gv_com_net_host::exec_remote_call(const gv_net_cmd& cmd,
										  gv_packet& packet,
										  gv_net_error& error)
{
	gv_bool ret = false;
	switch (cmd.m_index)
	{
#define GVM_REMOTE_CALL_IMP_RECV
#define GV_REMOTE_CALL_INC_FILE "..\network\gv_remote_call_basic.h"
#include "gv_remote_call_maker.h"
	};
	return ret;
};

gv_bool gv_com_net_host::exec_remote_call_ack(const gv_net_cmd& cmd,
											  gv_packet& packet,
											  gv_net_error& error)
{
	gv_bool ret = false;
	// error.m_has_error=true;
	// error.m_has_error="error in exec_remote_call_ack!!";
	switch (cmd.m_index)
	{
#define GVM_REMOTE_CALL_IMP_RECV_ACK
#define GV_REMOTE_CALL_INC_FILE "..\network\gv_remote_call_basic.h"
#include "gv_remote_call_maker.h"
	};
	if (ret && this->get_bt_state() == gve_bt_wait_for_signal)
	{
		this->signal();
	}
	return ret;
};
gv_bool gv_com_net_host::exec_replicate(const gv_net_cmd& cmd, gv_packet& pkt,
										gv_net_error& error)
{
	if (!this->m_in_coming_replica)
	{
		error.m_error_string = "no replica component for receive!!";
		error.m_has_error = true;
		return false;
	}
	gv_ushort replica_id = cmd.m_index;
	gv_int array_size;
	gv_net_replica* preplica =this->m_in_coming_replica->get_replica_array(replica_id, array_size);
	if (!preplica)
	{
		error.m_error_string << "no replica with id " << replica_id
							 << "found for receive!! ";
		error.m_has_error = true;
		return false;
	}
	gv_byte array_idx;
	gv_byte var_idx;
	pkt >> var_idx;
	while (var_idx != gvc_invalid_replica_var_idx && !pkt.eof())
	{
		pkt >> array_idx;
		while (array_idx != gvc_invalid_replica_array_idx && !pkt.eof())
		{
			gv_net_replica* cu_replica = preplica->get_replica_in_array(array_idx);
			cu_replica->set_sq_no(get_cu_packet_sequence_number());
			if (array_idx > array_size)
			{
				error.m_error_string << "invalid replica index  " << array_idx << "in "
									 << preplica->get_class()->get_name_id()
									 << " found for recieve!! ";
				error.m_has_error = true;
				return false;
			}
			if (!cu_replica->recv_a_var(pkt, var_idx))
			{
				error.m_error_string << "error in receive a var " << array_idx
									 << "found for receive "
									 << preplica->get_class()->get_name_id();
				error.m_has_error = true;
				return false;
			}
			cu_replica->set_dirty(true);
			pkt >> array_idx;
		}
		pkt >> var_idx;
	}
	if (pkt.eof())
	{
		error.m_error_string = "unexpected end of replicate in packet ";
		error.m_has_error = true;
		return false;
	}
	return true;
};
gv_bool gv_com_net_host::task_connect(gv_float time_out)
{
	GV_ASSERT(this->m_is_remote);
	this->m_network->connect_host(this);
	this->set_time_out(time_out);
	return true;
};
gv_bool gv_com_net_host::task_disconnect(gv_float time_out)
{
	GV_ASSERT(this->m_is_remote);
	this->m_network->disconnect_host(this);
	this->set_time_out(time_out);
	return true;
};
gv_bool gv_com_net_host::task_wait_net_cmd(const gv_net_cmd& cmd,
										   gv_float time_out)
{
	this->set_time_out(time_out);
	this->m_waiting_command = cmd;
	return true;
};

gv_bool gv_com_net_host::task_remote_call(gv_remote_call* call,
										  gv_remote_call* ret,
										  gv_float time_out)
{
	this->set_time_out(time_out);
	this->m_call_return = ret;
	return true;
};

gv_bool gv_com_net_host::send_remote_call(gv_remote_call* call)
{
	GV_ASSERT(m_is_remote);
	GVM_DEBUG_LOG(net, "[send_remote_cmd]" << call->get_name()
										   << "  to:" << host_name() << gv_endl);
	gv_packet_size_counter counter;
	(*call) >> counter;
	gv_int size_to_send = counter.get_size() + 2;
	gv_packet* p = get_packet_with_size(size_to_send, true);
	if (!p)
		return false;
	gv_net_cmd header;
	(*p) << call->get_header();
	(*call) >> (*p);
	return true;
};
//============================================================================================
//								:
//============================================================================================

gv_bool gv_com_net_host::recieve_net_cmd(gv_net_cmd_host_ping& cmd)
{
	return true;
}
gv_bool gv_com_net_host::recieve_net_cmd(gv_net_cmd_host_sync_game_time& cmd)
{
	return true;
}
gv_bool
gv_com_net_host::recieve_net_cmd(gv_net_cmd_host_sync_system_time& cmd)
{
	return true;
}
gv_bool gv_com_net_host::recieve_net_cmd(gv_net_cmd_host_chat& cmd)
{
	GVM_CONSOLE_OUT(host_name() << " says: " << cmd.content << gv_endl);
	return true;
}

gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_0& cm)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_1& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_2& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_3& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_4& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_5& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_6& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_7& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_8& cmd)
{
	return true;
};
gv_bool gv_com_net_host::recieve_net_cmd(class gv_net_cmd_cmd_generic_9& cmd)
{
	return true;
};
//============================================================================================
//								:
//============================================================================================
void gv_com_net_host::update_out_going_replica()
{
	if (!this->m_out_going_replica)
		return;
	gv_replica_update_result result;
	this->m_out_going_replica->update_all_replica(result,  get_max_replica_var_one_time());
	gv_ushort last_replica_id = gvc_max_replica_struct_id;
	gv_byte array_index = gvc_invalid_replica_array_idx;
	gv_byte var_index = gvc_invalid_replica_var_idx;
	result.sort();
	gvt_ptr< gv_packet > last_packet[2];
	bool first_unreliable = false;
	for (int i = 0; i < result.size(); i++)
	{
		bool need_change_packet = false;
		gv_net_replica_update_info& info = result[i];
		bool need_reliable = info.m_priority == 0;
		if (!first_unreliable && need_reliable == false)
		{
			first_unreliable = true;
			last_replica_id = gvc_max_replica_struct_id;
			array_index = gvc_invalid_replica_array_idx;
			var_index = gvc_invalid_replica_var_idx;
		}
		int pkt_idx = need_reliable ? 0 : 1;
		{ //===================test
			//size================================================
			gv_packet_size_counter pkc;
			gv_ushort last_replica_id_c = last_replica_id;
			gv_byte array_index_c = array_index;
			gv_byte var_index_c = var_index;
			fill_one_replica_update(info, pkc, last_replica_id_c, array_index_c,
									var_index_c);
			int update_size = pkc.get_size();
			gv_packet& current_pkt = *get_packet_with_size(0, need_reliable);
			if (&current_pkt &&
				!current_pkt.test_network_diagram_size(update_size + 4))
			{
				if (&current_pkt == last_packet[pkt_idx])
					current_pkt << gvc_invalid_replica_array_idx
								<< gvc_invalid_replica_var_idx;
				last_replica_id = gvc_max_replica_struct_id;
				array_index = gvc_invalid_replica_array_idx;
				var_index = gvc_invalid_replica_var_idx;
				this->flush_out_going_packets(need_reliable);
				this->create_packet(need_reliable);
			}
		};
		gv_packet& current_pkt = *get_pending_packet(need_reliable);
		last_packet[pkt_idx] = &current_pkt;
		fill_one_replica_update(info, current_pkt, last_replica_id, array_index,
								var_index);
		info.m_owner->on_send(info);
		GV_ASSERT(current_pkt.get_network_diagram_size() < gvc_default_mtu);
	}
	if (last_packet[0])
	{
		*last_packet[0] << gvc_invalid_replica_array_idx
						<< gvc_invalid_replica_var_idx;
	}
	if (last_packet[1])
	{
		*last_packet[1] << gvc_invalid_replica_array_idx
						<< gvc_invalid_replica_var_idx;
	}
}

void gv_com_net_host::fill_one_replica_update(gv_net_replica_update_info& info,
											  gv_packet& pkt,
											  gv_ushort& last_replica_id,
											  gv_byte& last_array_index,
											  gv_byte& last_var_idx)
{
	if (last_replica_id != info.m_replica_id)
	{
		if (last_replica_id != gvc_max_replica_struct_id)
		{
			// close last replica
			pkt << gvc_invalid_replica_array_idx << gvc_invalid_replica_var_idx;
		}
		gv_net_cmd cmd;
		cmd.m_is_replicate = 1;
		cmd.m_index = info.m_replica_id;
		pkt << cmd;
		last_replica_id = info.m_replica_id;
		last_var_idx = gvc_invalid_replica_var_idx;
	}
	if (last_var_idx != (gv_byte)info.m_var->get_index_in_cls())
	{
		if (last_var_idx != gvc_invalid_replica_var_idx)
		{
			// close last replica var
			pkt << gvc_invalid_replica_array_idx;
		}
		last_var_idx = (gv_byte)info.m_var->get_index_in_cls();
		pkt << last_var_idx;
	}
	last_array_index = info.m_index_in_array;
	pkt << last_array_index;
	info.m_owner->send_a_var(pkt, info.m_var);
	GV_ASSERT(pkt.get_network_diagram_size() < gvc_default_mtu);
};

gv_int gv_com_net_host::get_max_replica_var_one_time()
{
	return m_max_replica_var_one_time;
}

void gv_com_net_host::update_in_coming_replica()
{
}
gv_com_net_host* gv_com_net_host::create_client()
{
	gv_com_net_host* pclient =
		get_sandbox()->create_nameless_object< gv_com_net_host >();
	pclient->m_is_remote = true;
	return pclient;
};

gv_int gv_com_net_host::get_nb_connected_host()
{
	return this->m_connected_hosts.size();
};

gv_com_net_host* gv_com_net_host::get_connected_host(gv_int idx)
{
	return this->m_connected_hosts[idx];
};
//============================================================================================
//								:
//============================================================================================
void gv_com_net_host::process_pending_call()
{
	gv_remote_call* pcall = gvt_lock_free_list_pop_all(this->m_posted_call_list);
	while (pcall)
	{
		gv_remote_call* tmp = pcall;
		pcall = pcall->next;
		this->send_remote_call(tmp);
		delete tmp;
	}
};

gv_bool gv_com_net_host::post_remote_call(gv_remote_call* call,
										  bool need_return)
{
	gvt_lock_free_list_push(call, m_posted_call_list);
	return true;
};

void gv_com_net_host::process_pending_net_cmd()
{
	gv_net_cmd_list_base* plist =
		gvt_lock_free_list_pop_all(this->m_posted_net_cmd_list);
	while (plist)
	{
		gv_net_cmd_list_base* tmp = plist;
		plist = plist->next;
		gv_packet_size_counter counter;
		(*tmp) >> counter;
		gv_int size_to_send = counter.get_size();
		gv_packet* p = get_packet_with_size(size_to_send, tmp->m_is_reliable);
		(*tmp) >> (*p);
		GVM_DEBUG_LOG(net, "[send_net_cmd]" << tmp->get_name()
											<< " to:" << host_name() << gv_endl);
		delete tmp;
	}
};
//============================================================================================
//								:
//============================================================================================
gv_bool
gv_com_net_host::exec_remote_call(const gv_remote_call_client_login& call)
{
	return true;
};
gv_bool
gv_com_net_host::exec_remote_call(const gv_remote_call_client_login_ack& call)
{
	// call.is_ok=false;
	return true;
};

//============================================================================================
//								:
//============================================================================================
gv_bool gv_com_net_host::post_net_cmd(const gv_net_cmd_generic& cmd)
{
	switch (cmd.m_nb_pay_load)
	{
	case 0:
	{
		gv_net_cmd_cmd_generic_0 c;
		c.code_id = cmd.m_generic_cmd_id;
		post_net_cmd(c);
		break;
	}
	case 1:
	{
		gv_net_cmd_cmd_generic_1 c;
		c.code_id = cmd.m_generic_cmd_id;
		c.arg_1 = cmd.m_payloads[0].data.ui32;
		post_net_cmd(c);
		break;
	}
	case 2:
	{
		gv_net_cmd_cmd_generic_2 c;
		c.code_id = cmd.m_generic_cmd_id;
		c.arg_1 = cmd.m_payloads[0].data.ui32;
		c.arg_2 = cmd.m_payloads[1].data.ui32;
		post_net_cmd(c);
		break;
	}

	case 3:
	{
		gv_net_cmd_cmd_generic_3 c;
		c.code_id = cmd.m_generic_cmd_id;
		c.arg_1 = cmd.m_payloads[0].data.ui32;
		c.arg_2 = cmd.m_payloads[1].data.ui32;
		c.arg_3 = cmd.m_payloads[2].data.ui32;
		post_net_cmd(c);
		break;
	}
	default:
		break;
	}
	return true;
};

gv_net_cmd_generic::gv_net_cmd_generic(const gv_net_cmd_cmd_generic_0& cmd)
{
	m_generic_cmd_id = cmd.code_id;
	m_nb_pay_load = 0;
	// gv_net_cmd_payload	m_payloads[9];
};

gv_net_cmd_generic::gv_net_cmd_generic(const gv_net_cmd_cmd_generic_1& cmd)
{
	m_generic_cmd_id = cmd.code_id;
	m_nb_pay_load = 1;
	m_payloads[0] = cmd.arg_1;
};

gv_net_cmd_generic::gv_net_cmd_generic(const gv_net_cmd_cmd_generic_2& cmd)
{
	m_generic_cmd_id = cmd.code_id;
	m_nb_pay_load = 0;
	m_payloads[0] = cmd.arg_1;
	m_payloads[1] = cmd.arg_2;
};

gv_net_cmd_generic::gv_net_cmd_generic(const gv_net_cmd_cmd_generic_3& cmd)
{
	m_generic_cmd_id = cmd.code_id;
	m_nb_pay_load = 0;
	m_payloads[0] = cmd.arg_1;
	m_payloads[1] = cmd.arg_2;
	m_payloads[2] = cmd.arg_3;
};

gv_net_cmd_generic::gv_net_cmd_generic(const gv_net_cmd_cmd_generic_4& cmd)
{
	m_generic_cmd_id = cmd.code_id;
	m_nb_pay_load = 0;
	m_payloads[0] = cmd.arg_1;
	m_payloads[1] = cmd.arg_2;
	m_payloads[2] = cmd.arg_3;
	m_payloads[3] = cmd.arg_4;
};
//============================================================================================
//								:
//============================================================================================
GVM_IMP_CLASS(gv_com_net_host, gv_com_tasklet)
GVM_VAR(gv_socket_address, m_ip_address)
GVM_VAR(gv_uint, m_unique_user_id)
GVM_VAR(gv_int, m_host_id)
GVM_VAR(gv_int, m_peer_id)
GVM_VAR(gv_bool, m_connected)
GVM_VAR(gv_bool, m_is_remote)
GVM_VAR(gvt_ptr< gv_packet >, m_pending_reliable_pkt)
GVM_VAR(gvt_ptr< gv_packet >, m_pending_unreliable_pkt)
GVM_VAR(gvt_ptr< gv_network_mgr >, m_network)
GVM_END_CLASS
}