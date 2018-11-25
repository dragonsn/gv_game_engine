//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
namespace gv
{
	extern gv_atomic_count packet_number;
}
namespace game_2d_bricks_online
{
#include	"2d_bricks_maps.h"
#include	"2d_bricks_online.h"
	//=================================================
	//======================================================================
	class gv_game_2d_bricks_online:public gv_unit_test_with_renderer
	{
	public:
		virtual gv_string  name()
		{ 
			return "2d_bricks_online"; 
		}
		gv_game_2d_bricks_online()
		{
			m_server=NULL; m_log=NULL;
			m_host_id=m_peer_id=0 ;
			m_connected=false;
			m_server_send_rate=20.f;
			m_quit=false;
		}
		map			m_map;
		gv_uint		m_max_frame;
		gv_double	m_last_tick_time;
		gv_double	m_delta_time;
		gv_uint		m_tick_count;
		actor*		m_main_actor;
		bool		m_is_drag;
		gv_double   m_start_drag_time;
		gv_vector2i	m_start_drag_pos;
		bool		m_pause;
		bool		m_is_server;
		//=========================================
		// network related 
		gv_log	 *					m_log; 
		gvi_server_session_layer  * m_server;
		gv_socket_address			m_server_address;
		gv_int						m_host_id; 
		gv_int						m_peer_id;
		bool						m_connected;
		gv_float					m_server_send_rate;
		bool						m_quit;
		class	connection 
		{
		public:
			connection () 
			{
				m_send_times=m_peer_id;
			}
			connection (gv_uint peer_id) 
			{
				m_peer_id=peer_id;
			}
			gv_uint		m_peer_id;
			gv_uint		m_send_times;
			actor *		m_actor;
			gv_socket_address	m_address;
			bool operator ==(const connection & conn)
			{
				return m_peer_id==conn.m_peer_id;
			}
		};

		class proxy
		{
		public:
			proxy()	{	pactor=NULL;}
			actor *		pactor;
			gv_double	time_stamp;
		};

		enum brick_2d_command
		{
			e_client_adjust_proxy_pos,  //send by server
			e_client_adjust_player_pos,
			e_client_created,
			e_client_disconnect,
			e_server_move,				//send by client
			e_server_disconnect,
			e_command_end,
		};

		gvt_array<connection>			m_connections;
		gvt_array_cached<proxy, 2048>	m_proxy_cache;

		connection * find_connection(gvt_ref_ptr< gv_session_event >  & event)
		{
			GV_PROFILE_EVENT(find_connection.connection,0);
			gv_int idx; 
			if (m_connections.find(event->peer_id,idx) )
			{
				return &m_connections[idx];
			}
			return NULL;
		}

		connection * create_connection(gvt_ref_ptr< gv_session_event >  & event)
		{
			GV_PROFILE_EVENT(create_connection,0);
			connection * p=m_connections.add(event->peer_id);
			map::actor_list::iterator it=m_map.m_actor_list.begin(); 
			while(it !=m_map.m_actor_list.end() )
			{
				gvt_ref_ptr<actor> a=(*it);
				if (a->m_log_in==false )	{break;	}
				++it; 
			}
			if (it== m_map.m_actor_list.end() ) GV_ASSERT(0 && "max client linked!!!");
			p->m_actor=*it;
			p->m_actor->m_log_in=true;
			//send the connect update 
			gv_byte command=(gv_byte)e_client_created; 
			gv_packet * packet =m_server->create_packet(gve_packet_type_send_reliable);
			(*packet)<<command ; 
			(*packet)<<p->m_actor->m_index; 
			command=(gv_byte)e_command_end; 
			(*packet)<<command;
			m_server->post_send_reliable(m_host_id, p->m_peer_id, 0, packet);
			return p;
		}
		void remove_connection(gvt_ref_ptr< gv_session_event >  & event)
		{
			GV_PROFILE_EVENT(remove_connection,0);
			connection * pc=find_connection(event); 
			if (!pc) return;
			pc->m_actor->m_log_in=false; 
			m_connections.erase_item(event->peer_id);
			gv_string_tmp output;
			output<<"disconnected a client from"<< event->host_id<< "@"<< event->peer_id << event->address.string()<<" !!! \r\n";
			std::cout<<*output;		*m_log<<output;
		}
		void	server_update_all_connections(gv_float dt)
		{
			GV_PROFILE_EVENT(server_update_all_connections , 0 );
			for ( int i=0; i< this->m_connections.size(); i++)
			{
				//find relevant actors , and replicate the position to the client
				actor * pactor=this->m_connections[i].m_actor; 
				//connection & the_connection=this->m_connections[i];
				if (!pactor) continue; 
				gv_rect rect = pactor->m_aabb; 
				rect.extend(400, 300 ); 
				actor_cache_1024 actors; 
				m_map.collect_actors (rect, actors);
				//TODO sort the relevant list , check the priority !!!
				gv_byte command =(gv_byte)e_client_adjust_proxy_pos;
				gv_ushort nb_actors=(gv_ushort) gvt_min(actors.size() , 200);
				bool need_reliable=this->m_connections[i].m_send_times %20==0; 
				gv_packet  * packet;
				if	(need_reliable) packet=m_server->create_packet(gve_packet_type_send_reliable);
				else packet=m_server->create_packet(gve_packet_type_send_unreliable);
				(*packet)<<command;
				gv_double secs=gv_global::time->get_sec_from_start();
				(*packet)<<secs;
				(*packet)<<nb_actors; 
				for (int j=0; j< nb_actors; j++ )
				{
					actor * pactor=actors[j];
					gvt_vector2<gv_ushort> vs=m_map.compress_pos(pactor->m_aabb.center());
					(*packet)<<pactor->m_index; 
					(*packet)<<vs.x; 	(*packet)<<vs.y; 
				}
				//update player iNfo
				command =(gv_byte)e_client_adjust_player_pos;
				(*packet)<<command;
				(*packet)<<pactor->m_index;
				(*packet)<<pactor->m_color.fixed32;
				(*packet)<<pactor->m_in_love_value; 
				(*packet)<<pactor->m_is_hit; 
				//update  			
				command=(gv_byte)e_command_end; 
				(*packet)<<command;
				GV_ASSERT(packet->get_network_diagram_size()<packet->get_default_mtu()) ; 
				this->m_connections[i].m_send_times++; 
				if	(need_reliable)		m_server->post_send_reliable(m_host_id,this->m_connections[i].m_peer_id,0,packet );
				else					m_server->post_send_unreliable(m_host_id,this->m_connections[i].m_peer_id,0,packet );
			}
		}
		void	init_server()
		{
			m_server =new gv_reliable_udp_server;
			gv_string_tmp name="bricks_2d_server";
			m_log =gv_global::log->create_log(*name);
			//m_server->enable_packet_dump(true, m_log);
			m_server->create(1234);
			m_server->post_host_create(2048,0,0);
		}
		void	init_client()
		{
			m_server =new gv_reliable_udp_server;
			gv_string_tmp name="bricks_2d_client";
			name<<gv_global::time->get_performance_counter_from_start();
			m_log=gv_global::log->create_log(*name);
			//m_server->enable_packet_dump(true, m_log);
			m_server->create(0,1,false);
			m_server_address.set(gv_ip_address_ipv4::addr_local_host(),1234); 
			if (gv_global::command_line_options.size()>1) 
			{
				gv_string_tmp s=gv_global::command_line_options[1];
				m_server_address.from_string(s);
			}
			m_proxy_cache.resize(2047);
			m_server->post_host_create(1,0,0);
			m_server_send_rate=30.f;
		}
		void	server_execute(connection * connection ,gv_packet * packet)
		{
			GV_PROFILE_EVENT(server_execute,0);
			if (!packet->get_packet_content_size() ) return; 
			gv_byte command; 
			(*packet)>>command; 
			while (command!=e_command_end && packet->tell()< packet->get_packet_content_size() )
			{	switch(command)
				{
				case e_server_move:
					{
						gv_vector2 v;
						(*packet)>>v.x;
						(*packet)>>v.y;
						connection->m_actor->m_speed+=v;
					}
					break;

				case e_server_disconnect:
					{
						m_server->post_disconnect(m_host_id, connection->m_peer_id);
					}
					break;
				default:
					GV_ASSERT(0);
				}//switch
				(*packet)>>command;
			}//next command
		}
		proxy *		find_proxy( gv_ushort  index , bool create=false)
		{
			GV_PROFILE_EVENT(find_proxy,0);
			if (m_proxy_cache.size()<=index) m_proxy_cache.resize(index+1);
			if (!m_proxy_cache[index].pactor)
			{
				m_proxy_cache[index].pactor=m_map.spawn_actor();
			}
			return &m_proxy_cache[index];
		}
		bool	client_execute(gv_packet * packet)
		{
			GV_PROFILE_EVENT(client_execute,0);
			if (!packet->get_packet_content_size() ) return false; 
			gv_byte command; 
			(*packet)>>command; 
			while (command!=e_command_end && packet->tell()< packet->get_packet_content_size() )
			{	
				switch(command)
				{
				case e_client_adjust_proxy_pos:
					{	
						gv_double time_stamp;
						gv_ushort nb_actors;
						(*packet)>>time_stamp; 
						(*packet)>>nb_actors ;
						for (int i=0; i< nb_actors; i++ )
						{
							gvt_vector2<gv_ushort> vs;
							gv_ushort proxy_index;
							(*packet)>>proxy_index; 
							(*packet)>>vs.x; 	
							(*packet)>>vs.y; 
							proxy * pproxy=find_proxy(proxy_index , true);
							gv_vector2 pos=m_map.uncompress_pos(vs);
							m_map.teleport_actor(pproxy->pactor,pos,false);
						}	
					}
					break;

				case	e_client_adjust_player_pos:
					{
						gv_ushort index; 
						(*packet)>>index; 
						m_main_actor=find_proxy( index, true)->pactor;
						(*packet)>>m_main_actor->m_color.fixed32;
						(*packet)>>m_main_actor->m_in_love_value; 
						(*packet)>>m_main_actor->m_is_hit; 
					}
					break;

				case	e_client_created:
					{
						gv_ushort m_index;
						(*packet)>>m_index;
						proxy * pp=find_proxy(m_index,true);
						m_main_actor=pp->pactor;
					}
				}//switch;
				(*packet)>>command;
			}//cmd
			GV_ASSERT(command==e_command_end);
			return true;
		}

		void	update_server(gv_float dt) 
		{
			GV_PROFILE_EVENT(update_server , 0 );
			//============================================
			if (!m_server) return ;
			m_server->do_service();
			gvt_ref_ptr< gv_session_event > event;
			while(m_server->peek_event(event) )
			{
				if (event->event_type==gve_session_event_host_create)
				{
					m_host_id=event->host_id; 
				}
				if (event->event_type==gve_session_event_connect_succeed)
				{
					//we can send things ;
					gv_string_tmp output;
					output<<"connect successes from "<< event->address.string() << event->host_id<< "@"<< event->peer_id<<"!!! \r\n";
					std::cout<<*output;		*m_log<<output;
					create_connection(event);
				}
				if (event->event_type== gve_session_event_recieve )
				{
					gv_packet * p=event->packet; 
					connection * pc=find_connection(event);
					server_execute(pc,p); 
				}
				if (event->event_type==gve_session_event_disconnected)
				{
					remove_connection(event);
					break;
				}
				if (m_connected)
				{
					gv_string_tmp ss;
					m_server->dump_peer(event->host_id,event->peer_id,ss);
					(*m_log)<<ss; 
				};
				event=NULL;
			}//next event
			//============================================
			m_map.update(dt);
			//============================================
			server_update_all_connections(dt);
			//============================================
			if (dt< 1.0f/m_server_send_rate+0.01f)   gv_thread::sleep((long)gvt_clamp((1.0f/m_server_send_rate-dt )*1000.f, 0.f, 100.f) );
			//============================================
			if (gv_global::input->is_key_down(e_key_return))
			{
				m_tick_count=m_max_frame;
			}
			//============================================
			static bool bool_is_space_down=false;
			if (gv_global::input->is_key_down(e_key_space) && !bool_is_space_down)
			{
				static int last_track_actor=0; 
				if (++last_track_actor>= m_connections.size()) last_track_actor=0; 
				if ( m_connections.size() )
				{
					m_main_actor=m_connections[last_track_actor].m_actor;
				}
				//track next actor
			}
			bool_is_space_down=gv_global::input->is_key_down(e_key_space);
			//============================================
		}
		void	update_client(gv_float dt)
		{
			gvt_ref_ptr< gv_session_event > event;
			m_server->do_service() ;
			while(m_server->peek_event(event) )
			{
				if (event->event_type==gve_session_event_host_create)
				{
					m_host_id=event->host_id; 
					m_server->post_connect(m_server_address,m_host_id,0,4 );
				}
				if (event->event_type==gve_session_event_connect)
				{
					m_peer_id=event->peer_id;
				}
				if (event->event_type==gve_session_event_connect_succeed)
				{
					//we can send things ;
					gv_string_tmp output;
					output<<"connect successes to "<< event->address.string() << event->host_id<< "@"<< event->peer_id<<"!!! \r\n";
					std::cout<<*output;	*m_log<<output;
					m_connected=true;
				}
				if (event->event_type== gve_session_event_recieve )
				{
					gv_packet * p=event->packet; 
					client_execute(p);
				}
				if (event->event_type==gve_session_event_disconnected)
				{
					gv_string_tmp output;
					output<<"disconnected from server "<< event->host_id<< "@"<< event->peer_id << event->address.string()<<" !!! \r\n";
					std::cout<<*output;	*m_log<<output;
					m_quit=true;
					break;
				}
				event=NULL;
			}
			if (m_connected)
			{
				//TODO player prediction
				if (m_main_actor) 
				{
					m_main_actor->m_speed.x=0;
					m_main_actor->m_speed.y=0;
				};
				update_play_ball_view_mode(dt);
				if(m_main_actor )
				{
					gv_packet  * packet=m_server->create_packet(gve_packet_type_send_reliable);
					gv_byte command =(gv_byte)e_server_move;
					(*packet)<<command;
					(*packet)<<m_main_actor->m_speed.x;
					(*packet)<<m_main_actor->m_speed.y; 
					command=(gv_byte)e_command_end; 
					(*packet)<<command;
					m_server->post_send_reliable(m_host_id,m_peer_id,0,packet);
				}
			}
			if (dt< 1.0f/m_server_send_rate+0.01f)   gv_thread::sleep((long)gvt_clamp((1.0f/m_server_send_rate-dt )*1000.f, 0.f, 100.f) );
			if (m_quit)
			{
				m_tick_count=m_max_frame;
			}
			if (gv_global::input->is_key_down(e_key_return))
			{
				m_server->post_disconnect(m_host_id, m_peer_id);
			}
		}
		void	server_render()
		{
			GV_PROFILE_EVENT(server_render,0 );
			int cell_drawed=0;
			gv_string_tmp s="[BRICK2D SERVER]>>";
			s<< "fps: "<<1.0/m_delta_time;
			s<< "memory:"<<gvp_memory_base::s_info.alloc_size.get();
			
			s<< "packet:"<<packet_number.get();
			if (!m_main_actor)	
			{
				if(m_connections.size() )	m_main_actor=m_connections[0].m_actor;
				else						m_main_actor=*m_map.m_actor_list.begin(); 
			}
			if (m_main_actor)
			{
				m_map.m_camera_pos=m_main_actor->m_location; 
				m_map.m_camera_zoom=2;
				gv_string_tmp title="GAME SCORE:";
				title<<m_main_actor->m_score;
				gv_global::debug_draw.get()->draw_string(*title,gv_vector2i(60,120),gv_color::RED());
			}
			cell_drawed=m_map.render();
			s<<" cell in view "<<cell_drawed;
			gv_global::debug_draw.get()->draw_string(*s,gv_vector2i(60,60),gv_color::RED());
		}

		void	client_render()
		{
			GV_PROFILE_EVENT(client_render,0 );
			int cell_drawed=0;
			gv_string_tmp s="[BRICK2D CLIENT]>>";
			s<< "fps: "<<1.0/m_delta_time;
			if (!m_main_actor && m_map.m_actor_list.begin() ) m_main_actor=*m_map.m_actor_list.begin(); 
			if (m_main_actor)
			{
				m_map.m_camera_pos=m_main_actor->m_location; 
				m_map.m_camera_zoom=2;
				gv_string_tmp title="GAME SCORE:";
				title<<m_main_actor->m_score;
				gv_global::debug_draw.get()->draw_string(*title,gv_vector2i(60,120),gv_color::RED());
			}
			cell_drawed=m_map.render();
			s<<" cell in view "<<cell_drawed;
			gv_global::debug_draw.get()->draw_string(*s,gv_vector2i(60,60),gv_color::RED());
		}
		//===========================================================
		void	initialize()		
		{
			m_is_server=false;
			if(gv_global::command_line_options.size() )
			{
				if (gv_global::command_line_options[0]=="server")
				{	
					m_is_server=true;
					this->init_server();
					m_max_frame=10000000;
				}
				else if (gv_global::command_line_options[0]=="client")
				{
					m_is_server=false;
					this->init_client();
					m_max_frame=1000000;
				}
			}
			else m_max_frame=1000;
			m_map.init(the_map,map_width,map_height);
			if (m_is_server)	m_map.spawn_actors(2000);
			m_main_actor=NULL;
			m_is_drag=false;
			m_tick_count=0;
			m_pause=false;
		};
		bool	update_map_view_mode( float dt)
		{
			gv_string_tmp title="MAP VIEW MODE ====>>";
			gv_global::debug_draw.get()->draw_string(*title,gv_vector2i(60,120),gv_color::RED());
			float step=200.0f*dt;
			if (gv_global::input->is_key_down(e_key_up) )
			{
				m_map.m_camera_pos.y-=step;
			}
			if (gv_global::input->is_key_down(e_key_down) )
			{
				m_map.m_camera_pos.y+=step;
			}
			if (gv_global::input->is_key_down(e_key_left))
			{
				m_map.m_camera_pos.x-=step;
			}
			if (gv_global::input->is_key_down(e_key_right))
			{
				m_map.m_camera_pos.x+=step;
			}
			return true;
		}

		bool  update_play_ball_view_mode( float dt)
		{
			bool down=	gv_global::input->is_key_down(e_key_lbutton);
			if(m_is_drag && !down)
			{
				gv_global::debug_draw->release_mouse();
				gv_double time=gv_global::time->get_sec_from_start();
				time-=m_start_drag_time;
				gv_vector2i cu_mouse_pos;
				gv_global::input->get_mouse_pos(cu_mouse_pos);
				cu_mouse_pos-=m_start_drag_pos; 
				m_main_actor->m_speed.x+=(gv_float)(cu_mouse_pos.x*0.5f)/(gv_float)time;
				m_main_actor->m_speed.y+=(gv_float)(cu_mouse_pos.y*0.5f)/(gv_float)time;
				m_main_actor->m_speed.x=gvt_clamp(m_main_actor->m_speed.x, -100.f, 100.f);
				m_main_actor->m_speed.y=gvt_clamp(m_main_actor->m_speed.y, -100.f, 100.f);
			}
			if (down && !m_is_drag )
			{
				gv_global::debug_draw->capture_mouse();
				m_start_drag_time=gv_global::time->get_sec_from_start();
				gv_global::input->get_mouse_pos(m_start_drag_pos);
			}
			m_is_drag=down;
			return true;
		}
		virtual	void update( gv_float dt)
		{
			m_delta_time=dt;
			if (!m_pause)
			{//UPDATE
				GV_PROFILE_EVENT(m_map_update,0 );
				if (m_is_server )	update_server(dt); 
				else				update_client(dt);
			}
		}
		virtual void render()	
		{
			GV_PROFILE_EVENT(render_all,0 );
			if (m_is_server)
				server_render();
			else 
				client_render();
			m_tick_count++;
		};
		virtual bool is_finished  ()	
		{ 
			if (m_tick_count<m_max_frame) return false;
			return true; 
		}
		virtual void destroy()
		{
			m_map.destroy();
			m_connections.clear();
			gvt_safe_delete(m_log);
			gvt_safe_delete(m_server);
		}
	}test;
	gv_unit_test_with_renderer* ptest=&test;
};
