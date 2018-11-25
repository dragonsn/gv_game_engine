#pragma once

namespace gv
{
//---------------------------------------------------------------
// can be completed customized ,old struct of the data is
//---------------------------------------------------------------
//|          |           |            |       |             |
//| USER_DATA|PROTOHEAD0 | PROTOHEAD1 | ...   | PACKET_DATA |
//---------------------------------------------------------------
//!! don't use complex structure for the header and user data, here only copy
//!the memory
//!!, no constructor
//---------------------------------------------------------------
// the packet class for network application ;
//
//!!user date can be used to contain some system information ,like iocp,epoll
//!handle
//	see iocp_udp_packet_factory for more detail , it will not be send
//through network

class gv_packet : public gvi_stream
{
	static const int gvc_max_proto_header_in_packet = 8;
	static const int buffer_size = gvc_default_mtu + 256;
	inline gv_packet(const gv_packet& p)
	{
		*this = p;
	}

public:
	GVM_OVERRIDE_NEW;

	gv_packet();

	gv_packet(const gv_byte* data, gv_int count, gv_socket_address address);

	virtual ~gv_packet()
	{
	}

	void reset();

	inline gv_int get_number_of_protocol_header()
	{
		return m_protocol_head_size_stack.size();
	}

	template < class type_of_header >
	inline gv_int push_protocol_header(const type_of_header& head,
									   bool copy = true,
									   bool need_resize = true)
	{
		gv_int s = sizeof(head);
		m_protocol_heads_size += s;
		m_protocol_head_size_stack.push(s);
		if (copy)
		{
			char* ps = (char*)&head;
			m_data.insert(ps, m_user_data_size + m_protocol_heads_size - s, s);
		}
		else if (need_resize)
		{
			m_data.resize(m_data.size() + s);
		}
		else
		{
			if (this->m_data.size() < this->get_packet_content_offset())
				return 0;
		}
		return m_protocol_head_size_stack.size();
	}

	template < class type_of_header >
	inline gv_int pop_protocol_header(const type_of_header& head,
									  bool do_erase = true)
	{
		gv_int s = sizeof(head);
		m_protocol_heads_size -= s;
		gv_int size_in_stack = m_protocol_head_size_stack.pop();
		GV_ASSERT(s == size_in_stack);
		if (do_erase)
			m_data.erase(m_user_data_size, s);
		else
			m_data.resize(m_data.size() - s);
		return s;
	}

	template < class type_of_header >
	inline type_of_header* get_protocol_header(int idx)
	{
		type_of_header* p;
		if (get_protocol_header(idx, p))
			return p;
		return NULL;
	}

	template < class type_of_header >
	inline bool get_protocol_header(int idx, type_of_header*& header_pointer)
	{
		if (idx == m_protocol_head_size_stack.size())
		{
			if (!this->push_protocol_header(*header_pointer, false, false))
				return false;
		}
		GV_ASSERT(idx < m_protocol_head_size_stack.size());
		GV_ASSERT(m_protocol_head_size_stack[idx] == sizeof(type_of_header));
		gv_int offset = m_user_data_size;
		for (int i = 0; i < idx; i++)
			offset += m_protocol_head_size_stack[i];
		header_pointer = (type_of_header*)(this->m_data.begin() + offset);
		return true;
	}

	inline gv_char* get_network_diagram_start()
	{
		return m_data.begin() + m_user_data_size;
	}

	inline gv_int get_network_diagram_size()
	{
		return m_protocol_heads_size + get_packet_content_size();
	}

	inline gv_bool test_network_diagram_size(gv_int size_to_add,
											 gv_int max_size = gvc_default_mtu)
	{
		return (get_network_diagram_size() + size_to_add + 1) < max_size;
	}

	inline gv_int get_default_mtu()
	{
		return gvc_default_mtu;
	}

	template < class type_of_user_data >
	inline bool set_user_data(const type_of_user_data& header,
							  bool do_memcpy = true)
	{
		const type_of_user_data* header_pointer = &header;
		gv_int old_size = this->m_user_data_size;
		this->m_user_data_size = sizeof(type_of_user_data);
		if (do_memcpy)
		{
			m_data.erase(0, old_size);
			m_data.insert((gv_char*)header_pointer, 0, m_user_data_size);
		}
		else
		{
			m_data.resize(m_data.size() - old_size + m_user_data_size);
		}
		return true;
	}

	template < class type_of_user_data >
	inline bool get_user_data(type_of_user_data*& header_pointer)
	{
		GV_ASSERT(this->m_user_data_size >= sizeof(type_of_user_data));
		header_pointer = (type_of_user_data*)this->m_data.begin();
		return true;
	}

	template < class type_of_user_data >
	inline type_of_user_data* get_user_data()
	{
		type_of_user_data* header_pointer;
		GV_ASSERT(this->m_user_data_size >= sizeof(type_of_user_data));
		header_pointer = (type_of_user_data*)this->m_data.begin();
		return header_pointer;
	}

	inline gv_char* get_packet_content()
	{
		return m_data.begin() + m_user_data_size + m_protocol_heads_size;
	}

	inline int get_packet_content_size()
	{
		return m_data.size() - m_user_data_size - m_protocol_heads_size;
	}
	inline int get_packet_content_offset()
	{
		return m_user_data_size + m_protocol_heads_size;
	}
	inline bool reserve_content_size(gv_int size)
	{
		this->m_data.resize(get_packet_content_offset() + size);
		return true;
	}
	inline gv_int reserve_max_content_size()
	{
		gv_int max_buffer_size = buffer_size - get_packet_content_offset();
		this->m_data.resize(buffer_size);
		return max_buffer_size;
	}

public:
	using gvi_stream::operator<<;
	using gvi_stream::operator>>;

	virtual gvi_stream& operator>>(gv_string& s);

	virtual gvi_stream& operator>>(gv_string_tmp& s);

	virtual gvi_stream& operator>>(const char* p)
	{
		return *this;
	}

	virtual gvi_stream& operator<<(const char* p)
	{
		return *this;
	}

	virtual void flush(){};

	virtual gv_int read(void* pdata, gv_int isize);

	virtual gv_int write(const void* pdata, gv_int isize);

	virtual gv_int tell()
	{
		return m_cu_point;
	};

	virtual bool eof()
	{
		return m_cu_point == m_data.size();
	};

	virtual bool is_size_counter()
	{
		return false;
	}

private:
	virtual bool open(const char* name)
	{
		return false;
	};

	virtual bool close()
	{
		return false;
	};

	virtual gv_int size()
	{
		return m_data.size();
	};

	virtual gv_int read_all(gvt_array< gv_byte >& data)
	{
		int file_size = size();
		data.resize(file_size);
		read(data.begin(), file_size);
		return data.size();
	}

	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir);

public:
	gv_ulong m_time_stamp;
	gv_socket_address m_address; // remote address
	gv_uint m_user_param;
	gv_uint m_sequence_number;
	gv_uint m_resend_times;
	gv_ushort m_error_code;
	gv_packet* next; // used for lock free list !queue packets .
	gvt_ref_ptr< gv_packet > m_next_ref;

protected:
	typedef gvt_array_cached< gv_char, buffer_size > packet_buffer;
	typedef gvt_stack_static< gv_int, gvc_max_proto_header_in_packet >
		packet_protocol_stack;
	gv_int m_user_data_size; // not for send , some control information >
	gv_int
		m_protocol_heads_size;						  // protocol header, include in the network diagram
	gv_int m_cu_point;								  // write,read current point
	packet_buffer m_data;							  // actual data
	packet_protocol_stack m_protocol_head_size_stack; // protocol info
};

inline bool operator<(const gvt_ref_ptr< gv_packet >& p1,
					  const gvt_ref_ptr< gv_packet >& p2)
{
	return p1->m_sequence_number < p2->m_sequence_number;
}

class gv_packet_size_counter : public gv_packet
{
public:
	gv_packet_size_counter()
	{
		m_size = 0;
	};

	~gv_packet_size_counter(){};

	gv_int get_size()
	{
		return m_size;
	}

	virtual bool is_size_counter()
	{
		return true;
	}

private:
	virtual gv_int read(void* pdata, gv_int isize)
	{
		m_size += isize;
		return m_size;
	};

	virtual gv_int write(const void* pdata, gv_int isize)
	{
		m_size += isize;
		return isize;
	};
	gv_int m_size;
};

template < class T >
inline gv_int gvt_size_in_packet(const T& t)
{
	gv_packet_size_counter p;
	p << t;
	return p.get_size();
}
}
