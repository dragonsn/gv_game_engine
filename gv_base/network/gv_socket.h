#pragma once
namespace gv
{
typedef gv_ushort gv_socket_port;
typedef gv_uint_ptr gv_socket_handle;
struct gv_system_sockaddr_in;
struct gv_system_sockaddr;
static const gv_uint gvc_max_listen_connection = 0x7fffffff; // SOMAXCONN

class gv_socket_address
{
	friend class gv_socket;
	friend gv_uint gvt_hash(const gv_socket_address& addr);

public:
	gv_socket_address()
	{
		GVM_ZERO_ME;
		set_port(8080); // default  port
		m_sin_family = m_sin_addr.get_addr_family();
	};

	gv_socket_address(const gv_ip_address_ipv4& ip, gv_socket_port port)
	{
		GVM_ZERO_ME;
		m_sin_family = m_sin_addr.get_addr_family();
		m_sin_addr = ip;
		set_port(port);
	}

	explicit gv_socket_address(gv_socket_port port)
	{
		GVM_ZERO_ME;
		m_sin_family = m_sin_addr.get_addr_family();
		m_sin_addr = gv_ip_address_ipv4::addr_local_host();
		set_port(port);
	};

public:
	// os & API depency
	gv_system_sockaddr* system_socket_addr();
	const gv_system_sockaddr* system_socket_addr() const;
	gv_socket_port get_port() const;
	void set_port(gv_socket_port);

public:
	inline void set(const gv_ip_address_ipv4& address, gv_socket_port port)
	{
		set_address(address);
		set_port(port);
	}
	inline int size() const
	{
		return sizeof(gv_socket_address);
	}
	inline const gv_ip_address_ipv4& get_address() const
	{
		return this->m_sin_addr;
	};
	inline void set_address(const gv_ip_address_ipv4& vp)
	{
		this->m_sin_addr = vp;
	};

	inline bool from_string(const gv_string_tmp& s)
	{

		gv_uint a, b, c, d, port;
		int nb = sscanf(*s, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port);
		if (nb != 5)
			return false;
		this->set_address(
			gv_ip_address_ipv4((gv_byte)a, (gv_byte)b, (gv_byte)c, (gv_byte)d));
		this->set_port((gv_ushort)port);
		return true;
	};
	inline gv_string_tmp string() const
	{
		gv_string_tmp s;
		s << m_sin_addr << ":" << get_port();
		return s;
	};
	inline bool operator==(const gv_socket_address& address) const
	{
		if (m_sin_addr != address.m_sin_addr)
			return false;
		if (m_sin_port != address.m_sin_port)
			return false;
		return true;
	}
	inline bool operator!=(const gv_socket_address& address)
	{
		return !(*this == address);
	}
	inline bool operator<(const gv_socket_address& address)
	{
		if (m_sin_addr < address.m_sin_addr)
			return true;
		if (m_sin_port < address.m_sin_port)
			return true;
		return false;
	}

public:
	gv_short m_sin_family;
	gv_socket_port m_sin_port;
	gv_ip_address_ipv4 m_sin_addr;
	gv_char m_sin_zero[8];
};

enum e_socket_type
{
	e_socket_type_stream = 1, // SOCK_STREAM
	e_socket_type_datagram,   // SOCK_DGRAM
	e_socket_type_raw,		  // SOCK_RAW
	e_socket_type_reliable,   // SOCK_RDM
	e_socket_type_seq_packet, // SOCK_SEQPACKET
};

enum e_socket_proto
{
	e_socket_proto_tcp = 6, // IPPROTO_TCP           = 6,
	e_socket_proto_udp = 17 // IPPROTO_UDP           = 17,
};

// used for tracking
enum e_socket_state
{
	e_socket_state_not_inited,
	e_socket_state_inited,
	e_socket_state_binded,
	e_socket_state_listened,
	e_socket_state_accepted,
	e_socket_state_sending,
	e_socket_state_recving,
	e_socket_state_closed,
};

static const int gvc_default_mtu = 1400;
class gv_socket
{
public:
	gv_socket();
	gv_socket(const gv_socket& socket);
	gv_socket(gv_socket_handle);
	virtual ~gv_socket();
	gv_socket& operator=(const gv_socket& socket);
	int error()
	{
		return -1;
	};
	void init_socket(e_socket_type type, e_socket_proto proto,
					 bool use_overlapping = false);
	void close();
	virtual bool bind(gv_socket_address&, bool reuse_address = false,
					  bool reuse_port = true);
	virtual bool listen(int backlog = 64);
	virtual int send(const void* buffer, int length, int flags = 0);
	virtual int send_all(const void* buffer, int length);
	virtual int receive(void* buffer, int length, int flags = 0);
	virtual int receive_all(void* buffer, int length, int flags = 0);
	virtual int send_to(const void* buffer, int length,
						const gv_socket_address& address, int flags = 0);
	virtual int receive_from(void* buffer, int length, gv_socket_address& address,
							 int flags = 0);
	virtual gv_socket* accept();
	virtual bool connect(const gv_socket_address& address);
	virtual gv_int readable_time_out(gv_int milliseconds);

	static int last_error();

	gv_socket_handle get_socket_handle() const;

	void opt_set_send_buffer_size(int size);
	int opt_get_send_buffer_size() const;
	void opt_set_receive_buffer_size(int size);
	int opt_get_receive_buffer_size() const;
	// SO_SNDTIMEO
	void opt_set_send_timeout(int milliseconds);
	int opt_get_send_timeout() const;
	// SO_RCVTIMEO
	void opt_set_receive_timeout(int milliseconds);
	int opt_get_receive_timeout() const;
	// SO_LINGER
	void opt_set_linger(bool on, int seconds);
	void opt_get_linger(bool& on, int& seconds) const;
	// TCP_NODELAY socket option.
	void opt_set_no_delay(bool flag);
	bool opt_get_no_delay() const;
	// SO_KEEPALIVE
	void opt_set_keep_alive(bool flag);
	bool opt_get_keep_alive() const;
	// SO_REUSEADDR
	void opt_set_reuse_address(bool flag);
	bool opt_get_reuse_address() const;
	// SO_REUSEPORT
	void opt_set_reuse_port(bool flag);
	bool opt_get_reuse_port() const;
	// blocking use
	void opt_set_blocking(bool flag);
	bool opt_get_blocking() const;
	// SO_BROADCAST
	void opt_set_broadcast(bool flag);
	bool opt_get_broadcast() const;

	const gv_socket_address& get_host_address() const;
	gv_socket_address get_peer_address() const;
	e_socket_state get_socket_state() const;

protected:
	gv_socket_address m_host_address;
	gv_socket_address m_peer_address;
	gv_socket_handle m_handle;
	e_socket_state m_state;
	bool m_use_blocking;
};

inline gv_uint gvt_hash(const gv_socket_address& addr)
{
	return (gv_uint)(addr.m_sin_port +
					 addr.m_sin_addr.get_addr_u32_net_byte_order());
}
}