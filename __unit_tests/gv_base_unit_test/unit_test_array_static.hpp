

namespace unit_test_array_static
{
void main(gvt_array< gv_string >& args)
{
	{
		gvt_array_with_fixed_memory< int, 32 > array;

		GV_TEST_PRINT_FUNC(array.push_back(2), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(1), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(3), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(22), test_log())
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;;
		GV_TEST_PRINT_FUNC(array.sort(), test_log());
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		int idx;
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(11, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array += array, test_log());
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		GV_TEST_PRINT_FUNC(array += array, test_log());
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		GV_TEST_PRINT_FUNC(array += array, test_log());
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
	}
}
}