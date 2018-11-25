namespace unit_test_rudp_server
{

void test_dump()
{
	/*gv_log * plog =gv_global::log->create_log("packet_test");
		gv_reliable_udp_server server;
		gv_string_tmp s;
		gv_packet * packet;
		{
			packet=server.create_packet(gve_packet_type_connection);
			server.dump_packet(packet,s);
			server.destroy_packet(packet);
		}
		(*plog)<<s;
		gv_global::log->close_log(plog);*/
}

class server_io : public gv_runnable
{
	//simplest 1 to n chat server
public:
	server_io()
	{
		quit = false;
	}
	void run()
	{
		while (1)
		{
			char buffer[1024];
			gets_s(buffer);
			gv_string_tmp s = buffer;
			if (s == "quit")
			{
				quit = true;
				break;
			}
			if (_event.try_wait(1))
				return;
		};
	}
	void notify()
	{
		_event.set();
	}
	gv_event _event;
	bool quit;
};
void server(gvt_array< gv_string >& args)
{
	gv_reliable_udp_server server;
	gv_thread server_thread(gv_id_server);
	server_io server_run;
	server_thread.start(&server_run);

	gv_string_tmp name = "rudp_server_packet";
	gv_ulong uuid = gv_global::time->get_performance_counter_from_start();
	name << uuid;
	gv_log* plog = gv_global::log->create_log(*name);
	gvt_xml_write_element_open(*plog, "root");
	server.enable_packet_dump(true, plog);
	server.create(1234);
	int host_id = 0;
	int peer_id = 0;
	gv_socket_address server_address(gv_ip_address_ipv4::addr_local_host(), 1234);
	if (args.size() > 1)
	{
		gv_string_tmp s = *args[1];
		server_address.from_string(s);
	}
	server.post_host_create(2048, 0, 0);
	bool connected = false;
	bool quit = false;
	while (server.do_service())
	{
		gvt_ref_ptr< gv_session_event > event;
		while (server.peek_event(event))
		{
			if (event->event_type == gve_session_event_host_create)
			{
				host_id = event->host_id;
			}
			if (event->event_type == gve_session_event_connect_succeed)
			{
				//we can send things ;
				gv_string_tmp output;
				output << "connect successed from " << event->address.string() << event->host_id << "@" << event->peer_id << "!!! \r\n";
				test_log() << *output;
				GVM_LOG(net, output);
				connected = true;
			}
			if (event->event_type == gve_session_event_recieve)
			{
				gv_packet* p = event->packet;
				gv_string_tmp s;
				(*p) >> s;
				GVM_LOG(net, "get message from " << event->host_id << "@" << event->peer_id << event->address.string() << s << "\r\n");
				gv_string_tmp s2;
				s2 << " SERVER SEE IT:!! " << s;
				gv_packet* packet = server.create_packet(gve_packet_type_send_reliable);
				(*packet) << s2;
				server.post_send_reliable(event->host_id, event->peer_id, 0, packet);
			}
			if (event->event_type == gve_session_event_disconnected)
			{
				gv_string_tmp output;
				output << "disconnected a client from" << event->host_id << "@" << event->peer_id << event->address.string() << " !!! \r\n";
				test_log() << *output;
				GVM_LOG(net, *output);
				break;
			}
			if (connected)
			{
				gv_string_tmp ss;
				server.dump_peer(event->host_id, event->peer_id, ss);
				(*plog) << ss;
			};
			event = NULL;
		} //next event
		if (server_run.quit)
			break;
		gv_thread::sleep(10);
	}
	server_run.notify();
	server_thread.join();
	gvt_xml_write_element_close(*plog, "root");
	gv_global::log->close_log(plog);
}

void client(gvt_array< gv_string >& args)
{
	gv_reliable_udp_server server;
	gv_string_tmp name = "rudp_client_packet";
	gv_ulong uuid = gv_global::time->get_performance_counter_from_start();
	name << uuid;
	gv_log* plog = gv_global::log->create_log(*name);
	gvt_xml_write_element_open(*plog, "root");
	server.enable_packet_dump(true, plog);
	server.create(0);
	int host_id = 0;
	int peer_id = 0;
	gv_socket_address server_address(gv_ip_address_ipv4::addr_local_host(), 1234);
	if (args.size() > 1)
	{
		gv_string_tmp s = *args[1];
		server_address.from_string(s);
	}
	server.post_host_create(1, 0, 0);
	bool connected = false;
	bool quit = false;
	while (server.do_service())
	{
		gvt_ref_ptr< gv_session_event > event;
		while (server.peek_event(event))
		{
			if (event->event_type == gve_session_event_host_create)
			{
				host_id = event->host_id;
				server.post_connect(server_address, host_id, 0, 4);
			}
			if (event->event_type == gve_session_event_connect)
			{
				peer_id = event->peer_id;
			}
			if (event->event_type == gve_session_event_connect_succeed)
			{
				//we can send things ;
				gv_string_tmp output;
				output << "connect successed to " << event->address.string() << event->host_id << "@" << event->peer_id << "!!! \r\n";
				test_log() << *output;
				GVM_LOG(net, output);
				connected = true;
			}
			if (event->event_type == gve_session_event_recieve)
			{
				gv_packet* p = event->packet;
				gv_string_tmp s;
				(*p) >> s;
				GVM_LOG(net, "get message from " << event->host_id << "@" << event->peer_id << event->address.string() << s << "\r\n");
			}
			if (event->event_type == gve_session_event_disconnected)
			{
				gv_string_tmp output;
				output << "disconnected from server " << event->host_id << "@" << event->peer_id << event->address.string() << " !!! \r\n";
				test_log() << *output;
				GVM_LOG(net, *output);
				quit = true;
				break;
			}
			if (connected)
			{
				gv_string_tmp ss;
				server.dump_peer(event->host_id, event->peer_id, ss);
				(*plog) << ss;
			};
			event = NULL;
		}

		if (connected)
		{
			static int index = 0;
			static gv_ulong last_time_stamp = 0;
			if (gv_global::time->time_stamp_unix() - last_time_stamp > 10000)
			{
				last_time_stamp = gv_global::time->time_stamp_unix();
				gv_string_tmp s;
				s << " hello from "
				  << "client" << uuid << " index " << index++ << " \r\n";
				gv_packet* packet = server.create_packet(gve_packet_type_send_reliable);
				(*packet) << s;
				GVM_LOG(net, "send message to " << host_id << "@" << peer_id << s);
				server.post_send_reliable(host_id, peer_id, 0, packet);
			}
			if (index > 1000)
				server.post_disconnect_now(host_id, peer_id);
		}
		if (quit)
			break;
		gv_thread::sleep(10);
	}
	gvt_xml_write_element_close(*plog, "root");
	gv_global::log->close_log(plog);
}
void main(gvt_array< gv_string >& args)
{
	test_dump();
	if (args.size() >= 1)
	{
		if (args[0] == "server")
		{
			server(args);
		}
		if (args[0] == "client")
		{
			client(args);
		}
	}
}
}
