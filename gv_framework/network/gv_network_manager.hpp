#define DEBUG_PACKAGE (0 && GV_DEBUG_VERSION)

namespace gv
{
static gv_atomic_count s_number_connection;
class gv_network_mgr_imp : public gv_refable
{
public:
	gv_network_mgr_imp()
	{
		GV_ASSERT(0);
	}
	gv_network_mgr_imp(gv_com_net_host* local_host)
	{
		if (local_host->is_server())
		{
			m_rudp_server = new gv_reliable_udp_server;
#if defined(WIN32)
			m_rudp_server->create(local_host->get_host_address().get_port(), 0,
								  new gv_iocp_udp_server);
#else
			m_rudp_server->create(local_host->get_host_address().get_port());
#endif

#if DEBUG_PACKAGE
			m_rudp_server->enable_packet_dump(true, gv_global::log->get_log());
#endif
		}
		else
		{
			m_rudp_server = new gv_reliable_udp_server;
			m_rudp_server->create(0, 1);
#if DEBUG_PACKAGE
			m_rudp_server->enable_packet_dump(true, &gv_global::log->get_log());
#endif
		}
	};
	~gv_network_mgr_imp()
	{
		gvt_safe_delete(m_rudp_server);
	};
	gvi_server_session_layer* get_rudp_server()
	{
		return m_rudp_server;
	}
	gvt_ptr< gvi_server_session_layer > m_rudp_server;
};

gv_network_mgr::gv_network_mgr()
{
	GVM_SET_CLASS(gv_network_mgr);
	set_thread_priority(gv_thread::E_PRIO_HIGHEST);
};

gv_network_mgr::~gv_network_mgr(){};

void gv_network_mgr::register_events()
{
	GVM_HOOK_EVENT(net_add_host, on_event_add_host, net);
	GVM_HOOK_EVENT(net_remove_host, on_event_remove_host, net);
	GVM_HOOK_EVENT(net_add_net_replica, on_event_add_net_replica, net);
	GVM_HOOK_EVENT(net_remove_net_replica, on_event_remove_net_replica, net);
};
void gv_network_mgr::unregister_events()
{
	GVM_UNHOOK_EVENT(net_add_host, net);
	GVM_UNHOOK_EVENT(net_remove_host, net);
	GVM_UNHOOK_EVENT(net_add_net_replica, net);
	GVM_UNHOOK_EVENT(net_remove_net_replica, net);
};

bool gv_network_mgr::tick(gv_float dt)
{
	GV_PROFILE_EVENT(gv_network_mgr_tick, (gv_int)(dt * 1000))
	super::tick(dt);
	if (!m_local_host)
	{
		return true;
	}
	if (!get_impl()->get_rudp_server())
		return true;
	//=======================================================================
	//	tick udp server
	//=======================================================================
	gvi_server_session_layer* pserver = get_impl()->get_rudp_server();
	pserver->do_service();
	gvt_ref_ptr< gv_session_event > event;
	{
		GV_PROFILE_EVENT(gv_network_mgr_process_event, 0)
		while (pserver->peek_event(event))
		{
			switch (event->event_type)
			{
			case (gve_session_event_host_create):
			{
				if (m_local_host)
					m_local_host->m_host_id = event->host_id;
			}
			break;

			case (gve_session_event_connect_succeed):
			{
				++s_number_connection;
				GVM_DEBUG_LOG(net, "total_connection:" << s_number_connection.get()
													   << "connect successes from "
													   << event->address.string()
													   << event->host_id << "@"
													   << event->peer_id << "!!! \r\n");
				GVM_CONSOLE_OUT("total_connection:" << s_number_connection.get()
													<< "connect successes from "
													<< event->address.string()
													<< event->host_id << "@"
													<< event->peer_id << "!!! \r\n");
				create_connection(event);
			}
			break;

			case (gve_session_event_recieve):
			{
				GVM_DEBUG_LOG(net, "recieve data  successes from "
									   << event->address.string() << event->host_id
									   << "@" << event->peer_id << "!!! \r\n");
				gv_packet* p = event->packet;
				gvt_ptr< gv_com_net_host > phost;
				if (this->m_address_map.find(event->address, phost))
				{
					phost->queue_incoming_packet(p);
				}
				else
				{
					GVM_WARNING("recieve data   from UNKOWN HOST!!! "
								<< event->address.string() << event->host_id << "@"
								<< event->peer_id << "!!! \r\n");
					GV_ASSERT(0);
				}
			}
			break;

			case (gve_session_event_disconnected):
			{
				--s_number_connection;
				GVM_DEBUG_LOG(net, "total_connection:" << s_number_connection.get()
													   << "disconnected successes from "
													   << event->address.string()
													   << event->host_id << "@"
													   << event->peer_id << "!!! \r\n");
				GVM_CONSOLE_OUT("total_connection:" << s_number_connection.get()
													<< "disconnected successes from "
													<< event->address.string()
													<< event->host_id << "@"
													<< event->peer_id << "!!! \r\n");
				remove_connection(event);
			}
			break;

			case gve_session_event_connect_fail:
			{
				GVM_DEBUG_LOG(net, "total_connection:" << s_number_connection.get()
													   << "!connect failed to "
													   << event->address.string()
													   << event->host_id << "@"
													   << event->peer_id << "!!! \r\n");
				GVM_CONSOLE_OUT("total_connection:"
								<< s_number_connection.get() << "!connect failed to "
								<< event->address.string() << event->host_id << "@"
								<< event->peer_id << "!!! \r\n");
				cancel_connection(event);
			}
			break;

			case gve_session_event_none:
			case gve_session_event_connect:
			case gve_session_event_send_unreliable:
			case gve_session_event_send_reliable:
			case gve_session_event_ping:
			case gve_session_event_disconnect:
			case gve_session_event_limit_bandwidth:
			case gve_session_event_host_broadcast:
			case gve_session_event_host_destroy:
				break;

			default:
			{
				GV_ASSERT(0 && "unhandled net session event!! ");
			}
			break;
			}

			event = NULL;
		} // next event
	}
	//=======================================================================
	//	tick hosts
	//=======================================================================
	{
		GV_PROFILE_EVENT(gv_network_mgr_tick_hosts, 0)
		if (this->m_local_host)
			this->m_local_host->tick(dt);
		for (int i = 0; i < this->m_remote_hosts.size(); i++)
		{
			this->m_remote_hosts[i]->tick(dt);
		}
	}
	return true;
};

gv_int gv_network_mgr::on_event_add_host(gv_object_event_net_add_host* pevent)
{
	GV_ASSERT(pevent->do_check());
	gv_com_net_host* phost = gvt_cast< gv_com_net_host >(pevent->host);
	if (phost)
	{
		if (phost->m_is_remote)
		{
			this->m_remote_hosts.add_unique(phost);
		}
		else
		{
			GV_ASSERT(!this->m_local_host);
			this->m_local_host = phost;
			if (!m_pimpl)
				m_pimpl = new gv_network_mgr_imp(m_local_host);
			get_impl()->get_rudp_server()->post_host_create(phost->m_max_connection,
															0, 0);
		}
		phost->m_network = this;
		m_address_map.add(phost->get_host_address(), phost);
	}
	return 1;
};

gv_int
gv_network_mgr::on_event_remove_host(gv_object_event_net_remove_host* pevent)
{
	GV_ASSERT(pevent->do_check());
	gv_com_net_host* phost = gvt_cast< gv_com_net_host >(pevent->host);
	phost->manual_disconnect();
	return 1;
};

gv_int gv_network_mgr::on_event_add_net_replica(
	class gv_object_event_net_add_net_replica* pevent)
{
	GV_ASSERT(pevent->do_check());
	gv_com_net_replica* prep = gvt_cast< gv_com_net_replica >(pevent->replica);
	GV_ASSERT(prep);
	GV_ASSERT(prep->m_bound_host);
	if (prep->m_is_outgoing)
	{
		prep->m_bound_host->m_out_going_replica = prep;
	}
	else
	{
		prep->m_bound_host->m_in_coming_replica = prep;
	}
	return 1;
};

gv_int gv_network_mgr::on_event_remove_net_replica(
	class gv_object_event_net_remove_net_replica* pevent)
{
	GV_ASSERT(pevent->do_check());
	gv_com_net_replica* prep = gvt_cast< gv_com_net_replica >(pevent->replica);
	GV_ASSERT(prep);
	GV_ASSERT(prep->m_bound_host);
	if (prep->m_is_outgoing)
	{
		prep->m_bound_host->m_out_going_replica = NULL;
	}
	else
	{
		prep->m_bound_host->m_in_coming_replica = NULL;
	}
	return 1;
};

bool gv_network_mgr::do_synchronization()
{
	return super::do_synchronization();
};

gv_network_mgr_imp* gv_network_mgr::get_impl()
{
	return m_pimpl;
};

gv_int gv_network_mgr::connect_host(gv_com_net_host* remote)
{
	if (!m_local_host)
		return 0;
	get_impl()->get_rudp_server()->post_connect(
		remote->get_host_address(), m_local_host->m_host_id,
		remote->m_remote_host_id, gvc_net_default_channel_nb);
	return 1;
};

gv_int gv_network_mgr::disconnect_host(gv_com_net_host* remote)
{
	if (!m_local_host)
		return 0;
	get_impl()->get_rudp_server()->post_disconnect(remote->m_host_id,
												   remote->m_peer_id);
	return 1;
};

void gv_network_mgr::send_packet(gv_com_net_host* remote, gv_packet* pkt,
								 bool need_reliable)
{
	if (!get_impl())
		return;
	if (remote->get_host_state() != gve_host_connected)
		return;
	if (need_reliable)
		get_impl()->get_rudp_server()->post_send_reliable(
			remote->m_host_id, remote->m_peer_id, gvc_net_cmd_channel, pkt);
	else
		get_impl()->get_rudp_server()->post_send_unreliable(
			remote->m_host_id, remote->m_peer_id, gvc_net_cmd_channel, pkt);
};

gv_packet* gv_network_mgr::create_packet(bool need_reliable)
{
	if (!get_impl())
		return NULL;
	if (need_reliable)
		return get_impl()->get_rudp_server()->create_packet(
			gve_packet_type_send_reliable);
	else
		return get_impl()->get_rudp_server()->create_packet(
			gve_packet_type_send_unreliable);
};

gv_com_net_host* gv_network_mgr::create_remote_host()
{
	GV_ASSERT(this->m_local_host);
	return this->m_local_host->create_client();
};

void gv_network_mgr::create_connection(gvt_ref_ptr< gv_session_event >& event)
{
	GV_PROFILE_EVENT(create_connection, 0)
	gvt_ptr< gv_com_net_host > phost;
	if (this->m_address_map.find(event->address, phost))
	{
		if (phost->get_host_state() == gve_host_connecting)
		{
			phost->m_connected = true;
			phost->signal();
		}
		phost->m_host_id = event->host_id;
		phost->m_peer_id = event->peer_id;
	}
	else
	{
		phost = create_remote_host();
		phost->set_owner(this);
		this->m_remote_hosts.add_unique(phost.ptr());
		phost->m_is_remote = true;
		phost->m_ip_address = event->address;
		phost->m_peer_id = event->peer_id;
		phost->m_host_id = event->host_id;
		phost->set_host_state(gve_host_connected);
		phost->m_network = this;
		m_address_map.add(event->address, phost);
	}
	m_local_host->m_connected_hosts.add_unique(phost.ptr());
	m_local_host->on_connection(phost);
};

void gv_network_mgr::cancel_connection(gvt_ref_ptr< gv_session_event >& event)
{
	GV_PROFILE_EVENT(cancel_connection, 0)
	gvt_ptr< gv_com_net_host > phost;
	if (this->m_address_map.find(event->address, phost))
	{
		if (phost->get_host_state() == gve_host_connecting)
		{
			phost->m_connected = false;
			phost->signal();
		}
	}
};

void gv_network_mgr::remove_connection(gvt_ref_ptr< gv_session_event >& event)
{
	GV_PROFILE_EVENT(remove_connection, 0)
	gvt_ptr< gv_com_net_host > phost;
	if (this->m_address_map.find(event->address, phost))
	{
		phost->m_connected = false;
		if (phost->get_host_state() == gve_host_disconnecting)
		{
			phost->signal();
		}
		else
		{
			phost->set_host_state(gve_host_disconnected);
		}
		if (!phost->is_persistent())
		{
			this->m_remote_hosts.erase_item(phost.ptr());
			phost->on_disconnection(m_local_host);
			m_local_host->on_disconnection(phost);
			m_local_host->m_connected_hosts.erase_item(phost.ptr());
			m_address_map.erase(event->address);
		}
	}
	else
	{
		GVM_WARNING("remove connection   from UNKOWN HOST!!! "
					<< event->address.string() << event->host_id << "@"
					<< event->peer_id << "!!! \r\n");
	}
};

GVM_IMP_CLASS(gv_network_mgr, gv_event_processor)
GVM_VAR(gvt_ref_ptr< gv_com_net_host >, m_local_host)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_com_net_host > >, m_remote_hosts)
GVM_END_CLASS
}