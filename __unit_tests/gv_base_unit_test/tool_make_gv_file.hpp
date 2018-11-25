namespace  tool_make_gv_file
{

	bool  ignore_uppcase( gv_string_tmp &s)
	{
		gv_char * p=s.begin();
		bool have_upcase=false;
		bool have_lowcase=false;
		while (*p)
		{
			if (*p>='A' && *p<='Z' )
			{
				have_upcase=true;
			}
			if (*p >= 'a' && *p <= 'z')
			{
				have_lowcase=true;
			}
			p++; 
		}
		return !(have_upcase&& have_lowcase);
	}

	void main( gvt_array<gv_string > &args )
	{
		using namespace gv::gv_lang_cpp;
		{

			gvt_lexer<gv_scanner_string> lexer; 
			if (args.size() )			 lexer.load_file(*args[0]); 
			else						 lexer.load_file("../test.c"); 
			gv_string_tmp text; 
			gv_string_tmp replace_prefix="";
			gv_string_tmp ignore_prefix="";

			gvi_stream * ps=gv_global::fm->open_text_file_for_write("gv_c_result.c"); 

		
			if (args.size()>1) 
			{
				replace_prefix=*args[1];
			}

			if (args.size()>2)
			{
				ignore_prefix=*args[2];	
			}

			lexer.enable_record(true);
			TOKEN token=(TOKEN)lexer.lex(); 
			lexer.enable_record(false);
			
			while (token)
			{
				if(token==TOKEN_ID)
				{
					
					gv_string_tmp s=lexer.get_record();
					gv_string_tmp id=lexer.get_id();
					if (replace_prefix !="")
					{
						gv_string_tmp id_old=lexer.get_id();
						if(id.has_prefix( replace_prefix) )
						{
							id.replace_prefix(replace_prefix,"gv");
							s.replace_all(id_old ,id);
						}
					}

					if (ignore_prefix !="" && id.has_prefix( ignore_prefix))
					{
						text<<s; 
					}
					else if (ignore_uppcase(s) )
					{
						text<<s; 
					}
					else
					{
						char * p=s.begin();
						char   last_char =0; 
						gv_string_tmp result;
						int		diff='a'-'A';
						//ignore all uppercase

						while (*p)
						{
							if (*p>='A' && *p<='Z' )
							{
								if (last_char >= 'a' && last_char <= 'z')
								{
									result << '_'; 
								}
								char c=*p+diff; 
								result<<c; 
							}
							else
								result<<*p; 
							last_char=*p; 
							p++; 
						}
						text<<result; 
					}
				}
				else
				{
					text<<lexer.get_record(); 
				}
				lexer.enable_record(true);
				token=(TOKEN)lexer.lex(); 
				lexer.enable_record(false);
			};
			(*ps)<<text;
			delete(ps);
		}
	}
}

