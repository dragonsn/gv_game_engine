
namespace unit_test_network_lobby_with_replica
{
	//class name
	#include	"2d_bricks_maps.h"
	#include	"2d_bricks_online.h"
#if GV_DEBUG_VERSION
	static const int max_actor_nb=50;
#else
	static const int max_actor_nb=1024;
#endif
	static const int max_relevant_actor_nb=50;
	int			 current_actor_nb=0; 
	
	int			 loop=200;
	gv_world_rpg2d   *	my_world=NULL;
	static const int	cell_size=32;
	gv_vector2			actors_pos[max_actor_nb]; 

	gv_ip_address_ipv4	server_ip;

	
	//============================================================================================
	//								:
	//============================================================================================
	void init_world(int w,int h ,char * map)
	{
		gv_global::rnd_opt.m_max_debug_line=10000;
		gv_global::rnd_opt.m_max_debug_tri=1000;
		m_sandbox->register_processor ( gv_id("gv_world_rpg2d") ,gve_event_channel_world);
		my_world=gvt_cast<gv_world_rpg2d> ( m_sandbox->get_event_processor(gve_event_channel_world) );
		{
			gv_vector3 min_p=to_3d(gv_vector2(0,0)); min_p.y=-100;
			gv_vector3 max_p=to_3d(gv_vector2( (gv_float)(w*cell_size),(gv_float)(h*cell_size) ) ); max_p.y=100;
			my_world->init(1, gv_vector2i(w, h),gv_box(min_p,max_p ),map);
		}
	}
	//============================================================================================
	//								:SERVER
	//============================================================================================
	static float max_map_size=10000.f;
	inline gvt_vector2<gv_ushort> compress_pos(const gv_vector2 & pos)
	{
		gv_vector2 v=pos; 
		v.x=v.x/(max_map_size)* 65535.f;
		v.y=v.y/(max_map_size)* 65535.f;
		v.y=gvt_clamp(v.y, 0.f,65535.f); 
		gvt_vector2<gv_ushort> vs; 
		vs.x=(gv_ushort)v.x; 
		vs.y=(gv_ushort)v.y; 
		return vs;
	};

	inline gv_vector2 uncompress_pos(const gvt_vector2<gv_ushort>  & vs)
	{
		gv_vector2 v; 
		v.x=(gv_float)vs.x*(max_map_size)/ 65535.f;
		v.y=(gv_float)vs.y*(max_map_size)/ 65535.f;
		return v; 
	};


	class my_replica_server :public gv_com_net_host
	{
	public:
		GVM_DCL_CLASS(my_replica_server,gv_com_net_host ); 

		my_replica_server()
		{
			m_current_user_id=0;
		}; 
		virtual	void				register_events		()
		{
			GVM_HOOK_EVENT(net_actor_2d_move			,on_event_move			,net);
			GVM_HOOK_EVENT(net_actor_2d_state_change	,on_event_state_change	,net);
			GVM_HOOK_EVENT(net_actor_2d_facing_change	,on_event_facing_change	,net);
			GVM_HOOK_EVENT(net_actor_input_command		,on_event_input_command	,net);
		};
		virtual void				unregister_events	()
		{
			GVM_UNHOOK_EVENT(net_actor_2d_move			,net);
			GVM_UNHOOK_EVENT(net_actor_2d_state_change	,net);
			GVM_UNHOOK_EVENT(net_actor_2d_facing_change ,net);
			GVM_UNHOOK_EVENT(net_actor_input_command	,net);
		};
		gv_int						on_event_move(gv_object_event_net_actor_2d_move * pevent)
		{
			actors_pos[pevent->uuid]=pevent->new_pos;
			return 1; 
		}
		gv_int						on_event_state_change(gv_object_event_net_actor_2d_state_change * pevent)
		{
			return 1;
		}
		gv_int						on_event_facing_change(gv_object_event_net_actor_2d_facing_change * pevent)
		{
			return 1; 
		}
		gv_int						on_event_input_command(gv_object_event_net_actor_input_command * pevent)
		{
			return 1; 
		}
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
			pclient->set_uuid(m_current_user_id++);
			return pclient; 
		}
		void						on_connection(gv_com_net_host * remote)
		{

			gv_com_net_replica * com_replica=get_sandbox()->create_object<gv_com_net_replica>(); 
			com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_relevant_actor_nb );
			com_replica->set_out_going_host(remote);
			GVM_POST_EVENT(net_add_net_replica,net, pe->replica=com_replica);
			m_uuid_view_map.add(remote->get_uuid(),com_replica);
		}
		void						on_disconnection(gv_com_net_host * remote)
		{
			m_uuid_view_map.erase(remote->get_uuid());
		}

		void						update_replica_from_world(gv_com_net_replica * com)
		{
			if (!com) return; 
			gv_int actor_nb;
			gv_net_replica * replica= com->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
			gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
			GV_ASSERT(replica); 
			for ( int i=0; i< actor_nb; i++, punit++)
			{
				gv_vector2 pos=actors_pos[i];
				punit->m_pos=compress_pos(pos);
				punit->m_visible=true; 
			}
		}

		virtual gv_bool				tick_local_host 			(gv_float dt	 )
		{
			super::tick_local_host(dt);
			for ( int i=0;i<this->m_connected_hosts.size(); i++)
			{
				gv_com_net_host * remote=m_connected_hosts[i];
				update_replica_from_world(remote->get_out_going_replica() ); 
			}
			return true;
		};
		gv_uint				m_current_user_id;
		gvt_hash_map<gv_uint , gvt_ref_ptr<gv_com_net_replica>	> m_uuid_view_map;
	};
	//============================================================================================
	//								:
	//============================================================================================
	void do_server()
	{	
		using namespace std;
		cout<<"replica server...."<<endl;
		cout<<"done , start to work...."<<endl;
		gvt_random<gv_float> m_random;

		gv_entity *  my_entity =m_sandbox->create_object<gv_entity>();
		gvt_ref_ptr<gv_com_net_host > host_local;
		host_local=new my_replica_server; 
		m_sandbox->add_object(host_local);
		host_local->rename(gv_id("lobby_server"));
		host_local->init_host(gv_socket_address(1234),true,0.1f,0,1024);
		host_local->register_events();

		my_entity ->add_component(host_local);
		
		gv_entity * actor_entity=m_sandbox->create_object<gv_entity> ();
		actor_entity->add_component(gv_com_rpg2d_actor_online::static_class());
		my_world->add_entity(actor_entity);
		gv_com_rpg2d_actor_online * pactor=actor_entity->get_component<gv_com_rpg2d_actor_online>( );
		pactor->m_auto_move=true;
		pactor->m_auto_move_speed=30.f;
		pactor->m_adjust_angle=-gv_float_trait::pi()/2.0f;;
		my_world->set_current_debug_actor(0);

		while(1)
		{
			gv_vector3 pos;
			pos.x =m_random.get_uniform()*1000.f;	pos.y =0; 
			pos.z =m_random.get_uniform()*1000.f;
			if (my_world->teleport_entity( actor_entity , pos ,gv_euler::get_zero_rotation() )  )  	break;
		};

		pactor->m_uuid=0; 

		for ( int i=1; i< max_actor_nb; i++)
		{
			gv_entity * new_entity=actor_entity->clone(); 
			gv_com_rpg2d_actor_online * pactor=new_entity->get_component<gv_com_rpg2d_actor_online>( );
			pactor->m_uuid=i;
			my_world->add_entity(new_entity);
			while(1)
			{
				gv_vector3 pos;
				pos.x =m_random.get_uniform()*1000.f;	
				pos.y =0; 
				pos.z =m_random.get_uniform()*1000.f;
				if (my_world->teleport_entity( new_entity , pos ,gv_euler::get_zero_rotation() )  )  
				{
					break;
				}
			};
		}
		
		my_world->enable_debug_radar();
		while (loop--)
		{
			gv_thread::sleep(3);
			if (!m_sandbox->tick() ) break;
		}
		host_local->unregister_events();
	}
	//--------------------------------------------------------------------------------------
	/*
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
		host_local->init_host(0,true,0.1f);
		my_entity->add_component(host_local);
		remote_host=m_sandbox->create_object<gv_com_net_host>(); 
		remote_host->init_host(gv_socket_address( server_ip , 1234), false,1.0f,0); 
		com_replica->set_in_coming_host(remote_host);
		my_entity->add_component(remote_host);
		
		my_entity->add_component(com_replica);
		while (loop--)
		{
			gv_thread::sleep(30);
			if (!m_sandbox->tick() ) break;
			if (remote_host &&remote_host->get_host_state()==gve_host_disconnected)		
			{
				loop=2;
			}
			if (remote_host && remote_host->is_connected())
			{
				gv_net_cmd_host_chat cmd;
				cmd.content<<"hello !!"<<"i="<<loop;
				remote_host->send_net_cmd(cmd ,	false); 
				if (loop==100)
				{
					my_entity->detach_component(remote_host);
				}
			};
		}
	}*/
		//--------------------------------------------------------------------------------------
	void update_replica_to_world (gv_com_net_replica * com)
	{
		gv_int actor_nb;
		gv_net_replica * replica= com->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
		gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
		GV_ASSERT(replica); 
		for ( int i=0; i< actor_nb; i++, punit++)
		{
			if (punit->m_visible)
			{
				gv_entity * pentity=my_world->get_entity(i);
				gv_vector2 pos=uncompress_pos(punit->m_pos);
				//pentity->set_rotation(gv_euler (0,punit, 0));
				pentity->set_position(to_3d(pos));
				pentity->update_matrix();
			}
		}
	}

	void do_client_3d()
	{using namespace std;
		cout<<"replica client...."<<endl;
		cout<<"done , start to work...."<<endl;
		gv_entity *  my_entity=m_sandbox->create_object<gv_entity>();
		gvt_ref_ptr<gv_com_net_host > host_local;
		gvt_ref_ptr<gv_com_net_host > remote_host;
		gvt_ref_ptr<gv_com_net_replica> com_replica=m_sandbox->create_object<gv_com_net_replica>(); 
		com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_relevant_actor_nb );
		host_local=m_sandbox->create_object<gv_com_net_host>();
		host_local->init_host(gv_socket_address(0),true,0.1f);
		my_entity->add_component(host_local);
		remote_host=m_sandbox->create_object<gv_com_net_host>(); 
		remote_host->init_host(gv_socket_address( server_ip , 1234), false,1.0f,0); 
		com_replica->set_in_coming_host(remote_host);
		my_entity->add_component(remote_host);
		my_entity->add_component(com_replica);

		//===============================================================================
		{//init scene
			my_entity	=m_sandbox->create_object<gv_entity> (gv_id("entity"),my_world);
			gv_com_skeletal_mesh*	com_sk_mesh	=get_sandbox()->create_object<gv_com_skeletal_mesh>(my_entity);
			gv_material* my_material			=m_sandbox->create_object<gv_material>();
			gv_skeletal_mesh *		my_skeletal_mesh	=NULL;
			gv_static_mesh	 *		my_static_mesh		=NULL;
			gv_ani_set *			my_animation		=NULL; 
			gv_effect *				my_effect=NULL;
			gvt_random<gv_float>	m_random;
			int current_idx=0; 
			{//load model & animation
				gv_string_tmp	file_name= FILE_FBX_HERON;
				gv_model * model= m_sandbox->create_object<gv_model> ();
				m_sandbox->import_external_format( model,file_name);
				if (!model->get_nb_animation()) return; 
				my_animation=model->get_animation(0);
				my_static_mesh=model->get_skeletal_mesh(0)->m_t_pose_mesh;
				my_skeletal_mesh=model->get_skeletal_mesh(0);
				if (!my_static_mesh->get_nb_normal()) my_static_mesh->rebuild_normal();
				my_static_mesh->rebuild_binormal_tangent();
			}

			{//load material
				gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> ();
				gv_string_tmp  file_name= FILE_RFX_BIRD;
				m_sandbox->import_external_format( effect,*file_name);
				my_effect=effect->get_base_effect(); 
				my_effect=my_effect->get_renderable_effect(gv_id("TexturedPhong"));
				my_material->m_effect=my_effect;

			}

			{//link animation & material
				com_sk_mesh->set_material(my_material);
				com_sk_mesh->set_resource(my_skeletal_mesh);
				com_sk_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_effect_renderer") );
				my_entity->add_component(com_sk_mesh); 
				gv_com_animation * com_ani=m_sandbox->create_object<gv_com_animation> ( my_entity);
				com_ani->set_resource(my_animation); 
				my_entity->add_component(com_ani);
				com_ani->play_animation(my_animation->get_sequence(0)->get_name_id(),0.f,true); 
			}

			my_entity->set_position(gv_vector3 ( 30, 1 ,30 ) );
			my_entity->update_matrix();
			my_world->add_entity( my_entity);

			for ( int i=1; i< max_actor_nb; i++)
			{
				gv_entity * new_entity=my_entity->clone(); 
				my_world->add_entity(new_entity);
				new_entity->update_matrix();
			}

			{//set camera;
				//my_entity->add_component(gv_id("gv_com_3d_lobby_controller") ); 
				gv_com_camera  * camera=m_sandbox->create_object<gv_com_camera> (gv_id("main_camera"), my_entity);
				camera->set_fov(60.f,1.333f,0.1f, 500 );
				camera->set_look_at(gv_vector3 ( 0, 2,5.f)*2 , gv_vector3(0,0,0) );
				camera->update_projection_view_matrix();
				my_entity->add_component(camera); 
				GVM_POST_EVENT(render_set_camera,render, (pe->camera=camera) );
			}
		
		}
		//===============================================================================
		while (loop--)
		{
			gv_thread::sleep(1);
			update_replica_to_world(com_replica);
			my_entity->get_component<gv_com_camera> ()->sync_to_entity(my_entity);
			if (!m_sandbox->tick() ) break;
			if (remote_host &&remote_host->get_host_state()==gve_host_disconnected)		
			{
				loop=gvt_min(loop,2);
			}
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
		bool is_3d_lobby =args.find("3d_lobby");
		server_ip=gv_ip_address_ipv4::addr_local_host();
		if (args.find("ip", idx) )
		{
			args[idx+1]>>server_ip; 
		}
		{
			sub_test_timer timer("test_event_processor");
			gv_unit_test_context_guard context(!is_server);
#if GV_DEBUG_VERSION
			init_world	(map_width3, map_height3,the_map3);
#else
			init_world	(map_width, map_height,the_map);
#endif
			m_sandbox->register_processor	(gv_network_mgr::static_class() ,gve_event_channel_net); 
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_synchronization(true);
			m_sandbox->get_event_processor	(gve_event_channel_net)->set_autonomous(true);
			{
				//-----------------------------------------------------------------------------------
				//!!!!
				//-----------------------------------------------------------------------------------
				if (is_server)			do_server();
				//else if (is_3d_lobby)	do_client_3d();
				else					do_client_3d();
				//-----------------------------------------------------------------------------------
			}
		}
	}
}