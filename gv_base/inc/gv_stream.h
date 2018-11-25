#pragma once
// use a interface to polymorph the stream
#include "gv_raw_data.h"
namespace gv
{
class gvi_stream : public gv_refable
{
public:
	// operations!!
	friend class gv_file_manager;

	gvi_stream()
	{
		m_endian_swap = false;
		m_user_data = NULL;
	};
	virtual ~gvi_stream(){};

	virtual bool open(const char* name)
	{
		return false;
	};
	virtual bool close()
	{
		return false;
	}

	template < class T >
	inline bool write_any(T& t)
	{
		if (!is_endian_swap_needed())
			return write(&t, sizeof(t)) == sizeof(t);
		else
		{
			T cpy = t;
			gvt_swap_endian(cpy);
			return write(&cpy, sizeof(cpy)) == sizeof(cpy);
		}
	};
	template < class T >
	inline bool read_any(T& t)
	{
		if (!is_endian_swap_needed())
			return read(&t, sizeof(t)) == sizeof(t);
		else
		{
			bool ret = read(&t, sizeof(t)) == sizeof(t);
			gvt_swap_endian(t);
			return ret;
		}
	};

	virtual gvi_stream& operator<<(const char* p)
	{
		return (*this);
	}

	virtual gvi_stream& operator<<(const gv_string& s)
	{
		gv_int str_size = s.size();
		write_any(str_size);
		if (str_size)
			this->write(*s, str_size);
		return *this;
	};

	virtual gvi_stream& operator<<(const gv_string_tmp& s)
	{
		gv_int str_size = s.size();
		write_any(str_size);
		if (str_size)
			this->write(*s, str_size);
		return *this;
	};

	virtual gvi_stream& operator<<(const gv_float& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_double& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_short& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_ushort& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_int& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_uint& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_long& s)
	{
		write_any(s);
		;
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_ulong& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_byte& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_char& s)
	{
		write_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_string& s)
	{
		gv_int str_size;
		(*this) >> str_size;
		s.reserve(str_size);
		s.resize(str_size);
		if (str_size)
			read(s.begin(), str_size);
		if (str_size && s.last()[0] != 0)
		{
			s.add((gv_char)0);
		}
		return *this;
	};
	virtual gvi_stream& operator>>(gv_float& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_double& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_short& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ushort& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_int& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_uint& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_long& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ulong& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_byte& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_char& s)
	{
		read_any(s);
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_text& t)
	{
		const gv_string& s = t;
		return (*this) << s;
	};

	virtual gvi_stream& operator<<(const gv_time_stamp& t)
	{
		return (*this) << t.m_u64;
	};
	virtual gvi_stream& operator<<(const gv_raw_data& t)
	{
		GV_ASSERT(0);
		return *this;
	}; // only used in database stream , override it if you want to use!!

	virtual gvi_stream& operator>>(const char* p)
	{
		return *this;
	} // default do nothing

	virtual gvi_stream& operator>>(gv_text& t)
	{
		return (*this) >> (gv_string&)(t);
	};
	virtual gvi_stream& operator>>(gv_time_stamp& t)
	{
		return (*this) >> t.m_u64;
	};

	virtual gvi_stream& operator>>(gv_raw_data& t)
	{
		GV_ASSERT(0);
		return *this;
	};

	virtual gv_int tell()
	{
		return 0;
	};
	virtual gv_int read(void* pdata, gv_int isize)
	{
		return 0;
	};
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		return 0;
	};
	virtual gv_int size()
	{
		return 0;
	};
	virtual bool eof()
	{
		return 0;
	};
	virtual void flush()
	{
		return;
	};

	virtual void enable_endian_swap(bool b)
	{
		m_endian_swap = b;
	};
	virtual bool is_endian_swap_needed()
	{
		return m_endian_swap;
	};
	// for visit pointers. useful.
	virtual void visit_ptr_to_object(void*&){};
	virtual void visit_ptr_to_struct(void*&){};
	virtual void visit_ptr_to_type(void*&){};
	virtual void visit_ref_ptr_to_object(void*&){};
	virtual void visit_ref_ptr_to_struct(void*&){};
	virtual void visit_ref_ptr_to_type(void*&){};

public:
	virtual const char* get_file_name()
	{
		return "TEMP";
	};
	virtual bool seek_from_begin(gv_uint pos)
	{
		return seek(pos, std::ios_base::beg);
	};
	virtual bool seek_from_end(gv_uint pos)
	{
		return seek(pos, std::ios_base::end);
	};
	virtual bool seek_from_current(gv_uint pos)
	{
		return seek(pos, std::ios_base::cur);
	};
	virtual gv_int read_all(gvt_array< gv_byte >& data)
	{
		seek_from_begin(0);
		int file_size = size();
		data.resize(file_size);
		read(data.begin(), file_size);
		return data.size();
	}
	virtual void set_user_data(void* pdata)
	{
		m_user_data = pdata;
	};
	virtual void* get_user_data()
	{
		return m_user_data;
	};
	virtual bool seek(gv_uint pos, std::ios_base::seekdir)
	{
		return false;
	}

protected:
	bool m_endian_swap;
	void* m_user_data;
};

class gv_stream_dummy : public gvi_stream
{
public:
	gv_stream_dummy(){};
	virtual ~gv_stream_dummy(){};
	virtual bool open(const char* name)
	{
		return true;
	}
	virtual bool close()
	{
		return true;
	}

	virtual gvi_stream& operator<<(const char* p)
	{
		return *this;
	} // default do noting
	virtual gvi_stream& operator<<(const gv_string&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_string_tmp&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_float&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_double&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_short&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_ushort&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_int&)
	{
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_uint&)
	{
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_long&)
	{
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_ulong&)
	{
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_byte&)
	{
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_char&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(const char* p)
	{
		return *this;
	} // default do nothing
	virtual gvi_stream& operator>>(gv_string&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_float&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_double&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_short&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ushort&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_int&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_uint&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_long&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ulong&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_byte&)
	{
		return *this;
	};
	virtual gvi_stream& operator>>(gv_char&)
	{
		return *this;
	};

	virtual gv_int tell()
	{
		return 0;
	};
	virtual gv_int read(void* pdata, gv_int isize)
	{
		return 0;
	};
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		return 0;
	};
	virtual gv_int size()
	{
		return 0;
	};
	virtual bool eof()
	{
		return true;
	};
	virtual void flush()
	{
	}
	virtual bool seek(gv_uint pos, std::ios_base::seekdir)
	{
		return true;
	}
};

class gv_stream_proxy : public gv_stream_dummy
{
public:
	gv_stream_proxy()
	{
		m_real_stream = 0;
	};
	virtual ~gv_stream_proxy()
	{
		if (m_real_stream)
			delete m_real_stream;
	};
	virtual bool open(const char* name)
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->open(name);
	}
	virtual bool close()
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->close();
	}
	virtual void set_proxy(gvi_stream* p)
	{
		m_real_stream = p;
	}

	virtual gvi_stream& operator<<(const char* x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_string& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_string_tmp& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_float& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_double& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_short& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_ushort& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_int& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_uint& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_long& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_ulong& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_byte& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator<<(const gv_char& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator>>(const char* x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) << x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_string& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_float& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_double& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_short& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_ushort& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_int& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_uint& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_long& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_ulong& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_byte& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}
	virtual gvi_stream& operator>>(gv_char& x)
	{
		GV_ASSERT(m_real_stream);
		(*m_real_stream) >> x;
		return *this;
	}

	virtual gv_int tell()
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->tell();
	};
	virtual gv_int read(void* pdata, gv_int isize)
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->read(pdata, isize);
	};
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->write(pdata, isize);
	};
	virtual gv_int size()
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->size();
	};
	virtual bool eof()
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->eof();
	};
	virtual void flush()
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->flush();
	};
	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir)
	{
		GV_ASSERT(m_real_stream);
		return m_real_stream->seek(pos, dir);
	};

protected:
	gvi_stream* m_real_stream;
};

template < class T, class policy_memory, class policy_thread_mode,
		   class policy_size_calculator >
inline gvi_stream& operator>>(gvi_stream& s,
							  gvt_array< T, policy_memory, policy_thread_mode,
										 policy_size_calculator >& array)
{
	gv_int size;
	s >> size;
	array.resize(size);
	for (int i = 0; i < array.size(); i++)
	{
		s >> array[i];
	}
	return s;
}

template < class T, class policy_memory, class policy_thread_mode,
		   class policy_size_calculator >
inline gvi_stream&
operator<<(gvi_stream& s, const gvt_array< T, policy_memory, policy_thread_mode,
										   policy_size_calculator >& array)
{
	gv_int size = array.size();
	s << size;
	for (int i = 0; i < array.size(); i++)
	{
		s << array[i];
	}
	return s;
}
};