

namespace unit_test_ring_buf_simple
{
void main(gvt_array< gv_string >& args)
{
	gvt_ring_buf< int > cb(3);

	// Insert some elements into the buffer.
	GV_TEST_PRINT_FUNC(cb.push_back(1), test_log());
	GV_TEST_PRINT_FUNC(cb.push_back(2), test_log());
	GV_TEST_PRINT_FUNC(cb.push_back(3), test_log());

	//std::copy(cb.begin(), cb.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;

	int a;
	GV_TEST_PRINT_FUNC(a = cb[0], test_log()); // a == 1
	GV_TEST_PRINT_VAR(a, test_log());
	int b;
	GV_TEST_PRINT_FUNC(b = cb[1], test_log()); // b == 2
	GV_TEST_PRINT_VAR(b, test_log());
	int c; // c == 3
	GV_TEST_PRINT_FUNC(c = cb[2], test_log());
	GV_TEST_PRINT_VAR(c, test_log());

	GV_TEST_PRINT_FUNC(cb.push_back(4), test_log());
	GV_TEST_PRINT_FUNC(cb.push_back(5), test_log());
	//std::copy(cb.begin(), cb.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
}
};
