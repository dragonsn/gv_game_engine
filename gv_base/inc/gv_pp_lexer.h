// command
// GVM_PP_LEXER_DEF_TOKEN
// GVM_PP_LEXER_GET_TOKEN_STR
// GVM_PP_LEXER_DONE

#if GVM_PP_LEXER_DEF_TOKEN
#define GVM_MAKE_TOKEN(x) TOKEN_##x,
#elif GVM_PP_LEXER_GET_TOKEN_STR
#define GVM_MAKE_TOKEN(x) \
	case TOKEN_##x:       \
		return #x;
#else
#undef GVM_MAKE_TOKEN
#endif

#undef GVM_PP_LEXER_DEF_TOKEN
#undef GVM_PP_LEXER_GET_TOKEN_STR
#undef GVM_PP_LEXER_DONE
