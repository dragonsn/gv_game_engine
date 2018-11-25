namespace gv
{
void GV_STDCALL log_apc_call_back(unsigned long);
class gv_log_impl
{
	friend class gv_log;
	friend void GV_STDCALL log_apc_call_back(unsigned long);

public:
	static const int pading_size = 8;
	gv_log_impl(gv_log* owner, int buffer_size = 4096)
	{
		m_owner = owner;
		m_start_time_stamp = gv_global::time->time_stamp_unix();
	}
	~gv_log_impl()
	{
	}
	gv_log* m_owner;
	gv_ulong m_start_time_stamp;
	gv_string m_name;

public:
	gv_log_impl& operator<<(const gv_string_tmp& s)
	{
		gv_console_output(*s);
		return *this;
	};
	gv_ulong start_time_stamp()
	{
		return m_start_time_stamp;
	};

	bool open(const char* name)
	{
		m_name = name;
		return true;
	};
	bool close()
	{
		return true;
	}
	bool flush()
	{
		return false;
	}
	void clear(){};
};

gv_log_manager::gv_log_manager()
	: m_log_thread(gv_id_async_log_thread), m_event(false)
{
	if (!gv_global::config.no_log_all)
	{
		if (gv_global::config.main_log_file_name == 0)
			gv_global::config.main_log_file_name = "gv_base";
		m_log = create_log(gv_global::config.main_log_file_name);
		;
	}
	m_is_quiting = false;
};

gv_log_manager::~gv_log_manager()
{
	m_is_quiting = true;
	if (m_log)
	{
		m_log = NULL;
	}
};

void gv_log_manager::async_log_thread(){};
}