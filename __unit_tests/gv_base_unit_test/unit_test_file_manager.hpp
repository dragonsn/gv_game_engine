namespace unit_test_file_manager
{

using namespace std;
void main(gvt_array< gv_string >& args)
{
	{
		gv_file_manager fm;
		{ //write bin
			sub_test_timer timer("write_bin");
			gvi_stream* ps = fm.open_binary_file_for_write("test.bin");
			GV_ASSERT(ps);
			gvi_stream& s = *ps;
			s << (gv_uint)12;
			s << (gv_double)123456.789;
			s << gv_vector3(1.f, -2.000, 3);
			s << gv_matrix44::get_identity();
			gv_string name("tom yip");
			s << name;
			s.close();
			delete ps;
		}
		{ //read bin
			sub_test_timer timer("read_bin");
			gvi_stream* ps = fm.open_binary_file_for_read("test.bin");
			GV_ASSERT(ps);
			gvi_stream& s = *ps;
			gv_uint i;
			gv_double d;
			gv_vector3 v;
			gv_matrix44 m;
			s >> i;
			GV_ASSERT(i == 12);
			s >> d;
			GV_ASSERT(d == 123456.789);
			s >> v;
			GV_ASSERT(v == gv_vector3(1, -2, 3))
			s >> m;
			GV_ASSERT(m == gv_matrix44::get_identity());
			gv_string name;
			s >> name;
			GV_ASSERT(name == ("tom yip"));
			s.close();
			delete ps;
		}

		{ //cache bin
			sub_test_timer timer("cache");
			gvi_stream* ps = fm.cache_file_for_write("test.bin");
			GV_ASSERT(ps);
			gvi_stream& s = *ps;
			s << (gv_uint)12;
			s << (gv_double)123456.789;
			s << gv_vector3(1.f, -2.000, 3);
			s << gv_matrix44::get_identity();
			gv_string name("tom yip");
			s << name;
			s.close();
			delete ps;
		}
		{ //cache bin
			sub_test_timer timer("cache");
			gvi_stream* ps = fm.cache_file_for_read("test.bin");
			GV_ASSERT(ps);
			gvi_stream& s = *ps;
			gv_uint i;
			gv_double d;
			gv_vector3 v;
			gv_matrix44 m;
			s >> i;
			GV_ASSERT(i == 12);
			s >> d;
			GV_ASSERT(d == 123456.789);
			s >> v;
			GV_ASSERT(v == gv_vector3(1, -2, 3))
			s >> m;
			GV_ASSERT(m == gv_matrix44::get_identity());
			gv_string name;
			s >> name;
			GV_ASSERT(name == ("tom yip"));
			s.close();
			delete ps;
		}

		{ //write text
			sub_test_timer timer("write_text");
			gvi_stream* ps = fm.open_text_file_for_write("test.txt");
			GV_ASSERT(ps);
			gvi_stream& s = *ps;
			s << (gv_uint)12 << " ";
			s << (gv_double)123456.789 << " ";
			s << gv_vector3(1, -2, 3);
			s << gv_matrix44::get_identity();
			gv_string name("tom_yip");
			s << name;
			s.close();
			delete ps;
		}
		{ //read text can't read string with space properly notes!!

			sub_test_timer timer("read_text");
			gv_lexer s;
			s.load_file("test.txt");

			gv_uint i;
			gv_double d;
			gv_vector3 v;
			gv_matrix44 m;
			s >> i;
			GV_ASSERT(i == 12);
			s >> d;
			GV_ASSERT(d == 123456.789);
			s >> v;
			GV_ASSERT(v == gv_vector3(1, -2, 3))
			s >> m;
			GV_ASSERT(m == gv_matrix44::get_identity());
			gv_string name;
			s >> name;
			GV_ASSERT(name == ("tom_yip"));
		}

		{ //write bin swap endian
			sub_test_timer timer("endian");
			gvi_stream* ps = fm.open_binary_file_for_write("test.bin");
			GV_ASSERT(ps);
			ps->enable_endian_swap(true);
			gvi_stream& s = *ps;
			s << (gv_uint)12;
			s << (gv_double)123456.789;
			gv_string name("tom yip");
			s << name;
			s.close();
			delete ps;
		}
		{ //read bin
			sub_test_timer timer("read_bin_endian");
			gvi_stream* ps = fm.open_binary_file_for_read("test.bin");
			GV_ASSERT(ps);
			ps->enable_endian_swap(true);
			gvi_stream& s = *ps;
			gv_uint i;
			gv_double d;
			s >> i;
			GV_ASSERT(i == 12);
			s >> d;
			GV_ASSERT(d == 123456.789);
			gv_string name;
			s >> name;
			GV_ASSERT(name == ("tom yip"));
			s.close();
			delete ps;
		}
		{
			sub_test_timer timer("find_file_with_pattern_with_boost");
			gvt_array< gv_string > names;
			fm.find_file_with_pattern("*.xml", names, "./");
			for (int i = 0; i < names.size(); i++)
			{
				test_log() << names[i] << gv_endl;
			}
			gvt_array< gv_string > names2;
			fm.find_file_with_pattern_with_boost("*.xml", names2, "./");
			GV_ASSERT(names2.size() == names.size());
		}

		{
			sub_test_timer timer("test_file_name");
			gv_string s("d:/veryfun/xxx.bmp");
			gv_string f;
			test_log() << s << gv_endl;
			f = fm.get_filename(s);
			GV_ASSERT(f == "xxx.bmp");
			test_log() << f << gv_endl;
			f = fm.get_extension(s);
			GV_ASSERT(f == ".bmp");
			test_log() << f << gv_endl;
			f = fm.get_stem(s);
			test_log() << f << gv_endl;
			GV_ASSERT(f == "xxx");
			f = fm.get_directory_string(s);
			test_log() << f << gv_endl;
			GV_ASSERT(f == "d:/veryfun/xxx.bmp");
			f = fm.get_parent_path(s);
			GV_ASSERT(f == "d:/veryfun");
			test_log() << f << gv_endl;
		}
	}
	{
		sub_test_timer timer("serialize_math");
		gv_vector3 v(1, -2, 3);
		gv_string_tmp s;
		s << v;
		gv_vector3 v2;
		gv_lexer lexer;
		lexer.load_string(*s);
		lexer >> v2;
		GV_ASSERT(v == v2);

		s = "";
		gv_matrix44 mat;
		mat.matrix44.randomize(0, 2);
		s << mat;
		lexer.load_string(*s);
		gv_matrix44 mat2;
		lexer >> mat2;
		GV_ASSERT(mat == mat2);
	}
	{
		sub_test_timer timer("find_file");
		gv_string s;
		bool found = gv_global::fm->find_file("test.txt", s);
		GV_ASSERT(found);
		test_log() << "find test.txt,full name is " << *s << gv_endl;
	}

	{
#if defined(__ANDROID__)
		sub_test_timer timer("addroid_asset");
		gv_string s;
		bool b = gv_load_file_to_string("game.config", s);
		if (b)
		{
			test_log() << "[ANDROID] read asset: successful!! ";
			test_log() << "[game.config] " << s;
		}
		else
		{
			test_log() << "[ANDROID][error] read asset failed!! ";
		}
#endif
	}
	{

		gv_stream_cached s;
		int i;
		s << i;
		float f;
		s << f;
	}
	test_log() << " ";
}
}
