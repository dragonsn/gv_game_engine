#include "gv_base_internal.h"

namespace gv
{

namespace gv_cpp_string_to_string_detail
{

///////////////////////////////////////////////////////////////////////////
/* esc.c	Map escape sequences to single characters */
/*------------------------------------------------------------*/

#define ISHEXDIGIT(x) \
	(isdigit(x) || ('a' <= (x) && (x) <= 'f') || ('A' <= (x) && (x) <= 'F'))
#define ISOCTDIGIT(x) ('0' <= (x) && (x) <= '7')

static int hex2bin(int c)

{
	/* Convert the hex digit represented by 'c' to an int. 'c' must be one of
  * the following characters: 0123456789abcdefABCDEF
  */
	return (isdigit(c) ? (c) - '0' : ((toupper(c)) - 'A') + 10) & 0xf;
}

static int oct2bin(int c)
{
	/* Convert the hex digit represented by 'c' to an int. 'c' must be a
  * digit in the range '0'-'7'.
  */
	return (((c) - '0') & 0x7);
}

/*------------------------------------------------------------*/

int esc(char** s)
{
	/* Map escape sequences into their equivalent symbols. Return the equivalent
  * ASCII character. *s is advanced past the escape sequence. If no escape
  * sequence is present, the current character is returned and the string
  * is advanced by one. The following are recognized:
  *
  *	\b	backspace
  *	\f	formfeed
  *	\n	newline
  *	\r	carriage return
  *	\s	space
  *	\t	tab
  *	\e	ASCII ESC character ('\033')
  *	\DDD	number formed of 1-3 octal digits
  *	\xDDD	number formed of 1-3 hex digits
  *	\^C	C = any letter. Control code
  */

	int rval;

	if (**s != '\\')
		rval = *((*s)++);
	else
	{
		++(*s); /* Skip the \ */
		switch (toupper(**s))
		{
		case '\0':
			rval = '\\';
			break;
		case 'B':
			rval = '\b';
			break;
		case 'F':
			rval = '\f';
			break;
		case 'N':
			rval = '\n';
			break;
		case 'R':
			rval = '\r';
			break;
		case 'S':
			rval = ' ';
			break;
		case 'T':
			rval = '\t';
			break;
		case 'E':
			rval = '\033';
			break;

		case '^':
			rval = *++(*s);
			rval = toupper(rval) - '@';
			break;

		case 'X':
			rval = 0;
			++(*s);
			if (ISHEXDIGIT(**s))
			{
				rval = hex2bin(*(*s)++);
			}
			if (ISHEXDIGIT(**s))
			{
				rval <<= 4;
				rval |= hex2bin(*(*s)++);
			}
			if (ISHEXDIGIT(**s))
			{
				rval <<= 4;
				rval |= hex2bin(*(*s)++);
			}
			--(*s);
			break;

		default:
			if (!ISOCTDIGIT(**s))
				rval = **s;
			else
			{
				++(*s);
				rval = oct2bin(*(*s)++);
				if (ISOCTDIGIT(**s))
				{
					rval <<= 3;
					rval |= oct2bin(*(*s)++);
				}
				if (ISOCTDIGIT(**s))
				{
					rval <<= 3;
					rval |= oct2bin(*(*s)++);
				}
				--(*s);
			}
			break;
		}
		++(*s);
	}
	return rval;
}
}

void gv_cpp_string_to_string(gv_string_tmp& s, bool do_esc)
{
#pragma GV_REMINDER("MEMO optimize gv_cpp_string_to_string")
	using namespace gv_cpp_string_to_string_detail;
	gv_string_tmp ret;
	if (s.strlen() < 2)
		return;
	s.middle(1, s.strlen() - 2, ret);
	char* pc = ret.begin();
	s = "";
	int c;
	if (do_esc)
		while ((c = esc(&pc)) != 0)
			s += (char)c;
	else
		s = ret;
};

bool gv_scanner_string::load_file(const char* file_name)
{
	this->m_is_from_file = true;
	this->m_file_name = file_name;
	return gv_load_file_to_string(*m_file_name, this->m_string);
}

#define d_trans d_trans_string
#define tab_accept tab_accept_string
#define LEX_FUNC_NAME template <> \
int gvt_lexer< gv_scanner_string >::lex

#include "gv_lex.hpp"

#undef d_trans
#undef tab_accept
#undef LEX_FUNC_NAME
};
