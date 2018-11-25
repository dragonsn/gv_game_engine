namespace gv
{
extern gv_atomic_count packet_number;

enum command_3d_lobby
{
	e_client_adjust_proxy_pos, // send by server
	e_client_adjust_player_pos,
	e_client_created,
	e_client_disconnect,

	e_server_move, // send by client
	e_server_disconnect,
	e_command_end,
};

class gv_3dlobby_connection
{
public:
	gv_3dlobby_connection()
	{
		m_send_times = m_peer_id;
	}
	gv_3dlobby_connection(gv_uint peer_id)
	{
		m_peer_id = peer_id;
	}
	gv_uint m_peer_id;
	gv_uint m_send_times;
	actor* m_actor;
	gv_socket_address m_address;
	bool operator==(const gv_3dlobby_connection& conn)
	{
		return m_peer_id == conn.m_peer_id;
	}
};

class gv_com_3dlobby_proxy : public gv_component
{
public:
	struct proxy_track_point
	{
		gv_float m_time;
		gv_vector2 m_pos;
	}

	GVM_DCL_CLASS(gv_com_3dlobby_proxy, gv_component);
	gv_com_3dlobby_proxy()
	{
		pactor = NULL;
	}
	~gv_com_3dlobby_proxy()
	{
	}
	gvt_ptr< gv_com_rpg2d_actor > pactor;
	gvt_ring_buf< proxy_track_point > m_history;
	gv_double time_stamp;
};

//======================================================================
class gv_com_server_3d_lobby : public gv_component_renderer
{
public:
	gv_com_server_3d_lobby()
	{
		m_server = NULL;
		m_log = NULL;
		m_host_id = m_peer_id = 0;
		m_connected = false;
		m_server_send_rate = 20.f;
		m_quit = false;
	}

	gv_world_rpg2d* m_map;
	gv_uint m_max_frame;
	gv_uint m_tick_count;
	actor* m_main_actor;
	bool m_is_drag;
	gv_double m_start_drag_time;
	gv_vector2i m_start_drag_pos;
	bool m_pause;
	bool m_is_server;
	gv_command_console m_console;
	//=========================================
	// network related
	gv_log* m_log;
	gvi_server_session_layer* m_server;
	gv_socket_address m_server_address;
	gv_int m_host_id;
	gv_int m_peer_id;
	bool m_connected;
	gv_float m_server_send_rate;
	bool m_quit;

	gvt_array< gv_3dlobby_connection > m_connections;

	gv_3dlobby_connection* find_connection(gvt_ref_ptr< gv_session_event >& event)
	{
		GV_PROFILE_EVENT(find_connection.gv_3dlobby_connection, 0);
		gv_int idx;
		if (m_connections.find(event->peer_id, idx))
		{
			return &m_connections[idx];
		}
		return NULL;
	}

	gv_3dlobby_connection*
	create_connection(gvt_ref_ptr< gv_session_event >& event)
	{
		GV_PROFILE_EVENT(create_connection, 0);
		gv_3dlobby_connection* p = m_connections.add(event->peer_id);
		map::actor_list::iterator it = m_map.m_actor_list.begin();
		while (it != m_map.m_actor_list.end())
		{
			gvt_ref_ptr< actor > a = (*it);
			if (a->m_log_in == false)
			{
				break;
			}
			++it;
		}
		if (it == m_map.m_actor_list.end())
			GV_ASSERT(0 && "max client linked!!!");
		p->m_actor = *it;
		p->m_actor->m_log_in = true;
		// send the connect update
		gv_byte command = (gv_byte)e_client_created;
		gv_packet* packet = m_server->create_packet(gve_packet_type_send_reliable);
		(*packet) << command;
		(*packet) << p->m_actor->m_index;
		command = (gv_byte)e_command_end;
		(*packet) << command;
		m_server->post_send_reliable(m_host_id, p->m_peer_id, 0, packet);
		return p;
	}

	void remove_connection(gvt_ref_ptr< gv_session_event >& event)
	{
		GV_PROFILE_EVENT(remove_connection, 0);
		gv_3dlobby_connection* pc = find_connection(event);
		if (!pc)
			return;
		pc->m_actor->m_log_in = false;
		m_connections.erase_item(event->peer_id);
		gv_string_tmp output;
		output << "disconnected a client from" << event->host_id << "@"
			   << event->peer_id << event->address.string() << " !!! \r\n";
		std::cout << *output;
		*m_log << output;
	}

	void server_update_all_connections(gv_float dt)
	{
		GV_PROFILE_EVENT(server_update_all_connections, 0);
		for (int i = 0; i < this->m_connections.size(); i++)
		{
			// find relevant actors , and replicate the position to the client
			actor* pactor = this->m_connections[i].m_actor;
			// gv_3dlobby_connection & the_connection=this->m_connections[i];
			if (!pactor)
				continue;
			gv_rect rect = pactor->m_aabb;
			rect.extend(400, 300);
			actor_cache_1024 actors;
			m_map.collect_actors(rect, actors);
			// TODO sort the relevant list , check the priority !!!
			gv_byte command = (gv_byte)e_client_adjust_proxy_pos;
			gv_ushort nb_actors = (gv_ushort)gvt_min(actors.size(), 200);
			bool need_reliable = this->m_connections[i].m_send_times % 20 == 0;
			gv_packet* packet;
			if (need_reliable)
				packet = m_server->create_packet(gve_packet_type_send_reliable);
			else
				packet = m_server->create_packet(gve_packet_type_send_unreliable);
			(*packet) << command;
			gv_double secs = gv_global::time->get_sec_from_start();
			(*packet) << secs;
			(*packet) << nb_actors;
			for (int j = 0; j < nb_actors; j++)
			{
				actor* pactor = actors[j];
				gvt_vector2< gv_ushort > vs = m_map.compress_pos(pactor->m_aabb.center());
				(*packet) << pactor->m_index;
				(*packet) << vs.x;
				(*packet) << vs.y;
			}
			// update player iNfo
			command = (gv_byte)e_client_adjust_player_pos;
			(*packet) << command;
			(*packet) << pactor->m_index;
			(*packet) << pactor->m_color.fixed32;
			(*packet) << pactor->m_in_love_value;
			(*packet) << pactor->m_is_hit;
			// update
			command = (gv_byte)e_command_end;
			(*packet) << command;
			GV_ASSERT(packet->get_network_diagram_size() < packet->get_default_mtu());
			this->m_connections[i].m_send_times++;
			if (need_reliable)
				m_server->post_send_reliable(
					m_host_id, this->m_connections[i].m_peer_id, 0, packet);
			else
				m_server->post_send_unreliable(
					m_host_id, this->m_connections[i].m_peer_id, 0, packet);
		}
	}

	void init_server()
	{
		m_server = new gv_reliable_udp_server;
		gv_string_tmp name = "bricks_2d_server";
		m_log = gv_global::log->create_log(*name);
		// m_server->enable_packet_dump(true, m_log);
		m_server->create(1234);
		m_server->post_host_create(2048, 0, 0);
		m_console.start();
	}

	void server_execute(gv_3dlobby_connection* connection, gv_packet* packet)
	{
		GV_PROFILE_EVENT(server_execute, 0);
		if (!packet->get_packet_content_size())
			return;
		gv_byte command;
		(*packet) >> command;
		while (command != e_command_end &&
			   packet->tell() < packet->get_packet_content_size())
		{
			switch (command)
			{
			case e_server_move:
			{
				gv_vector2 v;
				(*packet) >> v.x;
				(*packet) >> v.y;
				connection->m_actor->m_speed += v;
			}
			break;

			case e_server_disconnect:
			{
				m_server->post_disconnect(m_host_id, connection->m_peer_id);
			}
			break;
			default:
				GV_ASSERT(0);
			} // switch
			(*packet) >> command;
		} // next command
	}

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

	void update_server(gv_float dt)
	{
		GV_PROFILE_EVENT(update_server, 0);
		if (!m_server)
			return;
		m_server->do_service();
		gvt_ref_ptr< gv_session_event > event;
		while (m_server->peek_event(event))
		{
			if (event->event_type == gve_session_event_host_create)
			{
				m_host_id = event->host_id;
			}
			if (event->event_type == gve_session_event_connect_succeed)
			{
				// we can send things ;
				gv_string_tmp output;
				output << "connect successes from " << event->address.string()
					   << event->host_id << "@" << event->peer_id << "!!! \r\n";
				std::cout << *output;
				*m_log << output;
				create_connection(event);
			}
			if (event->event_type == gve_session_event_recieve)
			{
				gv_packet* p = event->packet;
				gv_3dlobby_connection* pc = find_connection(event);
				server_execute(pc, p);
			}
			if (event->event_type == gve_session_event_disconnected)
			{
				remove_connection(event);
				break;
			}
			if (m_connected)
			{
				gv_string_tmp ss;
				m_server->dump_peer(event->host_id, event->peer_id, ss);
				(*m_log) << ss;
			};
			event = NULL;
		} // next event
		m_map.update(dt);
		server_update_all_connections(dt);
		if (dt < 1.0f / m_server_send_rate + 0.01f)
			gv_thread::sleep((long)gvt_clamp(
				(1.0f / m_server_send_rate - dt) * 1000.f, 0.f, 100.f));
	}

	virtual void update(gv_float dt)
	{
		m_delta_time = dt;
		if (!m_pause)
		{ // UPDATE
			GV_PROFILE_EVENT(m_map_update, 0);
			if (m_is_server)
				update_server(dt);
			else
				update_client(dt);
		}
	}

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
		m_connections.clear();
		gvt_safe_delete(m_log);
		gvt_safe_delete(m_server);
	}
}
};
