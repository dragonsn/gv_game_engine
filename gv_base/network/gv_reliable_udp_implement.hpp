//--SOME NOT SO IMPORTANT HELPER
//FUNCTION-----------------------------------------------------------
void dump_header(void*, gv_string_tmp& string)
{
}
void dump_header(gv_rudp_protocol_acknowledge* header, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "acknowledge", false);
	gvt_xml_write_attribute(*ps, "received_reliable_sequence_number",
							(gv_uint)header->received_reliable_sequence_number);
	gvt_xml_write_attribute(*ps, "received_sent_time_stamp",
							header->received_sent_time_stamp);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_connection* header, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "connection", false);
	gvt_xml_write_attribute(*ps, "channel_count", (gv_uint)header->channel_count);
	gvt_xml_write_attribute(*ps, "outgoing_host_id",
							(gv_uint)header->outgoing_host_id);
	gvt_xml_write_attribute(*ps, "outgoing_peer_id", header->outgoing_peer_id);
	gvt_xml_write_attribute(*ps, "session_id", header->session_id);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_attribute(*ps, "mtu", header->mtu);
	gvt_xml_write_attribute(*ps, "window_size", header->window_size);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_verify_connection* header,
				 gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "verify_connection", false);
	gvt_xml_write_attribute(*ps, "channel_count", (gv_uint)header->channel_count);
	gvt_xml_write_attribute(*ps, "outgoing_host_id",
							(gv_uint)header->outgoing_host_id);
	gvt_xml_write_attribute(*ps, "outgoing_peer_id", header->outgoing_peer_id);
	gvt_xml_write_attribute(*ps, "session_id", header->session_id);
	gvt_xml_write_attribute(*ps, "received_reliable_sequence_number",
							header->received_reliable_sequence_number);
	gvt_xml_write_attribute(*ps, "received_sent_time_stamp",
							header->received_sent_time_stamp);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_attribute(*ps, "window_size", header->window_size);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_disconnection* header,
				 gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "disconnection", false);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_send_reliable* header,
				 gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "send_reliable", false);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_attribute(*ps, "fragment_id", (gv_uint)header->fragment_id);
	gvt_xml_write_attribute(*ps, "total_fragment", header->total_fragment);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_send_unreliable* header,
				 gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "send_unreliable", false);
	gvt_xml_write_attribute(*ps, "send_time_stamp", header->send_time_stamp);
	gvt_xml_write_element_close(*ps);
}
//--
void dump_header(gv_rudp_protocol_command_header* header,
				 gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "command_header", false);
	gvt_xml_write_attribute(*ps, "channel_id", (gv_uint)header->channel_id);
	gvt_xml_write_attribute(*ps, "command", (gv_uint)header->command);
	gvt_xml_write_attribute(*ps, "packet_size", header->packet_size);
	gvt_xml_write_attribute(*ps, "sequence_number", header->sequence_number);
	gvt_xml_write_element_close(*ps);
}
void dump_header(gv_rudp_protocol_header* header, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "protocol_header", false);
	gvt_xml_write_attribute(*ps, "flags", (gv_uint)header->flags);
	gvt_xml_write_attribute(*ps, "host_id", (gv_uint)header->host_id);
	gvt_xml_write_attribute(*ps, "peer_id", header->peer_id);
	gvt_xml_write_attribute(*ps, "session_id", header->session_id);
	gvt_xml_write_element_close(*ps);
}
void dump_packet(gv_packet* packet, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "gv_packet", false);
	gvt_xml_write_attribute(*ps, "time", gv_global::time->get_sec_from_start());
	gvt_xml_write_attribute(*ps, "nb_headers",
							packet->get_number_of_protocol_header());
	gvt_xml_write_attribute(*ps, "target_address", packet->m_address.string());
	gvt_xml_write_attribute(*ps, "sequence", packet->m_sequence_number);
	gvt_xml_write_attribute(*ps, "timestamp", packet->m_time_stamp);
	gvt_xml_write_attribute(*ps, "diagram_size",
							packet->get_network_diagram_size());
	gvt_xml_write_attribute(*ps, "content_size",
							packet->get_packet_content_size());
	gvt_xml_write_attribute(*ps, "resend_times", packet->m_resend_times);
	gvt_xml_write_element_open(*ps);
	int nb_head = packet->get_number_of_protocol_header();
	if (nb_head >= 1)
	{
		gv_rudp_protocol_header* phead =
			packet->get_protocol_header< gv_rudp_protocol_header >(0);
		dump_header(phead, string);
	}
	gv_rudp_protocol_command_header* command = NULL;
	if (nb_head >= 2)
	{
		command = packet->get_protocol_header< gv_rudp_protocol_command_header >(1);
		dump_header(command, string);
	}
	if (nb_head >= 3 && command)
	{
		switch (command->command)
		{
		case gve_packet_type_acknowledge:
			dump_header(packet->get_protocol_header< gv_rudp_protocol_acknowledge >(2),
						string);
			break;

		case gve_packet_type_connection:
			dump_header(packet->get_protocol_header< gv_rudp_protocol_connection >(2),
						string);
			break;

		case gve_packet_type_verify_connection:
			dump_header(
				packet->get_protocol_header< gv_rudp_protocol_verify_connection >(2),
				string);
			break;

		case gve_packet_type_disconnection:
			dump_header(
				packet->get_protocol_header< gv_rudp_protocol_disconnection >(2),
				string);
			break;

		case gve_packet_type_ping:
			dump_header(
				packet->get_protocol_header< gv_rudp_protocol_send_reliable >(2),
				string);
			break;

		case gve_packet_type_send_reliable:
			dump_header(
				packet->get_protocol_header< gv_rudp_protocol_send_reliable >(2),
				string);
			break;

		case gve_packet_type_send_unreliable:
			dump_header(
				packet->get_protocol_header< gv_rudp_protocol_send_unreliable >(2),
				string);
			break;
		} // handle
	}
	gv_byte* pb = (gv_byte*)packet->get_packet_content();
	for (int i = 0; i < packet->get_packet_content_size(); i++)
	{
		if (*pb < 16)
			string << "0";
		(string) << (void*)(*pb++) << "\t";
	}
	gvt_xml_write_element_close(*ps, "gv_packet");
}
bool dump_channel(gv_rudp_channel* channel, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "gv_rudp_channel");
	gvt_xml_write_attribute(*ps, "m_incoming_reliable_sequence",
							channel->m_incoming_reliable_sequence);
	gvt_xml_write_attribute(*ps, "m_incoming_unreliable_sequence",
							channel->m_incoming_unreliable_sequence);
	gvt_xml_write_attribute(*ps, "m_outgoing_reliable_sequence",
							channel->m_outgoing_reliable_sequence);
	gvt_xml_write_attribute(*ps, "m_outgoing_unreliable_sequence",
							channel->m_outgoing_unreliable_sequence);
	gvt_xml_write_attribute(*ps, "m_start_server_time",
							channel->m_start_server_time);
	gvt_xml_write_attribute(*ps, "m_last_recv_time", channel->m_last_recv_time);
	gvt_xml_write_attribute(*ps, "m_last_send_time", channel->m_last_send_time);
	gvt_xml_write_attribute(*ps, "m_send_bytes", channel->m_send_bytes);
	gvt_xml_write_attribute(*ps, "m_recv_bytes", channel->m_recv_bytes);
	gvt_xml_write_attribute(*ps, "m_max_send_window_size",
							channel->m_max_send_window_size);
	gvt_xml_write_attribute(*ps, "m_cu_send_window_size",
							channel->m_cu_send_window_size);
	{
		gvt_xml_write_element_open(*ps, "m_outgoing_reliable_list", false);
		gv_rudp_channel::packets_list::iterator it =
			channel->m_outgoing_reliable_list.begin();
		gvt_xml_write_attribute(*ps, "size",
								channel->m_outgoing_reliable_list.size());
		gvt_xml_write_element_open(*ps);
		while (it != channel->m_outgoing_reliable_list.end())
		{
			dump_packet(*it, *ps);
			++it;
		}
		gvt_xml_write_element_close(*ps, "m_outgoing_reliable_list");
	}
	{
		gvt_xml_write_element_open(*ps, "m_outgoing_reliable_wait_for_ack_list",
								   false);
		gv_rudp_channel::packets_list::iterator it =
			channel->m_outgoing_reliable_wait_for_ack_list.begin();
		gvt_xml_write_attribute(
			*ps, "size", channel->m_outgoing_reliable_wait_for_ack_list.size());
		gvt_xml_write_element_open(*ps);
		while (it != channel->m_outgoing_reliable_wait_for_ack_list.end())
		{
			dump_packet(*it, *ps);
			++it;
		}
		gvt_xml_write_element_close(*ps, "m_outgoing_reliable_wait_for_ack_list");
	}
	{
		gvt_xml_write_element_open(*ps, "m_incoming_reliable_list", false);
		gv_rudp_channel::packets_list::iterator it =
			channel->m_incoming_reliable_list.begin();
		gvt_xml_write_attribute(*ps, "size",
								channel->m_incoming_reliable_list.size());
		gvt_xml_write_element_open(*ps);
		while (it != channel->m_incoming_reliable_list.end())
		{
			dump_packet(*it, *ps);
			++it;
		}
		gvt_xml_write_element_close(*ps, "m_incoming_reliable_list");
	}
	{
		gvt_xml_write_element_open(*ps, "m_incoming_reliable_fragment_list", false);
		gv_rudp_channel::packets_list::iterator it =
			channel->m_incoming_reliable_fragment_list.begin();
		gvt_xml_write_attribute(*ps, "size",
								channel->m_incoming_reliable_fragment_list.size());
		gvt_xml_write_element_open(*ps);
		while (it != channel->m_incoming_reliable_fragment_list.end())
		{
			dump_packet(*it, *ps);
			++it;
		}
		gvt_xml_write_element_close(*ps, "m_incoming_reliable_fragment_list");
	}
	gvt_xml_write_element_close(*ps, "gv_rudp_channel");
	return true;
}
bool dump_peer(gv_rudp_peer* peer, gv_string_tmp& string)
{
	gv_string_tmp* ps = &string;
	gvt_xml_write_element_open(*ps, "gv_rudp_peer");
	gvt_xml_write_attribute(*ps, "host_id", peer->m_host_id);
	gvt_xml_write_attribute(*ps, "m_peer_id", peer->m_peer_id);
	gvt_xml_write_attribute(*ps, "m_state", peer->m_state);
	gvt_xml_write_attribute(*ps, "m_address", peer->m_address.string());
	gvt_xml_write_attribute(*ps, "m_session_id", peer->m_session_id);
	gvt_xml_write_attribute(*ps, "m_outgoing_host_id", peer->m_outgoing_host_id);
	gvt_xml_write_attribute(*ps, "m_outgoing_peer_id", peer->m_outgoing_peer_id);
	gvt_xml_write_attribute(*ps, "m_nb_acks", peer->m_nb_acks);
	gvt_xml_write_attribute(*ps, "m_utc_time_difference",
							peer->m_utc_time_difference);
	gvt_xml_write_attribute(*ps, "m_min_round_trip_time",
							peer->m_min_round_trip_time);
	gvt_xml_write_attribute(*ps, "m_average_round_trip_time",
							peer->m_average_round_trip_time);
	gvt_xml_write_attribute(*ps, "m_max_round_trip_time",
							peer->m_max_round_trip_time);
	gvt_xml_write_attribute(*ps, "m_last_round_trip_time",
							peer->m_last_round_trip_time);
	gvt_xml_write_attribute(*ps, "m_last_update_time", peer->m_last_update_time);
	gvt_xml_write_attribute(*ps, "m_last_recv_time", peer->m_last_recv_time);
	gvt_xml_write_attribute(*ps, "m_last_send_time", peer->m_last_send_time);
	gvt_xml_write_attribute(*ps, "m_create_time", peer->m_create_time);
	gvt_xml_write_attribute(*ps, "m_incoming_limit", peer->m_incoming_limit);
	gvt_xml_write_attribute(*ps, "m_outgoing_limit", peer->m_outgoing_limit);
	gvt_xml_write_attribute(*ps, "m_connection_sequence",
							peer->m_connection_sequence);
	gvt_xml_write_element_open(*ps, "channels", false);
	gvt_xml_write_attribute(*ps, "size", peer->m_channels.size());
	gvt_xml_write_element_open(*ps);
	for (int i = 0; i < peer->m_channels.size(); i++)
	{
		dump_channel(&peer->m_channels[i], *ps);
	}
	gvt_xml_write_element_close(*ps, "channels");
	gvt_xml_write_element_close(*ps, "gv_rudp_peer");
	return true;
}