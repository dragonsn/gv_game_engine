#pragma once

namespace gv
{
// the ip address , already in network byte order!.
class gv_ip_address_ipv4 : public gv_vector4b
{
	friend class gv_socket;
	friend class gv_socket_address;

public:
	gv_ip_address_ipv4()
		: gv_vector4b(127, 0, 0, 1)
	{
	}
	explicit gv_ip_address_ipv4(const char* name)
	{
		get_host_by_name(name);
	}
	gv_ip_address_ipv4(gv_byte a, gv_byte b, gv_byte c, gv_byte d)
		: gv_vector4b(a, b, c, d)
	{
	}
	inline gv_uint get_addr_u32_net_byte_order() const
	{
		return *(gv_uint*)this;
	};

	inline void set_addr_u32_net_byte_order(gv_uint addr)
	{
		(*(gv_uint*)this) = addr;
	};

	void parse(const char* s);
	void get_host_by_name(const char* s);
	gv_ushort get_addr_family();

public:
	inline gv_bool is_local_host()
	{
		return (*this) == addr_local_host();
	}
	static const gv_ip_address_ipv4& addr_wild_card()
	{
		static gv_ip_address_ipv4 the_addr = gv_ip_address_ipv4(0, 0, 0, 0);
		return the_addr;
	}
	static const gv_ip_address_ipv4& addr_broadcast()
	{
		static gv_ip_address_ipv4 the_addr = gv_ip_address_ipv4(255, 255, 255, 255);
		return the_addr;
	}
	static const gv_ip_address_ipv4& addr_none()
	{
		static gv_ip_address_ipv4 the_addr = gv_ip_address_ipv4(255, 255, 255, 255);
		return the_addr;
	}
	static const gv_ip_address_ipv4& addr_loop_back()
	{
		static gv_ip_address_ipv4 the_addr = gv_ip_address_ipv4(127, 0, 0, 1);
		return the_addr;
	}
	static const gv_ip_address_ipv4& addr_local_host()
	{
		return addr_loop_back();
	}
};

template < class type_of_string >
inline type_of_string& operator<<(type_of_string& ss,
								  const gv_ip_address_ipv4& s)
{
	ss << s.x << "." << s.y << "." << s.z << "." << s.w;
	return ss;
}

template < class type_of_string >
inline const type_of_string& operator>>(type_of_string& ss,
										gv_ip_address_ipv4& s)
{
	s.parse(*ss);
	return ss;
}

// TO DO:ipv6
}