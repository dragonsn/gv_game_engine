// a wrapper of OTL implementation;
namespace gv
{

//============================================================================================
//								:
//============================================================================================
static const gv_int gvc_size_db_con_shadow = 1024;
class gv_database_connection : public gv_refable
{
	friend class gv_database_stream;

public:
	gv_database_connection();
	gv_database_connection(const char* connect_str, const int auto_commit = 0);
	virtual ~gv_database_connection();
	void direct_exec(const char* sqlstm, const int exception_enabled = 1);
	void rlogon(const char* connect_str, const int auto_commit = 1);
	void logoff(void);
	void commit(void);
	void auto_commit_on(void);
	void auto_commit_off(void);
	void begin_transaction();
	void end_transaction();
	void rollback_transaction();
	void rollback(void);
	void set_max_long_size(int amax_size);
	const char* get_connect_string();
	static void static_init();
	static void static_uninit();

protected:
	gv_byte* get_impl()
	{
		return m_impl_shadow;
	}
	gv_byte m_impl_shadow[gvc_size_db_con_shadow];
	gv_string128 m_connect_string;
};

//============================================================================================
//								:
//============================================================================================
static const gv_int gvc_size_db_stream_shadow = 256;
const int gvc_explicit_select = 0;
const int gvc_implicit_select = 1;
class gv_database_stream : public gvi_stream
{
public:
	gv_database_stream(gv_int arr_size, const char* sqlstm,
					   gv_database_connection& db,
					   const int select = gvc_explicit_select,
					   const char* sqlstm_label = 0);
	gv_database_stream();
	~gv_database_stream();

	void open(gv_int arr_size, const char* sqlstm, gv_database_connection& db,
			  const int select = gvc_explicit_select,
			  const char* sqlstm_label = 0);
	bool close();
	bool eof();
	void flush();
	void clean(const int clean_up_error_flag = 0);
	int is_null();
	void set_commit(int auto_commit = 0);

	virtual gvi_stream& operator<<(const char*);
	virtual gvi_stream& operator<<(const gv_string&);
	virtual gvi_stream& operator<<(const gv_string_tmp&);
	virtual gvi_stream& operator<<(const gv_text&);
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
	virtual gvi_stream& operator<<(const gv_time_stamp& t);
	virtual gvi_stream& operator<<(const gv_raw_data& t);

	virtual gvi_stream& operator>>(const char*); // default do nothing
	virtual gvi_stream& operator>>(char*);		 // default do nothing
	virtual gvi_stream& operator>>(gv_string&);
	virtual gvi_stream& operator>>(gv_string_tmp& s);
	virtual gvi_stream& operator>>(gv_text& t);
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
	virtual gvi_stream& operator>>(gv_time_stamp& t);
	virtual gvi_stream& operator>>(gv_raw_data& t);

protected:
	gv_byte m_impl_shadow[gvc_size_db_stream_shadow];
};

//============================================================================================
//								:
//============================================================================================
class gv_database_exception
{
public:
	gv_database_exception();
	virtual ~gv_database_exception();
	virtual gv_string_tmp what() const throw();
	virtual gv_int error_code() const throw();
};
//============================================================================================
//								:
//============================================================================================
};
