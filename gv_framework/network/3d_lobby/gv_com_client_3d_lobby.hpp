//======================================================================
class gv_com_client_3d_lobby : public gv_component_renderer
{
	gv_com_client_3d_lobby()
	{
		m_host_id = m_peer_id = 0;
		m_connected = false;
		m_quit = false;
	}
	~gv_com_client_3d_lobby()
	{
		destroy();
	}

	gvt_ptr< gv_world_rpg2d > m_map;
	gv_uint m_max_frame;
	gv_uint m_tick_count;
	actor* m_main_actor;
	bool m_is_drag;
	bool m_pause;
	bool m_is_server;
	gv_command_console m_console;

	gvt_ptr< gv_log > m_log;
	gvt_ptr< gvi_server_session_layer > m_server;
	gv_socket_address m_server_address;
	gv_int m_host_id;
	gv_int m_peer_id;
	bool m_connected;
	bool m_quit;
	gvt_array_cached< gv_3dlobby_proxy, 2048 > m_proxy_cache;

	gv_3dlobby_proxy* find_proxy(gv_ushort index, bool create = false)
	{
		GV_PROFILE_EVENT(find_proxy, 0);
		if (m_proxy_cache.size() <= index)
			m_proxy_cache.resize(index + 1);
		if (!m_proxy_cache[index].pactor)
		{
			m_proxy_cache[index].pactor = m_map.spawn_actor();
		}
		return &m_proxy_cache[index];
	}

	void init_client()
	{
		m_server = new gv_reliable_udp_server;
		gv_string_tmp name = "bricks_2d_client";
		name << gv_global::time->get_performance_counter_from_start();
		m_log = gv_global::log->create_log(*name);
		// m_server->enable_packet_dump(true, m_log);
		m_server->create(0);
		m_server_address.set(gv_ip_address_ipv4::addr_local_host(), 1234);
		if (gv_global::command_line_options.size() > 1)
		{
			gv_string_tmp s = gv_global::command_line_options[1];
			m_server_address.from_string(s);
		}
		m_proxy_cache.resize(2048);
		m_server->post_host_create(1, 0, 0);
		m_server_send_rate = 30.f;
	}

	bool client_execute(gv_packet* packet)
	{
		GV_PROFILE_EVENT(client_execute, 0);
		if (!packet->get_packet_content_size())
			return false;
		gv_byte command;
		(*packet) >> command;
		while (command != e_command_end &&
			   packet->tell() < packet->get_packet_content_size())
		{
			switch (command)
			{
			case e_client_adjust_proxy_pos:
			{
				gv_double time_stamp;
				gv_ushort nb_actors;
				(*packet) >> time_stamp;
				(*packet) >> nb_actors;
				for (int i = 0; i < nb_actors; i++)
				{
					gvt_vector2< gv_ushort > vs;
					gv_ushort proxy_index;
					(*packet) >> proxy_index;
					(*packet) >> vs.x;
					(*packet) >> vs.y;
					gv_3dlobby_proxy* pproxy = find_proxy(proxy_index, true);
					gv_vector2 pos = m_map->uncompress_pos(vs);
					m_map->teleport_actor(pproxy->pactor, pos, false);
				}
			}
			break;

			case e_client_adjust_player_pos:
			{
				gv_ushort index;
				(*packet) >> index;
				m_main_actor = find_proxy(index, true)->pactor;
				(*packet) >> m_main_actor->m_color.fixed32;
				(*packet) >> m_main_actor->m_in_love_value;
				(*packet) >> m_main_actor->m_is_hit;
			}
			break;

			case e_client_created:
			{
				gv_ushort m_index;
				(*packet) >> m_index;
				gv_3dlobby_proxy* pp = find_proxy(m_index, true);
				m_main_actor = pp->pactor;
			}
			} // switch;
			(*packet) >> command;
		} // cmd
		GV_ASSERT(command == e_command_end);
		return true;
	}

	void update_client(gv_float dt)
	{
		gvt_ref_ptr< gv_session_event > event;
		m_server->do_service();
		while (m_server->peek_event(event))
		{
			if (event->event_type == gve_session_event_host_create)
			{
				m_host_id = event->host_id;
				m_server->post_connect(m_server_address, m_host_id, 0, 4);
			}
			if (event->event_type == gve_session_event_connect)
			{
				m_peer_id = event->peer_id;
			}
			if (event->event_type == gve_session_event_connect_succeed)
			{
				// we can send things ;
				gv_string_tmp output;
				output << "connect successes to " << event->address.string()
					   << event->host_id << "@" << event->peer_id << "!!! \r\n";
				std::cout << *output;
				*m_log << output;
				m_connected = true;
			}
			if (event->event_type == gve_session_event_recieve)
			{
				gv_packet* p = event->packet;
				client_execute(p);
			}
			if (event->event_type == gve_session_event_disconnected)
			{
				gv_string_tmp output;
				output << "disconnected from server " << event->host_id << "@"
					   << event->peer_id << event->address.string() << " !!! \r\n";
				std::cout << *output;
				*m_log << output;
				m_quit = true;
				break;
			}
			event = NULL;
		}
		if (m_connected)
		{
			// TODO player prediction
			if (m_main_actor)
			{
				m_main_actor->m_speed.x = 0;
				m_main_actor->m_speed.y = 0;
			};
			if (m_main_actor)
			{
				gv_packet* packet =
					m_server->create_packet(gve_packet_type_send_reliable);
				gv_byte command = (gv_byte)e_server_move;
				(*packet) << command;
				(*packet) << m_main_actor->m_speed.x;
				(*packet) << m_main_actor->m_speed.y;
				command = (gv_byte)e_command_end;
				(*packet) << command;
				m_server->post_send_reliable(m_host_id, m_peer_id, 0, packet);
			}
		}
		if (m_quit)
		{
			m_tick_count = m_max_frame;
		}
		if (gv_global::input->is_key_down(e_key_return))
		{
			m_server->post_disconnect(m_host_id, m_peer_id);
		}
	}

	virtual void update(gv_float dt)
	{
		update_client(dt);
	}
	virtual void render()
	{
		GV_PROFILE_EVENT(render_all, 0);
		if (m_is_server)
			server_render();
		else
			client_render();
		m_tick_count++;
	};
	virtual bool is_finished()
	{
		if (m_tick_count < m_max_frame)
			return false;
		return true;
	}
	virtual void destroy()
	{
		m_console.stop();
		m_map.destroy();
		gvt_safe_delete(m_log);
		gvt_safe_delete(m_server);
	}
}
