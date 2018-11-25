namespace unit_test_memory_cached
{
using namespace std;
void main(gvt_array< gv_string >& args)
{

	{
		gvt_memory_cached< 24 > memory;
		union {
			char* pc;
			void* pv;
		} ptr;
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(12), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_FUNC(strcpy(ptr.pc, "1234567890a"), test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.realloc(ptr.pv, 100), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.realloc(ptr.pv, 12), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(100), test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(24), test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
	}
	{
		gvp_mem_cached_16 memory;
		union {
			char* pc;
			void* pv;
		} ptr;
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(12), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_FUNC(strcpy(ptr.pc, "1234567890a"), test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.realloc(ptr.pv, 100), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.realloc(ptr.pv, 12), test_log());
		GV_TEST_PRINT_VAR((gv_int_ptr)ptr.pv, test_log());
		GV_TEST_PRINT_VAR(ptr.pc, test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(100), test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
		GV_TEST_PRINT_FUNC(ptr.pv = memory.alloc(24), test_log());
		GV_TEST_PRINT_FUNC(memory.free(ptr.pv), test_log());
	}
}
}
