#include "gv_base_internal.h"
#include "gvt_array.h"
#include "gvt_list.h"
#include "gv_string.h"
#include "gv_file_manager.h"
#include "gv_time.h"
#include "gv_lexer.h"
#include "gv_xml_parser.h"

#include <memory.h>
#include <iostream>
namespace gv
{
// docment-> start_ele element | document;
using namespace gv_lang_xml;
bool gv_xml_parser::do_docment(document& doc)
{
	while (next_token())
	{
		switch (m_token)
		{
		case TOKEN_XML_PROCESS_BLOCK:
			this->do_process_block();
			break;

		case TOKEN_XML_CDATA_BLOCK:
			this->do_data_block();
			break;

		case TOKEN_XML_DOCTYPE_BLOCK:
			this->do_data_block();
			break;

		case TOKEN_XML_ELE_BGN_LEFT:
			this->do_element_open(doc);
			break;

		case TOKEN_XML_COMMENT:
			break;

		default:
			error("wrong token!illigal error xml file!");
		}
	}
	return true;
};
// element -> attrib (end_ele | ele_half_end   (start_ele element)*  content
// end_ele_with_name) ;
bool gv_xml_parser::do_element_open(element& father)
{
	next_match_token(TOKEN_XML_NAME);
	this->m_current_element_name = this->get_id();
	this->do_element_close(father, m_current_element_name);
	return false;
};
// attrib -> id equal_op (string|id |float)
bool gv_xml_parser::do_attrib(gv_lang_xml::element& owner)
{
	gv_byte b = this->look_ahead();
	if (b != '>' && b != '/')
	{
		this->next_match_token(TOKEN_XML_NAME);
		gv_string_tmp name = get_id();
		this->next_match_token(TOKEN_ASSIGNOP);
		next_token();
		switch (this->m_token)
		{
		case TOKEN_STRING:
			owner.get_data()->set_attrib_value(name, get_string());
			break;
		default:
			error("error in attribute !!");
			break;
		}
		return true;
	}
	return false;
};
//
bool gv_xml_parser::do_element_close(element& father,
									 const gv_string_tmp& element_name)
{
	// overrider ther function , do your own parser herer!!!
	if (this->filter_element(element_name))
		return false;
	element* pel = &*father.m_children.add_dummy();
	pel->m_father = &father;
	pel->m_element_data = this->create_element_data(element_name);
	pel->m_element_name = element_name;
	pel->m_element_depth = father.m_element_depth + 1;
	GV_ASSERT(pel->m_element_depth <= element::gv_max_xml_element_depth);
	while (this->do_attrib(*pel))
	{
		// process attrib
	}
	int token = lex();
	while (token)
	{
		if (token == TOKEN_XML_ELE_END_RIGHT)
		{
			return true;
		}
		else if (token == TOKEN_XML_ELE_END_LEFT)
		{
			this->next_match_token(TOKEN_XML_NAME);
			if (this->get_id() != pel->m_element_name)
			{
				error("brace <> mismatch!!!");
			}
			this->strip_until('>');
			return true;
		}
		else if (token == TOKEN_XML_ELE_BGN_LEFT)
		{
			// sub element;
			this->do_element_open(*pel);
		}
		else
		{
			pel->get_data()->add_content();
		}
		token = lex();
	};
	return false;
};
//===========================================
bool gv_xml_parser::next_match_token(int target, bool do_assert)
{
	next_token();
	if (m_token != target)
	{
		if (do_assert)
		{
			gv_string_tmp s;
			s << "want a " << gv_lang_xml::token_to_string(target) << "but find a "
			  << gv_lang_xml::token_to_string(m_token);
			error(*s);
		}
		return false;
	}
	return true;
};
//===========================================
void gv_xml_parser::step_out_element(const char* name)
{
	int depth = 1;
	if (name)
	{
		m_current_element_name = name;
	}
	while (depth && !is_empty())
	{
		int token = lex();
		if (token == TOKEN_XML_ELE_END_RIGHT)
		{
			depth--;
		}
		else if (token == TOKEN_XML_ELE_END_LEFT)
		{
			this->next_match_token(TOKEN_XML_NAME);
			if (!depth && this->get_id() != this->m_current_element_name)
			{
				error("brace <> mismatch!!!");
			}
			this->strip_until('>');
			depth--;
		}
		else if (token == TOKEN_XML_ELE_BGN_LEFT)
		{
			depth++;
		}
	}
}
//==========================================

#define d_trans d_trans_string_xml
#define tab_accept tab_accept_string_xml
#define LEX_FUNC_NAME int gv_xml_parser::lex

#include "../lex/gv_lex_xml.hpp"

#undef d_trans
#undef tab_accept
#undef LEX_FUNC_NAME
}