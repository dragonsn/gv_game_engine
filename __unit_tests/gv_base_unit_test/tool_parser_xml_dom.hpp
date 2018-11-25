namespace  tool_parser_xml_dom
{

	void main( gvt_array<gv_string > &args )
	{

		/*!!!!!!!!!!!!!>>>>Function:................
		[Frames below may be incorrect and/or missing, no symbols loaded for kernel32.dll]
		*/

		// !TOKEN_BOOL_UNOP
		//<!ATTLIST RmEffectWorkspace 
			//NAME                          CDATA #REQUIRED
			//API                           CDATA #REQUIRED
			//ACTIVE_EFFECT                 CDATA #REQUIRED
			//VALIDATION_STRING             CDATA #REQUIRED
			//XML_VERSION                   CDATA #REQUIRED
		//>
		/*
		GVM_DOM_ELEMENT(test_el_1)
			GVM_DOM_ATTRIB(gv_string , att_s)
			GVM_DOM_ATTRIB(gv_float  , att_f)
			GVM_DOM_ATTRIB(gv_bool   , att_b)
		GVM_DOM_END_E
		*/
		
		using namespace gv::gv_lang_cpp;
		{

			sub_test_timer timer("unit_test_tool_trace_parser");
			gvt_lexer<gv_scanner_string> lexer; 
			if (args.size() )
				lexer.load_file(*args[0]); 
			else 
				lexer.load_file("../dom_test.txt"); 

			TOKEN token=(TOKEN)lexer.lex(); 
			gvi_stream * ps=test_get_fm()->open_text_file_for_write("dom_result.txt"); 
			gv_string_tmp stream; 
			while (token==TOKEN_SMALLER)
			{	
				lexer>>"!"; 
				lexer>>"ATTLIST";
				gv_string struct_name;
				lexer>>struct_name;
				stream<<"GVM_DOM_ELEMENT("<<struct_name<<")"<<gv_endl; 
				token=(TOKEN)lexer.lex(); 
				while(token==TOKEN_ID)
				{
					stream<<"\tGVM_DOM_ATTRIB(gv_string ,"<< lexer.get_id()<<")"<<gv_endl;
					lexer.strip_this_line();
					token=(TOKEN)lexer.lex(); 
				}
				stream<<"GVM_DOM_END_E"<<gv_endl<<gv_endl;
				GV_ASSERT(token==TOKEN_GREATER);
				token=(TOKEN)lexer.lex(); 
			}
			(*ps)<<stream;
			delete ps; 
		}
	}
}
