#pragma once

namespace gv
{
const int LEX_INVALID_STATE = -1;
const int MAX_CHARS = 255;

namespace gv_lang_cpp
{
#define GVM_PP_LEXER_DEF_TOKEN 1
#include "gv_pp_lexer.h"
enum TOKEN
{
	TOKEN_FIRST = 255,
#include "gv_lang_cpp.h"
	TOKEN_LAST
};
#include "gv_pp_lexer.h"

#define GVM_PP_LEXER_GET_TOKEN_STR 1
#include "gv_pp_lexer.h"
inline const char* token_to_string(int token)
{
	switch (token)
	{
#include "gv_lang_cpp.h"
	};
	return "unknown";
}
#include "gv_pp_lexer.h"
};

class gv_scanner_string
{
public:
	class iterator
	{ //
	public:
		typedef char value_type;
		typedef char* pointer;
		typedef char& reference;
		friend class gv_scanner_string;

		iterator()
			: ptr(0), owner(0)
		{ // construct with null node pointer
		}
		iterator(char* _ptr, gv_scanner_string* _owner)
			: ptr(_ptr), owner(_owner)
		{ // construct with node pointer _pnode
		}
		reference operator*() const
		{ // return designated value
			GV_ASSERT_SLOW(ptr);
			GV_ASSERT_SLOW(owner);
			return *ptr;
		}
		operator char*()
		{
			return ptr;
		}
		pointer operator->() const
		{ // return pointer to class object
			return ptr;
		}
		iterator& operator++()
		{ // preincrement
			GV_ASSERT_SLOW(ptr);
			ptr++;
			return (*this);
		}
		iterator operator++(int)
		{ // postincrement
			iterator _tmp = *this;
			++*this;
			return (_tmp);
		}
		iterator& operator=(const iterator& _right)
		{ // test for iterator equality
			ptr = _right.ptr;
			owner = _right.owner;
			return (*this);
		}
		bool
		operator==(const iterator& _right) const
		{ // test for iterator equality
			return (ptr == _right.ptr);
		}
		bool
		operator!=(const iterator& _right) const
		{ // test for iterator inequality
			return (!(*this == _right));
		}
		bool operator<(const iterator& _right) const
		{ // test if this < _right
			return (ptr < _right.ptr);
		}
		bool operator>(const iterator& _right) const
		{ // test if this > _right
			return (_right < *this);
		}
		bool operator<=(const iterator& _right) const
		{ // test if this <= _right
			return (!(_right < *this));
		}
		bool operator>=(const iterator& _right) const
		{ // test if this >= _right
			return (!(*this < _right));
		}

	protected:
		char* ptr;
		gv_scanner_string* owner;
	};

	friend class iterator;

	gv_scanner_string()
	{
		m_is_temp_string = m_is_from_file = false;
	}
	iterator begin()
	{
		return iterator(m_is_temp_string ? m_temp_string : this->m_string.begin(),
						this);
	};
	iterator end()
	{
		return iterator(m_is_temp_string ? m_temp_string_end : this->m_string.end(),
						this);
	};
	bool load_string(const char* s)
	{
		m_is_temp_string = m_is_from_file = false;
		this->m_string = s;
		return true;
	};
	bool load_file(const char* file_name);
	bool goto_string(const char* s)
	{
		m_is_temp_string = true, m_is_from_file = false;
		m_temp_string = (char*)s;
		m_temp_string_end = m_temp_string + gvt_strlen(m_temp_string);
		return true;
	}
	const gv_string& get_file_name()
	{
		static gv_string s_file("temp");
		if (m_is_from_file)
			return m_file_name;
		return s_file;
	};

protected:
	gv_string_tmp m_string;
	gv_string m_file_name;
	bool m_is_from_file;
	bool m_is_temp_string;
	char* m_temp_string;
	char* m_temp_string_end;
};

class gv_scanner_cached_file
{
public:
	class iterator
	{
	public:
	protected:
		int pos;
		gv_byte* ptr;
	};

	friend class iterator;
	gv_scanner_cached_file();
	~gv_scanner_cached_file();
	bool load_string(const char* file_name);
	bool load_file(const char* file_name);
	iterator begin();
	iterator end();

protected:
	void cache(iterator&);
};

class gvi_lexer : public gv_refable
{
public:
	gvi_lexer(){};
	virtual ~gvi_lexer(){};

public:
	virtual int lex() = 0;
	virtual bool load_string(const char* s) = 0;
	virtual bool load_file(const char* s) = 0;
	virtual bool goto_string(const char* s) = 0;
	virtual int get_line_no() = 0;
	virtual void enable_record(bool enable = true) = 0;
	virtual void step_out(int start_token, int end_token) = 0;
	virtual void lex_error(const gv_string& s){};
	virtual gv_byte look_ahead(int count = 0) = 0;
	virtual const gv_string& get_file_name() = 0;
	virtual const gv_string_tmp& get_record() = 0;
	virtual const gv_string_tmp& strip_until(gv_byte) = 0;
	virtual const gv_string_tmp& strip_this_line() = 0;
	virtual const char* get_token_name(int token) = 0;
	virtual void error(const char* s) = 0;
	virtual void flush_file(){};

	template < class type_of_data >
	inline bool get_next_number(type_of_data& d)
	{
		using namespace gv::gv_lang_cpp;
		int t = lex();
		// assume all the constant token is the same value define in different
		// language.
		if (t == gv_lang_cpp::TOKEN_ICON || t == gv_lang_cpp::TOKEN_FCON)
		{
			this->get_id() >> d;
			return true;
		}
		error(" want a number here!!");
		return false;
	}

	inline gv_float get_next_float()
	{
		gv_float f=0; 
		get_next_number(f); 
		return f; 
	}

	inline gv_int get_next_int()
	{
		gv_int f = 0;
		get_next_number(f);
		return f;
	}
		
	inline   gv_string_tmp get_next_string()
	{
		using namespace gv::gv_lang_cpp;
		int t = lex();
		if (t == TOKEN_STRING )
		{
			return get_string(); 
		}
		error(" want a string here!!");
		return "";
	}

	inline  gv_string_tmp get_next_id()
	{
		using namespace gv::gv_lang_cpp;
		int t = lex();
		if (t == TOKEN_ID)
		{
			return get_string();
		}
		error(" want a string here!!");
		return gv_string_tmp("");
	}


	inline bool next_match(gv_int token, bool do_assert = true)
	{
		gv_int t = lex();
		if (t != token)
		{
			if (do_assert)
			{
				gv_string_tmp s;
				s << "want a " << this->get_token_name(token) << " here ";
				this->error(*s);
			}
			return false;
		}
		return true;
	}

	inline bool next_match_id(const gv_string_tmp & t, bool do_assert = true)
	{
		gv_string_tmp result= get_next_id(); 
		if (t != result)
		{
			if (do_assert)
			{
				gv_string_tmp s;
				s << "want a " << t<< " here ";
				this->error(*s);
			}
			return false;
		}
		return true;
	}

	inline bool nest_match_string(const gv_string_tmp & s, bool do_assert = true)
	{
		gv_string_tmp result = get_next_string();
		if (s != result)
		{
			if (do_assert)
			{
				gv_string_tmp s;
				s << "want a " << s << " here ";
				this->error(*s);
			}
			return false;
		}
		return true;
	}

	

	template < class type_of_data >
	inline gvi_lexer& operator>>(type_of_data& d)
	{
		gv_int t = lex();
		if (t)
			get_id() >> d;
		return (*this);
	}
	template < class type_of_data >
	inline gvi_lexer& operator<<(const type_of_data& d)
	{
		m_val_string << d;
		flush_file();
		return (*this);
	}
	inline gvi_lexer& operator>>(const char* s)
	{
		lex();
		GV_ASSERT(get_id() == s);
		return (*this);
	}
	inline int get_int()
	{
		return m_val_int;
	};
	inline gv_float get_float()
	{
		return m_val_float;
	};
	inline gv_double get_double()
	{
		return m_val_double;
	};
	inline const gv_string_tmp& get_string()
	{
		return m_val_string;
	};
	inline const gv_string_tmp& get_id()
	{
		return m_val_string;
	};

public:
	int m_val_int;
	float m_val_float;
	double m_val_double;
	gv_string_tmp m_val_string;
};

template < class type_of_scanner >
class gvt_lexer : public gvi_lexer
{
protected:
	struct state
	{
		state()
		{
		}
		state(const state& s)
		{
			(*this) = s;
		}
		state& operator=(const state& s)
		{
			current = s.current;
			line_no = s.line_no;
			lex_state = s.lex_state;
			token_length = s.token_length;
			record_length = s.record_length;
			return *this;
		}
		typename type_of_scanner::iterator current;
		int line_no;
		int lex_state;
		int token_length;
		int record_length;
	};

	inline type_of_scanner& scanner()
	{
		return m_scanner;
	};
	inline void mark()
	{
		m_state_marked = m_state_current;
		m_is_marked = true;
		m_state_marked.token_length = m_val_string.strlen();
		m_state_marked.record_length = m_record_string.strlen();
	};
	inline bool is_marked()
	{
		return m_is_marked;
	};
	inline void restore()
	{
		m_state_current = m_state_marked;
		m_is_marked = false;
		m_val_string.clip(m_state_current.token_length);
		m_record_string.clip(m_state_marked.record_length);
	}
	inline int get_last_state()
	{
		return m_state_current.lex_state;
	};
	inline void set_last_state(int state)
	{
		m_state_current.lex_state = state;
	};

	virtual const gv_string_tmp& token_to_string()
	{
		gv_cpp_string_to_string(m_val_string);
		return m_val_string;
	};
	inline const gv_string_tmp& token_to_id()
	{
		return this->m_val_string;
	};

	inline int token_to_int()
	{
		m_val_string >> m_val_int;
		return m_val_int;
	};
	inline gv_uint token_to_hex()
	{
		gv_uint val = gv_string_to_hex(*m_val_string);
		return val;
	};
	inline gv_float token_to_float()
	{
		m_val_string >> m_val_float;
		return m_val_float;
	};
	inline gv_double token_to_double()
	{
		m_val_string >> m_val_double;
		return m_val_double;
	};
	inline void token_start()
	{
		m_is_marked = false;
		m_val_string = "";
	}
	inline const char* current()
	{
		return this->m_state_current.current;
	}
	inline gv_byte next()
	{
		gv_byte ret;
		if (is_empty())
		{
			return 0;
		}
		if (*m_state_current.current == '\n')
			m_state_current.line_no++;
		ret = *m_state_current.current;
		m_val_string += ret;
		if (m_is_recording)
			m_record_string += ret;
		++m_state_current.current;
		return ret;
	}

	virtual void error(const char* s)
	{
		GVM_CONSOLE_OUT(m_file_name << "(" << m_state_current.line_no << "):lex error !" << s);
	};

public:
	inline gv_ulong token_to_ulong()
	{
		gv_ulong val;
		m_val_string >> val;
		return val;
	};
	gvt_lexer()
	{
		m_val_double = 0;
		m_val_float = 0;
		m_val_int = 0;
		m_is_recording = false;
		m_is_marked = false;
	}
	//===================need to use lexc++ to generate this function
	virtual int lex();

	virtual const gv_string& get_file_name()
	{
		return m_scanner.get_file_name();
	}

	virtual int get_line_no()
	{
		return this->m_state_current.line_no;
	};

	gv_string_tmp get_file_info()
	{
		gv_string_tmp s;
		s << get_file_name() << "(" << get_line_no() << ")";
		return s;
	}

	virtual bool load_string(const char* s)
	{
		this->m_scanner.load_string(s);
		this->m_state_current.line_no = 0;
		this->m_state_current.current = this->m_scanner.begin();
		this->m_val_string = "";
		this->m_record_string = "";
		this->m_is_recording = false;
		return true;
	};
	virtual bool goto_string(const char* s)
	{
		this->m_scanner.goto_string(s);
		this->m_state_current.line_no = 0;
		this->m_state_current.current = this->m_scanner.begin();
		this->m_val_string = "";
		this->m_record_string = "";
		this->m_is_recording = false;
		return true;
	}
	virtual bool load_file(const char* s)
	{
		if (this->m_scanner.load_file(s))
		{
			this->m_state_current.line_no = 0;
			this->m_file_name = s;
			this->m_state_current.current = this->m_scanner.begin();
			this->m_val_string = "";
			this->m_record_string = "";
			this->m_is_recording = false;
			return true;
		}
		return false;
	};

	virtual gv_byte look_ahead(int count = 0)
	{
		typename type_of_scanner::iterator pc = this->m_state_current.current;
		while (*pc && gvt_is_space(*pc))
		{
			pc++;
		}
		for (int i = 0; i < count; i++)
		{
			pc++;
			while (*pc && gvt_is_space(*pc))
			{
				pc++;
			}
		}
		gv_byte ret = *pc;
		return ret;
	}
	virtual int look_ahead_token(int count = 0)
	{
		gvt_lexer l;
		int ret = 0;
		l.goto_string(current());
		for (int i = 0; i <= count; i++)
			ret = l.lex();
		return ret;
	};

	virtual const gv_string_tmp& strip_until(gv_byte key)
	{
		enable_record(true);
		gv_byte t = next();
		while (t != key && t != '\0')
		{
			t = next();
		}
		enable_record(false);
		return m_record_string;
	};

	virtual const gv_string_tmp& strip_until_token(int end_token)
	{
		enable_record(true);
		int token = lex();
		while (token != end_token && token != '\0')
		{
			token = lex();
		}
		enable_record(false);
		return m_record_string;
	};

	virtual const gv_string_tmp& strip_until_string(const gv_string_tmp & end_string)
	{
		enable_record(true);
		int token = lex();
		while ( token != '\0')
		{
			if (this->get_string() == end_string) break;
			token = lex();
		}
		enable_record(false);
		return m_record_string;
	};


	virtual const gv_string_tmp& strip_this_line()
	{
		enable_record(true);
		gv_byte t = next();
		while (t != '\r' && t != '\0' && t != '\n')
		{
			t = next();
		}
		// Windows use "\r\n" as end of line, check if we need read one more byte
		if (t == '\r' && m_state_current.current[0] == '\n')
		{
			t = next();
		}
		enable_record(false);
		return m_record_string;
	};

	virtual const gv_string_tmp& get_record()
	{
		return m_record_string;
	};

	virtual void enable_record(bool enable = true)
	{
		this->m_is_recording = enable;
		if (enable)
			this->m_record_string = "";
	}

	virtual void step_out_use_token(int start_token, int end_token)
	{
		int depth = 1;
		while (depth && !is_empty())
		{
			int token = lex();
			if (token == start_token)
				depth++;
			if (token == end_token)
				depth--;
		}
	};

	template < int start_token_num, int end_token_num >
	inline void step_out_use_tokens(int (&start_tokens)[start_token_num],
									int (&end_tokens)[end_token_num])
	{
		int depth = 1;
		while (depth && !is_empty())
		{
			int token = lex();
			for (int i = 0; i < start_token_num; i++)
				if (start_tokens[i] == token)
					depth++;
			for (int i = 0; i < end_token_num; i++)
				if (end_tokens[i] == token)
					depth--;
		}
	};

	inline void step_out(int start_char, int end_char)
	{
		int depth = 1;
		while (depth && !is_empty())
		{
			int c = (int)next();
			if (c == start_char)
				depth++;
			if (c == end_char)
				depth--;
		}
	};

	virtual const char* get_token_name(int token)
	{
		return gv_lang_cpp::token_to_string(token);
	}

	inline bool is_empty()
	{
		return this->m_state_current.current == m_scanner.end();
	}

	inline const char* get_current()
	{
		return this->current();
	}

private:
	bool m_is_marked;
	type_of_scanner m_scanner;
	gv_string m_file_name;
	state m_state_current;
	state m_state_marked;
	bool m_is_recording;
	gv_string_tmp m_record_string;
};

typedef gvt_lexer< gv_scanner_string > gv_lexer;
}