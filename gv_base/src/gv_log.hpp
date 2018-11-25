#include "gv_base_internal.h"
#include "gv_log.h"

#if defined(WIN32)
#include "../win32/gv_log_win32.hpp"
#else
#include "../ansi/gv_log_ansi.hpp"
#endif

namespace gv
{

gv_log::gv_log(gv_int buffer_size)
{
	m_pimpl = new gv_log_impl(this, buffer_size);
	m_ref_count = 0;
};

gv_log::~gv_log()
{
	gvt_safe_delete(m_pimpl);
}

bool gv_log::open(const char* name)
{
	return m_pimpl->open(name);
};

bool gv_log::close()
{
	GV_PROFILE_EVENT(gv_log_close, 0);
	return m_pimpl->close();
};

const char* gv_log::get_file_name()
{
	return *m_pimpl->m_name;
};

gvi_stream& gv_log::operator<<(const gv_string& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};

gvi_stream& gv_log::operator<<(const gv_string_tmp& s)
{
	(*m_pimpl) << s;
	return *this;
};

gvi_stream& gv_log::operator<<(const gv_float& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_double& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_short& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_ushort& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_int& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_uint& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_long& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_ulong& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_byte& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator<<(const gv_char& d)
{
	gv_string_tmp s;
	s << d;
	(*m_pimpl) << s;
	return *this;
};
gvi_stream& gv_log::operator>>(gv_string&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_float&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_double&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_short&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_ushort&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_int&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_uint&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_long&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_ulong&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_byte&)
{
	return *this;
};
gvi_stream& gv_log::operator>>(gv_char&)
{
	return *this;
};
gv_int gv_log::tell()
{
	return 0;
};
gv_int gv_log::read(void* pdata, gv_int isize)
{
	return 0;
};

gv_int gv_log::write(const void* pdata, gv_int isize)
{
	return 0;
};
gv_int gv_log::size()
{
	return 0;
};
bool gv_log::eof()
{
	return false;
};
bool gv_log::seek(gv_uint pos, std::ios_base::seekdir)
{
	return false;
};
void gv_log::flush()
{
	m_pimpl->flush();
};

gv_ulong gv_log::start_time_stamp()
{
	return m_pimpl->start_time_stamp();
};
void gv_log::clear()
{
	return m_pimpl->clear();
};

gv_log* gv_log_manager::create_log(const char* name, int buffer_size)
{
	gv_log* plog = new gv_log(buffer_size);
	gv_string_tmp s = name;
	s << gv_global::time->get_local_time_string() << ".log";
	s.replace_all(":", "_");
	s.replace_all(" ", "@");
	plog->open(*s);
	plog->inc_ref();
	return plog;
}
void gv_log_manager::close_log(gv_log* plog)
{
	plog->dec_ref();
};

void gv_write_debug_log(const gv_id& id, const char* message, gv_log* log,
						const char* file_name, gv_int line_no)
{
#if 1 // GV_DEBUG_VERSION
	gv_write_log(id, message, log, file_name, line_no, true);
#endif
};

void gv_write_error_log(const gv_id& id, const char* message, gv_log* log,
						const char* file_name, gv_int line_no, bool is_fatal)
{
	gv_write_log(id, message, log, file_name, line_no, true);
	if (is_fatal)
	{
		gv_halt();
	}
};

void gv_write_log(const gv_id& id, const char* message, gv_log* log,
				  const char* file_name, gv_int line_no, bool use_debug_out)
{
	if (id.string().m_flags.m_is_log_suppressed)
	{
		return;
	}
	if (log || use_debug_out)
	{
		gv_string_tmp s;
		if (use_debug_out)
		{
			s << file_name << "(" << line_no << "):" << id << ":" << gv_endl;
		};
		s << "[" << gv_global::time->get_local_time_string() << "]";
		s << "[" << id << "]";
		s << message;
		s << gv_endl;
		if (log)
			(*log) << *s;
		if (use_debug_out)
			gv_debug_output(*s);
	}
};
};
