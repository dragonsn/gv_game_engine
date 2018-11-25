
namespace gv
{
// idea mnet server
// UDP based
// with resend machanisim .
namespace gv_asio
{

// virtual			~gv_asio_udp_server()	{};
// virtual bool	create(gv_ushort port, gv_uint number_threads = 0, gv_uint
// number_workers = 0, gv_int start_recieves = 0, gv_int pending_recieves = 0);
// virtual void	destroy()	{};
// virtual void	update()	{};
// virtual bool	send_to(const gv_socket_address & address, int count,
// gv_byte  * data);
using namespace boost::asio::ip;
udp::endpoint to_asio(const gv_socket_address& address)
{
	gv_ip_address_ipv4 a = address.get_address();
	boost::asio::ip::address_v4 addr;
	gv_string_tmp s;
	s << a;
	addr = boost::asio::ip::address_v4::from_string(*s);
	return udp::endpoint(addr, address.get_port());
}

gv_socket_address from_asio(const udp::endpoint& end)
{
	gv_string_tmp s;
	s << end.address().to_string().c_str();
	s << ":" << end.port();
	gv_socket_address a;
	a.from_string(s);
	return a;
}

class udp_node : public gv_refable
{
public:
	udp_node(short port)
		: io_service(), socket_(io_service, udp::endpoint(udp::v4(), port)),
		  tick_timer(io_service)
	{
		is_sending = false;
		is_recieving = false;
		is_quitting = false;
		packet_send_list = NULL;
		if (port)
			receive_one_packet();
		tick_timer.expires_from_now(boost::posix_time::milliseconds(100));
		tick_timer.async_wait(boost::bind(&udp_node::tick, this));
	}
	void post_packet_for_send(gv_packet* packet)
	{
		gvt_lock_free_list_push(packet, posted_packets_for_send);
	}
	void run()
	{
		io_service.run();
		GVM_DEBUG_OUT("asio udp node stoped");
	}
	void stop()
	{

		is_quitting = true;
		packet_send_list = gvt_lock_free_list_pop_all(posted_packets_for_send);
		while (packet_send_list)
		{
			is_sending = true;
			gv_packet* packet = packet_send_list;
			packet_send_list = packet->next;
			delete packet;
		}

		packet_send_list = gvt_lock_free_list_pop_all(received_packets);
		while (packet_send_list)
		{
			is_sending = true;
			gv_packet* packet = packet_send_list;
			packet_send_list = packet->next;
			packet->dec_ref();
		}

		packet_send_list = NULL;
		socket_.cancel();
		tick_timer.cancel();
		io_service.stop();
	
		//!! SN TODO there is memory leak here, if there is still packages waiting for data.! the new asio will not call the recieve callback
	/*	while (is_quitting)
		{
			gv_thread::sleep(1);
		}*/
	}
	void tick()
	{
		try_send_pending_packet();
		tick_timer.expires_from_now(boost::posix_time::milliseconds(100));
		tick_timer.async_wait(boost::bind(&udp_node::tick, this));
	}

protected:
	void receive_one_packet()
	{
		if (is_recieving)
			return;
		is_recieving = true;
		gv_packet* packet = new gv_packet;
		packet->inc_ref();
		packet->reserve_max_content_size();
		socket_.async_receive_from(
			boost::asio::buffer(packet->get_network_diagram_start(),
								gvc_default_mtu),
			sender_address,
			boost::bind(&udp_node::handle_receive_from, this, packet,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}
	void handle_receive_from(gvt_ref_ptr< gv_packet > packet,
							 const boost::system::error_code& error,
							 size_t bytes_recvd)
	{
		is_recieving = false;
		if (is_quitting)
		{
			packet->dec_ref();
			is_quitting = false;
			return;
		}
		if (!error)
		{
			// sending pending packets.
			packet->m_address = from_asio(sender_address);
			packet->reserve_content_size((gv_int)bytes_recvd);
			gvt_lock_free_list_push(packet.ptr(), received_packets);
		}
		else
		{
			packet->dec_ref();
			GVM_DEBUG_OUT("[net] recieve udp network error net_error_code=" )
			// TODO if (func_io_error) func_io_error(NULL,)
		}
		// continue to receive
		receive_one_packet();
		try_send_pending_packet();
	}
	bool try_send_pending_packet()
	{
		if (is_sending)
			return true;
		if (!packet_send_list)
		{
			packet_send_list = gvt_lock_free_list_pop_all(posted_packets_for_send);
		}
		if (packet_send_list)
		{
			is_sending = true;
			gv_packet* packet = packet_send_list;
			packet_send_list = packet->next;
			udp::endpoint end_point = to_asio(packet->m_address);
			socket_.async_send_to(
				boost::asio::buffer(packet->get_network_diagram_start(),
									packet->get_network_diagram_size()),
				end_point, boost::bind(&udp_node::handle_send_to, this, packet,
									   boost::asio::placeholders::error,
									   boost::asio::placeholders::bytes_transferred));
			return true;
		}
		packet_send_list = NULL;
		return false;
	}
	void handle_send_to(gvt_ref_ptr< gv_packet > packet,
						const boost::system::error_code& error,
						size_t bytes_sent)
	{
		is_sending = false;
		if (!error)
		{
			// sending pending packetes.
			try_send_pending_packet();
		}
		else
		{
			// error in sending!!
			packet->m_error_code = error.value();
			GVM_DEBUG_OUT("[net] recieve udp network error net_error_code=" )
		}
		receive_one_packet();
	}

public:
	gvt_lock_free_header< gv_packet* > received_packets;
	gv_func_io_error func_io_error;

private:
	boost::asio::io_service io_service;
	bool is_sending;
	bool is_recieving;
	bool is_quitting;
	udp::socket socket_;
	udp::endpoint sender_address;
	gv_packet* packet_send_list;
	gvt_lock_free_header< gv_packet* > posted_packets_for_send;
	boost::asio::deadline_timer tick_timer;
};
}
class gv_asio_udp_server_imp
{
public:
	gv_asio_udp_server_imp()
	{
	}
	void run()
	{
		io_thread.start(boost::bind(&gv_asio::udp_node::run, m_node.ptr()));
	}
	void stop()
	{
		m_node->stop();
	}
	gvt_ref_ptr< gv_asio::udp_node > m_node;
	gv_thread io_thread;
};

gv_asio_udp_server::gv_asio_udp_server(){
	// this->m_port = new gv_asio::port;
};
gv_asio_udp_server::~gv_asio_udp_server(){};
bool gv_asio_udp_server::create(gv_ushort port, gv_uint, gv_uint, gv_int,
								gv_int)
{
	m_imp = new gv_asio_udp_server_imp;
	m_imp->m_node = new gv_asio::udp_node(port);
	m_imp->run();
	return true;
};
void gv_asio_udp_server::destroy()
{
	m_imp->stop();
	gvt_safe_delete(m_imp);
};
void gv_asio_udp_server::update()
{
	gv_packet* p = gvt_lock_free_list_pop_all(m_imp->m_node->received_packets);
	while (p)
	{
		if (m_func_recv)
		{
			m_func_recv(p);
		}
		p = p->next;
	}
};
bool gv_asio_udp_server::send_to(const gv_socket_address& address,
								 gv_packet* packet)
{
	packet->m_address = address;
	m_imp->m_node->post_packet_for_send(packet);
	return true;
};
}