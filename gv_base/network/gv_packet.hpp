
namespace gv
{
GVM_IMP_POOLED(gv_packet, 2048);

gv_packet::gv_packet()
{
	m_user_data_size = m_protocol_heads_size = m_cu_point = 0;
	m_user_param = 0;
	m_sequence_number = 0;
	m_resend_times = 0;
	m_error_code = 0;
	next = NULL;
}

gv_packet::gv_packet(const gv_byte* data, gv_int count,
					 gv_socket_address address)
{
	m_user_data_size = m_protocol_heads_size = m_cu_point = 0;
	m_user_param = 0;
	m_sequence_number = 0;
	m_resend_times = 0;
	m_error_code = 0;
	next = NULL;
	m_address = address;
	reserve_content_size(count);
	memcpy(get_network_diagram_start(), data, count);
	m_time_stamp = gv_global::time->time_stamp_unix();
}

void gv_packet::reset()
{
	m_user_data_size = m_protocol_heads_size = m_cu_point = 0;
	m_data.clear();
	m_protocol_head_size_stack.pop_all();
}

// stream operator
gvi_stream& gv_packet::operator>>(gv_string& s)
{
	gv_int str_size;
	(*this) >> str_size;
	s.reserve(str_size);
	s.resize(str_size);
	if (str_size)
	{
		read(s.begin(), str_size);
		if (gvt_strlen(*s) != str_size - 1)
		{
			GVM_DEBUG_LOG(net, "bad string in packet \r\n");
			s.clear(); // bad string
		}
	}
	return *this;
};
gvi_stream& gv_packet::operator>>(gv_string_tmp& s)
{
	gv_int str_size;
	(*this) >> str_size;
	s.reserve(str_size);
	s.resize(str_size);
	if (str_size)
	{
		read(s.begin(), str_size);
		if (gvt_strlen(*s) != str_size - 1)
			s.clear(); // wrong string
	}
	return *this;
};

gv_int gv_packet::read(void* pdata, gv_int isize)
{
	int size =
		gvt_min(m_data.size() - m_cu_point - get_packet_content_offset(), isize);
	memcpy(pdata, get_packet_content() + m_cu_point, size);
	m_cu_point += size;
	return size;
};
gv_int gv_packet::write(const void* pdata, gv_int isize)
{
	if (m_cu_point + get_packet_content_offset() + isize > m_data.size())
		m_data.resize(m_cu_point + get_packet_content_offset() + isize);
	memcpy(get_packet_content() + m_cu_point, pdata, isize);
	m_cu_point += isize;
	return isize;
};
bool gv_packet::seek(gv_uint pos, std::ios_base::seekdir dir)
{
	switch (dir)
	{
	case std::ios_base::beg:
		m_cu_point = pos;
		break;
	case std::ios_base::end:
		m_cu_point = get_packet_content_size() - pos - 1;
		break;
	case std::ios_base::cur:
		m_cu_point += pos;
		break;
	}
	m_cu_point = gvt_clamp(m_cu_point, 0, get_packet_content_size() - 1);
	return true;
}
}