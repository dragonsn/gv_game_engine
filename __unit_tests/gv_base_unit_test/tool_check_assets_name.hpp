namespace  tool_check_asset_name
{
	void report_teamcity_error(const gv_string_tmp & s)
	{
		GVM_CONSOLE_OUT(gv_endl);
		GVM_CONSOLE_OUT("##teamcity[compilationStarted compiler='tool_check_asset_name']" );
		GVM_CONSOLE_OUT("##teamcity[message text = '" << s << " status='ERROR' ']" );
		//GVM_CONSOLE_OUT("##teamcity[message text='compiler error' status='ERROR']" );
		GVM_CONSOLE_OUT("##teamcity[compilationFinished compiler='tool_check_asset_name']" );
	};

	void main(gvt_array<gv_string > &args)
	{

		/*!!!!!!!!!!!!!>>>>Function:................
		[Frames below may be incorrect and/or missing, no symbols loaded for kernel32.dll]
		*/

		// !TOKEN_BOOL_UNOP
		// [TOKEN_LB
		struct info
		{
			info(){  }
			info(const info & in)
			{
				*this = in;
			}
			info & operator=(const info & i)
			{
				packages = i.packages;
				return *this;
			}
			gvt_array<gv_string> packages;
			gv_int times;

		};
		
		using namespace gv::gv_lang_cpp;
		{
			//[Step 3 / 5]  170.8 kb	 77.3% Assets / Models / Monsters / FODDER / TEX / TEX_EN010_HotpotFire_D.tga
			//***Player size statistics***
			//[12:37 : 14] :	 [Step 3 / 5] * **Player size statistics***
			//[12:37 : 14] : [Step 3 / 5] Level 0 'Assets/StreamingAssets/TEX_CFXM3_T_SmallRay' uses 0.9 KB compressed / 7.0 KB uncompressed

		
			if (args.size() != 2) return;
			{//check missing meta//READ ONLY FLAG NOT RELIABLE
				/*gvt_lexer<gv_scanner_string> lexer;
				lexer.load_file(*args[0]);
				bool include_cs = false;
				if (args.size() > 1){
					include_cs = true;
				}
				gv_string_tmp line = lexer.strip_this_line();
				while (line.strlen())
				{
					if (line.find_string(".meta")) {
						gv_string_tmp msg = "[missing meta file in perforce]";
						msg << line;
						report_teamcity_error(msg);
					}
					line = lexer.strip_this_line();
				};
				*/
			}
			{//check GUID & duplicated resource name
				gvt_lexer<gv_scanner_string> lexer;
				lexer.load_file(*args[1]);
				gvt_hash_map<gv_string, gv_string> name_table;
				gvt_hash_map<gv_string, gv_string >guid_table; 
				gvt_hash_map<gv_string, gv_string >meta_table;
				bool include_cs = false;
				if (args.size() > 1){
					include_cs = true;
				}
				gv_string_tmp line = lexer.strip_this_line();
				while (line.strlen())
				{
					line.trim_back();
					if (line.find_string(".meta")) {
						gvt_lexer<gv_scanner_string> lexer_meta;
						lexer_meta.load_file(*line);
						gv_string_tmp main_name = line;
						main_name.replace_postfix(".meta", "");
						meta_table[*main_name] = line;
						gv_int token = lexer_meta.lex();
						gv_string guid;
						while (token)
						{
							if (token == TOKEN_ID)
							{
								gv_string_tmp s = lexer_meta.get_record();
								gv_string_tmp id = lexer_meta.get_id();
								if (id == "guid"){
									lexer_meta>>":" ;//skip :
									//lexer_meta.lex();//
									guid = lexer_meta.strip_until('\n');
									break; 
								}
							}
							token = (TOKEN)lexer_meta.lex();
						};
						if (guid.size())
						{
							gv_string old_name;
							if (guid_table.find(guid, old_name)){
								gv_string_tmp msg;
								msg << "!!!guid collision!!! " << line << " ->[VS]<- " << old_name;
								report_teamcity_error(msg);
							}
						}
					}
					else
					{
						gv_string main_name=gv::gv_global::fm->get_main_name(*line);
						gv_string old_name;
						if (name_table.find(main_name,old_name)){
							gv_string_tmp msg;
							msg << " !!!name collision!!! " << line << " ->[VS]<- " << old_name;
							//report_teamcity_error(msg);
							gv_console_output(msg);
						}
						else name_table.add(main_name, *line);
						if (!meta_table.find(*line)){
							meta_table.add(*line, "");
						}
					}
					line = lexer.strip_this_line();
				};
				gvt_hash_map<gv_string, gv_string >::iterator it = meta_table.begin();
				while (it != meta_table.end()){
					if (!it.is_empty()){
						if (!it->strlen()) {
							gv_string_tmp msg = "!!!missing meta file in perforce!!!";
							msg << it.get_key();
							report_teamcity_error(msg);
						}
					}
					it++;
				}
				//=====================================
			}

		}//main
	}
}
