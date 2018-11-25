#pragma once
namespace gv
{

class gv_stream_iostream : public gvi_stream
{
public:
	gv_stream_iostream(){};
	virtual ~gv_stream_iostream()
	{
		close();
	};
	virtual bool open(const char* name, std::ios_base::openmode _mode)
	{
		GVM_DEBUG_OUT("[fileio] open file " << name << " for  mod:" << _mode);
		open_mode = _mode;
		if (_mode & std::fstream::app || _mode & std::fstream::out)
		{
			full_name = *gv_global::config.path_for_write;
		}
		full_name += name;
		stream.open(full_name, open_mode);
		if (!stream.is_open() && _mode & std::fstream::in)
		{
			// maybe want to read a file in output dir.
			full_name = *gv_global::config.path_for_write;
			full_name += name;
			stream.open(full_name, open_mode);
		}
#if __ANDROID__ && GV_WITH_OS_API
		if (!stream.is_open())
		{
			GVM_DEBUG_OUT("[fileio]cache file using create_file_from_asset :"
						  << name << " to SD card  for   " << _mode);
			extern void create_file_from_asset(const char* name);
			create_file_from_asset(name);
			stream.open(full_name, open_mode);
		}
#endif
		if (!stream.is_open())
		{
			GVM_WARNING("[fileio] failed to open file :" << *full_name << "  for   "
														 << _mode);
		}
		stream.precision(18);
		return stream.is_open();
	};
	virtual bool close()
	{
		if (stream.is_open())
		{
			stream.close();
			GVM_DEBUG_OUT("[fileio] closed file " << full_name
												  << " for  mod:" << open_mode);
		}
		return true;
	}

	virtual gv_int tell()
	{
		if (open_mode & (gv_uint)(std::fstream::in))
			return (gv_int)stream.tellg();
		if (open_mode & (gv_uint)(std::fstream::out))
			return (gv_int)stream.tellp();
		return -1;
	};
	virtual void flush()
	{
		stream.flush();
	}
	virtual gv_int read(void* pdata, gv_int isize)
	{
		stream.read((char*)pdata, isize);
		return (gv_int)stream.gcount();
	};
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		stream.write((const char*)pdata, isize);
		return (gv_int)stream.gcount();
	};
	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir)
	{
		if (open_mode & std::fstream::in)
			stream.seekg(pos, dir);
		if (open_mode & std::fstream::out)
			stream.seekp(pos, dir);
		return true;
	};
	virtual gv_int size()
	{
		int pos = tell();
		seek_from_end(0);
		int size = tell();
		seek_from_begin(pos);
		return size;
	};

	virtual bool eof()
	{
		return stream.eof();
	}

	virtual void set_buf_size(gv_int size){

	};

protected:
	std::fstream stream;
	std::ios_base::openmode open_mode;
	gv_string full_name;
};

class gv_stream_text_read : public gv_stream_iostream
{
public:
	virtual gvi_stream& operator<<(const char* p)
	{
		return (*this) << gv_string_tmp(p);
	}
	virtual gvi_stream& operator<<(const gv_string& s)
	{
		if (s.strlen() == 0)
			return *this;
		stream << s << "\t";
		return *this;
	};

	virtual gvi_stream& operator<<(const gv_string_tmp& s)
	{
		if (s.strlen() == 0)
			return *this;
		stream << s << "\t";
		return *this;
	};

	virtual gvi_stream& operator<<(const gv_float& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_double& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_short& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_ushort& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_int& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_uint& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_long& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_ulong& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_byte& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator<<(const gv_char& s)
	{
		stream << s << "\t";
		return *this;
	};
	virtual gvi_stream& operator>>(gv_string& s)
	{
		std::string ss;
		stream >> ss;
		s = ss.c_str();
		return *this;
	};
	virtual gvi_stream& operator>>(gv_float& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_double& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_short& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ushort& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_int& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_uint& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_long& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ulong& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_byte& s)
	{
		stream >> s;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_char& s)
	{
		stream >> s;
		return *this;
	};
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name, std::fstream::in);
	};
};

class gv_stream_text_write : public gv_stream_text_read
{
public:
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name, std::fstream::out);
	};
};

class gv_stream_text_append : public gv_stream_iostream
{
public:
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name, std::fstream::app);
	};
};

class gv_stream_binary : public gv_stream_iostream
{
public:
};

class gv_stream_binary_read : public gv_stream_binary
{
public:
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name,
										std::fstream::binary | std::fstream::in);
	};
};

class gv_stream_binary_write : public gv_stream_binary
{
public:
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name,
										std::fstream::binary | std::fstream::out);
	};
};

class gv_stream_binary_append : public gv_stream_binary
{
public:
	virtual bool open(const char* name)
	{
		return gv_stream_iostream::open(name,
										std::fstream::binary | std::fstream::app);
	};
};

class gv_stream_binary_size_count : public gv_stream_binary
{
public:
	gv_stream_binary_size_count()
	{
		m_size = 0;
	};
	~gv_stream_binary_size_count()
	{
	}

	virtual bool open(const char* name)
	{
		return true;
	}
	virtual bool close()
	{
		return true;
	}
	virtual gv_int tell()
	{
		return m_size;
	};
	virtual void flush()
	{
	}
	virtual gv_int read(void* pdata, gv_int isize)
	{
		m_size += isize;
		return isize;
	};
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		m_size += isize;
		return isize;
	};
	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir)
	{
		m_size = pos;
		return true;
	};
	virtual gv_int size()
	{
		return m_size;
	};
	gv_int m_size;
};
};