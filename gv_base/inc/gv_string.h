#pragma once
#include <boost/boost/algorithm/string/case_conv.hpp>
#include <boost/boost/algorithm/string/classification.hpp>

namespace gv
{
const static char* gv_endl = "\r\n";

template < typename type_of_char, class policy_memory,
		   class policy_size_calculator = gvp_array_size_calculator_string >
class gvt_string : public gvt_array< type_of_char, policy_memory,
									 gvp_single_thread, policy_size_calculator >
{
public:
	typedef gvt_string< type_of_char, policy_memory, policy_size_calculator >
		type_of_string;
	typedef gvt_array< type_of_char, policy_memory, gvp_single_thread,
					   policy_size_calculator >
		super;

	inline gvt_string(){};
	inline gvt_string(const type_of_char* p)
	{
		if (p)
			super::init(p, gvt_strlen(p) + 1);
	};
	inline gvt_string(const type_of_string& s)
	{
		*this = s;
	}
	inline ~gvt_string(){};

	gv_int size() const
	{
		return super::size();
	}
	const type_of_char* end() const
	{
		return super::end();
	}
	const type_of_char* begin() const
	{
		return super::begin();
	}
	type_of_char* end()
	{
		return super::end();
	}
	type_of_char* begin()
	{
		return super::begin();
	}

public:
	inline const type_of_string operator+(const type_of_string& s) const
	{
		type_of_string ret(*this);
		ret += s;
		return ret;
	};

	inline const type_of_string& operator-(const type_of_string& s) const
	{
		type_of_string ret(*this);
		ret -= s;
		return ret;
	};

	inline type_of_string& operator+=(const type_of_string& s)
	{
		if (!size())
			(*this) = this->empty_string();
		this->insert(s.begin(), this->strlen(), s.strlen());
		return *this;
	};

	inline type_of_string& operator+=(const type_of_char* s)
	{
		if (s)
		{
			if (!size())
				(*this) = this->empty_string();
			this->insert(s, this->strlen(), gvt_strlen(s));
		}
		return *this;
	};

	inline type_of_string& operator+=(const type_of_char& c)
	{
		if (!c)
			return *this;
		if (!size())
			(*this) = this->empty_string();
		this->insert(&c, this->strlen());
		return *this;
	}

	inline type_of_string& operator-=(const type_of_string& s)
	{
		gv_int len = std::min(strlen(), s.strlen());
		if (gvt_strcmp(super::end() - len - 1, s.begin()) == 0)
		{
			this->erase(strlen() - len, len);
		}
		return *this;
	}

	inline type_of_string& operator-=(const type_of_char& c)
	{
		if (strlen() >= 1 && *(end() - 1 - 1) == c)
		{
			this->erase(strlen() - 1, 1);
		}
		return *this;
	};

	inline type_of_string& operator=(const type_of_char* p)
	{
		if (p)
			this->init(p, gvt_strlen(p) + 1);
		else
			this->clear();
		return *this;
	};

	inline bool operator==(const type_of_char* p) const
	{
		return gvt_strcmp(p, begin()) == 0;
	};

	inline bool operator!=(const type_of_char* p) const
	{
		return !((*this) == p);
	};

	inline bool operator==(const type_of_string& s) const
	{
		if (s.strlen() != strlen())
			return false;
		return gvt_strcmp(s.begin(), begin()) == 0;
	};

	inline bool operator<(const type_of_string& s) const
	{
		gv_int len = gvt_min(strlen(), s.strlen());
		const type_of_char* ps = begin();
		const type_of_char* pt = s.begin();
		for (int i = 0; i < len; i++)
			if ((*ps) < (*pt))
				return true;
			else if ((*ps++) > (*pt++))
				return false;
		if (strlen() < s.strlen())
			return true;
		return false;
	};

	inline bool operator!=(const type_of_string& s) const
	{
		return !((*this) == s);
	};

	template < class T1 >
	inline gvt_string& operator=(const gvt_string< T1, policy_memory >& a)
	{
		this->resize(a.size());
		for (int i = 0; i < a.size(); i++)
			(*this)[i] = (type_of_char)a[i];
		return *this;
	}

	inline bool is_empty_string() const
	{
		return size() < 2;
	}

	inline const type_of_char* operator*() const
	{
		if (begin())
			return begin();
		else
			return empty_string();
	};

	inline operator const type_of_char*() const
	{
		return begin();
	};

public:
	inline const type_of_char* empty_string() const
	{
		static type_of_char buffer[] = {0};
		return buffer;
	}

	inline void add_end()
	{
		if (size())
			*(end() - 1) = 0;
	}

	inline void insert_string(const type_of_string& s, const type_of_char* pos)
	{
		if (!size())
			(*this) = this->empty_string();
		insert(s.begin(), index(pos), s.strlen());
	};

	inline void delete_string(type_of_char* pos, gv_int count)
	{
		erase(index(pos), count);
	};

	inline void left(gv_int count, type_of_string& s) const
	{
		s.init(begin(), std::min(count + 1, size()));
		s.add_end();
	};

	inline void right(gv_int count, type_of_string& s) const
	{
		gv_int len = std::min(count + 1, size());
		s.init(end() - len, len);
		s.add_end();
	};

	inline void middle(gv_int pos, gv_int count, type_of_string& s) const
	{
		type_of_string s1;
		int l = strlen();
		right(l - pos, s1);
		s1.left(count, s);
	};

	inline type_of_string left(gv_int count) const
	{
		type_of_string s;
		left(count, s);
		return s;
	};
	inline type_of_string right(gv_int count) const
	{
		type_of_string s;
		right(count, s);
		return s;
	};
	inline type_of_string middle(gv_int pos, gv_int count) const
	{
		type_of_string s;
		middle(pos, count, s);
		return s;
	};
	inline void clip(gv_int count)
	{
		if (size() >= count)
		{
			super::resize(count + 1);
			add_end();
		}
	};
	inline bool has_prefix(const type_of_string& s) const
	{
		return this->left(s.strlen()) == s;
	}
	inline bool has_postfix(const type_of_string& s) const
	{
		return this->right(s.strlen()) == s;
	}
	inline bool replace_prefix(const type_of_string& old_prefix,
							   const type_of_string& new_prefix)
	{
		if (!has_prefix(old_prefix))
			return false;
		type_of_string s;
		s = new_prefix;
		s += this->right(this->strlen() - old_prefix.strlen());
		(*this) = s;
		return true;
	}
	inline bool replace_postfix(const type_of_string& old_postfix,
								const type_of_string& new_postfix)
	{
		if (!has_postfix(old_postfix))
			return false;
		type_of_string s;
		s += this->left(this->strlen() - old_postfix.strlen());
		s += new_postfix;
		(*this) = s;
		return true;
	}

	inline gv_int strlen() const
	{
		if (!size())
			return 0;
		// GV_ASSERT_SLOW(size()==gvt_strlen(begin())+1);
		return size() - 1;
	};
	inline type_of_string& strcpy(const type_of_char* p)
	{
		*this = p;
		return *this;
	}
	inline type_of_string& strcat(const type_of_char* p)
	{
		*this += p;
		return *this;
	}
	inline void trim_back()
	{
		if (this->strlen() <= 0)
			return;
		type_of_char* p = this->last() - 1;
		int cnt = 0;
		while (isspace(*p) && cnt < this->strlen())
		{
			cnt++;
			p--;
		}
		*this = this->left(this->strlen() - cnt);
	}
	inline void trim_front()
	{
		if (this->strlen() <= 0)
			return;
		type_of_char* p = this->first();
		int cnt = 0;
		while (isspace(*p) && cnt < this->strlen())
		{
			cnt++;
			p++;
		}
		*this = this->right(this->strlen() - cnt);
	}
	inline void to_upper()
	{
		for (int i = 0; i < strlen(); i++)
		{
			this->data_ptr[i] = (type_of_char)::toupper(this->data_ptr[i]);
		}
		// boost::algorithm::to_upper(*this);
	};
	inline void to_lower()
	{
		for (int i = 0; i < strlen(); i++)
		{
			this->data_ptr[i] = (type_of_char)::tolower(this->data_ptr[i]);
		}
		// boost::algorithm::to_lower(*this);
	};
	inline gv_uint hash() const
	{
		const type_of_char* str = begin();
		gv_uint hash = 5381;
		if (!str)
			return hash;
		;
		int c;
		while ((c = *str++) != 0)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash;
	};
	inline void split(gvt_array< type_of_string >& result) const
	{
		const type_of_char* p = begin();
		if (!p)
			return;
		type_of_string s;
		s.reserve(100);
		while (*p)
		{
			s = empty_string();
			while (*p && !gvt_is_space(*p))
				s += *p++;
			result.push_back(s);
			while (*p && (gvt_is_space(*p)))
				p++;
		}
	};
	inline int split_count() const
	{
		const type_of_char* p = begin();
		int count = 0;
		if (!p)
			return count;
		while (*p)
		{
			while (*p && !gvt_is_space(*p))
				p++;
			count++;
			while (*p && (gvt_is_space(*p)))
				p++;
		}
		return count;
	};
	inline void split(gvt_array< type_of_string >& result,
					  const type_of_string& spliter) const
	{
		const type_of_char* p = begin();
		if (!p)
			return;
		type_of_string s;
		s.reserve(100);
		while (*p)
		{
			s = empty_string();
			while (*p && !spliter.find(*p))
				s += *p++;
			result.add(s);
			while (*p && (spliter.find(*p)))
				p++;
		}
	};
	inline const type_of_char* find_string(const type_of_string& s,
										   const type_of_char* pos = NULL,
										   bool from_front = true) const
	{
		GV_ASSERT(!pos || this->is_valid_ptr(pos));
		if (!begin())
			return 0;
		int len_s = s.strlen();
		int len_this = strlen();
		if (from_front)
		{
			if (!pos)
			{
				pos = begin();
			}
			return gvt_strstr(pos, *s);
		}
		else
		{
			if (!pos)
			{
				pos = begin() + len_this - len_s;
			}
			while (pos > begin())
			{
				const type_of_char* p = pos;
				const type_of_char* q = s.begin();
				bool is_equal = true;
				for (int j = 0; j < len_s; j++, p++, q++)
				{
					if (*p != *q)
					{
						is_equal = false;
						break;
					}
				}
				if (is_equal)
				{
					return pos;
				}
				else
					pos--;
			}
		}
		return NULL;
	};
	inline gv_int replace_all(const type_of_string& old_s,
							  const type_of_string& new_s)
	{
		int ret = 0;
		int len_old = old_s.strlen();
		int len_new = new_s.strlen();
		const type_of_char* pos = NULL;
		while ((pos = this->find_string(old_s, pos)) != 0)
		{
			GV_ASSERT(this->is_valid_ptr(pos));
			int idx = this->index(pos);
			this->erase(idx, len_old);
			this->insert(new_s, idx, len_new);
			pos = begin() + idx + len_new;
			ret++;
		}
		return ret;
	};
	inline void replace_char(type_of_char old_c, type_of_char new_c)
	{
		super::for_each(gvf_replace< type_of_char >(old_c, new_c));
	};
	inline type_of_string& operator<<(const type_of_string& s)
	{
		(*this) += s;
		return *this;
	};
	inline type_of_string& operator<<(const type_of_char* p)
	{
		(*this) += p;
		return *this;
	};
	inline type_of_string& operator<<(const void* p)
	{
		char buffer[2048];
#if GV_64
		sprintf(buffer, "%llx", (gv_int_ptr)p);
#else
		sprintf(buffer, "%x", (gv_int_ptr)p);
#endif
		(*this) << buffer;
		return *this;
	};
	inline type_of_string& operator<<(const type_of_char& c)
	{
		(*this) += c;
		return *this;
	};
	inline type_of_string& operator<<(const gv_float f)
	{
		type_of_char buffer[1024];
		gv_double_to_string(f, buffer);
		(*this) += buffer;
		if ((*(this->last() - 1)) == '.')
		{
			(*this) += '0'; // patch for mysql...
		}
		return *this;
	};
	inline type_of_string& operator<<(const gv_double f)
	{
		type_of_char buffer[1024];
		gv_double_to_string(f, buffer);
		(*this) += buffer;
		if ((*(this->last() - 1)) == '.')
		{
			(*this) += '0';
		}
		return *this;
	};
	inline type_of_string& operator<<(const gv_int i)
	{
		type_of_char buffer[GV_ATOI_BUF_SIZE];
		gv_int_to_string(i, buffer);
		(*this) += buffer;
		return *this;
	};

	inline type_of_string& operator<<(const gv_uint u)
	{
		type_of_char buffer[GV_ATOI_BUF_SIZE];
		gv_uint_to_string(u, buffer);
		(*this) += buffer;
		return *this;
	};

	inline type_of_string& operator<<(const gv_long l)
	{
		type_of_char buffer[GV_ATOI_BUF_SIZE];
		gv_long_to_string(l, buffer);
		(*this) += buffer;
		return *this;
	};

	inline type_of_string& operator<<(const gv_ulong l)
	{
		type_of_char buffer[GV_ATOI_BUF_SIZE];
		gv_ulong_to_string(l, buffer);
		(*this) += buffer;
		return *this;
	};

	inline type_of_string& operator<<(const gv_byte b)
	{
		return (*this) << (gv_uint)b;
	};

	//===read from string
	inline const type_of_string& operator>>(const char* c) const
	{
		GV_ASSERT(0);
		return *this;
	};
	inline const type_of_string& operator>>(void*& c) const
	{
		sscanf(*(*this), "%x", &c);
		return *this;
	};
	inline const type_of_string& operator>>(gv_byte& b) const
	{
		gv_uint u;
		(*this) >> u;
		b = (gv_byte)u;
		return *this;
	};
	inline const type_of_string& operator>>(gv_char& c) const
	{
		gv_int i;
		(*this) >> i;
		c = (gv_char)i;
		return *this;
	};
	inline const type_of_string& operator>>(gv_ushort& b) const
	{
		gv_uint u;
		(*this) >> u;
		b = (gv_ushort)u;
		return *this;
	};
	inline const type_of_string& operator>>(gv_short& c) const
	{
		gv_int i;
		(*this) >> i;
		c = (gv_short)i;
		return *this;
	};
	inline const type_of_string& operator>>(gv_float& f) const
	{
		f = (float)gv_string_to_double(begin());
		return *this;
	};
	inline const type_of_string& operator>>(gv_double& f) const
	{
		f = gv_string_to_double(begin());
		return *this;
	};
	inline const type_of_string& operator>>(gv_int& i) const
	{
		i = gv_string_to_int(begin());
		return *this;
	};
	inline const type_of_string& operator>>(gv_uint& u) const
	{
		u = gv_string_to_uint(begin());
		return *this;
	};
	inline const type_of_string& operator>>(gv_long& l) const
	{
		l = gv_string_to_long(begin());
		return *this;
	};
	inline const type_of_string& operator>>(gv_ulong& l) const
	{
		l = gv_string_to_ulong(begin());
		return *this;
	};

	inline const type_of_string& operator>>(type_of_string& s) const
	{
		s = *this;
		return *this;
	};

	template < class T, class policy_memory_2, class policy_thread_mode_2,
			   class policy_size_calculator_2 >
	inline type_of_string&
	write_array(const gvt_array< T, policy_memory_2, policy_thread_mode_2,
								 policy_size_calculator_2 >& array)
	{
		gv_int size = array.size();
		(*this) << size << gv_endl;
		for (int i = 0; i < array.size(); i++)
		{
			(*this) << array[i] << gv_endl;
		}
		return (*this);
	}
};
typedef gvt_string< char, gvp_memory_default > gv_string;
typedef gvt_string< wchar_t, gvp_memory_default > gv_wstring;

static const int gv_string_tmp_cache_size = 512;
typedef gvt_string<
	char, gvt_memory_cached< gv_string_tmp_cache_size >,
	gvt_policy_size_calculator_cached< gv_string_tmp_cache_size,
									   gvp_array_size_calculator_string > >
	gv_string_tmp;
typedef gvt_string<
	wchar_t, gvt_memory_cached< gv_string_tmp_cache_size >,
	gvt_policy_size_calculator_cached< gv_string_tmp_cache_size,
									   gvp_array_size_calculator_string > >
	gv_wstring_tmp;

typedef gvt_string<
	char, gvp_mem_cached_256,
	gvt_policy_size_calculator_cached< 256, gvp_array_size_calculator_string > >
	gv_string_256;
typedef gvt_string<
	char, gvp_mem_cached_32,
	gvt_policy_size_calculator_cached< 32, gvp_array_size_calculator_string > >
	gv_string_32;

inline const gv_string& gv_get_const_string(const gv_string_tmp& s)
{
	// some hack here..
	return *(gv_string*)(&s);
}
inline const gv_wstring& gv_get_const_string(const gv_wstring_tmp& s)
{
	// some hack here..
	return *(gv_wstring*)(&s);
}
#if GV_64
GV_STATIC_ASSERT(sizeof(gv_string) == 24);
GV_STATIC_ASSERT(sizeof(gv_wstring) == 24);
#else
GV_STATIC_ASSERT(sizeof(gv_string) == 16);
GV_STATIC_ASSERT(sizeof(gv_wstring) == 16);
#endif

namespace detail
{
template < typename type_char_a, typename type_char_b, class policy_memory_a,
		   class policy_memory_b, class policy_size_a, class policy_size_b,
		   bool bval >
inline gvt_string< type_char_a, policy_memory_a, policy_size_a >&
add_string_imp(gvt_string< type_char_a, policy_memory_a, policy_size_a >& a,
			   gvt_string< type_char_b, policy_memory_b, policy_size_b >& b,
			   const boost::integral_constant< bool, bval >&)
{
	type_char_b* p = b.begin();
	if (!p)
		return a;
	while (*p)
	{
		a += (type_char_a)(*p++);
	};
	return a;
}

template < typename type_char_a, typename type_char_b, class policy_memory_a,
		   class policy_memory_b, class policy_size_a, class policy_size_b >
inline gvt_string< type_char_a, policy_memory_a, policy_size_a >&
add_string_imp(gvt_string< type_char_a, policy_memory_a, policy_size_a >& a,
			   gvt_string< type_char_b, policy_memory_b, policy_size_b >& b,
			   const boost::true_type&)
{
	a += b;
	return a;
}
}
// insert string , no matter what's the allocater.
template < typename type_char_a, typename type_char_b, class policy_memory_a,
		   class policy_memory_b, class policy_size_a, class policy_size_b >
inline gvt_string< type_char_a, policy_memory_a, policy_size_a >&
operator<<(gvt_string< type_char_a, policy_memory_a, policy_size_a >& a,
		   gvt_string< type_char_b, policy_memory_b, policy_size_b >& b)
{
	return detail::add_string_imp(
		a, b, boost::is_same<
				  gvt_string< type_char_a, policy_memory_a, policy_size_a >,
				  gvt_string< type_char_b, policy_memory_b, policy_size_b > >());
}
template < typename type_char_a, typename type_char_b, class policy_memory_a,
		   class policy_memory_b, class policy_size_a, class policy_size_b >
inline const gvt_string< type_char_a, policy_memory_a, policy_size_a >&
operator>>(const gvt_string< type_char_a, policy_memory_a, policy_size_a >& a,
		   gvt_string< type_char_b, policy_memory_b, policy_size_b >& b)
{
	b = a;
	return a;
}

extern void gv_cpp_string_to_string(gv_string_tmp& string, bool do_esc = true);

template < typename type_of_char, class policy_memory,
		   class policy_size_calculator >
inline gv_uint gvt_hash(
	const gvt_string< type_of_char, policy_memory, policy_size_calculator >& s)
{
	return s.hash();
}

#define DCL_CACHED_STRING(size)                                        \
	typedef gvt_string< char, gvt_memory_cached< size >,               \
						gvt_policy_size_calculator_cached<             \
							size, gvp_array_size_calculator_string > > \
		gv_string##size;

DCL_CACHED_STRING(16)
DCL_CACHED_STRING(32)
DCL_CACHED_STRING(64)
DCL_CACHED_STRING(128)
DCL_CACHED_STRING(512)

#define DCL_STATIC_STRING(size)                                        \
	typedef gvt_string< char, gvt_memory_static< size >,               \
						gvt_policy_size_calculator_cached<             \
							size, gvp_array_size_calculator_string > > \
		gv_string_static_##size;
DCL_STATIC_STRING(16)
DCL_STATIC_STRING(32)
DCL_STATIC_STRING(64)
DCL_STATIC_STRING(128)
DCL_STATIC_STRING(512)
DCL_STATIC_STRING(1024)

//============================================================================================
//								:
//============================================================================================
gv_string_tmp gv_multi_byte_to_utf8(const char* multi_byte);
gv_string_tmp gv_utf8_to_multi_byte(const char* utf8);
gv_string_tmp gv_get_full_path_name(const gv_string_tmp&);
//============================================================================================
//								:
//============================================================================================

extern void gv_time_stamp_to_string(const gv_time_stamp& t, gv_string_tmp& s);
extern void gv_time_stamp_from_string(gv_time_stamp& t, const gv_string_tmp& s);
inline gv_string_tmp& operator<<(gv_string_tmp& s, const gv_time_stamp& t)
{
	gv_string_tmp ss;
	gv_time_stamp_to_string(t, ss);
	s << ss;
	return s;
}

inline const gv_string_tmp& operator>>(const gv_string_tmp& s,
									   gv_time_stamp& t)
{
	gv_string_tmp ss;
	s >> ss;
	gv_time_stamp_from_string(t, ss);
	return s;
}

class gv_text : public gv_string
{
public:
	gv_text(){};
	gv_text(const gv_text& t)
	{
		(*this) = *t;
	};
	gv_text(const char* p)
		: gv_string(p)
	{
	}
	~gv_text()
	{
	}
};

struct gv_int_string_pair
{
	gv_int_string_pair()
	{
	}

	gv_int_string_pair(int ii, gv_string is)
	{
		s = is;
		i = ii;
	}

	gv_int_string_pair(const gv_int_string_pair& a)
	{
		*this = a;
	}

	gv_int_string_pair& operator=(const gv_int_string_pair& a)
	{
		i = a.i;
		s = a.s;
		return *this;
	}

	int i;
	gv_string s;
};

inline gv_uint gvt_hash(const gv_text& s)
{
	return s.hash();
}

// a helper function
template < class T >
class gvf_stdout
{
public:
	void operator()(const T& s) const
	{
		gv_string_tmp ss;
		ss << s;
		gv_console_output(*ss);
	}
};

} // gv

#include "gv_string_detail.h"
