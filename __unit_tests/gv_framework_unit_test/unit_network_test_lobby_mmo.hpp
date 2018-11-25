#include "gv_framework/component/gv_com_attach.h"

namespace unit_test_network_lobby_mmo
{
	//class name
#include	"2d_bricks_maps.h"
#include	"2d_bricks_online.h"
#if GV_DEBUG_VERSION
	static const int max_actor_nb=10;
#else
	static const int max_actor_nb=1024;
#endif

#if GV_DEBUG_VERSION
	static const int		max_relevant_actor_nb=50;
#else
	static const int		max_relevant_actor_nb=200;
#endif

	static const int		max_visible_dist	 =50;
	int						current_actor_nb=0; 
	int						loop=200;
	gv_world_rpg2d   *		my_world=NULL;
	static const int		cell_size=16;
	gv_entity *				my_terrain_entity	=NULL;
	gv_entity *				main_actor;
	gv_com_terrain_roam *	my_terrain			=NULL;
	gv_ip_address_ipv4		server_ip;
	gv_bool					is_offline=false;
	gv_effect *				atmosphere_effect=NULL; 
	gv_effect *				phong_effect=NULL;
	gv_char	  *				s_world_grid; 
	gv_int					s_world_w, s_world_h;

	
	//============================================================================================
	//								:
	//============================================================================================
	

	//============================================================================================
	//								:
	//============================================================================================
	void init_world(int w,int h ,char * map)
	{
		m_sandbox->register_processor ( gv_id("gv_world_rpg2d") ,gve_event_channel_world);
		my_world=gvt_cast<gv_world_rpg2d> ( m_sandbox->get_event_processor(gve_event_channel_world) );
		{
			gv_vector3 min_p=to_3d(gv_vector2(0,0)); min_p.y=-100;
			gv_vector3 max_p=to_3d(gv_vector2( (gv_float)(w*cell_size),(gv_float)(h*cell_size) ) ); max_p.y=100;
			my_world->init(1, gv_vector2i(w, h),gv_box(min_p,max_p ),map);
			s_world_grid=map; 
			s_world_h=h;
			s_world_w=w; 
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

	struct	relevant_priority
	{
		relevant_priority()
		{
		
		}
		~relevant_priority()
		{
		}
		relevant_priority(gv_net_replica_net_unit_2d * _unit_table,  gv_int _index)
		{
			unit_table=_unit_table;
			index=_index; 
		}

		gv_int						index; 
		gv_net_replica_net_unit_2d*	unit_table;
		gv_vector2i get_location ()
		{
			gvt_vector2<gv_ushort> &upos=unit_table[index].m_pos;
			gv_vector2i pos=gv_vector2i((gv_int)upos.get_x(),(gv_int)upos.get_y());
			return pos;
		}; 
		gv_vector2i get_client_location ()
		{
			gvt_vector2<gv_ushort> &upos=unit_table[0].m_pos;
			gv_vector2i pos=gv_vector2i((gv_int)upos.get_x(),(gv_int)upos.get_y());
			return pos;
		}; 

		gv_int		get_to_client_distance()
		{
			gv_vector2i pos=get_location();
			gv_vector2i client_pos=get_client_location();
			return gvt_abs(pos.get_x()-client_pos.get_x())+gvt_abs(pos.get_y()-client_pos.get_y());
		}
		gv_net_replica_net_unit_2d & get_unit()
		{
			return unit_table[index];
		}

		bool operator<(relevant_priority &  a)
		{
			if (get_unit().m_visible && !a.get_unit().m_visible) return true; 
			if (!get_unit().m_visible && a.get_unit().m_visible)return false; 
			if (index==0  ) return true; 
			if (a.index==0) return false; 
			if (get_to_client_distance() < a.get_to_client_distance() ) return true; 
			return false; 
		}
	};

	class my_replica_client :public gv_com_net_host
	{
	public:
		my_replica_client(){}
		typedef			gvt_hash_map<gv_ushort, gv_int> uuid_cache;
		uuid_cache					 m_relevant_cache;
		gvt_array<relevant_priority> m_relevant_priority_table;
	};
	class my_replica_server :public gv_com_net_host
	{
	public:
		GVM_DCL_CLASS(my_replica_server,gv_com_net_host ); 

		my_replica_server()
		{
			m_current_user_id=0;
			m_world_image.init(s_world_grid,s_world_w, s_world_h);
			m_world_image.spawn_actors(max_actor_nb);
		
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
			m_world_image.teleport_actor( m_world_image.m_actor_list[pevent->uuid],pevent->new_pos,false,false);
			return 1; 
		}
		gv_int						on_event_state_change(gv_object_event_net_actor_2d_state_change * pevent)
		{
			m_world_image.m_actor_list[pevent->uuid]->m_state=pevent->new_state;
			return 1;
		}
		gv_int						on_event_facing_change(gv_object_event_net_actor_2d_facing_change * pevent)
		{
			m_world_image.m_actor_list[pevent->uuid]->m_rotation=pevent->new_facing;
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
			my_replica_client * pclient=new my_replica_client;
			pclient->set_uuid(m_current_user_id++);
			pclient->m_relevant_priority_table.resize(max_relevant_actor_nb);
			return pclient; 
		}
		void						on_connection(gv_com_net_host * remote)
		{

			gv_com_net_replica * com_replica=get_sandbox()->create_object<gv_com_net_replica>(); 
			com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_relevant_actor_nb );
			com_replica->set_out_going_host(remote);
			GVM_POST_EVENT(net_add_net_replica,net, pe->replica=com_replica);
			my_replica_client * pclient=(my_replica_client *  )remote;
			gv_int actor_nb;
			gv_net_replica * replica= com_replica->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
			gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
			for ( int i=0; i<max_relevant_actor_nb; i++ )
			{
				pclient->m_relevant_priority_table[i]=relevant_priority(punit, i);
			}
			m_uuid_view_map.add(remote->get_uuid(),com_replica);
		}
		void						on_disconnection(gv_com_net_host * remote)
		{
			m_uuid_view_map.erase(remote->get_uuid());
		}

		void						update_replica_from_world(my_replica_client * host,gv_com_net_replica * com ,gv_uint uuid)
		{
			if (!com) return; 
			gv_int actor_nb;
			gv_net_replica * replica= com->get_replica_array(gv_net_replica_net_unit_2d::static_id() ,actor_nb);
			gv_net_replica_net_unit_2d *punit=(gv_net_replica_net_unit_2d *)replica;
			//=================================================
			actor * main_actor=m_world_image.m_actor_list[uuid];
			fill_one_actor(punit[0], main_actor);
			//=================================================
			gv_rect rect = main_actor->m_aabb; 
			rect.extend((float)max_visible_dist, (float)max_visible_dist ); 
			actor_cache_1024 actors; 
			m_world_image.collect_actors (rect, actors);
			gv_ushort nb_actors=(gv_ushort) gvt_min(actors.size() , max_relevant_actor_nb-1);
			//=====add to replica=================================================
			int new_added=0; 
			GV_ASSERT(replica); 
			for ( int i=0; i< nb_actors; i++)
			{
				actor * neighbour=actors[i]; 
				if (neighbour==main_actor) continue;
				gv_int index;
				if (host->m_relevant_cache.find(neighbour->m_index,index))
				{
					fill_one_actor(punit[index],neighbour) ;
				}
				else 
				{
					//try to add to the bottom of relevant -list
					gv_int new_index=host->m_relevant_priority_table[max_relevant_actor_nb-1-new_added].index;
					gv_net_replica_net_unit_2d * new_unit =punit+new_index; 
					actor * new_actor=m_world_image.m_actor_list[new_unit->m_id];
					
					if (!new_unit->m_visible 
						|| 
						(neighbour->m_location-main_actor->m_location).length_squared()< (new_actor->m_location-main_actor->m_location).length_squared() 
						)
					{
						fill_one_actor(punit[new_index],neighbour) ;
						host->m_relevant_cache.erase(new_actor->m_index); 
						host->m_relevant_cache.add(neighbour->m_index, new_index);
						new_added++;
					}
					else
					{
						GVM_DEBUG_LOG(net,"can't find a empty slot in relevant list!!" << gv_endl);
					}
				}
			}
			host->m_relevant_priority_table.sort();
		}

		void					fill_one_actor(gv_net_replica_net_unit_2d & unit , actor * a)
		{
			gv_vector2 pos=a->m_location;
			unit.m_pos=compress_pos(pos);
			unit.m_visible=true; 
			unit.m_state=a->m_state;
			unit.m_id=(gv_ushort)a->m_index;
			unit.m_facing=a->m_rotation;
		}

		virtual gv_bool				tick_local_host 			(gv_float dt	 )
		{
			super::tick_local_host(dt);
			for ( int i=0;i<this->m_connected_hosts.size(); i++)
			{
				my_replica_client * remote=(my_replica_client*)m_connected_hosts[i].ptr();
				update_replica_from_world(remote, remote->get_out_going_replica() ,remote->get_uuid()); 
			}
			return true;
		};
		gv_uint			m_current_user_id;
		gvt_hash_map<gv_uint , gvt_ref_ptr<gv_com_net_replica>	> m_uuid_view_map;
		map				m_world_image;
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
		host_local->init_host(gv_socket_address( 1234),true,0.1f,0,1024);
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
		gv_int current_debug_actor=0;
		while (loop--)
		{
			gv_thread::sleep(3);
			if (!m_sandbox->tick() ) break;
			//--------------------------------------------------------------------------
			{
			
				static bool last_key_down=false;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
				{
					current_debug_actor++;
					current_debug_actor%=max_actor_nb;
					my_world->set_current_debug_actor(current_debug_actor);
				}
				last_key_down=m_sandbox->get_input_manager()->is_key_down(e_key_space);
			}
			//--------------------------------------------------------------------------
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
				pentity->set_rotation(gv_euler (0,punit->m_facing, 0));
				pentity->set_position(to_3d(pos));
				pentity->update_matrix();
			}
		}
	}
	void init_scene()
	{
		gv_string	height_map_name=FILE_TEX_HEIGHTMAP_S;
		{
			//create sky box
			gv_string_tmp	file_name=FILE_3DS_SPHERE;
			gv_model * model= m_sandbox->create_object<gv_model> (my_world);
			if (!m_sandbox->import_external_format(model, file_name))
			{
				return; 
			};
			gv_static_mesh * pmesh=model->get_static_mesh(0);
			gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> (my_world);
			file_name= FILE_RFX_NEW_SKY;
			m_sandbox->import_external_format( effect,*file_name);
			gv_effect * my_effect=effect->get_base_effect(); 
			atmosphere_effect=my_effect;
			my_effect=my_effect->get_renderable_effect(gv_id("SkyBox") );
			gv_entity *  sky_entity	=m_sandbox->create_object<gv_entity> (gv_id("sky_entity"),my_world);
			gv_material* sky_material			=m_sandbox->create_object<gv_material>();
			sky_material->m_effect=my_effect;
			gv_com_static_mesh*	com_static_mesh	=get_sandbox()->create_object<gv_com_static_mesh>();
			com_static_mesh->set_material(sky_material); 
			com_static_mesh->set_resource(pmesh);
			com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_effect_renderer") );
			com_static_mesh->set_always_visible(true);
			com_static_mesh->set_background(true);
			sky_entity->add_component(com_static_mesh);
			my_world->add_entity(sky_entity);
		}

		{//ocean
			gv_string_tmp	file_name= FILE_3DS_SCREEN_QUAD;
			gv_model * model= m_sandbox->create_object<gv_model> (my_world);
			m_sandbox->import_external_format( model,file_name);
			gv_static_mesh * pmesh=model->get_static_mesh(0);
			gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> (my_world);
			gv_effect * my_effect=atmosphere_effect->get_renderable_effect(gv_id("OceanWater") );
			gv_entity *  ocean_entity	=m_sandbox->create_object<gv_entity> (gv_id("ocean_entity"),my_world);
			gv_material* ocean_material	=m_sandbox->create_object<gv_material>();
			ocean_material->m_effect=my_effect;
			gv_com_static_mesh*	com_static_mesh	=get_sandbox()->create_object<gv_com_static_mesh>();
			com_static_mesh->set_material(ocean_material); 
			com_static_mesh->set_resource(pmesh);
			com_static_mesh->set_renderer_id(gve_render_pass_opaque, gv_id( "gv_com_effect_renderer") );
			com_static_mesh->set_always_visible(true);
			ocean_entity->add_component(com_static_mesh);
			ocean_entity->set_position(gv_vector3(0,0,0));
			ocean_entity->update_matrix();
			my_world->add_entity(ocean_entity);
			gv_vector4 ocean_h(0.f,1,1,1);
			my_effect->set_param(gv_id("g_OceanHeight"),ocean_h);
		}

		{//load material 
			gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> (my_world);
			gv_string_tmp  file_name= FILE_RFX_BIRD;
			m_sandbox->import_external_format( effect,*file_name);
			phong_effect=effect->get_base_effect(); 
			phong_effect=phong_effect->get_renderable_effect(gv_id("TexturedPhong"));
		}

		{//terrain

			my_terrain_entity	=m_sandbox->create_object<gv_entity> (gv_id("terrain_entity"),my_world);
			my_terrain			=m_sandbox->create_object<gv_com_terrain_roam> (my_terrain_entity);
			my_terrain_entity->add_component(my_terrain); 
			my_terrain_entity->set_scale	(gv_vector3(16.f , 4.f , 16.f ));
			my_terrain_entity->update_matrix();
			my_world->add_entity(my_terrain_entity);

			//============================================================================================
			//								:
			//============================================================================================
			gv_effect * terrain_base_effect=NULL;
			{
				//create terrain
				gv_effect * pterrain_effect=NULL;
				my_terrain->enable_export_normal_map(true);
				my_terrain->enable_island_generation(gv_vector2i(64,64),0.4f ,0.8f);
				my_terrain->init_from_image_file(*height_map_name);
				my_terrain->enable_dynamic_LOD(false);
				gv_material * terrain_material=get_sandbox()->create_object<gv_material>(my_terrain_entity);
				gv_effect_project * effect= m_sandbox->create_object<gv_effect_project> (my_world);
				gv_string_tmp  file_name= FILE_RFX_TERRAIN;
				m_sandbox->import_external_format( effect,*file_name);
				terrain_base_effect=effect->get_base_effect();
				pterrain_effect=effect->get_base_effect()->get_renderable_effect(gv_id("bump_2_layer"));
				terrain_material->m_effect=pterrain_effect;
				my_terrain->set_material(terrain_material);
				gv_vector4 scale(1.f,1,1,1);
				pterrain_effect->set_param(gv_id("g_TexCoordScale"),scale);
			}

			//============================================================================================
			//								:
			//============================================================================================
			//grass
			{
				gv_material * grass_material=get_sandbox()->create_object<gv_material>(my_terrain_entity);
				gv_effect * grass_effect=terrain_base_effect->get_renderable_effect(gv_id("grass_normal"));
				grass_material->m_effect=grass_effect	;
				my_terrain->add_grass_layer(grass_material,50000,1.0f, gv_vector2(0, 1500),0.4f,0.1f);
			}

			//============================================================================================
			//								:
			//============================================================================================
			{//flower
				gv_material * grass_material=get_sandbox()->create_object<gv_material>(my_terrain_entity);
				gv_effect * grass_effect=terrain_base_effect->get_renderable_effect(gv_id("flower_normal"));
				grass_material->m_effect=grass_effect	;
				gv_float width=0.8f;
				grass_effect->set_param(gv_id("g_FlowerWidth"),width);
				my_terrain->add_grass_layer(grass_material,35500,1.0f, gv_vector2(0, 1500),0.4f,0.0f);
			}
		
			//============================================================================================
			//								:
			//============================================================================================
		}
	}
	void init_main_actor()
	{
		main_actor=m_sandbox->create_object<gv_entity>();
		gv_com_skeletal_mesh*	com_sk_mesh	=get_sandbox()->create_object<gv_com_skeletal_mesh>();
		gv_ani_set *			my_animation		=NULL; 
		gv_skeletal_mesh *		my_skeletal_mesh	=NULL;
		gv_static_mesh	 *		my_static_mesh		=NULL;
		gv_effect *				my_effect=NULL;
		gv_material			*	my_material=get_sandbox()->create_object<gv_material> ();
		{//load model & animation
			m_sandbox->load_module(gv_id("full_girl") );
			gvt_object_iterator<gv_ani_set> it_ani(m_sandbox); 
			if (it_ani.is_empty())
			{
				return;
			};
			my_animation=it_ani;		
			gvt_object_iterator<gv_skeletal_mesh> it_sk(m_sandbox); 
			if (!it_sk.is_empty() )	
			{
				my_skeletal_mesh=it_sk;
				my_static_mesh=my_skeletal_mesh->m_t_pose_mesh;
			}
			my_static_mesh->rebuild_normal();
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
			main_actor->add_component(com_sk_mesh); 
			gv_com_animation * com_ani=m_sandbox->create_object<gv_com_animation> ( );
			com_ani->set_resource(my_animation); 
			main_actor->add_component(com_ani);
			com_ani->play_animation(gv_id("Walk"),0.f,true);
		}
		main_actor->add_component(gv_id("gv_com_rpg2d_actor"));
		
		my_world->add_entity(main_actor);
		my_world->set_main_actor(main_actor);
		main_actor->set_owner(my_terrain_entity);
		//test attachment
		gv_entity * entity_attach =m_sandbox->create_object<gv_entity>();
		entity_attach->set_local_aabb(gv_box(gv_vector3 ( -0.1f,-0.1f,0) , gv_vector3 ( 0.1f,0.1f,2)));
		entity_attach->add_component(gv_com_attach::static_class());
		gv_com_attach * pattach=entity_attach->get_component<gv_com_attach>(); 
		pattach->set_attached_entity(main_actor); 
		pattach->set_attached_bone_id(gv_id("_bone_ArmR"));
		my_world->add_entity(entity_attach ); 
		gvt_random<gv_float> m_random;
		while(1)
		{
			gv_vector3 pos;
			pos.x =m_random.get_uniform()*1000.f;	pos.y =0; 
			pos.z =m_random.get_uniform()*1000.f;
			if (my_world->teleport_entity( main_actor , pos ,gv_euler::get_zero_rotation() )  )  
				break;
		};
		
		//---------------------------------------
		if (is_offline)
		{
			for ( int i=1; i< max_actor_nb; i++)
			{
				gv_entity * new_entity=main_actor->clone(); 
				my_world->add_entity(new_entity);
				new_entity->set_owner(my_terrain_entity);
				gv_com_rpg2d_actor * pactor=new_entity->get_component<gv_com_rpg2d_actor>( );
				pactor->m_auto_move=true;
				pactor->m_auto_move_speed=5.f;
				pactor->m_adjust_angle=-gv_float_trait::pi()/2.0f;;
				while(1)
				{
					gv_vector3 pos;
					pos.x =m_random.get_uniform()*1000.f;		pos.y =0; 
					pos.z =m_random.get_uniform()*1000.f;
					if (my_world->teleport_entity( new_entity , pos ,gv_euler::get_zero_rotation() )  )  
					{
						break;
					}
				};
			}
		}
		else
		{
			for ( int i=1; i< max_relevant_actor_nb; i++)
			{
				gv_entity * new_entity=main_actor->clone(); 
				my_world->add_entity(new_entity);
				new_entity->set_owner(my_terrain_entity);
				gv_com_rpg2d_actor * pactor=new_entity->get_component<gv_com_rpg2d_actor>( );
			}

		}

		{//add controller
			
			main_actor->add_component(gv_id("gv_com_ai_player_controller_rpg") ); 
			//my_entity->add_component(gv_id("gv_com_3d_lobby_controller") ); 
			//my_entity->add_component(gv_id("gv_com_cam_fps_fly") ); 
		}

		{//set camera;
			gv_com_camera  * camera=m_sandbox->create_object<gv_com_camera> ();
			camera->set_fov(60.f,1.333f,0.2f, 5000 );
			camera->set_look_at(gv_vector3 ( -4, 2.0f,0) , gv_vector3(0,0.8f,0) );
			camera->update_projection_view_matrix();
			main_actor->add_component(camera); 
			GVM_POST_EVENT(render_set_camera,render, (pe->camera=camera) );
		}
	}

	void do_client_3d()
	{
		using namespace std;
		cout<<"replica client...."<<endl;
		cout<<"done , start to work...."<<endl;
		gvt_ref_ptr<gv_com_net_host > host_local;
		gvt_ref_ptr<gv_com_net_host > remote_host;
		gvt_ref_ptr<gv_com_net_replica> com_replica=m_sandbox->create_object<gv_com_net_replica>(); 
		//-----------------------------------------------------------------------------------------
		if (!is_offline)
		{//init network component
			gv_entity *  my_entity=m_sandbox->create_object<gv_entity>();
			com_replica->append_replica_array(gv_net_replica_net_unit_2d::static_class() , max_relevant_actor_nb );
			host_local=m_sandbox->create_object<gv_com_net_host>();
			host_local->init_host(gv_socket_address( 0),true,0.1f);
			my_entity->add_component(host_local);
			remote_host=m_sandbox->create_object<gv_com_net_host>(); 
			remote_host->init_host(gv_socket_address( server_ip , 1234), false,1.0f,0); 
			com_replica->set_in_coming_host(remote_host);
			my_entity->add_component(remote_host);
			my_entity->add_component(com_replica);
		}

		//init scene
		init_scene();
		init_main_actor();
		//===============================================================================
		while (loop--)
		{
			//gv_thread::sleep(10);
			if (!is_offline) 
			{
				update_replica_to_world(com_replica);
				gv_com_camera * camera = main_actor->get_component<gv_com_camera>();
				if (!camera) return; 
				camera->sync_to_entity(main_actor);
				if (remote_host &&remote_host->get_host_state()==gve_host_disconnected)		
				{
					loop=gvt_min(loop,2);
				}
			}
			if (!m_sandbox->tick() ) break;
			{
				{
					static bool last_key_down=false;
					if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
					{
						my_terrain->enable_wireframe_mode(!my_terrain->is_wireframe_mode()	);
					}
					last_key_down=m_sandbox->get_input_manager()->is_key_down(e_key_space);
				}
				{
					static bool last_key_down=false;
					if (m_sandbox->get_input_manager()->is_key_down(e_key_f) && !last_key_down)
					{
						//use_fly_mode=!use_fly_mode;
					}
					last_key_down=m_sandbox->get_input_manager()->is_key_down(e_key_f);
				}
				static float threshold=1000;
				if (m_sandbox->get_input_manager()->is_key_down(e_key_f1) )
				{
					threshold-=1; 
					threshold=gvt_max(threshold, 0.1f);
				}
				if (m_sandbox->get_input_manager()->is_key_down(e_key_f2) )
				{
					threshold+=1; 
					threshold=gvt_min(threshold, 100000.f);
				}
				gv_string_tmp s; 
				s<< "nb visible patch: "<<my_terrain->get_nb_visible_patch();
				s<< "--------nb trangles : "<<my_terrain->get_nb_tri() << "----current threshold :"<<threshold <<"use f1 f2 to change threthod, use f change fly mode";
				get_debug_draw()->draw_string(*s ,gv_vector2i( 60, 20), gv_color::GREEN_D() );
				my_terrain->set_lod_threshold(threshold);
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
		is_offline=args.find("offline");
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