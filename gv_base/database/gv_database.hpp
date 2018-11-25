
#pragma once 

#include "gv_database.h"
#define OTL_ODBC
#define OTL_EXCEPTION_DERIVED_FROM gv::gv_database_exception
#include "otlv4/otlv4.h"
#include <boost/boost/date_time.hpp>
#include <boost/boost/date_time/gregorian/gregorian.hpp>
#include <boost/boost/date_time/posix_time/posix_time.hpp>
#pragma GV_REMINDER( \
	"[database]		: must use utf8 to support chinese character")
#pragma GV_REMINDER( \
	"[database] in window7 use c:/windows/sysWOW64/odbcad32.exe to add data source")
namespace gv
{
//============================================================================================
//								:
//============================================================================================
otl_connect& get_otl_connect(gv_byte* p)
{
	return *((otl_connect*)p);
}

otl_stream& get_otl_stream(gv_byte* p)
{
	return *((otl_stream*)p);
}
GV_STATIC_ASSERT(gvc_size_db_con_shadow >= sizeof(otl_connect));

void gv_database_connection::static_init()
{
	otl_connect::otl_initialize(1);
};
void gv_database_connection::static_uninit(){
	// otl_connect::otl_terminate();
};
gv_database_connection::gv_database_connection()
{
	gvt_construct(&get_otl_connect(m_impl_shadow));
}
gv_database_connection::gv_database_connection(const char* connect_str,
											   const int auto_commit)
{
	gvt_construct(&get_otl_connect(m_impl_shadow));
	get_otl_connect(m_impl_shadow).rlogon(connect_str, auto_commit);
	m_connect_string = connect_str;
}
gv_database_connection::~gv_database_connection()
{
	gvt_destroy(&get_otl_connect(m_impl_shadow));
}
const char* gv_database_connection::get_connect_string()
{
	return *m_connect_string;
};
void gv_database_connection::direct_exec(const char* sqlstm,
										 const int exception_enabled)
{
	GVM_LOG(database, "[" << get_connect_string() << "]" << sqlstm);
	get_otl_connect(m_impl_shadow).direct_exec(sqlstm, exception_enabled);
};
void gv_database_connection::rlogon(const char* connect_str,
									const int auto_commit)
{
	GV_PROFILE_EVENT(gv_database_connection_rlogon, 0);
	m_connect_string = connect_str;
	get_otl_connect(m_impl_shadow).rlogon(connect_str, auto_commit);
};
void gv_database_connection::logoff(void)
{
	GV_PROFILE_EVENT(gv_database_connection_rlogoff, 0);
	get_otl_connect(m_impl_shadow).logoff();
};
void gv_database_connection::commit(void)
{
	get_otl_connect(m_impl_shadow).commit();
};
void gv_database_connection::auto_commit_on(void)
{
	get_otl_connect(m_impl_shadow).auto_commit_on();
};
void gv_database_connection::auto_commit_off(void)
{
	get_otl_connect(m_impl_shadow).auto_commit_off();
};
void gv_database_connection::rollback(void)
{
	get_otl_connect(m_impl_shadow).rollback();
};

void gv_database_connection::begin_transaction()
{
	get_otl_connect(m_impl_shadow).auto_commit_off();
	get_otl_connect(m_impl_shadow)
		.set_transaction_isolation_level(
			otl_tran_read_committed /*otl_tran_serializable*/);
};
void gv_database_connection::end_transaction()
{
	get_otl_connect(m_impl_shadow).auto_commit_on();
	get_otl_connect(m_impl_shadow).commit();
};
void gv_database_connection::rollback_transaction()
{
	rollback();
	get_otl_connect(m_impl_shadow).auto_commit_on();
};
void gv_database_connection::set_max_long_size(int amax_size)
{
	get_otl_connect(m_impl_shadow).set_max_long_size(amax_size);
};
//============================================================================================
//								:
//============================================================================================
GV_STATIC_ASSERT(gvc_size_db_stream_shadow >= sizeof(otl_stream));
gv_database_stream::gv_database_stream()
{
	gvt_construct(&get_otl_stream(m_impl_shadow));
};
gv_database_stream::~gv_database_stream()
{
	gvt_destroy(&get_otl_stream(m_impl_shadow));
};
gv_database_stream::gv_database_stream(gv_int arr_size, const char* sqlstm,
									   gv_database_connection& db,
									   const int select,
									   const char* sqlstm_label)
{
	gvt_construct(&get_otl_stream(m_impl_shadow));
	open(arr_size, sqlstm, db, select, sqlstm_label);
};
void gv_database_stream::open(gv_int arr_size, const char* sqlstm,
							  gv_database_connection& db, const int select,
							  const char* sqlstm_label)
{
	GVM_LOG(database, "[" << db.get_connect_string() << "]" << sqlstm);
	return get_otl_stream(m_impl_shadow)
		.open(arr_size, sqlstm, get_otl_connect(db.get_impl()), select,
			  sqlstm_label);
}
bool gv_database_stream::close()
{

	get_otl_stream(m_impl_shadow).close();
	return true;
};
bool gv_database_stream::eof()
{
	return get_otl_stream(m_impl_shadow).eof() != NULL;
};
void gv_database_stream::flush()
{
	get_otl_stream(m_impl_shadow).flush();
};
void gv_database_stream::clean(const int clean_up_error_flag)
{
	get_otl_stream(m_impl_shadow).clean(clean_up_error_flag);
};
int gv_database_stream::is_null()
{
	return get_otl_stream(m_impl_shadow).is_null();
};

void gv_database_stream::set_commit(int auto_commit)
{
	return get_otl_stream(m_impl_shadow).set_commit(auto_commit);
};
//============================================================================================
//								:
//============================================================================================
gvi_stream& gv_database_stream::operator<<(const char* p)
{
	gv_string_tmp s = gv_multi_byte_to_utf8(p);
	get_otl_stream(m_impl_shadow) << *s;
	return *this;
}

gvi_stream& gv_database_stream::operator<<(const gv_string& s)
{
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t << *s;
	return *this;
};

gvi_stream& gv_database_stream::operator<<(const gv_string_tmp& s)
{
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t << *s;
	return *this;
};

gvi_stream& gv_database_stream::operator<<(const gv_text& _t)
{
	gv_string_tmp t = gv_multi_byte_to_utf8(*_t);
	otl_long_string f2(*t, t.size()); // define long string variable
	// db.set_max_long_size(70000);
	otl_stream& s = get_otl_stream(m_impl_shadow);
	s << f2;
	return *this;
};

gvi_stream& gv_database_stream::operator<<(const gv_float& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_double& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_short& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_ushort& d)
{
	get_otl_stream(m_impl_shadow) << (gv_short)d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_int& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_uint& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_long& d)
{
	// get_otl_stream(m_impl_shadow)<<d;
	GV_ASSERT(0);
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_ulong& d)
{
	// get_otl_stream(m_impl_shadow)<<d;
	GV_ASSERT(0);
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_byte& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};
gvi_stream& gv_database_stream::operator<<(const gv_char& d)
{
	get_otl_stream(m_impl_shadow) << d;
	return *this;
};

gvi_stream& gv_database_stream::operator<<(const gv_time_stamp& t)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	time_t t_t = t.m_u64 / (1000 * 1000);
	if (t_t != 0)
	{
		t_t += gv_get_time_zone_bias() * 3600; // to utc time stamp
	}
	ptime t1 = from_time_t(t_t);
	tm tt = to_tm(t1);
	otl_datetime tm1;
	tm1.year = tt.tm_year + 1900;
	tm1.month = tt.tm_mon + 1;
	tm1.day = tt.tm_mday;
	tm1.hour = tt.tm_hour;
	tm1.minute = tt.tm_min;
	tm1.second = tt.tm_sec;
	tm1.month = gvt_clamp(tm1.month, 1, 12);
	tm1.day = gvt_clamp(tm1.day, 1, 31);
	get_otl_stream(m_impl_shadow) << tm1;
	return *this;
};

gvi_stream& gv_database_stream::operator>>(const char* d)
{

	return (*this) >> (char*)d;
}

gvi_stream& gv_database_stream::operator>>(char* d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
}; // default do nothing
gvi_stream& gv_database_stream::operator>>(gv_string& s)
{
	char buffer[4096];
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t >> buffer;
	s = gv_utf8_to_multi_byte(buffer);
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_string_tmp& s)
{
	char buffer[4096];
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t >> buffer;
	s = gv_utf8_to_multi_byte(buffer);
	return *this;
};

gvi_stream& gv_database_stream::operator>>(gv_text& s)
{
	otl_long_string f2(70000);
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t >> f2;
	if (&f2[0])
	{
		s.init((char*)&f2[0], f2.len());
		if (s.last() && *s.last() != 0)
			s.add_end();
		s = *gv_utf8_to_multi_byte(*s);
	}
	return *this;
};

gvi_stream& gv_database_stream::operator>>(gv_float& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_double& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_short& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_ushort& d)
{
	gv_short s;
	get_otl_stream(m_impl_shadow) >> s;
	d = (gv_ushort)s;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_int& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_uint& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_long& d)
{
	// get_otl_stream(m_impl_shadow)>>d;
	GV_ASSERT(0);
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_ulong& d)
{
	// get_otl_stream(m_impl_shadow)>>d;
	GV_ASSERT(0);
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_byte& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};
gvi_stream& gv_database_stream::operator>>(gv_char& d)
{
	get_otl_stream(m_impl_shadow) >> d;
	return *this;
};

gvi_stream& gv_database_stream::operator>>(gv_time_stamp& t)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	otl_datetime tm1;
	get_otl_stream(m_impl_shadow) >> tm1;
	tm tt;
	gvt_zero(tt);
	tt.tm_year = tm1.year - 1900;
	tt.tm_mon = tm1.month - 1;
	tt.tm_mday = tm1.day;
	tt.tm_hour = tm1.hour; //	;
	tt.tm_min = tm1.minute;
	tt.tm_sec = tm1.second;
	ptime pt(date(1970, Jan, 1), time_duration(0, 0, 0));
	try
	{
		pt = ptime_from_tm(tt);
	}
	catch (...)
	{
		// out of unix time stamp range. well just use the epoch time
	}

	tt = to_tm(pt);
	time_t t_t = mktime(&tt);
	if (t_t == -1)
	{
		t.m_u64 = 0;
	}
	else
	{
		t_t -= gv_get_time_zone_bias() * 3600; // to local time stamp...
		t.m_u64 = t_t * 1000 * 1000;
	}
	return *this;
};

gvi_stream& gv_database_stream::operator<<(const gv_raw_data& t)
{
	otl_long_string f2(t.m_data.begin(),
					   t.m_data.size()); // define long string variable
	otl_stream& s = get_otl_stream(m_impl_shadow);
	s << f2;
	return *this;
}
gvi_stream& gv_database_stream::operator>>(gv_raw_data& s)
{
	otl_long_string f2(70000);
	otl_stream& t = get_otl_stream(m_impl_shadow);
	t >> f2;
	if (&f2[0])
	{
		s.m_data.init((gv_byte*)&f2[0], f2.len());
	}
	return *this;
}
//============================================================================================
//								:
//============================================================================================
gv_database_exception::gv_database_exception(){

};
gv_database_exception::~gv_database_exception(){

};
gv_string_tmp gv_database_exception::what() const throw()
{
	gv_string_tmp s;
	const otl_exception& p = *static_cast< const otl_exception* >(this);
	s << (const char*)p.msg << gv_endl; // print out error message
	s << (const char*)p.stm_text
	  << gv_endl;							 // print out SQL that caused the error
	s << (const char*)p.sqlstate << gv_endl; // print out SQLSTATE message
	s << (const char*)p.var_info << gv_endl;
	s << "error code :" << p.code << gv_endl;
	return s;
};

gv_int gv_database_exception::error_code() const throw()
{
	const otl_exception& p = *static_cast< const otl_exception* >(this);
	return p.code;
};
}