#pragma once
namespace gv
{

template < class type_of_string >
inline void gv_xml_backup_gt_lt(type_of_string& s, gv_int gt_rp = '}',
								gv_int lt_rp = '{')
{
	typename type_of_string::iterator it;
	it = s.begin();
	while (it != s.end())
	{
		if (*it == '>')
			*it = (typename type_of_string::type_of_data)gt_rp;
		if (*it == '<')
			*it = (typename type_of_string::type_of_data)lt_rp;
		++it;
	}
}

template < class type_of_string >
inline void gv_xml_restore_gt_lt(type_of_string& s, gv_int gt_rp = '}',
								 gv_int lt_rp = '{')
{
	typename type_of_string::iterator it;
	it = s.begin();
	while (it != s.end())
	{
		if (*it == (typename type_of_string::type_of_data)gt_rp)
			*it = '>';
		if (*it == (typename type_of_string::type_of_data)lt_rp)
			*it = '<';
		++it;
	}
}

// the real structure hold the true data, implement your own!!
template < class type_of_stream, class type_of_value >
inline void gvt_xml_write_attribute(type_of_stream& s, const char* name,
									const type_of_value& v)
{
	gv_string_tmp shelp;
	shelp << name << "=\"";
	shelp << v;
	shelp << "\" ";
	s << *shelp;
};

template < class type_of_stream >
inline void gvt_xml_write_attribute_name(type_of_stream& s, const char* name)
{
	gv_string_tmp shelp;
	shelp << name << "=";
	s << *shelp;
};

template < class type_of_stream >
inline void gvt_xml_write_element_open(type_of_stream& s,
									   const char* name = NULL,
									   bool complete = true)
{
	gv_string_tmp shelp;
	if (!name)
	{
		s << ">\r\n";
	}
	else
	{
		shelp << "<";
		shelp << name << " ";
		if (complete)
			shelp << "> \r\n ";
		s << *shelp;
	}
};

template < class type_of_stream >
inline void gvt_xml_write_element_close(type_of_stream& s,
										const char* name = NULL)
{
	gv_string_tmp shelp;
	if (!name)
	{
		s << "/>\r\n";
	}
	else
	{
		shelp << "</";
		shelp << name;
		shelp << ">\r\n";
		s << *shelp;
	}
};

namespace gv_lang_xml
{
#define GVM_PP_LEXER_DEF_TOKEN 1
#include "gv_pp_lexer.h"
enum TOKEN
{
	TOKEN_FIRST = 255,
#include "gv_lang_xml.h"
	TOKEN_LAST
};
#include "gv_pp_lexer.h"

#define GVM_PP_LEXER_GET_TOKEN_STR 1
#include "gv_pp_lexer.h"
inline const char* token_to_string(int token)
{
	switch (token)
	{
#include "gv_lang_xml.h"
	};
	return "unknown";
}
#include "gv_pp_lexer.h"

class element_data
{
public:
	element_data(){};
	virtual ~element_data(){};
	virtual const char* get_type_name()
	{
		return "undefined";
	};
	virtual bool set_attrib_value(const gv_string_tmp& name,
								  const gv_string_tmp& value)
	{
		return false;
	};
	virtual bool add_content()
	{
		return false;
	}
};

class element
{
public:
	element()
	{
		m_element_data = 0;
		m_father = 0;
		m_element_depth = 0;
	};
	virtual ~element()
	{
		gvt_safe_delete(m_element_data);
	};
	element(const element& e)
	{
	}
	gvt_dlist< element > m_children;
	element* m_father;
	element_data* m_element_data;
	gv_string_tmp m_element_name;
	int m_element_depth;

	static const int gv_max_xml_element_depth = 32;
	template < class stream_type >
	inline void dump_tree(stream_type& str, int indent = 0)
	{
		for (int i = 0; i < indent; i++)
			str << "\t";
		str << "===D " << *this->m_element_name << "\n";
		gvt_dlist< element >::iterator it = m_children.begin();
		while (it != m_children.end())
		{
			it->dump_tree(str, indent + 1);
			it++;
		}
	}
	element_data* get_data()
	{
		return m_element_data;
	}
};

class document : public element
{
public:
	document(){};
	~document(){};
	gvt_dlist< gv_string > m_process_blocks;
	gvt_dlist< gv_string > m_data_blocks;
};

}; // gv_lang_xml

class gv_xml_parser : public gvt_lexer< gv_scanner_string >
{
public:
	gv_xml_parser()
	{
		m_user_data = 0;
	}
	virtual ~gv_xml_parser(){};
	typedef void (gv_xml_parser::*ELEMENT_CALLBACK)();
	//==============you can call the lex directly=======
	virtual int lex();
	virtual const gv_string_tmp& token_to_string()
	{
		gv_cpp_string_to_string(m_val_string, false);
		return m_val_string;
	};
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111
	virtual const char* get_token_name(int token)
	{
		return gv_lang_xml::token_to_string(token);
	}
	//-=====================parser of xml  grammar
	// docment-> start_ele element | document;
	virtual bool do_docment(gv_lang_xml::document& doc);
	// element -> attrib (end_ele | ele_half_end   (start_ele element)*  content
	// end_ele_with_name) ;
	virtual bool do_element_open(gv_lang_xml::element& father);
	// attrib -> id equal_op (string|id |float)
	virtual bool do_attrib(gv_lang_xml::element& owner);
	//
	virtual bool do_element_close(gv_lang_xml::element& father,
								  const gv_string_tmp& element_name);
	//
	virtual bool do_process_block()
	{
		return false;
	};
	//
	virtual bool do_data_block()
	{
		return false;
	}
	//
	virtual bool filter_element(const gv_string_tmp& name)
	{
		return false;
	}
	//
	virtual gv_lang_xml::element_data*
	create_element_data(const gv_string_tmp& element_name)
	{
		return new gv_lang_xml::element_data;
	};

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111
	// some light weight xml helper function , can be used to parse some fixed
	// format xml
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111
	inline bool read_element_open(const char* name = NULL, bool complete = true)
	{
		using namespace gv_lang_xml;
		if (!name)
		{
			// s<<">\r\n";
			this->next_match_token(TOKEN_XML_ELE_BGN_RIGHT);
		}
		else
		{
			this->next_match_token(TOKEN_XML_ELE_BGN_LEFT);
			this->next_match_token(TOKEN_XML_NAME);
			GV_ASSERT(get_id() == name);
			if (complete)
				next_match_token(TOKEN_XML_ELE_BGN_RIGHT);
		}
		return true;
	};

	inline bool read_element_open_get_name(gv_string_tmp& name,
										   bool complete = true)
	{
		using namespace gv_lang_xml;
		if (!this->next_match_token(TOKEN_XML_ELE_BGN_LEFT))
			return false;
		if (!this->next_match_token(TOKEN_XML_NAME))
			return false;
		name = get_id();
		if (complete)
		{
			if (!next_match_token(TOKEN_XML_ELE_BGN_RIGHT))
				return false;
		}
		return true;
	};

	inline bool read_element_close(const char* name = NULL)
	{
		using namespace gv_lang_xml;
		gv_string_tmp shelp;
		if (!name)
		{
			next_match_token(TOKEN_XML_ELE_END_RIGHT);
		}
		else
		{
			next_match_token(TOKEN_XML_ELE_END_LEFT);
			this->next_match_token(TOKEN_XML_NAME);
			GV_ASSERT(get_id() == name);
			next_match_token(TOKEN_XML_ELE_BGN_RIGHT);
		}
		return false;
	};

	// the real structure hold the true data, implement your own!!
	template < class type_of_value >
	void read_attribute(const char* name, type_of_value& v)
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_XML_NAME);
		GV_ASSERT(get_id() == name);
		this->next_match_token(TOKEN_ASSIGNOP);
		this->next_match_token(TOKEN_STRING);
		gv_xml_parser ps;
		ps.goto_string(*this->get_string());
		ps >> v;
		;
	};

	void read_attribute(const char* name, gv_string_tmp& v)
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_XML_NAME);
		GV_ASSERT(get_id() == name);
		this->next_match_token(TOKEN_ASSIGNOP);
		this->next_match_token(TOKEN_STRING);
		v = this->get_string();
	};

	const gv_string_tmp& read_attribute_name()
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_XML_NAME);
		return get_id();
	};

	template < class type_of_value >
	void read_attribute_value(type_of_value& v)
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_STRING);
		gv_xml_parser ps;
		ps.goto_string(*this->get_string());
		ps >> v;
	};

	void read_attribute_value(gv_string_tmp& v)
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_STRING);
		v = this->get_string();
	};

	void read_attribute_value(gv_string& v)
	{
		using namespace gv_lang_xml;
		this->next_match_token(TOKEN_STRING);
		v = this->get_string();
	};

	gv_string_tmp get_cdata_content()
	{
		const gv_string_tmp& s = get_id();
		gv_string_tmp t;
		if (s.strlen() > 12)
		{
			t = s.middle(9, s.strlen() - 12);
		}
		return t;
	}
	bool is_element_will_close()
	{
		return this->look_ahead() == '<' && this->look_ahead(1) == '/';
	}

	int next_token()
	{
		m_token = lex();
		return m_token;
	};
	bool next_match_token(int target, bool do_assert = true);
	void step_out_element(const char* name = NULL);
	gv_uint get_user_data()
	{
		return m_user_data;
	}
	void set_user_data(gv_uint _user_data)
	{
		m_user_data = _user_data;
	}

protected:
	int m_token;
	gv_lang_xml::document* m_doc;
	gv_string_tmp m_current_element_name;
	gv_uint m_user_data;
};

} // gv