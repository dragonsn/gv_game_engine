namespace gv
{
//==============================================
//============gv_ip_address=====================
//==============================================
void gv_ip_address_ipv4::get_host_by_name(const char* name)
{
	hostent* he = gethostbyname(name);
	if (he == NULL || he->h_addrtype != AF_INET)
	{
		gv_uint host = inet_addr(name);
		if (host == INADDR_NONE)
			return;
		this->set_addr_u32_net_byte_order(host);
		return;
	}
	this->set_addr_u32_net_byte_order(*(gv_uint*)he->h_addr_list[0]);
};

gv_ushort gv_ip_address_ipv4::get_addr_family()
{
	return AF_INET;
};

void gv_ip_address_ipv4::parse(const char* name)
{
	gv_uint host = inet_addr(name);
	this->set_addr_u32_net_byte_order(host);
}

//==============================================
//============gv_socket_address=================
//==============================================
gv_system_sockaddr* gv_socket_address::system_socket_addr()
{
	return (gv_system_sockaddr*)this;
};

const gv_system_sockaddr* gv_socket_address::system_socket_addr() const
{
	return (const gv_system_sockaddr*)this;
};
gv_socket_port gv_socket_address::get_port() const
{
	return ntohs(m_sin_port);
};
void gv_socket_address::set_port(gv_socket_port port)
{
	this->m_sin_port = htons(port);
};
}