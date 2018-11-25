#include "gv_framework_private.h"
#include "gv_id_lexer.h"
namespace gv
{

#define d_trans d_trans_id
#define tab_accept tab_accept_id
#define LEX_FUNC_NAME int gv_id_lexer::lex

#include "gv_id_lex.hpp"

#undef d_trans
#undef tab_accept
#undef LEX_FUNC_NAME
}