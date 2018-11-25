#pragma once
namespace gv
{
typedef void(GV_STDCALL* gv_callback_on_recv_complete)(
	class gvi_server_transport_layer_udp* server, gv_socket_address& address,
	gv_packet*);
typedef void(GV_STDCALL* gv_callback_on_io_error)(
	class gvi_server_transport_layer_udp* server, gv_socket_address& address,
	gv_packet*, gv_byte operation, gv_uint error_code);

typedef boost::function< void(gv_packet*) > gv_func_io_recv;
typedef boost::function< void(gv_packet*) > gv_func_io_error;

typedef void(GV_STDCALL* gv_callback_on_io_error)(
	class gvi_server_transport_layer_udp* server, gv_socket_address& address,
	gv_packet*, gv_byte operation, gv_uint error_code);
class gvi_server_transport_layer_udp
{
public:
	gvi_server_transport_layer_udp()
	{
		m_user_data = 0;
	};
	virtual ~gvi_server_transport_layer_udp(){};
	virtual bool create(gv_ushort port, gv_uint number_threads = 0,
						gv_uint number_workers = 0, gv_int start_recieves = 0,
						gv_int pending_recieves = 0)
	{
		return false;
	};
	virtual bool connect(const gv_socket_address& address)
	{
		return false;
	};
	virtual void destroy(){};
	virtual void update(){};
	// older callback API just for compatibility
	virtual void set_callback_on_recv(gv_callback_on_recv_complete function){};
	virtual void set_callback_on_error(gv_callback_on_io_error function)
	{
	}
	virtual void set_func_on_recv(const gv_func_io_recv& function)
	{
		m_func_recv = function;
	};
	virtual void set_func_on_error(gv_func_io_error function)
	{
		m_func_error = function;
	}
	virtual bool send_to(const gv_socket_address& address, int count,
						 gv_byte* data)
	{
		gv_packet* pk = new gv_packet(data, count, address);
		return send_to(address, pk);
	};
	virtual void* get_user_data()
	{
		return m_user_data;
	}
	virtual void set_user_data(void* d)
	{
		m_user_data = d;
	}
	virtual gv_packet* create_packet()
	{
		return new gv_packet;
	}
	virtual void destroy_packet(gv_packet* p)
	{
		delete p;
	};
	virtual bool send_to(const gv_socket_address& address, gv_packet* packet)
	{
		return false;
	};

protected:
	void* m_user_data;
	gv_func_io_recv m_func_recv;
	gv_func_io_error m_func_error;
};
}