#pragma once
namespace gv
{

namespace gv_id_space
{
#define GVM_PP_LEXER_DEF_TOKEN 1
#include "gv_pp_lexer.h"
enum TOKEN
{
	TOKEN_FIRST = 255,
#include "../utility/gv_id_space.h"
	TOKEN_LAST
};
#include "gv_pp_lexer.h"

#define GVM_PP_LEXER_GET_TOKEN_STR 1
#include "gv_pp_lexer.h"
inline const char* token_to_string(int token)
{
	switch (token)
	{
#include "../utility/gv_id_space.h"
	};
	return "unknown";
}
#include "gv_pp_lexer.h"
};

class gv_id_lexer : public gvt_lexer< gv_scanner_string >
{
public:
	gv_id_lexer()
	{
	}
	virtual ~gv_id_lexer(){};
	virtual int lex();
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111
	virtual const char* get_token_name(int token)
	{
		return gv_id_space::token_to_string(token);
	}
	int next_token()
	{
		m_token = lex();
		return m_token;
	};

protected:
	int m_token;
};
};