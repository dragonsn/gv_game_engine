//console tool tool_file_visitor -cmd -"attrib %file%"
namespace  tool_file_visitor
{
	
	void main(gvt_array<gv_string > &args)
	{
		//-path -"../../../../gv_game_engine/"
		sub_test_timer timer("tool_file_visitor");
		gv_file_manager fm;
		gvt_array<gv_string> names;
		gvt_array<gv_string> patterns;
		gv_string path = "./";

		gv_string_tmp command = "dir %file%"; 

		int idx=0;
		if (args.find("path",idx))
		{
			path = args[idx + 1]; 
			path = fm.get_directory_string(path);
		}

		if (args.find("cmd", idx))
		{
			command = args[idx + 1];
			//gv_cpp_string_to_string(command, false);
		}

		while (args.find("pattern", idx))
		{
			patterns.add(args[idx + 1]);
			args.remove(idx, 1);
		}

		if (!patterns.size()) {
			patterns.add("*.*");
		}

		for (int i = 0; i < patterns.size(); i++)
		{
			fm.find_file_with_pattern(patterns[i], names, path);
		}
		
	
		for (int i = 0; i< names.size(); i++)
		{
			gv_string_tmp fn; 
			fn<<"\"" << names[i] << "\"";
			gv_string_tmp cmd;
			cmd = command;
			cmd.replace_all("%file%",fn); 
			gv_console_output(*cmd);
			gv_system_call(*cmd);
		}
	}
}
