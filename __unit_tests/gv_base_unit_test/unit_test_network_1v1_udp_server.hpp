//==========================================================================================

namespace unit_test_network_1v1_udp_server
{

static const char* client_greeting = "hello, I am client";
static const char* server_answer = "hello ,I am server";
static gv_string huge_data;

void GV_STDCALL client_recv(class gvi_server_transport_layer_udp* server, gv_socket_address& address, gv_packet* packet)
{
	static int package_recved = 0;
	gv_string_tmp s;
	(*packet) >> s;
	if (!s.size())
		return;
	test_log() << "client_recive::" << s << gv_endl;
	server->destroy_packet(packet);
	gv_packet p;
	gv_string_tmp reply;
	reply << "hello! server , your message " << s << " recved!!" << package_recved++;
	p << reply;
	server->send_to(address, &p);
	gv_thread::sleep(1);
};

void GV_STDCALL server_recv(class gvi_server_transport_layer_udp* server, gv_socket_address& address, gv_packet* packet)
{
	gv_string_tmp s;
	static int package_recved = 0;
	(*packet) >> s;
	test_log() << "server_recive::" << s << gv_endl;

	server->destroy_packet(packet);
	gv_packet p;
	gv_string_tmp reply;
	reply << "good boy! " << package_recved++;
	p << reply;
	server->send_to(address, &p);
	gv_thread::sleep(1);
};
void GV_STDCALL server_recv2(class gvi_server_transport_layer_udp* server, gv_socket_address& address, gv_packet* packet)
{
	gv_vector3 Rot;
	gv_vector3 Pos;
	gv_packet& p = *packet;
	gv_string sc;
	int frame_count;
	p >> frame_count;
	p >> sc;
	p >> Rot.x >> Rot.y >> Rot.z;
	p >> Pos.x >> Pos.y >> Pos.z;
	gv_string_tmp s;
	s << "tracking object:" << sc << " frame count: " << frame_count << " pos " << Pos << " rot " << Rot << gv_endl;
	gv_console_output(*s);
	server->destroy_packet(packet);
}
class server_runnable : public gv_runnable
{
	//simplest 1 to n chat server
public:
	server_runnable()
	{
	}
	void run()
	{
		GV_PROFILE_EVENT(server_run, 0);
		gv_normal_udp_server server;
		server.set_callback_on_recv(&server_recv);
		server.create(1234);
		for (int i = 0; i < 10000; i++)
		{
			gv_thread::sleep(1);
		};
		server.destroy();
	}
};

class client_runnable : public gv_runnable
{
public:
	client_runnable()
	{
	}

	void run()
	{
		GV_PROFILE_EVENT(server_run, 0);
		gv_normal_udp_server server;
		server.set_callback_on_recv(&client_recv);
		server.create(0);
		for (int i = 0; i < 1000; i++)
		{
			gv_socket_address a(gv_ip_address_ipv4::addr_local_host(), 1234);
			gv_packet p;
			p << gv_string_tmp("client:say hello ") << i << " times";
			server.send_to(a, &p);
			gv_thread::sleep(10);
		};
		server.destroy();
		;
	}
};

void test4_threads()
{
	gv_thread thread1(gv_id_server);
	gv_thread thread2(gv_id_client);
	server_runnable r1;
	client_runnable r2;

	{
		GV_PROFILE_EVENT(start_server, 0)
		thread1.start(&r1);
		gv_thread::sleep(100);
	}
	{
		GV_PROFILE_EVENT(start_client, 0)
		thread2.start(&r2);
		gv_thread::sleep(1000);
	}

	thread1.join();
	thread2.join();
}

void main(gvt_array< gv_string >& args)
{
	if (args.find("server"))
	{
		gv_normal_udp_server server;
		server.set_callback_on_recv(&server_recv2);
		gv_int port = 1234;
		if (args.size() > 1)
		{
			args[1] >> port;
		}
		server.create(port);
		gv_console_output("press a key to quit server ");
		getchar();
		server.destroy();
	}
	else
		test4_threads();
}
}
