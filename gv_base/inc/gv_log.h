#pragma once
namespace gv
{

class gv_log : public gvi_stream
{
	friend class gv_log_manager;

protected:
	gv_log(gv_int buf_size = 65536);

public:
	virtual ~gv_log();
	virtual bool open(const char* name);
	virtual bool close();
	//
	inline gvi_stream& operator<<(const char* p)
	{
		return (*this) << gv_string_tmp(p);
	}
	virtual gvi_stream& operator<<(const gv_string&);
	virtual gvi_stream& operator<<(const gv_string_tmp&);
	virtual gvi_stream& operator<<(const gv_float&);
	virtual gvi_stream& operator<<(const gv_double&);
	virtual gvi_stream& operator<<(const gv_short&);
	virtual gvi_stream& operator<<(const gv_ushort&);
	virtual gvi_stream& operator<<(const gv_int&);
	virtual gvi_stream& operator<<(const gv_uint&);
	virtual gvi_stream& operator<<(const gv_long&);
	virtual gvi_stream& operator<<(const gv_ulong&);
	virtual gvi_stream& operator<<(const gv_byte&);
	virtual gvi_stream& operator<<(const gv_char&);

	virtual gvi_stream& operator>>(gv_string&);
	virtual gvi_stream& operator>>(gv_float&);
	virtual gvi_stream& operator>>(gv_double&);
	virtual gvi_stream& operator>>(gv_short&);
	virtual gvi_stream& operator>>(gv_ushort&);
	virtual gvi_stream& operator>>(gv_int&);
	virtual gvi_stream& operator>>(gv_uint&);
	virtual gvi_stream& operator>>(gv_long&);
	virtual gvi_stream& operator>>(gv_ulong&);
	virtual gvi_stream& operator>>(gv_byte&);
	virtual gvi_stream& operator>>(gv_char&);

	virtual gv_int tell();
	virtual gv_int read(void* pdata, gv_int isize);
	virtual gv_int write(const void* pdata, gv_int isize);
	virtual gv_int size();
	virtual bool eof();
	virtual bool seek(gv_uint pos, std::ios_base::seekdir);
	virtual void flush();

	virtual gv_ulong start_time_stamp();
	virtual void clear();
	virtual const char* get_file_name();

protected:
	class gv_log_impl* m_pimpl;
};

class gv_log_manager
{
public:
	friend class gv_log_impl;
	gv_log_manager();
	~gv_log_manager();

	gv_log* create_log(const char* name, gv_int buffer_size = 65536);
	void close_log(gv_log*);
	gv_log* get_log()
	{
		return m_log;
	}
	void set_log(gv_log* l)
	{
		m_log = l;
	}

protected:
	void async_log_thread();
	gvt_runnable< gv_log_manager > m_runable;

	gv_thread m_log_thread;
	gv_event m_event;
	gvt_ref_ptr< gv_log > m_log;
	bool m_is_quiting;
};

extern void gv_write_debug_log(const gv_id& id, const char* message,
							   gv_log* log, const char* file_name,
							   gv_int line_no);
extern void gv_write_error_log(const gv_id& id, const char* message,
							   gv_log* log, const char* file_name,
							   gv_int line_no, bool is_fatal = true);
extern void gv_write_log(const gv_id& id, const char* message, gv_log* log,
						 const char* file_name, gv_int line_no,
						 bool use_debugout = false);

namespace gv_global
{
extern gvt_global< gv_log_manager > log;
extern gv_string debug_string; // for display on screen;
}

extern void gv_oos_log(const char* log_string);
#define GVM_LOG(id, msg)                                                           \
	{                                                                              \
		if (gv_global::log.try_get())                                              \
		{                                                                          \
			gv_string_tmp s_temp;                                                  \
			s_temp << msg;                                                         \
			gv_write_log(gv_id_##id, *s_temp, gv_global::log->get_log(), __FILE__, \
						 __LINE__);                                                \
		}                                                                          \
	}
#define GVM_INFO_LOG(msg)                                                          \
	{                                                                              \
		if (gv_global::log.try_get())                                              \
		{                                                                          \
			gv_string_tmp s_temp;                                                  \
			s_temp << msg;                                                         \
			gv_write_log(gv_id_info, *s_temp, gv_global::log->get_log(), __FILE__, \
						 __LINE__);                                                \
		}                                                                          \
	}
#define GVM_ERROR(msg, ...)                                                     \
	{                                                                           \
		if (gv_global::log.try_get())                                           \
		{                                                                       \
			gv_string_tmp s_temp;                                               \
			s_temp << msg;                                                      \
			gv_write_error_log(gv_id_error, *s_temp, gv_global::log->get_log(), \
							   __FILE__, __LINE__);                             \
		}                                                                       \
	}
#define GVM_WARNING(msg, ...)                                                     \
	{                                                                             \
		if (gv_global::log.try_get())                                             \
		{                                                                         \
			gv_string_tmp s_temp;                                                 \
			s_temp << msg;                                                        \
			gv_write_error_log(gv_id_warning, *s_temp, gv_global::log->get_log(), \
							   __FILE__, __LINE__, false);                        \
		}                                                                         \
	}
#define GVM_OOS_LOG(msg)          \
	{                             \
		{                         \
			gv_string_tmp s_temp; \
			s_temp << msg;        \
			gv_oos_log(*s_temp);  \
		}                         \
	}

#if GV_DEBUG_VERSION
#define GVM_DEBUG_LOG(id, msg)                                                 \
	{                                                                          \
		if (gv_global::log.try_get())                                          \
		{                                                                      \
			gv_string_tmp s_temp;                                              \
			s_temp << msg;                                                     \
			gv_write_debug_log(gv_id_##id, *s_temp, gv_global::log->get_log(), \
							   __FILE__, __LINE__);                            \
		}                                                                      \
	}
#else
#define GVM_DEBUG_LOG(id, msg) \
	{                          \
	}
#endif

#define GVM_DEBUG_OUT(a)         \
	{                            \
		GVM_DEBUG_LOG(debug, a); \
	}
};