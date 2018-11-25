namespace gv
{
//============================================================================================
//								:lowest level net command ,
//remote function call and replication is build upon it
//============================================================================================
static const int gvc_net_cmd_channel = 0;
static const int gvc_net_default_channel_nb = 8;
static const int gvc_net_cmd_max_id = 16383;
static const int gvc_max_replica_struct_id = 16383;
static const int gvc_max_call_id = 16383;
static const int gvc_invalid_replica_struct_id = 16383;
static const int gvc_invalid_call_id = 16383;
static const int gvc_invalid_net_cmd_id = 16383;
static const gv_uint gvc_invalid_global_user_id = (gv_uint)-1;

#define GVM_REGISTER_NET_CMD_ID(name, num)             \
	static const gv_ushort gv_net_cmd_id_##name = num; \
	static const gv_ushort GV_MACRO_CONCATE(gv_net_cmd_, num) = num;

struct gv_net_cmd
{
	gv_net_cmd()
	{
		m_cmd_id = 0;
	}
	gv_net_cmd(bool is_call, bool is_replica, gv_ushort index)
	{
		m_is_remote_call = is_call;
		m_is_replicate = is_replica;
		m_index = index;
	}
	inline gv_bool operator==(const gv_net_cmd& c)
	{
		return c.m_cmd_id == m_cmd_id;
	}
	inline gv_bool operator!=(const gv_net_cmd& c)
	{
		return c.m_cmd_id != m_cmd_id;
	}
	inline gv_bool operator<(const gv_net_cmd& c)
	{
		return c.m_cmd_id < m_cmd_id;
	}
	inline bool is_remote_call()
	{
		return (m_is_remote_call) && !(m_is_replicate);
	};
	inline bool is_remote_call_ack()
	{
		return (m_is_remote_call) && (m_is_replicate);
	};
	inline bool is_replicate()
	{
		return (!m_is_remote_call) && (m_is_replicate);
	};
	union {
		gv_ushort m_cmd_id;
		struct
		{
			gv_ushort m_index : 14;
			gv_ushort m_is_remote_call : 1;
			gv_ushort m_is_replicate : 1;
		};
	};
};

struct gv_net_cmd_end : public gv_net_cmd
{
	gv_net_cmd_end()
	{
		m_cmd_id = 0;
	}
};
template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& stream, const gv_net_cmd& c)
{
	stream << c.m_cmd_id;
	return stream;
}

template < class type_of_stream >
inline type_of_stream& operator>>(type_of_stream& stream, gv_net_cmd& c)
{
	stream >> c.m_cmd_id;
	return stream;
}

template < class type_of_cmd >
inline bool gvt_is_valid_net_cmd(const type_of_cmd& cmd)
{
	return true;
}

//============================================================================================
//								:
//============================================================================================
class gv_net_cmd_list_base
{
public:
	gv_net_cmd_list_base()
	{
	}
	virtual ~gv_net_cmd_list_base()
	{
	}
	virtual gv_ushort get_id()
	{
		return gvc_invalid_net_cmd_id;
	}
	virtual gv_net_cmd_list_base& operator>>(gv_packet& p)
	{
		return *this;
	};
	void* operator new(size_t size);
	void operator delete(void*);
	virtual const char* get_name()
	{
		return "[unknown_net_cmd]";
	}
	gvt_ptr< gv_net_cmd_list_base > next;
	gvt_ref_ptr< gv_com_tasklet > m_requester;
	bool m_is_reliable;
};

template < class type_of_command >
class gvt_net_cmd_list : public gv_net_cmd_list_base
{
public:
	gvt_net_cmd_list()
	{
	}
	gvt_net_cmd_list(const type_of_command& c, bool reliable,
					 gv_com_tasklet* sender)
	{
		data = c;
		m_requester = sender;
		m_is_reliable = reliable;
	}
	virtual gv_net_cmd_list_base& operator>>(gv_packet& p)
	{
		p << get_id();
		p << data;
		return *this;
	};
	virtual ~gvt_net_cmd_list(){};
	virtual gv_ushort get_id()
	{
		return type_of_command::static_net_cmd_id();
	}
	virtual const char* get_name()
	{
		return type_of_command::static_name();
	}
	type_of_command data;
};

template < class type_of_command >
class gvt_net_cmd_list_ptr : public gv_net_cmd_list_base
{
public:
	gvt_net_cmd_list_ptr()
	{
	}
	gvt_net_cmd_list_ptr(type_of_command* c, bool reliable,
						 gv_com_tasklet* sender)
	{
		data = c;
		m_requester = sender;
		m_is_reliable = reliable;
	}
	virtual gv_net_cmd_list_base& operator>>(gv_packet& p)
	{
		p << get_id();
		p << *data;
		return *this;
	};
	virtual ~gvt_net_cmd_list_ptr()
	{
		gvt_safe_delete(data);
	};
	virtual gv_ushort get_id()
	{
		return type_of_command::static_net_cmd_id();
	}
	virtual const char* get_name()
	{
		return type_of_command::static_name();
	}
	gvt_ptr< type_of_command > data;
};

//============================================================================================
//					a dynamic parameter, generic command to simplify
//the define of net command.
//					NOTICE: it will take extra bandwidth . (2 byte
//header , and the size of payload is unified as 4);
//============================================================================================
struct gv_net_cmd_payload
{
	template < class T >
	const gv_net_cmd_payload& operator=(const T& i)
	{
		GV_STATIC_ASSERT(sizeof(T) <= 4);
		data.ui32 = *((gv_uint*)&i);
		return (*this);
	}
	union {
		gv_float f32;
		gv_int i32;
		gv_uint ui32;
		gv_short i16;
		gv_ushort ui16;
		gv_char i8;
		gv_byte ui8;
		gv_bool b;
	} data;
};

GV_STATIC_ASSERT(sizeof(gv_net_cmd_payload) == 4);
// this net command simplified some simple net command , don't request complex
// structure and not send often.
struct gv_net_cmd_generic
{
	gv_net_cmd_generic()
	{
	}
	gv_net_cmd_generic(const class gv_net_cmd_cmd_generic_0& cmd);
	gv_net_cmd_generic(const class gv_net_cmd_cmd_generic_1& cmd);
	gv_net_cmd_generic(const class gv_net_cmd_cmd_generic_2& cmd);
	gv_net_cmd_generic(const class gv_net_cmd_cmd_generic_3& cmd);
	gv_net_cmd_generic(const class gv_net_cmd_cmd_generic_4& cmd);
	gv_ushort m_generic_cmd_id;
	gv_int m_nb_pay_load; // max 9
	gv_net_cmd_payload m_payloads[9];
};
}