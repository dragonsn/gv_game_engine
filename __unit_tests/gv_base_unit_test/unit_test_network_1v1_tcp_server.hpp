namespace unit_test_network_1v1_tcp_server
{

static const char* client_greeting = "hello, I am client";
static const char* server_answer = "hello ,I am server";
static gv_string huge_data;

void build_huge_data()
{

	huge_data.reserve(1024 * 1024);
	for (int i = 0; i < 1024 * 10; i++)
	{
		huge_data += client_greeting;
		huge_data += server_answer;
	}
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
		gvt_array< gv_char > buffer;
		gv_socket* pnew = NULL;
		gv_string_tmp s;
		{
			GV_PROFILE_EVENT(server_init_listen, 0);
			m_listen_socket.init_socket(e_socket_type_stream, e_socket_proto_tcp);
			m_listen_socket.bind(m_socket_address);
			m_listen_socket.listen();
		}

		{
			GV_PROFILE_EVENT(server_accept, 0);
			pnew = m_listen_socket.accept();
			test_log() << "server accepted  " << *m_listen_socket.get_peer_address().string() << gv_endl;
			GV_ASSERT(pnew);
		}

		{
			GV_PROFILE_EVENT(server_reciver0, 0);
			buffer.resize(2048);
			int i = pnew->receive(buffer.begin(), buffer.size());
			s = client_greeting;
			test_log() << "server recieved  :" << i << "bytes string is:" << s << gv_endl;
			GV_ASSERT(s == buffer.begin());
		}

		{
			GV_PROFILE_EVENT(server_answer1, 0);
			s = server_answer;
			int i = pnew->send(*s, s.strlen() + 1);
			test_log() << "server send  :" << i << "bytes string is:" << s << gv_endl;
		}
		{
			GV_PROFILE_EVENT(server_reciver1, 0)
			buffer.resize(huge_data.size() + 64);
			int i;
			s = "";
			while ((i = pnew->receive(buffer.begin(), buffer.size())) > 0)
			{
				buffer[i] = 0;
				s += buffer.begin();
			}
			//s=buffer.begin();
			test_log() << "server recieved  :" << s.size() << "bytes" << gv_endl;
			GV_ASSERT(s == huge_data);
		}
		{
			GV_PROFILE_EVENT(server_wait_event, 0)
			_event.wait();
		}
		pnew->close();
		m_listen_socket.close();
		delete pnew;
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
	client_runnable(const gv_socket_address& a)
	{
		m_socket_address = a;
	}

	void run()
	{
		GV_PROFILE_EVENT(client_run, 0);
		gvt_array< gv_char > buffer;
		gv_socket* pnew = NULL;
		gv_string_tmp s;
		{
			GV_PROFILE_EVENT(client_connect, 0)
			m_socket.init_socket(e_socket_type_stream, e_socket_proto_tcp);
			m_socket.connect(m_socket_address);
		}

		{
			GV_PROFILE_EVENT(client_send_greeting, 0)
			s = client_greeting;
			int i = m_socket.send(*s, s.size());
			test_log() << "client send  :" << i << "bytes string is:" << *s << gv_endl;
		}

		{
			GV_PROFILE_EVENT(client_wait_server_greeting, 0)
			buffer.resize(2048);
			int i = m_socket.receive(buffer.begin(), 2048);
			s = buffer.begin();
			GV_ASSERT(s == server_answer);
			test_log() << "client recieved  :" << i << "bytes string is:" << s << gv_endl;
		}
		{
			GV_PROFILE_EVENT(client_send_huge_data, 0)
			int size = m_socket.send(huge_data.begin(), huge_data.size());
			GV_ASSERT(size == huge_data.size());
			test_log() << "client send  :" << size << "bytes huge data" << gv_endl;
		}
		{
			GV_PROFILE_EVENT(client_wait_event, 0)
			_event.wait();
		}
		m_socket.close();
	}

	void notify()
	{
		_event.set();
	}

	bool _ran;
	gv_id _threadName;
	gv_event _event;
	gv_socket m_socket;
	gv_socket_address m_socket_address;
};

void test4_threads()
{
	build_huge_data();
	gv_socket_address a(gv_ip_address_ipv4::addr_local_host(), 1234);

	gv_thread thread1(gv_id_server);
	gv_thread thread2(gv_id_client);

	server_runnable r1(a);
	client_runnable r2(a);

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
	{
		GV_PROFILE_EVENT(singal_stop_server, 0)
		r1.notify();
		gv_thread::sleep(100);
		;
	}
	{
		GV_PROFILE_EVENT(singal_stop_client, 0)
		r2.notify();
		gv_thread::sleep(100);
		;
	}
	thread1.join();
	thread2.join();
	huge_data.clear();
}

void main(gvt_array< gv_string >& args)
{

	test4_threads();
}
}
