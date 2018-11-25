namespace unit_test_string_simple
{
class visitor
{
public:
	void operator()(const gv_string& s)
	{
		test_log() << s << "  ";
	}
} ccc;
void main(gvt_array< gv_string >& args)
{
	using namespace std;
	{
		gv_string s = "12345";
		gv_string s2 = s.left(2);
		GV_ASSERT(s2 == "12");
		gv_string s3 = s.right(3);
		GV_ASSERT(s3 == "345");
		GV_ASSERT(s.strlen() == 5);
		GV_ASSERT(s2.strlen() == 2);
		s3.strlen();
	}
	{
		gv_string s = "12345";
		gv_uint check_sum = s.crc32();
		GV_TEST_PRINT_VAR(check_sum, test_log());
	}
	{
		gv_string_tmp s;
		gv_uint i = 0xDEAD;
		s << i << "\t";
		s << (void*)(gv_int_ptr)i << "\t";
		s << (int)i << "\t";
		s << (gv_ulong)i << "\t";
		test_log() << *s;
	}

	{
		sub_test_timer timer("ansi_char_string");
		gv_string s = "I am super man  ";
		GV_TEST_PRINT_VAR(s, test_log());
		GV_TEST_PRINT_FUNC(s += s, test_log());
		GV_TEST_PRINT_VAR(s, test_log());
		GV_TEST_PRINT_FUNC(s.replace_all("man", "woman"), test_log());
		GV_TEST_PRINT_VAR(s, test_log());
		gvt_array< gv_string > result;
		GV_TEST_PRINT_FUNC(s.split(result), test_log());
		result.sort();
		result.for_each(ccc);
		gv_int i;
		GV_TEST_PRINT_FUNC(i = s.hash(), test_log());
		GV_TEST_PRINT_VAR(i, test_log());
		//boost::hash<gv_string> t;
		//	t(s);
		//	boost::flyweight<gv_string> name;
		//	name=gv_string("123");
		//	boost::flyweight<gv_string> name2;
		//name2=gv_string("123");

		gv_wstring ss;
		ss << s;
		test_log() << (const char*)ss.begin();

		gv_string_tmp s_tmp = "tom yip";
		s << s_tmp;
		GV_TEST_PRINT_VAR(s, test_log());
		//std::copy(result.begin(), result.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
	}
	/* TODO , problem here with wcout
			{
			
			sub_test_timer timer("wide_char_string");
			gv_wstring s=L"a list   ";
			s+=s;
			//wprintf(s);
			//std::wcout.imbue(std::locale("chs"));
			std::wcout<<L"I am super man  ";
			GV_TEST_PRINT_VAR(s ,std::wcout);
			GV_TEST_PRINT_FUNC(s+=s ,std::wcout); 
			GV_TEST_PRINT_VAR(s ,std::wcout);
			GV_TEST_PRINT_FUNC(s.replace_all(L"list",L"table") ,std::wcout ); 
			GV_TEST_PRINT_VAR(s ,std::wcout);
			gvt_array<gv_wstring> result;
			//wprintf(s);
			GV_TEST_PRINT_FUNC(s.split(result)  ,std::wcout); 

			gv_string ss; 
			ss<<s;
			test_log()<<ss.begin();

			gv_string_tmp s_tmp="tom yip"; 
			s<<s_tmp;
			wcout<<s.begin();

			//std::copy(result.begin(), result.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		}*/
	{
		gv_string_tmp s_tmp = "test_string";
		gv_string s = *s_tmp;
		GV_ASSERT(s == gv_get_const_string(s_tmp));
		gvt_hash_map< gv_string, int > map;
		map.add(s, 100);
		GV_ASSERT(map[gv_get_const_string(s_tmp)] == 100);
	}
	{
		gv_text t("aaa");
		gv_text t2(t);
		t2 = t;
	}
}
}
