#pragma once
namespace gv
{
class gvi_commandlet : public gv_refable
{
public:
	gvi_commandlet(){};
	virtual ~gvi_commandlet(){};
	virtual bool execute(const gv_string_tmp& str) = 0;
};

class gv_commandlet_memory_debug : public gvi_commandlet
{
public:
	gv_commandlet_memory_debug(){};
	virtual ~gv_commandlet_memory_debug(){};
	virtual bool execute(const gv_string_tmp& str)
	{
		if (str == "mem_track_dump")
		{
			gvp_memory_default::static_dump_tracking();
		}
		else if (str == "mem_track_reset")
		{
			gvp_memory_default::static_reset_tracking();
		}
		else if (str == "mem_track_on")
		{
			gvp_memory_default::static_enable_tracking();
		}
		else if (str == "mem_track_off")
		{
			gvp_memory_default::static_diable_tracking();
		}
		return true;
	};
};

class gv_commandlet_client_net_simulation : public gvi_commandlet
{
public:
	gv_commandlet_client_net_simulation(){};
	virtual ~gv_commandlet_client_net_simulation(){};
	virtual bool execute(const gv_string_tmp& str);
};

class gv_commandlet_lua_impl;
class gv_commandlet_lua : public gvi_commandlet
{
public:
	gv_commandlet_lua();
	virtual ~gv_commandlet_lua();
	virtual bool execute(const gv_string_tmp& str);

protected:
	gvt_ref_ptr< gv_commandlet_lua_impl > m_pimpl;
};

class gv_command_console_runable;
class gv_command_console_net_runable;
class gv_normal_udp_server;

class gv_command_console
{
public:
	gv_command_console();
	virtual ~gv_command_console();
	static gv_command_console* get();
	virtual void start();
	virtual void stop();
	virtual void hide(bool enable = true);
	virtual bool execute(const gv_string_tmp& str);
	inline void add(gvi_commandlet* p)
	{
		m_commandlet_list.push_back(p);
	};
	inline void remove(gvi_commandlet* p)
	{
		m_commandlet_list.erase_item(p);
	};
	inline bool is_quit()
	{
		return m_is_quit;
	}
	void get_command_array(gvt_array< gv_string >& ret, bool clear = true);
	void post_command(const char* cmd);

protected:
	typedef gvt_array< gvt_ref_ptr< gvi_commandlet > > commandlet_list;
	commandlet_list m_commandlet_list;
	gv_command_console_runable* m_runable;
	;
	gv_thread m_thread;
	gv_normal_udp_server* m_server;
	gv_bool m_is_quit;
};
}
