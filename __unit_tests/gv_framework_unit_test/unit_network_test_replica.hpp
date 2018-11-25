
namespace unit_test_network_replica
{
	//class name
	#include	"2d_bricks_maps.h"
	#include	"2d_bricks_online.h"
	static const int max_actor_nb=10;
	int			current_actor_nb=0; 
	gvt_ref_ptr<gv_com_net_replica>	 client_views[max_actor_nb];
	map			m_map;
	//class name
	class my_replica_server :public gv_com_net_host
	{
	public:
		my_replica_server()
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
			GV_ASSERT(current_actor_nb<max_actor_nb);
			gv_com_net_host * pclient=new gv_com_net_host;
			gv_com_net_replica * com_replica=get_sandbox()->create_object<gv_com_net_replica>(); 
			com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_actor_nb );
			com_replica->set_out_going_host(pclient);
			GVM_POST_EVENT(net_add_net_replica,net, pe->replica=com_replica);
			client_views[current_actor_nb++]=com_replica;
			return pclient; 
		}
		gvt_ref_ptr<gv_com_net_replica> m_replica;
	};

	void update_replica_from_world(gv_com_net_replica * com)
	{
		gv_int actor_nb;
		gv_net_replica * replica= com->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
		gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
		GV_ASSERT(replica); 
		for ( int i=0; i< max_actor_nb; i++, punit++)
		{
			actor * pactor=m_map.m_actor_list[i];
			gvt_vector2<gv_ushort> vs=m_map.compress_pos(pactor->m_aabb.center());
			punit->m_pos=m_map.compress_pos(pactor->m_location);
			punit->m_visible=true; 
		}
	}
	void update_replica_to_world (gv_com_net_replica * com)
	{
		gv_int actor_nb;
		gv_net_replica * replica= com->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
		gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
		GV_ASSERT(replica); 
		for ( int i=0; i< max_actor_nb; i++, punit++)
		{
			if (punit->m_visible)
			{
				actor * pactor=m_map.m_actor_list[i];
				gv_vector2 pos=m_map.uncompress_pos(punit->m_pos);
				m_map.teleport_actor(pactor,pos,false);
			}
			m_map.m_actor_list[i]->m_speed=gv_vector2::get_zero_vector();
		}
	}

	int loop=200;
	gv_ip_address_ipv4 ip;
	
	//--------------------------------------------------------------------------------------
	void do_server()
	{using namespace std;
		cout<<"replica server...."<<endl;
		cout<<"done , start to work...."<<endl;
		gv_entity *  my_entity=m_sandbox->create_object<gv_entity>();
		gvt_ref_ptr<gv_com_net_host > host_local;
		host_local=new my_replica_server; 
		m_sandbox->add_object(host_local);
		host_local->init_host(gv_socket_address(1234),true,0.1f,0,1024);
		my_entity->add_component(host_local);
		m_map.init(the_map2,map_width2,map_height2);
		m_map.spawn_actors(max_actor_nb);
		while (loop--)
		{
			gv_thread::sleep(30);
			if (!m_sandbox->tick() ) break;
			m_map.update(m_sandbox->get_delta_time() );
			m_map.render();
			//write all the replica
			for ( int i=0; i<current_actor_nb; i++)
			{
				if (client_views[i]&& !client_views[i]->is_deleted() )
				{
					update_replica_from_world(client_views[i]);
				}
			}
		}
		for ( int i=0; i< max_actor_nb; i++)
		{
			client_views[i]=NULL;
		}
	}
	//--------------------------------------------------------------------------------------
	void do_client()
	{using namespace std;
		cout<<"replica client...."<<endl;
		cout<<"done , start to work...."<<endl;
		gv_entity *  my_entity=m_sandbox->create_object<gv_entity>();
		gvt_ref_ptr<gv_com_net_host > host_local;
		gvt_ref_ptr<gv_com_net_host > remote_host;
		gvt_ref_ptr<gv_com_net_replica> com_replica=m_sandbox->create_object<gv_com_net_replica>(); 
		com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_actor_nb );
		host_local=m_sandbox->create_object<gv_com_net_host>();
		host_local->init_host(gv_socket_address(0),true,0.1f);
		my_entity->add_component(host_local);
		remote_host=m_sandbox->create_object<gv_com_net_host>(); 
		remote_host->init_host(gv_socket_address( ip , 1234), false,1.0f,0); 
		com_replica->set_in_coming_host(remote_host);
		my_entity->add_component(remote_host);
		
		my_entity->add_component(com_replica);
		m_map.init(the_map2,map_width2,map_height2);
		m_map.spawn_actors(max_actor_nb);
		while (loop--)
		{
			gv_thread::sleep(30);
			if (!m_sandbox->tick() ) break;
			if (remote_host &&remote_host->get_host_state()==gve_host_disconnected)		
			{
				loop=gvt_min(loop,2);
			}
			if (remote_host && remote_host->is_connected())
			{
				gv_net_cmd_host_chat cmd;
				gv_bool use_reliable=loop%4==0;
				cmd.content<<"hello !!"<<"i="<<loop;
				remote_host->send_net_cmd(cmd ,	use_reliable); 
				if (loop==100)
				{
					my_entity->detach_component(remote_host);
				}
			};
			m_map.update(m_sandbox->get_delta_time() );
			m_map.render();
			update_replica_to_world(com_replica);
		}
	}
	//--------------------------------------------------------------------------------------
	void main( gvt_array<gv_string > &args )
	{
		
		
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
		ip=gv_ip_address_ipv4::addr_local_host();
		if (args.find("ip", idx) )
		{
			args[idx+1]>>ip; 
		}
		{
			sub_test_timer timer("test_event_processor");
			gv_unit_test_context_guard context(!is_server);
			m_sandbox->register_processor	(gv_network_mgr::static_class() ,gve_event_channel_net); 
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_synchronization(true);
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_autonomous(true);
			{
				//-----------------------------------------------------------------------------------
				//!!!!
				//-----------------------------------------------------------------------------------
				if (is_server)	do_server(); 
				else			do_client();
				//-----------------------------------------------------------------------------------
			}
		}
	}
}