namespace unit_test_lexer
{

void main(gvt_array< gv_string >& args)
{
	using namespace gv::gv_lang_cpp;
	{
		TOKEN tokens[] =
			{
				TOKEN_STRING,
				TOKEN_ID,
				TOKEN_ICON,
				TOKEN_ICON,
				TOKEN_FCON,
				TOKEN_ICON,
				TOKEN_BOOL_UNOP,
				(TOKEN)0,
			};

		sub_test_timer timer("simple_string_lexer");
		char text[] = "\"1.01\" number 0 123 12.4 0x12112 /*fdffffffffff*/!";
		gvt_lexer< gv_scanner_string > lexer;
		lexer.load_string(text);
		TOKEN token = (TOKEN)lexer.lex();
		int count = 0;
		GV_ASSERT(token == tokens[count++]);
		test_log() << "text::==" << text << gv_endl;
		test_log() << "token::==";
		while (token)
		{
			test_log() << gv::gv_lang_cpp::token_to_string(token) << "\t";
			token = (TOKEN)lexer.lex();
			GV_ASSERT(count < GV_ARRAY_LENGTH(tokens));
			GV_ASSERT(token == tokens[count++]);
		};
		test_log() << gv_endl;
	}
	{
		TOKEN tokens[] =
			{
				TOKEN_ID,
				TOKEN_ICON,
				TOKEN_FCON,
				TOKEN_ICON,
				TOKEN_BOOL_UNOP,
				(TOKEN)0,
			};

		sub_test_timer timer("simple _string lexer load a small file!!");
		gvi_stream* ps = gv_global::fm->open_text_file_for_write("test.txt");
		char text[] = "number 123 12.4 0x12112 /*fdffffffffff*/!";
		GV_ASSERT(ps);
		gvi_stream& s = *ps;
		s << text;
		s.close();
		gvt_lexer< gv_scanner_string > lexer;
		lexer.load_file("test.txt");
		TOKEN token = (TOKEN)lexer.lex();
		int count = 0;
		GV_ASSERT(token == tokens[count++]);
		while (token)
		{
			token = (TOKEN)lexer.lex();
			GV_ASSERT(count < GV_ARRAY_LENGTH(tokens));
			GV_ASSERT(token == tokens[count++]);
		};
		delete (ps);
	}

	{
		//gvi_stream * ps=gv_global::fm->open_text_file_for_read("../good.xml");
		//GV_ASSERT(ps);
		//gv_string s;
		//(*ps)>>s;
		//delete ps;
	}
}
}
