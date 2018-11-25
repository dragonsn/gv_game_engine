#pragma once
namespace gv
{
#define GVM_REGISTER_REMOTE_CALL_ID(name, num) \
	static const gv_ushort gv_remote_call_id_##name = num;

#define GVM_CHECK_REMOTE_CALL_ID(num) \
	static const gv_ushort GV_MACRO_CONCATE(gv_remote_call_, num) = num;

class gv_remote_call
{
public:
	gv_remote_call(){};
	virtual ~gv_remote_call()
	{
		m_need_wait_return = true;
	}
	inline gv_net_cmd get_header()
	{
		return m_header;
	}
	inline bool is_ack()
	{
		return m_header.is_remote_call_ack();
	}
	virtual gv_packet& operator>>(gv_packet& p)
	{
		return p;
	};
	virtual gv_packet& operator<<(gv_packet& p)
	{
		return p;
	};
	gv_com_tasklet* get_caller()
	{
		return m_caller;
	}
	gv_remote_call* get_return_value()
	{
		return m_return;
	}
	virtual const char* get_name()
	{
		return "unknown_call ";
	};

public:
	gv_bool m_need_wait_return;
	gvt_ref_ptr< gv_com_tasklet > m_caller;
	gvt_ptr< gv_remote_call > m_return;
	gvt_ptr< gv_remote_call > next;

protected:
	gv_net_cmd m_header;
};
}