namespace  tool_trace_parser
{
		
		void main( gvt_array<gv_string > &args )
		{

			/*!!!!!!!!!!!!!>>>>Function:................
			[Frames below may be incorrect and/or missing, no symbols loaded for kernel32.dll]
			*/

			// !TOKEN_BOOL_UNOP
			// [TOKEN_LB

			using namespace gv::gv_lang_cpp;
			{

				sub_test_timer timer("unit_test_tool_trace_parser");
				gvt_lexer<gv_scanner_string> lexer; 
				if (args.size() )
					lexer.load_file(*args[0]); 
				else 
					lexer.load_file("../../tracedata.TXT"); 

			
				TOKEN token=(TOKEN)lexer.lex(); 
				gvt_hash_map<gv_string_tmp,int,1024> map; 
				
				while (token)
				{
					if(token==TOKEN_BOOL_UNOP)
					{
						lexer.strip_this_line();
						gv_byte b=lexer.look_ahead();
						gv_string_tmp callstack_name;
						while(b && b!='[') 
						{
							gv_string_tmp line=lexer.strip_this_line(); 
							callstack_name+=line;
							callstack_name+="->";
							b=lexer.look_ahead();
						}
						int * pint =map.find(callstack_name); 
						if (!pint)
						{
							map.add(callstack_name,1);
						}
						else
							(*pint)+=1;
					}
					else
					{
						lexer.strip_this_line();
					}
					token=(TOKEN)lexer.lex(); 
				};

				gvi_stream * ps=gv_global::fm->open_text_file_for_write("trace_result.txt"); 
				gvt_hash_map<gv_string_tmp,int,1024>::iterator it=map.begin();
				gv_string_tmp final;
				while(it!=map.end())
				{
					if (!it.is_empty())
					{
						gv_string_tmp  name= *it.get_key(); 
						int		time=*it;
						//name.replace_all("\r", ">"); 
						//name.replace_all("\n", ">"); 
						final<<"|times|"<<time << "\t"<<"\r\n"<< "|callstack|" << *name<<"\r\n" ;//<< gv_endl; 
					}
					it++;
				}
				*ps<<gv_get_const_string(final);
				delete(ps);
			}
		}
}
