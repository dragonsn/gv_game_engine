

namespace unit_test_call_stack
{

using namespace std;
//extern int ::gv::gvt_load_callstack ( gv_uint * pbuffer, gv_uint max_depth);
int fn1()
{
	void* buffer[256];
	int count = gv_load_callstack(buffer, GV_ARRAY_LENGTH(buffer));
	test_log() << "call_stack ( :" << count << ")";
	for (int i = 0; i < count; i++)
	{
		test_log() << (gv_int_ptr)buffer[i] << gv_endl;
		char func_name[1024];
		char file_name[1024];
		int line_no;
		gvt_get_symbol_from_offset(buffer[i], func_name, file_name, line_no);
		test_log() << file_name << "(" << line_no << ")" << func_name << gv_endl;
	}
	test_log() << gv_endl;
	return 1;
};

int fn2()
{
	test_log() << "fn2";
	return fn1();
};
int fn3()
{
	test_log() << "fn3 ";
	return fn2();
};
int fn4()
{
	test_log() << "fn4";
	return fn3();
};
int fn5()
{
	test_log() << "fn5";
	return fn4();
};
void main(gvt_array< gv_string >& args)
{
	test_log() << " ";
	fn5();
}
}
