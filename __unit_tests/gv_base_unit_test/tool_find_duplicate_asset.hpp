namespace  tool_find_duplicate_asset
{

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

			sub_test_timer timer("tool_find_duplicate_asset");
			gvt_lexer<gv_scanner_string> lexer;
			if (args.size())
				lexer.load_file(*args[0]);
			else
				lexer.load_string("[Step 3 / 5]  170.8 kb	 77.3% Assets / Models / Monsters / FODDER / TEX / TEX_EN010_HotpotFire_D.tga");
			bool include_cs = false;
			if (args.size() > 1){
				include_cs = true;
			}
			TOKEN token = (TOKEN)lexer.lex();
			gvt_hash_map<gv_string, info, 1024> map;
			gv_string_tmp line = lexer.strip_this_line();
			gvt_array<gv_string> asset_in_pack;
			while (line.strlen())
			{
				const char * ptr; 
				ptr = line.find_string("% Assets");
				const char * is_cs = line.find_string(".cs");
				if (ptr) {
					if (include_cs || !is_cs) 
					asset_in_pack.add_unique(ptr); 
				}
				ptr = line.find_string("**Player size statistics***");
				if (ptr){
					line = lexer.strip_this_line();
					ptr = line.find_string("'");
					char * end = (char*)line.find_string("'", NULL,false);
					GV_ASSERT(ptr);
					GV_ASSERT(end);
					*end = 0;
					gv_string_tmp pkname =ptr ;
					if (ptr){
						for (int i = 0; i < asset_in_pack.size(); i++){
							map[asset_in_pack[i]].packages.add_unique(*pkname);
						}
					}
					*end = '\'';
					asset_in_pack.clear();
				}
				line = lexer.strip_this_line();
			};

			gvi_stream * ps = gv_global::fm->open_text_file_for_write("result.txt");
			gvt_hash_map<gv_string, info, 1024>::iterator it = map.begin();
			gv_string_tmp final;
			while (it != map.end())
			{
				if (!it.is_empty())
				{
					gv_string_tmp  name = *it.get_key();
					info & info = *it;
					if (info.packages.size() > 1){
						final << name << "(" << info.packages.size()<<")";
						for (int i = 0; i < info.packages.size(); i++){
							final << "          <----------------------" << info.packages[i] << gv_endl;
						}
						final << gv_endl;
					}
				}
				it++;
			}
			*ps << gv_get_const_string(final);
			delete(ps);
		}
	}
}
