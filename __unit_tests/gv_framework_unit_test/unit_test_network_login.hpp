
namespace ref
{
struct gv_remote_call_client_login_ack
{
	gv_bool is_ok;
};
}
namespace unit_test_network_login
{
//class name
gvt_hash_map< gv_string, gv_socket_address > login_user_map;
int loop = 20;
int idx;
int log_in = 0;
class gv_com_chat_test_remote_client : public gv_com_net_host
{
public:
	gv_com_chat_test_remote_client()
	{
		logined = false;
	};
	~gv_com_chat_test_remote_client()
	{
		if (logined)
		{
			login_user_map.erase(user_name);
		}
	}
	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_chat& cmd)
	{
		gv_string_tmp s;
		s << host_name() << " says: " << cmd.content << gv_endl;
		std::cout << *s;
		my_server->broadcast_net_cmd(cmd);
		return true;
	};

	gv_bool exec_remote_call(const class gv_remote_call_client_login& call)
	{
		user_name = call.user_name;
		password = call.password;
		gv_remote_call_client_login_ack ret;
		ret.is_ok = false;
		if (login_user_map.find(user_name))
		{
			GVM_CONSOLE_OUT("[login_failed]:" << host_name() << ":" << user_name << " already login !" << gv_endl);
			ret.error_code = 1;
		}
		else if (password != "1234")
		{
			GVM_CONSOLE_OUT("[login_failed]:" << host_name() << ":" << user_name << " wrong password!" << gv_endl);
			ret.error_code = 2;
		}
		else
		{
			GVM_CONSOLE_OUT("[login_sucess]:" << host_name() << "as " << user_name << " !" << gv_endl);
			login_user_map.add(user_name, this->get_host_address());
			logined = true;
			ret.is_ok = true;
		}
		send_remote_call(&ret);
		return true;
	};

	gv_string user_name;
	gv_string password;
	gv_bool logined;
	gv_com_net_host* my_server;
};

class gv_com_chat_test_server : public gv_com_net_host
{
public:
	gv_com_chat_test_server(){};

	gv_com_net_host* create_client()
	{
		gv_com_chat_test_remote_client* new_client = new gv_com_chat_test_remote_client;
		gv_com_net_host* pclient = new_client;
		new_client->my_server = this;
		return pclient;
	}

	virtual gv_bool recieve_net_cmd(class gv_net_cmd_host_chat& cmd)
	{
		gv_string_tmp s;
		s << cmd.content << gv_endl;
		std::cout << *s;
		return true;
	};
};

//============================================================================================
//								:
//============================================================================================
class gv_com_chat_test_remote_server : public gv_com_net_host
{
public:
	gv_com_chat_test_remote_server()
	{
		logined = false;
	};

	gv_bool exec_remote_call(const class gv_remote_call_client_login_ack& call)
	{
		if (!call.is_ok && call.error_code == 1)
		{
			GVM_CONSOLE_OUT("[login_failed]:" << host_name() << ":" << user_name << " already login !" << gv_endl);
		}
		else if (!call.is_ok && call.error_code == 2)
		{
			GVM_CONSOLE_OUT("[login_failed]:" << host_name() << ":" << user_name << " wrong password!" << gv_endl);
		}
		else
		{
			GVM_CONSOLE_OUT("[login_sucess]:" << host_name() << "as " << user_name << " !" << gv_endl);
			logined = true;
		}
		return true;
	}
	gv_string user_name;
	gv_bool logined;
};

void main(gvt_array< gv_string >& args)
{
	using namespace std;
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();

	if (args.find("loop", idx))
	{
		args[idx + 1] >> loop;
	}
	bool is_server = false;
	if (args.find("server"))
	{
		is_server = true;
	}
	gv_ip_address_ipv4 ip = gv_ip_address_ipv4::addr_local_host();
	if (args.find("ip", idx))
	{
		args[idx + 1] >> ip;
	}
	gv_string user_name = "test1";
	gv_string password = "1234";

	if (args.find("password", idx))
	{
		args[idx + 1] >> password;
	}
	bool is_auto_client = !args.find("is_not_auto");
	{
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		m_sandbox->register_processor(gv_network_mgr::static_class(), gve_event_channel_net);
		m_sandbox->get_event_processor(gve_event_channel_net)->set_synchronization(true);
		m_sandbox->get_event_processor(gve_event_channel_net)->set_autonomous(true);
		if (is_server)
		{
			gv_entity* my_entity = m_sandbox->create_object< gv_entity >();
			gvt_ref_ptr< gv_com_net_host > host_local;
			gvt_ref_ptr< gv_com_net_host > remote_host;
			cout << "create server...." << endl;
			host_local = new gv_com_chat_test_server;
			m_sandbox->add_object(host_local);
			host_local->init_host(gv_socket_address(1234), true, 0.1f, 0, 1024);
			my_entity->add_component(host_local);
			cout << "done , start to work...." << endl;

			while (loop--)
			{
				gv_thread::sleep(100);
				m_sandbox->tick();
			}
		}
		else if (is_auto_client) //client
		{
			gv_entity* my_entity = m_sandbox->create_object< gv_entity >();
			gvt_ref_ptr< gv_com_net_host > host_local;
			gvt_ref_ptr< gv_com_net_host > remote_host;
			cout << "create client...." << endl;
			host_local = m_sandbox->create_object< gv_com_net_host >();
			host_local->init_host(gv_socket_address(0), true, 0.1f);
			my_entity->add_component(host_local);
			remote_host = new gv_com_chat_test_remote_server;
			m_sandbox->add_object(remote_host);
			gv_com_chat_test_remote_server* pserver = (gv_com_chat_test_remote_server*)remote_host.ptr();
			pserver->user_name = user_name;
			remote_host->init_host(gv_socket_address(ip, 1234), false, 1.0f, 0);
			my_entity->add_component(remote_host);
			cout << "done , start to work...." << endl;

			while (loop--)
			{
				gv_thread::sleep(100);
				m_sandbox->tick();
				if (remote_host->is_deleted())
				{
					break;
				}
				if (remote_host->get_host_state() == gve_host_disconnected)
					break;
				if (remote_host->is_connected())
				{
					if (!log_in)
					{
						gv_remote_call_client_login call;
						call.user_name = user_name;
						call.password = password;
						remote_host->send_remote_call(&call);
						log_in = 1;
					}
					if (log_in == 1)
					{
						if (pserver->logined)
							log_in = 2;
					}
					if (log_in == 2)
					{
						gv_net_cmd_host_chat cmd;
						gv_bool use_reliable = loop % 4 == 0;
						cmd.content = "hello !! i am ";
						cmd.content << user_name;
						remote_host->send_net_cmd(cmd, use_reliable);
						cmd.content = "i=";
						cmd.content << loop;
						remote_host->send_net_cmd(cmd, use_reliable);
						if (loop == 100)
						{
							my_entity->detach_component(remote_host);
						}
					}
				}
			} //loop;
		}
		else
		{
			std::cout << "user name:";
			std::string s;
			std::cin >> s;
			user_name = s.c_str();
			std::cout << "password:";
			s = "";
			std::cin >> s;
			password = s.c_str();
			gv_command_console console;
			console.start();
			gv_entity* my_entity = m_sandbox->create_object< gv_entity >();
			gvt_ref_ptr< gv_com_net_host > host_local;
			gvt_ref_ptr< gv_com_net_host > remote_host;
			cout << "create client...." << endl;
			host_local = m_sandbox->create_object< gv_com_net_host >();
			host_local->init_host(gv_socket_address(0), true, 0.1f);
			my_entity->add_component(host_local);
			remote_host = new gv_com_chat_test_remote_server;
			m_sandbox->add_object(remote_host);
			gv_com_chat_test_remote_server* pserver = (gv_com_chat_test_remote_server*)remote_host.ptr();
			pserver->user_name = user_name;
			remote_host->init_host(gv_socket_address(ip, 1234), false, 1.0f, 0);
			my_entity->add_component(remote_host);
			cout << "done , start to work...." << endl;

			while (loop--)
			{
				gv_thread::sleep(100);
				m_sandbox->tick();
				if (remote_host->is_deleted())
				{
					break;
				}
				if (remote_host->get_host_state() == gve_host_disconnected)
					break;
				if (remote_host->is_connected())
				{
					if (!log_in)
					{
						gv_remote_call_client_login call;
						call.user_name = user_name;
						call.password = password;
						remote_host->send_remote_call(&call);
						log_in = 1;
					}
					if (log_in == 1)
					{
						if (pserver->logined)
							log_in = 2;
					}
					if (log_in == 2)
					{
						gv_net_cmd_host_chat cmd;
						gv_bool use_reliable = loop % 4 == 0;

						gvt_array< gv_string > inputs;
						console.get_command_array(inputs);
						if (inputs.size())
						{
							cmd.content << user_name << " says:";
							remote_host->send_net_cmd(cmd, use_reliable);
							cmd.content = "";
							for (int i = 0; i < inputs.size(); i++)
							{
								cmd.content << inputs[i] << gv_endl;
							}
							cmd.content << "-------------------";
							remote_host->send_net_cmd(cmd, use_reliable);
						}
						if (loop == 100)
						{
							my_entity->detach_component(remote_host);
						}
					}
				}
			} //loop;
			console.stop();
		}
		std::cout << std::endl;
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}