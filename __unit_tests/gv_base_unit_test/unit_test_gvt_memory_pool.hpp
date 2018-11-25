
namespace unit_test_gvt_memory_pool
{
void main(gvt_array< gv_string >& args)
{
#if GV_64
	//not supported
	return;
#endif
	{
		///gv::gvt_memory_pool<16> memory;
		//using namespace std;
		/*
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(12),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(strcpy(ptr.pc,"1234567890a"),test_log()); 
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.realloc(ptr.pv,16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.realloc(ptr.pv,12),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());*/
	}
	/*
		{
			gvt_policy_memory_fixed_size<16,32> memory;
			using namespace std;
			union
			{
				char * pc;
				void * pv;
			}ptr;
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(12),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(strcpy(ptr.pc,"1234567890a"),test_log()); 
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.realloc(ptr.pv,16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.realloc(ptr.pv,12),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_VAR(ptr.pc,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(memory.free(ptr.pv),test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
			GV_TEST_PRINT_FUNC(ptr.pv=memory.alloc(16),test_log());
			GV_TEST_PRINT_VAR(ptr.pv,test_log());
		}
		*/
}
}