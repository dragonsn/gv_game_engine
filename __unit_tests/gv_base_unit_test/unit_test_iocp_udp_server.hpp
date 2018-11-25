#include "../../gv_base/win32/gv_iocp_udp_server.h"

namespace unit_test_iocp_udp_server
{

static const char* client_greeting = "hello, I am client";
static const char* server_answer = "hello ,I am server";
static gv_string huge_data;

void build_huge_data()
{
	gv_load_file_to_string("../profiler.xml", huge_data);
}

// Our packet processing function
void GV_STDCALL OnClientToServer(gvi_server_transport_layer_udp* server,
								 gv_socket_address& address,
								 gv_packet* p)
{
	if (p->get_network_diagram_size() == 0)
		return;
	static int i = 0;
	gv_mutex m;
	gv_thread_lock lock(m);

	// Show who this came from

	huge_data << i++ << ":get_message " << *address.string() << p->get_network_diagram_start() << "\r\n";
	// Echo it back

	if (server)
		server->send_to(address, p->get_network_diagram_size(), (gv_byte*)p->get_network_diagram_start());
}

class server_runnable : public gv_runnable
{
	//simplest 1 to n chat server
public:
	server_runnable(const gv_socket_address& address)
		: m_socket_address(address)
	{
	}

	void run()
	{
		GV_PROFILE_EVENT(server_run, 0);
		gv_iocp_udp_server server;

		server.create(m_socket_address.get_port(), 2, 4, 0, 0);

		server.set_callback_on_recv(OnClientToServer);

		_event.wait();

		server.destroy();
	}

	void notify()
	{
		_event.set();
	}

	bool _ran;
	gv_id _threadName;
	gv_event _event;
	gv_socket_address m_socket_address;
	gv_socket m_listen_socket;
};

class client_runnable : public gv_runnable
{
public:
	client_runnable()
	{
	}

	void run()
	{
		GV_PROFILE_EVENT(client_run, 0);
		gvt_array< gv_char > buffer;
		{
			GV_PROFILE_EVENT(client_connect, 0)
			m_socket.init_socket(e_socket_type_datagram, e_socket_proto_udp);
		}
		m_socket.opt_set_receive_timeout(100);
		gv_thread* pt = gv_thread::current();
		for (int i = 0; i < 1000; i++)
		{
			GV_PROFILE_EVENT(client_send, 0)
			gv_string_tmp s;
			s << "index:" << i << " " << pt->get_name().string() << client_greeting;
			int size = m_socket.send_to(*s, s.size(), m_socket_address);
			gv_thread::sleep(1);
			gv_socket_address ad;
			char buffer[2000];
			memset(buffer, 0, 2000);
			if (m_socket.readable_time_out(100) == 0)
			{
				s << pt->get_name().string() << " get_message "
				  << "time out!!!";
				GVM_LOG(net, *s);
			}
			else
			{
				size = m_socket.receive_from(buffer, gvt_array_length(buffer), ad);
				if (size < 0)
					continue;
				s << pt->get_name().string() << " get_message " << *ad.string() << buffer;
				if (size > 0)
				{
					//OnClientToServer(NULL,ad,s.size(),(gv_byte*)*s);
					GVM_LOG(net, s);
				}
			};
		};

		gv_thread::sleep(200);
		m_socket.close();
	}

	gv_id _threadName;
	gv_event _event;
	gv_socket m_socket;
	gv_socket_address m_socket_address;
};

void test4_threads(
	int nb_clients,
	int nb_server_cocurrents,
	int nb_server_workers)
{
	//build_huge_data();

	//gv_socket_address a(gv_ip_address_ipv4 ::addr_local_host(),1234);
	gv_socket_address a(gv_ip_address_ipv4::addr_local_host(), 1234);

	server_runnable server(a);
	gv_thread server_thread(gv_id_server_udp_iocp);

	gvt_array< gv_thread > client_threads;
	gvt_array< client_runnable > clients;

	client_threads.resize(nb_clients);
	clients.resize(nb_clients);
	{
		GV_PROFILE_EVENT(start_server, 0)
		server_thread.start(&server);
		gv_thread::sleep(100);
	}
	{
		GV_PROFILE_EVENT(start_client, 0)
		for (int i = 0; i < nb_clients; i++)
		{
			gv_string_tmp s;
			s << "client_" << i;
			client_threads[i].set_name(gv_id(*s));
			clients[i].m_socket_address = a;
			client_threads[i].start(&clients[i]);
		}
		gv_thread::sleep(4000 * nb_clients);
	}
	{
		GVM_DEBUG_LOG(net, "server shut down")
		GV_PROFILE_EVENT(singal_stop_server, 0)
		server.notify();
		gv_thread::sleep(100);
		;
	}
	test_log() << *huge_data;
	server_thread.join();
	for (int i = 0; i < nb_clients; i++)
	{
		client_threads[i].join();
	}
	huge_data.clear();
}

void main(gvt_array< gv_string >& args)
{
	int nb_clients = 1;
	int nb_server_cocurrents = 2;
	int nb_server_workers = 2;

	if (args.size() == 3)
	{
		args[0] >> nb_clients;
		args[1] >> nb_server_cocurrents;
		args[2] >> nb_server_workers;
	}
	if (args.size() == 1)
	{
		args[0] >> nb_clients;
	}
	test4_threads(nb_clients, nb_server_cocurrents, nb_server_workers);
}
}
