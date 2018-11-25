
namespace unit_test_stack
{
void main(gvt_array< gv_string >& args)
{
	{
		gvt_stack< int > array;
		GV_TEST_PRINT_FUNC(array.push(2), test_log());
		GV_TEST_PRINT_FUNC(array.push(1), test_log());
		GV_TEST_PRINT_FUNC(array.push(3), test_log());
		GV_TEST_PRINT_FUNC(array.push(22), test_log());
		//	std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;;
		GV_TEST_PRINT_FUNC(array.pop(), test_log());
		GV_TEST_PRINT_VAR(array.top(), test_log());
		GV_TEST_PRINT_FUNC(array.pop(), test_log());
		GV_TEST_PRINT_VAR(array.top(), test_log());
		//	std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;;
	}

	{
		gvt_stack_static< int, 12 > array;
		GV_TEST_PRINT_FUNC(array.push(2), test_log());
		GV_TEST_PRINT_FUNC(array.push(1), test_log());
		GV_TEST_PRINT_FUNC(array.push(3), test_log());
		GV_TEST_PRINT_FUNC(array.push(22), test_log());
		//	std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;;
		GV_TEST_PRINT_FUNC(array.pop(), test_log());
		GV_TEST_PRINT_VAR(array.top(), test_log());
		GV_TEST_PRINT_FUNC(array.pop(), test_log());
		GV_TEST_PRINT_VAR(array.top(), test_log());
		//	std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;;
	}
}
}
