#include <cstdlib>
#include <iostream>
#include <boost/boost/aligned_storage.hpp>
#include <boost/boost/array.hpp>
#include <boost/boost/bind.hpp>
#include <boost/boost/enable_shared_from_this.hpp>
#include <boost/boost/noncopyable.hpp>
#include <boost/boost/shared_ptr.hpp>
#include "../network/gv_asio.h"

namespace unit_test_asio_udp
{
using namespace boost::asio::ip;
void on_receive(gv_asio_udp_server* srv, const char* role, const char* response, gv_packet* p)
{
	static gv_atomic_count a;
	gv_packet* p2 = new gv_packet;
	gv_string_tmp s;
	(*p) >> s;
	GVM_CONSOLE_OUT(role << " recv text: " << s);
	s = "";
	s << response << a++;
	(*p2) << s;
	srv->send_to(p->m_address, p2);
	p->dec_ref();
}

void main(gvt_array< gv_string >& args)
{
	try
	{
		gv_asio_udp_server s;
		gv_asio_udp_server c;
		int port = 8903;
		s.set_func_on_recv(boost::bind(&unit_test_asio_udp::on_receive, &s, "srv ", " hehe ", _1));
		c.set_func_on_recv(boost::bind(&unit_test_asio_udp::on_receive, &c, "client ", " WOW  ", _1));
		s.create(port);
		c.create(0);
		gv_string_tmp hello("hello!!! I am your friend");
		gv_packet* p = new gv_packet;
		(*p) << hello;
		c.send_to(gv_socket_address(port), p);
		int loop_count = 1000;
		while (loop_count--)
		{
			s.update();
			c.update();
			gv_thread::sleep(5);
		}
		s.destroy();
		c.destroy();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}
}
