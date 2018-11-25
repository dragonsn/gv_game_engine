namespace  tool_c_to_cpp
{
	//--------------------------------------------
	//change the parameter declaration ; 
	//--------------------------------------------
	void main(gvt_array<gv_string > &args)
	{
		using namespace gv::gv_lang_cpp;
		{
			gvt_lexer<gv_scanner_string> lexer;
			gv_string file_name = "a.c"; 
			if (args.size()) file_name = *args[0];
			lexer.load_file(*file_name);
			gv_string_tmp text;
			gv_string_tmp replace_prefix = "";
			gv_string_tmp ignore_prefix = "";
			gv_string_tmp new_file_name = file_name + ".new";
			gvi_stream * ps = gv_global::fm->open_text_file_for_write(*new_file_name);
			lexer.enable_record(true);
			TOKEN token = (TOKEN)lexer.lex();
			lexer.enable_record(false);
			int scope_depth = 0;
			while (token)
			{
				if (token == TOKEN_ID && lexer.look_ahead_token() == TOKEN_LP )
				{
					//if (lexer.get_id() == "_tr_flush_block")
					//{
					//	GV_DEBUG_BREAK;
					//}
					if (lexer.look_ahead_token(1)==TOKEN_ID && scope_depth == 0)
					{
						//need to see is cpp compatible
						//good: add(int c , int a) {}
						//bad : add( c, a) int c ; int a {}; 
						//change the bad guy!!!
						text << lexer.get_record();
						lexer.enable_record(true);
						lexer.lex();
						lexer.step_out_use_token(TOKEN_LP, TOKEN_RP);
						lexer.enable_record(false);
						gv_string_tmp param_string = lexer.get_record();
						gvt_array<gv_string> param_names;
						bool is_param_list = false;
						{
							gvt_lexer<gv_scanner_string> sub_lexer;
							sub_lexer.load_string(param_string); 
							sub_lexer.lex();
							TOKEN st = (TOKEN)sub_lexer.lex();
							while (st == TOKEN_ID){
								param_names.push_back(*sub_lexer.get_id());
								st = (TOKEN)sub_lexer.lex();
								if (st && (st != TOKEN_COMMA  && st!=TOKEN_RP) ) {
									is_param_list = false; break;
								}
								st = (TOKEN)sub_lexer.lex();
								is_param_list = true;
							}
						}
						if (is_param_list && lexer.look_ahead_token() != TOKEN_LC)
						{//bad!!!
							gv_string_tmp param = lexer.strip_until_token(TOKEN_LC);
							char * last = (char *) param.find_string(";", 0, false); 
							if (last) *last = ')';
							param.replace_all(";", ",");
							text << "(" << param << gv_endl;
							scope_depth++;
						}
						else 
						{
							text << lexer.get_record();
						}
					}
					else
					{
						text << lexer.get_record();
					}
			
				}
				//need to skip define 
				else if (token == TOKEN_PRE_OP)
				{
					text << lexer.get_record();
					gv_string_tmp line = lexer.strip_this_line(); 
					text << line;
					line.trim_back(); 
					while (line.strlen() && line.right(1) == "\\")
					{
						line=lexer.strip_this_line();
						text << line;
						line.trim_back(); 
					}
				}
				else if (token == TOKEN_LC )
				{
					scope_depth++;
					text << lexer.get_record();
				}
				else if (token == TOKEN_RC)
				{
					scope_depth--;
					text << lexer.get_record();
				}
				else
				{
					text << lexer.get_record();
				}
				lexer.enable_record(true);
				token = (TOKEN)lexer.lex();
				lexer.enable_record(false);
			};
			text.replace_all(gv_endl, "\n");
			(*ps) << text;
			delete(ps);
		}
	}
}

