#include "gv_base.h"
#if !GV_NO_LUA
#include "../lua/gv_lua_commandlet.hpp"
#endif

namespace gv
{
bool gv_commandlet_client_net_simulation::execute(const gv_string_tmp& str)
{
	try
	{
		using namespace gv_lang_xml;
		gv_xml_parser xml;
		xml.load_string(str);
		gv_string_tmp name;
		xml >> name;
		if (name == "set_net_sim_latency")
		{
			gv_int lagency;
			xml >> lagency;
			gv_global::config.net_simulation_latency_ms = lagency;
			GVM_CONSOLE_OUT("set_net_sim_latency: setting latency to " << lagency
																	   << " ms");
		}
		else if (name == "set_net_sim_packet_loss")
		{
			gv_float pkt_loss_rate;
			xml >> pkt_loss_rate;
			gv_global::config.net_simulation_packet_loss_rate = pkt_loss_rate;
			GVM_CONSOLE_OUT("set_net_sim_packet_loss: setting packet loss rate to "
							<< pkt_loss_rate);
		}
	}
#if defined(WIN32)
	catch (const char* p)
	{
		GVM_ERROR("Error in exec " << str << "message is :" << p << "!!!"
								   << gv_endl);
	}
#endif
	catch (...)
	{
		GV_ASSERT(0);
	}
	return true;
}

//=========================================================================================
void GV_STDCALL
console_server_recv(class gvi_server_transport_layer_udp* server,
					gv_socket_address& address, gv_packet* packet)
{
	static int package_recved = 0;
	int cmd_size = packet->get_network_diagram_size();
	if (cmd_size > 1)
	{
		char* cmd = packet->get_network_diagram_start();
		cmd[cmd_size - 1] = 0;
		gv_command_console::get()->post_command(cmd);
	}
	server->destroy_packet(packet);
};

class gv_command_console_runable : public gv_runnable
{
public:
	gv_command_console_runable()
	{
		quit = false;
	}
	void run()
	{
		while (!quit)
		{
			// std::string ss;
			char buffer[2048];
			gets_s(buffer);
			// std::cin>>ss;
			gv_string_tmp s = buffer;
			if (m_event.try_wait(1))
				return;
			if (!m_owner->execute(s))
				return;
			{
				gv_thread_lock lock(m_mutex);
				m_command_array.push_back(*s);
			}
		};
	}

	void get_command_array(gvt_array< gv_string >& ret, bool clear = true)
	{
		gv_thread_lock lock(m_mutex);
		ret = m_command_array;
		if (clear)
			m_command_array.clear();
	}

	void post_command(const char* cmd)
	{
		gv_thread_lock lock(m_mutex);
		m_command_array.add(cmd);
	}
	void notify()
	{
		m_event.set();
	}
	gv_event m_event;
	gv_command_console* m_owner;
	gv_mutex m_mutex;
	gvt_array< gv_string > m_command_array;
	bool quit;
};
gv_command_console* s_console = 0;
gv_command_console* gv_command_console::get()
{
	return s_console;
}

gv_command_console::gv_command_console()
	: m_thread(gv_id_console)
{

	m_runable = new gv_command_console_runable;
	m_runable->m_owner = this;
	m_is_quit = false;

	m_server = new gv_normal_udp_server;
	gv_normal_udp_server& server = *m_server;
	server.set_callback_on_recv(&console_server_recv);
	server.create(7623);

	add(new gv_commandlet_memory_debug);
	add(new gv_commandlet_client_net_simulation);
	GV_ASSERT(s_console == NULL);
	s_console = this;
	;
};
gv_command_console::~gv_command_console()
{
	/*commandlet_list::iterator it =m_commandlet_list.begin();
  while(it!=m_commandlet_list.end() )
  {
          gvt_safe_delete( (*it));
          ++it;
  }*/
	stop();
	m_commandlet_list.clear();
	delete m_runable;
	s_console = NULL;
};
void gv_command_console::start()
{
	m_thread.start(m_runable);
};
void gv_command_console::stop()
{
	if (m_thread.is_running())
	{
		m_runable->notify();
		m_runable->quit = true;
		// std::cin<<"\r\n";
		// std::cin<<"\n\r";
		gv_send_console_key_event('A');
		gv_send_console_key_event('\r');
		gv_send_console_key_event('\n');
		// gv_send_console_key_event('\r');
		m_thread.join();
	}
	if (m_server)
	{
		m_server->destroy();
		gvt_safe_delete(m_server);
	};
};
void gv_command_console::hide(bool enable){

};
bool gv_command_console::execute(const gv_string_tmp& str)
{
	commandlet_list::iterator it = m_commandlet_list.begin();
	if (str == "quit" || str == "QUIT")
	{
		m_is_quit = true;
		return false;
	}
	while (it != m_commandlet_list.end())
	{
		if (!(*it)->execute(str))
			return false;
		++it;
	}
	return true;
};

void gv_command_console::get_command_array(gvt_array< gv_string >& ret,
										   bool clear)
{
	m_runable->get_command_array(ret, clear);
};

void gv_command_console::post_command(const char* cmd)
{
	m_runable->post_command(cmd);
};
};
