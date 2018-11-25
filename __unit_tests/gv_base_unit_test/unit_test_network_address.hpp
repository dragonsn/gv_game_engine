namespace unit_test_network_address
{
void main(gvt_array< gv_string >& args)
{
	{
		gv_ip_address_ipv4 addr1(127, 0, 0, 1);
		gv_ip_address_ipv4 addr2("127.0.0.1");
		GV_ASSERT(addr1 == addr2)
	}
	{
		const char* test_address[] =
			{
				"www.sina.com.cn",
				"www.163.com",
				"www.yahoo.com.cn",
				"www.google.com.cn",
			};
		for (int i = 0; i < gvt_array_length(test_address); i++)
		{
			gv_ip_address_ipv4 ip(test_address[i]);
			gv_string_tmp s;
			s << test_address[i] << "===>" << ip;
			test_log() << *s << gv_endl;
		};
	}
	{
		gv_ip_address_ipv4 ip = gv_ip_address_ipv4::addr_local_host();
		gv_string_tmp s;
		s << ip;
		gv_ip_address_ipv4 ip2(*s);
		GV_ASSERT(ip == ip2);
	}
}
}
