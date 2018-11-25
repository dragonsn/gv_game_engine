namespace unit_test_boost_range_test
{
using namespace boost;
using namespace std;
void main(gvt_array< gv_string >& args)
{
	const int sz = 9;
	typedef int array_t[sz];
	int my_array[sz] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	const array_t ca = {1, 2, 3, 4, 5, 6, 7, 8, 10};
	BOOST_STATIC_ASSERT((boost::is_same< range_value< array_t >::type, int >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< array_t >::type, int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_const_iterator< array_t >::type, const int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_difference< array_t >::type, std::ptrdiff_t >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_size< array_t >::type, std::size_t >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< array_t >::type, int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< const array_t >::type, const int* >::value));
	//		BOOST_STATIC_ASSERT(( boost::is_same< range_value<const array_t>::type, const int >::value ));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< const array_t >::type, const int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_const_iterator< const array_t >::type, const int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_difference< const array_t >::type, std::ptrdiff_t >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_size< const array_t >::type, std::size_t >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< const array_t >::type, const int* >::value));
	BOOST_STATIC_ASSERT((boost::is_same< range_iterator< const array_t >::type, const int* >::value));

	int* p = boost::begin(my_array);
	int* p2 = my_array;

	GV_CHECK_EQUAL(boost::begin(my_array), my_array);
	GV_CHECK_EQUAL(end(my_array), my_array + size(my_array));
	GV_CHECK_EQUAL(empty(my_array), false);

	GV_CHECK_EQUAL(begin(ca), ca);
	GV_CHECK_EQUAL(end(ca), ca + size(ca));
	GV_CHECK_EQUAL(empty(ca), false);

	const char A[] = "\0A";
	GV_CHECK_EQUAL(boost::size(A), 3);
}
}
