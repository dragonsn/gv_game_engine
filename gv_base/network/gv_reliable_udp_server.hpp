
gv_reliable_udp_server::gv_reliable_udp_server()
{
	m_pimpl = new gv_reliable_udp_server_impl;
	m_pimpl->m_owner = this;
};
gv_reliable_udp_server::~gv_reliable_udp_server()
{
	if (m_pimpl)
		delete m_pimpl;
};
bool gv_reliable_udp_server::create(
	gv_ushort port, int work_thread_nb,
	class gvi_server_transport_layer_udp* transport)
{
	return m_pimpl->create(port, work_thread_nb, transport);
};

gv_packet* gv_reliable_udp_server::create_packet(gve_packet_type type)
{
	return m_pimpl->create_packet(type);
};
void gv_reliable_udp_server::destroy_packet(gv_packet* p)
{
	return m_pimpl->destroy_packet(p);
};

bool gv_reliable_udp_server::do_service()
{
	return this->m_pimpl->do_service();
};

bool gv_reliable_udp_server::peek_event(gvt_ref_ptr< gv_session_event >& event)
{
	return this->m_pimpl->peek_event(event);
};

gv_session_event* gv_reliable_udp_server::create_event()
{
	return this->m_pimpl->create_event();
};
gv_uint gv_reliable_udp_server::post_request(
	const gvt_ref_ptr< gv_session_event >& event)
{
	return this->m_pimpl->post_request(event);
};
gv_uint
gv_reliable_udp_server::post_connect(const gv_socket_address& peer_address,
									 gv_uint host_id, gv_uint remote_host,
									 gv_ushort channel_nb)
{
	return this->m_pimpl->post_connect(peer_address, host_id, remote_host,
									   channel_nb);
};
gv_uint gv_reliable_udp_server::post_broadcast(gv_uint host_id,
											   gv_ushort channel,
											   gv_packet* packet)
{
	return this->m_pimpl->post_broadcast(host_id, channel, packet);
};
gv_uint gv_reliable_udp_server::post_send_unreliable(gv_uint host_id,
													 gv_uint peer_id,
													 gv_ushort channel,
													 gv_packet* packet)
{
	return this->m_pimpl->post_send_unreliable(host_id, peer_id, channel, packet);
}
gv_uint gv_reliable_udp_server::post_send_reliable(gv_uint host_id,
												   gv_uint peer_id,
												   gv_ushort channel,
												   gv_packet* packet)
{
	return this->m_pimpl->post_send_reliable(host_id, peer_id, channel, packet);
}
gv_uint gv_reliable_udp_server::post_ping(gv_uint host_id, gv_uint peer_id)
{
	return this->m_pimpl->post_ping(host_id, peer_id);
}
gv_uint gv_reliable_udp_server::post_disconnect(gv_uint host_id,
												gv_uint peer_id)
{
	return this->m_pimpl->post_disconnect(host_id, peer_id);
}
gv_uint gv_reliable_udp_server::post_disconnect_now(gv_uint host_id,
													gv_uint peer_id)
{
	return this->m_pimpl->post_disconnect_now(host_id, peer_id);
}
gv_uint gv_reliable_udp_server::post_throttle_configure(gv_uint host_id,
														gv_uint peer_id)
{
	return this->m_pimpl->post_throttle_configure(host_id, peer_id);
}
gv_uint gv_reliable_udp_server::post_host_create(gv_uint max_peer_count,
												 gv_uint incoming_limit,
												 gv_uint outgoing_limit)
{
	return this->m_pimpl->post_host_create(max_peer_count, incoming_limit,
										   outgoing_limit);
};
gv_uint gv_reliable_udp_server::post_host_destroy(gv_uint host_id)
{
	return this->m_pimpl->post_host_destroy(host_id);
};
gv_uint gv_reliable_udp_server::post_result(
	const gvt_ref_ptr< gv_session_event >& event)
{
	return this->m_pimpl->post_result(event);
};
gv_uint gv_reliable_udp_server::post_result(gv_uint host_id, gv_uint peer_id,
											gve_session_event type,
											gv_ushort channel_id,
											gv_packet* packet,
											gv_uint user_data)
{
	return this->m_pimpl->post_result(host_id, peer_id, type, channel_id, packet,
									  user_data);
};

bool gv_reliable_udp_server::send_to(const gv_socket_address& address,
									 gv_packet* packet)
{
	return this->m_pimpl->send_to(address, packet);
};

void gv_reliable_udp_server::dump_packet(gv_packet* packet,
										 gv_string_tmp& string)
{
	this->m_pimpl->dump_packet(packet, string);
}
void gv_reliable_udp_server::enable_packet_dump(bool enable, gv_log* log)
{
	this->m_pimpl->enable_packet_dump(enable, log);
};

bool gv_reliable_udp_server::dump_peer(gv_uint host_id, gv_uint peer_id,
									   gv_string_tmp& string)
{
	return this->m_pimpl->dump_peer(host_id, peer_id, string);
}