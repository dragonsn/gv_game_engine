

namespace unit_test_network_chat
{
	//class name
	class gv_com_chat_test_server :public gv_com_net_host
	{
	public:
		gv_com_chat_test_server()
		{}; 
		virtual	gv_bool				recieve_net_cmd(class gv_net_cmd_host_chat	&cmd)
		{
			gv_string_tmp s; 
			s<<host_name()<<" says: "<< cmd.content<<gv_endl;
			std::cout<<*s;
			return true;
		};
		gv_com_net_host *			create_client()
		{
			gv_com_net_host * pclient=new gv_com_chat_test_server;
			return pclient; 
		}
	};


	void main( gvt_array<gv_string > &args )
	{
		using namespace std;
		gv_global::sandbox_mama.get();
		gv_global::sandbox_mama->init();
		int loop=20;
		int idx;
		if (args.find("loop", idx) )
		{
			args[idx+1]>>loop; 
		}
		bool is_server=false; 
		if( args.find("server") )
		{
			is_server=true; 
		}
		gv_ip_address_ipv4 ip=gv_ip_address_ipv4::addr_local_host();
		if (args.find("ip", idx) )
		{
			args[idx+1]>>ip; 
		}
		{
			m_sandbox=gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox() );
			m_sandbox->register_processor	(gv_network_mgr::static_class() ,gve_event_channel_net); 
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_synchronization(true);
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_autonomous(true);

			gv_entity *  my_entity=m_sandbox->create_object<gv_entity>();
			{
				gvt_ref_ptr<gv_com_net_host > host_local;
				gvt_ref_ptr<gv_com_net_host > remote_host;
				if (is_server)
				{
					cout<<"create server...."<<endl;
					host_local=new gv_com_chat_test_server;
					m_sandbox->add_object(host_local);
					host_local->init_host(gv_socket_address(1234),true,0.1f,0,1024);
					my_entity->add_component(host_local);
					cout<<"done , start to work...."<<endl;
				}
				else
				{
					cout<<"create client...."<<endl;
					host_local=m_sandbox->create_object<gv_com_net_host>(); 
					host_local->init_host(gv_socket_address(0),true,0.1f);
					my_entity->add_component(host_local);
					remote_host=m_sandbox->create_object<gv_com_net_host>(); 
					remote_host->init_host(gv_socket_address( ip , 1234), false,1.0f,0); 
					my_entity->add_component(remote_host);
					cout<<"done , start to work...."<<endl;
				}
				while (loop--)
				{
					gv_thread::sleep(100);
					m_sandbox->tick();
					if (remote_host &&remote_host->is_deleted()) 
					{
						break;
					}
					if (remote_host &&remote_host->get_host_state()==gve_host_disconnected)		break;
					if (remote_host && remote_host->is_connected())
					{
						gv_net_cmd_host_chat cmd;
						gv_bool use_reliable=loop%4==0;
						cmd.content="hello !!";
						remote_host->send_net_cmd(cmd ,	use_reliable); 
						cmd.content="i="; 
						cmd.content<<loop; 
						remote_host->send_net_cmd(cmd , use_reliable);
						if (loop==100)
						{
							my_entity->detach_component(remote_host);
						}
					};
				}
			}
			gv_global::sandbox_mama->delete_sandbox(m_sandbox);
			gv_id::static_purge();
			m_sandbox=NULL;
		}
		gv_global::sandbox_mama.destroy();
		gv_id::static_purge();
	}
}



