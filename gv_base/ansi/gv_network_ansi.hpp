
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#define closesocket(x) shutdown(x, SHUT_RDWR)
#define ioctlsocket ioctl
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
typedef socklen_t SOCKLEN;
typedef signed int SOCKET;
typedef sockaddr_in SOCKADDR_IN;
typedef struct timeval TIMEVAL;

namespace gv
{
struct gv_system_in_addr : public in_addr
{
};
struct gv_system_sockaddr : public sockaddr
{
};
};

#include "gv_ip_address_ansi.hpp"

namespace gv
{

GV_STATIC_ASSERT(sizeof(gv_ip_address_ipv4) == sizeof(gv_system_in_addr));
GV_STATIC_ASSERT(sizeof(gv_socket_handle) == sizeof(SOCKET));
//==============================================
//============gv_network========================
//==============================================
gv_network::gv_network()
{
}
gv_network::~gv_network()
{
}

//==============================================
//============gv_socket=========================
//==============================================

GV_STATIC_ASSERT(e_socket_type_stream == SOCK_STREAM);
GV_STATIC_ASSERT(e_socket_type_datagram == SOCK_DGRAM);
GV_STATIC_ASSERT(e_socket_type_raw == SOCK_RAW);
GV_STATIC_ASSERT(e_socket_type_reliable == SOCK_RDM);
GV_STATIC_ASSERT(e_socket_type_seq_packet == SOCK_SEQPACKET);

GV_STATIC_ASSERT(e_socket_proto_tcp == (int)IPPROTO_TCP);
GV_STATIC_ASSERT(e_socket_proto_udp == (int)IPPROTO_UDP);

template < class T >
inline void gvt_set_sockopt(gv_socket_handle s, int lev, int opt, const T& t)
{
	::setsockopt(s, lev, opt, (char*)&t, sizeof(T));
};

template < class T >
inline void gvt_get_sockopt(gv_socket_handle s, int lev, int opt, T& t)
{
	int size = sizeof(t);
	::getsockopt(s, lev, opt, (char*)&t, &size);
	GV_ASSERT(size == sizeof(t));
}

gv_socket::gv_socket()
{
	m_state = e_socket_state_not_inited;
	m_handle = INVALID_SOCKET;
};
gv_socket::gv_socket(gv_socket_handle handle)
{
	// only used in accept , so the
	m_state = e_socket_state_binded;
	m_handle = handle;
};
gv_socket::gv_socket(const gv_socket& socket)
{
	(*this) = socket;
};
gv_socket::~gv_socket(){

};
gv_socket& gv_socket::operator=(const gv_socket& socket)
{
	this->m_handle = socket.m_handle;
	this->m_host_address = socket.m_host_address;
	this->m_state = socket.m_state;
	return *this;
};
void gv_socket::init_socket(e_socket_type type, e_socket_proto proto,
							bool use_overlapping)
{
	m_handle = socket(AF_INET, type, proto);
	GV_ASSERT(m_handle != INVALID_SOCKET);
	m_state = e_socket_state_inited;
};

void gv_socket::close()
{
	if (m_handle != INVALID_SOCKET)
		closesocket(m_handle);
	m_handle = INVALID_SOCKET;
	m_state = e_socket_state_closed;
};

bool gv_socket::bind(gv_socket_address& address, bool reuse_address,
					 bool reuse_port)
{
	GV_ASSERT(m_handle != INVALID_SOCKET);
	this->m_host_address = address;
	int bindResult = ::bind(m_handle, m_host_address.system_socket_addr(),
							sizeof(m_host_address));
	if (bindResult == SOCKET_ERROR)
	{
		GVM_DEBUG_OUT("failed to  bind the  socket to address "
					  << address.string() << "  error returned " << last_error());
		close();
		return false;
	}
	this->m_state = e_socket_state_binded;
	return true;
};

bool gv_socket::listen(int backlog)
{
	GV_ASSERT(m_handle != INVALID_SOCKET);
	int rc = ::listen(m_handle, backlog);
	if (rc != 0)
	{
		GVM_DEBUG_OUT("socket failed to listen!");
		return false;
	}
	this->m_state = e_socket_state_listened;
	return true;
};

int gv_socket::send(const void* buffer, int length, int flags)
{
	if (m_handle == INVALID_SOCKET)
		return -1;
	int size = ::send(m_handle, (const char*)buffer, length, flags);
	GV_ASSERT(size == length); // use
	this->m_state = e_socket_state_sending;
	return size;
};

int gv_socket::send_all(const void* buffer, int length)
{
	// Beej's Guide to Network Programming 41
	char* buf = (char*)buffer;
	int total = 0;			// how many bytes we've sent
	int bytesleft = length; // how many we have left to send
	int n;
	while (total < length)
	{
		n = ::send(m_handle, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}
	this->m_state = e_socket_state_sending;
	return total; // return number actually sent here
}

int gv_socket::receive(void* buffer, int length, int flags)
{
	if (m_handle == INVALID_SOCKET)
		return -1;
	int size = ::recv(m_handle, (char*)buffer, length, flags);
	this->m_state = e_socket_state_recving;
	return size;
};

int gv_socket::send_to(const void* buffer, int length,
					   const gv_socket_address& address, int flags)
{
	if (m_handle == INVALID_SOCKET)
		return -1;
	int size = ::sendto(m_handle, (const char*)buffer, length, flags,
						address.system_socket_addr(), sizeof(address));
	this->m_state = e_socket_state_sending;
	return size;
};

int gv_socket::receive_from(void* buffer, int length,
							gv_socket_address& address, int flags)
{
	if (m_handle == INVALID_SOCKET)
		return -1;
	int sa_len = address.size();
	int size = ::recvfrom(m_handle, (char*)buffer, length, flags,
						  address.system_socket_addr(), &sa_len);
	this->m_state = e_socket_state_recving;
	return size;
};

gv_int gv_socket::readable_time_out(gv_int timeout)
{
	fd_set rset;
	struct timeval timeVal;
	FD_ZERO(&rset);
	FD_SET(this->get_socket_handle(), &rset);
	timeVal.tv_sec = timeout / 1000;
	timeVal.tv_usec = (timeout % 1000) * 1000;
	gv_int ret = select(get_socket_handle() + 1, &rset, NULL, NULL, &timeVal);
	if (ret && ret != SOCKET_ERROR)
		return ret;
	else
		return 0;
};

gv_socket* gv_socket::accept()
{
	GV_ASSERT(m_handle != INVALID_SOCKET);
	gv_socket_address address;
	int saLen = address.size();
	gv_socket* pnew = NULL;
	gv_socket_handle new_hd =
		::accept(m_handle, address.system_socket_addr(), &saLen);
	if (new_hd != INVALID_SOCKET)
	{
		pnew = new gv_socket(new_hd);
		pnew->m_peer_address = address;
		pnew->m_host_address = this->m_host_address;
	}
	this->m_state = e_socket_state_accepted;
	return pnew;
};

bool gv_socket::connect(const gv_socket_address& address)
{
	GV_ASSERT(m_handle != INVALID_SOCKET);
	int rc = ::connect(m_handle, address.system_socket_addr(), address.size());
	if (rc != 0)
	{
		GVM_DEBUG_OUT("error to connect" << address.string());
		return false;
	}
	return true;
};

int gv_socket::receive_all(void* buffer, int length, int flags)
{
	int n = 0;
	char* ptr = (char*)buffer;
	while (n < length)
	{
		int recvd = receive(ptr + n, length - n, flags);
		if (recvd < 0)
		{
			/* Error */
			return -1;
		}
		else if (!recvd)
			break;
		n += recvd;
	}
	return n;
}

gv_socket_handle gv_socket::get_socket_handle() const
{
	return m_handle;
};
const gv_socket_address& gv_socket::get_host_address() const
{
	return m_host_address;
};
gv_socket_address gv_socket::get_peer_address() const
{
	return m_peer_address;
};

void gv_socket::opt_set_send_buffer_size(int size)
{
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_SNDBUF, size);
};

int gv_socket::opt_get_send_buffer_size() const
{
	int size;
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_SNDBUF, size);
	return size;
};

void gv_socket::opt_set_receive_buffer_size(int size)
{
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_RCVBUF, size);
};

int gv_socket::opt_get_receive_buffer_size() const
{
	int size;
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_RCVBUF, size);
	return size;
};

// SO_SNDTIMEO
void gv_socket::opt_set_send_timeout(int milliseconds)
{
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_SNDTIMEO, milliseconds);
};

int gv_socket::opt_get_send_timeout() const
{
	int value;
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_SNDTIMEO, value);
	return value;
};

// SO_RCVTIMEO
void gv_socket::opt_set_receive_timeout(int milliseconds)
{
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_RCVTIMEO, milliseconds);
};

int gv_socket::opt_get_receive_timeout() const
{
	int value;
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_RCVTIMEO, value);
	return value;
};

// SO_LINGER
void gv_socket::opt_set_linger(bool on, int seconds)
{
	struct linger l;
	l.l_onoff = on ? 1 : 0;
	l.l_linger = (u_short)seconds;
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_LINGER, l);
};

void gv_socket::opt_get_linger(bool& on, int& seconds) const
{
	struct linger l;
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_LINGER, l);
	on = l.l_onoff != 0;
	seconds = l.l_linger;
};

// TCP_NODELAY socket option.
void gv_socket::opt_set_no_delay(bool flag)
{
	int value = flag ? 1 : 0;
	gvt_set_sockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, value);
};

bool gv_socket::opt_get_no_delay() const
{
	int value(0);
	gvt_get_sockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, value);
	return value != 0;
};

// SO_KEEPALIVE
void gv_socket::opt_set_keep_alive(bool flag)
{
	int value = flag ? 1 : 0;
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_RCVBUF, value);
};

bool gv_socket::opt_get_keep_alive() const
{
	int value(0);
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_KEEPALIVE, value);
	return value != 0;
};

// SO_REUSEADDR
void gv_socket::opt_set_reuse_address(bool flag)
{
	int value = flag ? 1 : 0;
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, value);
};

bool gv_socket::opt_get_reuse_address() const
{
	int value(0);
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, value);
	return value != 0;
};

// SO_REUSEPORT
void gv_socket::opt_set_reuse_port(bool flag){
	// not in win32
};
bool gv_socket::opt_get_reuse_port() const
{
	return false;
};

// blocking use
void gv_socket::opt_set_blocking(bool flag)
{
	u_long arg = flag ? 0 : 1;
	ioctlsocket(m_handle, FIONBIO, &arg);
	m_use_blocking = flag;
};

bool gv_socket::opt_get_blocking() const
{
	return m_use_blocking;
};

// SO_BROADCAST
void gv_socket::opt_set_broadcast(bool flag)
{
	int value = flag ? 1 : 0;
	gvt_set_sockopt(m_handle, SOL_SOCKET, SO_BROADCAST, value);
};

bool gv_socket::opt_get_broadcast() const
{
	int value(0);
	gvt_get_sockopt(m_handle, SOL_SOCKET, SO_BROADCAST, value);
	return value != 0;
};

int gv_socket::last_error()
{
	return errno;
}
};