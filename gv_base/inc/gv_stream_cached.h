#pragma once
namespace gv
{
class gv_stream_cached : public gvi_stream
{
public:
	gv_stream_cached()
	{
		cu_pointer = 0;
		data.reserve(1024);
		open_mode = std::fstream::in;
	};
	gv_stream_cached(const gv_stream_cached& s)
	{
		*this = s;
	};
	virtual ~gv_stream_cached()
	{
		close();
	};
	void reset(bool destroy = false)
	{
		if (!destroy)
			data.clear_and_reserve();
		else
			data.clear();
		cu_pointer = 0;
	}
	void load_data(gv_byte* ptr, gv_int size)
	{
		cu_pointer = 0;
		data.clear_and_reserve();
		data.init(ptr, size);
	}
	gv_byte* get_data()
	{
		return (gv_byte*)data.get_data();
	}
	gv_bool load_from_file()
	{
		return gv_load_file_to_array(*full_name, data);
	}
	// keep it compatible with lexer
	void goto_string(const char* p)
	{
		load_data((gv_byte*)p, (gv_int)strlen(p));
	}
	//
	gv_char* get_current()
	{
		return (gv_char*)data.begin() + tell();
	}

	virtual bool open(const char* name, std::ios_base::openmode _mode)
	{
		gv_bool ret = true;
		GVM_DEBUG_OUT("[fileio] open file " << name << " for  mod:" << _mode);
		open_mode = _mode;
		full_name = name;
		if (_mode & std::fstream::in)
		{
			ret = load_from_file();
		}
		return ret;
	};
	virtual bool close()
	{
		if (data.size() && full_name.size())
		{
			if (open_mode & std::fstream::app || open_mode & std::fstream::out)
			{
				gv_save_array_to_file(*full_name, data);
			}
			GVM_DEBUG_OUT("[fileio] closed file " << full_name
												  << " for  mod:" << open_mode);
			data.clear_and_reserve();
			full_name.clear();
		}
		return true;
	}
	virtual gv_int tell()
	{
		return cu_pointer;
	};
	virtual void flush()
	{
	}
	virtual gv_int read(void* pdata, gv_int isize)
	{
		gv_int size = gvt_min(isize, data.size() - cu_pointer);
		memcpy(pdata, data.begin() + cu_pointer, isize);
		cu_pointer += size;
		return size;
	};
	virtual gvi_stream& operator>>(gv_float& s)
	{
		s = *(float*)(data.begin() + cu_pointer);
		cu_pointer += 4;
		return *this;
	};
	/*//alignment issue on android & other CPU
  virtual gvi_stream &	operator >>(gv_double	&s)
  {
          s = *(gv_double*)(data.begin() + cu_pointer);
          cu_pointer += 8;
          return *this;
  };*/
	virtual gvi_stream& operator>>(gv_short& s)
	{
		s = *(gv_short*)(data.begin() + cu_pointer);
		cu_pointer += 2;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_ushort& s)
	{
		s = *(gv_ushort*)(data.begin() + cu_pointer);
		cu_pointer += 2;
		return *this;
	};
	virtual gvi_stream& operator>>(gv_int& s)
	{
		s = *(gv_int*)(data.begin() + cu_pointer);
		cu_pointer += sizeof(gv_int);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_uint& s)
	{
		s = *(gv_uint*)(data.begin() + cu_pointer);
		cu_pointer += sizeof(gv_uint);
		return *this;
	};
	/*//alignment issue on android & other CPU
  virtual gvi_stream &	operator >>(gv_long		&s)
  {
          s = *(gv_long*)(data.begin() + cu_pointer);
          cu_pointer += sizeof(gv_long);
          return *this;
  };
  virtual gvi_stream &	operator >>(gv_ulong	&s)
  {
          s = *(gv_ulong*)(data.begin() + cu_pointer);
          cu_pointer += sizeof(gv_ulong);
          return *this;
  };
  */
	virtual gvi_stream& operator>>(gv_byte& s)
	{
		s = *(gv_byte*)(data.begin() + cu_pointer);
		cu_pointer += sizeof(gv_byte);
		return *this;
	};
	virtual gvi_stream& operator>>(gv_char& s)
	{
		s = *(gv_char*)(data.begin() + cu_pointer);
		cu_pointer += sizeof(gv_char);
		return *this;
	};
	virtual gv_int append_from(const gv_stream_cached& s)
	{
		return write((void*)s.data.get_data(), s.data.size());
	}
	virtual gv_int write(const void* pdata, gv_int isize)
	{
		if (cu_pointer + isize > size())
		{
			data.resize(cu_pointer + isize);
		}
		memcpy(data.begin() + cu_pointer, pdata, isize);
		cu_pointer += isize;
		return isize;
	};
	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir)
	{
		switch (dir)
		{
		case (std::ios_base::beg):
			cu_pointer = pos;
			break;
		case (std::ios_base::end):
			cu_pointer = data.size() - pos;
			break;
		case (std::ios_base::cur):
			cu_pointer = cu_pointer + pos;
			break;
		}
		return true;
	};
	virtual gv_int size()
	{
		return data.size();
	};

	virtual bool eof()
	{
		return cu_pointer == data.size();
	}

	virtual void set_buf_size(gv_int size){

	};

	virtual gv_stream_cached& operator<<(const gv_stream_cached& s)
	{
		append_from(s);
		return *this;
	};

	virtual gv_stream_cached& operator=(const gv_stream_cached& s)
	{
		reset();
		append_from(s);
		full_name = s.full_name;
		open_mode = s.open_mode;
		return *this;
	};

	using gvi_stream::operator<<;

protected:
	std::ios_base::openmode open_mode;
	gv_string full_name;
	gvt_array< gv_byte, gvp_mem_cached_1024 > data;
	gv_int cu_pointer;
};
};