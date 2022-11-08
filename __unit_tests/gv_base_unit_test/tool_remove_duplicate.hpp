//============================================================================================
//								:
//============================================================================================

namespace  tool_remove_duplicate
{
	void main(gvt_array<gv_string >& args)
	{

		/*!!!!!!!!!!!!!>>>>Function:................
		[Frames below may be incorrect and/or missing, no symbols loaded for kernel32.dll]
		*/

		using namespace gv::gv_lang_cpp;
		{

			sub_test_timer timer("unit_test_tool_remove_duplicate");
			gvt_lexer<gv_scanner_string> lexer;
			if (args.size())
				lexer.load_file(*args[0]);
			else
				lexer.load_file("../../tracedata.TXT");

			gv_string_tmp filter;
			if (args.size() > 1)
			{
				filter = args[1];
			}

			
			gvt_hash_map<gv_string_tmp, int, 1024> map;
			gvi_stream* ps = gv_global::fm->open_text_file_for_write("remove_result.txt");
			int line_count = 0;
			while (!lexer.is_empty())
			{
				gv_string_tmp line=lexer.strip_this_line();
				line_count++;
				if (filter.strlen() && line_count > 1)
				{
					if (!line.find_string(filter))
					{
						continue;
					}
				}
				int* pint = map.find(line);
				if (!pint)
				{
					map.add(line, 1);
					line.replace_all(gv_endl, "\n");
					*ps << line;
				}
				else
					(*pint) += 1;
			};
			delete(ps);
		}
	}
}