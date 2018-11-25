namespace unit_test_reflection
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("type_string");

		gv_string_tmp s;
		gv_type_string type_string;
		gv_string test = "hello";
		type_string.export_to_xml(gvt_byte_ptr(test), s);
		GV_ASSERT(s == "\"hello\"");
		gv_string test2 = "hello";
		GV_ASSERT(type_string.is_equal(gvt_byte_ptr(test), gvt_byte_ptr(test2)));
		gv_byte* pb = type_string.create_instance();
		type_string.copy_to(gvt_byte_ptr(test), pb);
		gv_string* ps = (gv_string*)pb;
		GV_ASSERT(*ps == "hello");
		type_string.delete_instance(pb);
	};

	{
		sub_test_timer timer("type_static_array");

		gvt_array_static< gv_string, 5 > the_array;
		the_array[0] = "zero";
		the_array[1] = "one";
		the_array[2] = "two";
		the_array[3] = "three";
		the_array[4] = "four";

		gv_type_static_array type_array;
		type_array.set_element_type(new gv_type_string);
		type_array.set_array_size(5);

		gv_byte* pb = type_array.create_instance();
		type_array.copy_to(gvt_byte_ptr(the_array), pb);
		gvt_array_static< gv_string, 5 >* pa = (gvt_array_static< gv_string, 5 >*)pb;
		GV_ASSERT((*pa)[3] == "three");
		gv_string sname = "three";
		gv_int index;
		GV_ASSERT(type_array.find(pb, gvt_byte_ptr(sname), index));
		GV_ASSERT(index == 3);

		gv_string_tmp output;
		type_array.export_to_xml(pb, output);
		std::cout << *output;

		gvt_array_static< gv_string, 5 > the_array2;
		gv_xml_parser lexer;
		lexer.load_string(*output);
		type_array.import_from_xml(gvt_byte_ptr(the_array2), &lexer);
		GV_ASSERT(the_array2 == the_array);

		the_array.sort();
		type_array.sort(pb);
		GV_ASSERT(type_array.is_equal(pb, gvt_byte_ptr(the_array)));

		output = "";
		type_array.export_to_xml(pb, output);
		std::cout << *output;

		type_array.delete_instance(pb);
	};

	{
		sub_test_timer timer("type_array");

		gvt_array< gv_string > the_array;
		the_array.resize(5);
		the_array[0] = "zero";
		the_array[1] = "one";
		the_array[2] = "two";
		the_array[3] = "three";
		the_array[4] = "four";

		gv_type_array type_array;
		type_array.set_element_type(new gv_type_string);
		gv_byte* pb = type_array.create_instance();
		//type_array.resize_array(pb,5);
		type_array.copy_to(gvt_byte_ptr(the_array), pb);
		gvt_array< gv_string >* pa = (gvt_array< gv_string >*)pb;
		GV_ASSERT((*pa)[3] == "three");
		gv_string sname = "three";
		gv_int index;
		GV_ASSERT(type_array.find(pb, gvt_byte_ptr(sname), index));
		GV_ASSERT(index == 3);

		gv_string_tmp output;
		type_array.export_to_xml(pb, output);
		std::cout << *output;

		gvt_array< gv_string > the_array2;
		gv_xml_parser lexer;
		lexer.load_string(*output);
		type_array.import_from_xml(gvt_byte_ptr(the_array2), &lexer);
		GV_ASSERT(the_array2 == the_array);
		gv_string s_5 = "five";

		the_array.push_back(s_5);
		type_array.push_back(pb, gvt_byte_ptr(s_5));
		GV_ASSERT(type_array.is_equal(pb, gvt_byte_ptr(the_array)));

		the_array.sort();
		type_array.sort(pb);
		GV_ASSERT(type_array.is_equal(pb, gvt_byte_ptr(the_array)));

		output = "";
		type_array.export_to_xml(pb, output);
		std::cout << *output;

		type_array.delete_instance(pb);

		gv_type_array* ptype = gvt_cast< gv_type_array >(&type_array);
		GV_ASSERT(ptype);

		gv_type_static_array* ptype2 = gvt_cast< gv_type_static_array >(&type_array);
		GV_ASSERT(!ptype2);
	};

	{
		sub_test_timer timer("test_id_lexer");
		//test id lexer
		using namespace gv_id_space;
		using namespace std;
		gv_id_lexer lexer;
		gv_string_tmp s;

#undef GVM_MAKE_TOKEN
#define GVM_MAKE_TOKEN(x) s << "gv_" << #x << "   ";

		GVM_MAKE_TOKEN(byte)
		GVM_MAKE_TOKEN(char)
		GVM_MAKE_TOKEN(short)
		GVM_MAKE_TOKEN(ushort)
		GVM_MAKE_TOKEN(int)
		GVM_MAKE_TOKEN(uint)
		GVM_MAKE_TOKEN(long)
		GVM_MAKE_TOKEN(ulong)
		GVM_MAKE_TOKEN(float)
		GVM_MAKE_TOKEN(double)
		GVM_MAKE_TOKEN(string)
		GVM_MAKE_TOKEN(id)
		//GVM_MAKE_TOKEN(ptr 	)
		//GVM_MAKE_TOKEN(ref_ptr	)
		GVM_MAKE_TOKEN(vector3)
		GVM_MAKE_TOKEN(matrix44)
		GVM_MAKE_TOKEN(object)
		//GVM_MAKE_TOKEN(array	)
		//GVM_MAKE_TOKEN(static_array)
		GVM_MAKE_TOKEN(object_name)
		//GVM_MAKE_TOKEN(gvi_type)
		GVM_MAKE_TOKEN(vector2)
		GVM_MAKE_TOKEN(vector4)
		GVM_MAKE_TOKEN(matrix43)
		GVM_MAKE_TOKEN(color)
		GVM_MAKE_TOKEN(colorf)
		GVM_MAKE_TOKEN(bool)

#undef GVM_MAKE_TOKEN
		for (int i = 0; i < 10; i++)
		{
			gv_int token;
			lexer.load_string(s);
#define GVM_MAKE_TOKEN(x) \
	token = lexer.lex();  \
	GV_ASSERT(token == TOKEN_##x); /* cout<<token_to_string(token )<<endl;*/

			GVM_MAKE_TOKEN(byte)
			GVM_MAKE_TOKEN(char)
			GVM_MAKE_TOKEN(short)
			GVM_MAKE_TOKEN(ushort)
			GVM_MAKE_TOKEN(int)
			GVM_MAKE_TOKEN(uint)
			GVM_MAKE_TOKEN(long)
			GVM_MAKE_TOKEN(ulong)
			GVM_MAKE_TOKEN(float)
			GVM_MAKE_TOKEN(double)
			GVM_MAKE_TOKEN(string)
			GVM_MAKE_TOKEN(id)
			//GVM_MAKE_TOKEN(ptr 	)
			//GVM_MAKE_TOKEN(ref_ptr	)
			GVM_MAKE_TOKEN(vector3)
			GVM_MAKE_TOKEN(matrix44)
			GVM_MAKE_TOKEN(object)
			//GVM_MAKE_TOKEN(array	)
			//GVM_MAKE_TOKEN(static_array)
			GVM_MAKE_TOKEN(object_name)
			//GVM_MAKE_TOKEN(gvi_type)
			GVM_MAKE_TOKEN(vector2)
			GVM_MAKE_TOKEN(vector4)
			GVM_MAKE_TOKEN(matrix43)
			GVM_MAKE_TOKEN(color)
			GVM_MAKE_TOKEN(colorf)
			GVM_MAKE_TOKEN(bool)
		}
	}
}
} //gv
