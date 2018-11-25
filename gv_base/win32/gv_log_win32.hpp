namespace gv
{
void GV_STDCALL log_apc_call_back(gv_ulong_ptr);
class gv_log_impl
{
	friend class gv_log;
	friend void GV_STDCALL log_apc_call_back(gv_ulong_ptr);

public:
	static const int pading_size = 8;
	gv_log_impl(gv_log* owner, int buffer_size = 4096)
	{
		m_owner = owner;
		m_buffer_size = buffer_size;
		m_buffer[0] = new char[buffer_size + pading_size];
		m_buffer[1] = new char[buffer_size + pading_size];
		memset(m_buffer[0], 0, buffer_size + pading_size);
		memset(m_buffer[1], 0, buffer_size + pading_size);
		m_start_time_stamp = gv_global::time->time_stamp_unix();
		m_file_stream = NULL;
	}

	~gv_log_impl()
	{
		close();
// check_padding
#if !RETAIL
		for (int i = 0; i < pading_size; i++)
		{
			GV_ASSERT(m_buffer[0][m_buffer_size + i] == 0);
			GV_ASSERT(m_buffer[1][m_buffer_size + i] == 0);
		}
#endif
		delete[] m_buffer[0];
		delete[] m_buffer[1];
		gvt_safe_delete(m_file_stream);
	}

protected:
	union buffer_pointer {
		gv_uint u_value;
		gv_int i_value;
		struct
		{
			gv_uint buffer_idx : 1;
			gv_uint buffer_offset : 31;
		};
	};

	GV_STATIC_ASSERT(sizeof(buffer_pointer) == sizeof(gv_int));
	class add_and_flip
	{
	public:
		add_and_flip(gv_int _step, gv_int _max)
		{
			m_max = _max;
			m_step = _step;
		}
		gv_int operator()(const gv_int& a)
		{
			buffer_pointer p;
			p.i_value = a;
			p.buffer_offset = p.buffer_offset + m_step;
			if (p.buffer_offset >= (gv_uint)m_max)
			{
				p.buffer_offset = m_step;
				p.buffer_idx = !p.buffer_idx;
			}
			return p.i_value;
		}

	protected:
		gv_int m_max, m_step;
	};

	char* m_buffer[2];
	gv_atomic_count m_pending_write[2];
	gv_atomic_count m_cu_pos;
	gv_mutex m_write_mutex;
	gvi_stream* m_file_stream;
	gv_int m_buffer_size;
	gv_ulong m_start_time_stamp;
	gv_string64 m_name;
	gv_log* m_owner;

	int get_front_idx()
	{
		buffer_pointer p;
		p.i_value = m_cu_pos.get();
		int idx = p.buffer_idx;
		return idx;
	}

	int get_back_idx()
	{
		buffer_pointer p;
		p.i_value = m_cu_pos.get();
		int idx = !p.buffer_idx;
		return idx;
	}

	char* get_back_buffer()
	{
		return m_buffer[get_back_idx()];
	}

	char* get_front_buffer()
	{
		return m_buffer[get_front_idx()];
	}

public:
	gvi_stream& operator<<(const gv_string_tmp& s)
	{
		// gv_thread_lock lock(this->m_write_mutex);
		if (s.size() >= m_buffer_size)
			return *this->m_owner;
		; // will find a way to export large string
		buffer_pointer p;
		add_and_flip func(s.strlen(), m_buffer_size);
		p.i_value = m_cu_pos.interlock_any(func);
		if ((gv_int)p.buffer_offset + s.strlen() >=
			m_buffer_size)
		{	 // flip happen here
			{ // wait the write complete , or will have problem,
				gv_thread_lock lock(this->m_write_mutex);
			}
			{						// use apc;
				m_owner->inc_ref(); // prevent to be deleted during log
				if (!gv_global::log->m_log_thread.apc_call(log_apc_call_back,
														   (gv_ulong_ptr) this))
				{
					// FAILED TO CALL APC
					m_owner->dec_ref();
				};
			}
			//{
			//	this->write_back_buffer_to_file();
			//}
			p.buffer_idx = !p.buffer_idx;
			p.buffer_offset = 0;
		}
		++m_pending_write[p.buffer_idx];
		memcpy(m_buffer[p.buffer_idx] + p.buffer_offset, *s, s.strlen());
		--m_pending_write[p.buffer_idx];
		return *this->m_owner;
	};

	bool open(const char* name)
	{
		m_name = name;
		m_file_stream = gv_global::fm->open_text_file_for_append(name);
		if (!m_file_stream)
			m_file_stream = gv_global::fm->open_text_file_for_write(name);
		return m_file_stream ? true : false;
	};

	bool close()
	{
		if (m_file_stream)
			(*m_file_stream) << get_front_buffer();
		gvt_safe_delete(m_file_stream);
		return true;
	}

	bool flush()
	{
		if (m_file_stream)
			m_file_stream->flush();
		return false;
	}

// WARNING IF CALL THE FUNCTION TOO FASTER , COULD BE PROBLEM!
#pragma GV_REMINDER( \
	"[MEMO]if log to a file too faster, flip the buffer before the  write is complete, possible lost some info ")
	bool write_back_buffer_to_file()
	{
		GV_PROFILE_EVENT(gv_log_write_back_buffer_to_file, 0)
		gv_thread_lock lock(this->m_write_mutex);

		if (!m_file_stream)
			return false;
		int idx = get_back_idx();
		int timeout = 200;
		while (m_pending_write[idx].get() && timeout)
		{
			gv_thread::yield();
			timeout--;
		}
		if (!timeout)
		{
			GVM_DEBUG_OUT("ERROR write_back_buffer_to_file_time_out!!");
		}
		(*m_file_stream) << m_buffer[idx];
		m_file_stream->flush();
		memset(m_buffer[idx], 0, m_buffer_size);
		return true;
	}

	gv_ulong start_time_stamp()
	{
		return m_start_time_stamp;
	};
	void clear()
	{
		if (!m_file_stream)
			return;
		delete m_file_stream;
		m_file_stream = gv_global::fm->open_binary_file_for_write(*m_name);
	};
}; // gv_log_imp

void GV_STDCALL log_apc_call_back(gv_ulong_ptr p)
{
	gv_log_impl* plog = (gv_log_impl*)p;
	if (plog)
	{
		plog->write_back_buffer_to_file();
		plog->m_owner->dec_ref();
	}
}

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
	m_runable.init(this, &gv_log_manager::async_log_thread);
	m_log_thread.start(&m_runable);
	m_is_quiting = false;
};

gv_log_manager::~gv_log_manager()
{
	m_is_quiting = true;
	if (m_log)
	{
		m_log = NULL;
	}
	m_log_thread.apc_call(log_apc_call_back, 0);
	m_log_thread.join(1000);
};

void gv_log_manager::async_log_thread()
{
	m_event.set();
	for (;;)
	{
		if (m_is_quiting)
			return;
		m_event.wait();
		m_event.reset();
	}
};
}