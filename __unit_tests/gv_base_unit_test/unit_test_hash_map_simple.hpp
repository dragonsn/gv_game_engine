

namespace unit_test_hash_map_simple
{
using namespace std;
class visitor
{
public:
	bool operator()(const gv_string& s, int& i)
	{
		test_log() << s << " 's age is" << i << gv_endl;
		return false;
	}
};
void main(gvt_array< gv_string >& args)
{

	{
		gvt_hash_map< gv_string, int, 2 > map;
		GV_TEST_PRINT_FUNC(map["tom yip"] = 33, test_log());
		GV_TEST_PRINT_FUNC(map["kate smith"] = 31, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 66, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 61, test_log());
		int age;
		GV_TEST_PRINT_FUNC(age = map["tom yip"], test_log());
		GV_ASSERT(age == 33);
		GV_TEST_PRINT_VAR(age, test_log());
		GV_TEST_PRINT_FUNC(age = map["kate smith"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 31);
		GV_TEST_PRINT_FUNC(age = map["bill gates"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 61);
		visitor v;
		map.for_each(v);

		GV_TEST_PRINT_FUNC(map.erase("tom yip"), test_log());
		map.for_each(v);
	}
	{
		gvt_hash_map< gv_string, int, 128 > map;
		GV_TEST_PRINT_FUNC(map["tom yip"] = 33, test_log());
		GV_TEST_PRINT_FUNC(map["kate smith"] = 31, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 66, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 61, test_log());
		int age;
		GV_TEST_PRINT_FUNC(age = map["tom yip"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 33);
		GV_TEST_PRINT_FUNC(age = map["kate smith"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 31);
		GV_TEST_PRINT_FUNC(age = map["bill gates"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 61);
		visitor v;
		map.for_each(v);

		GV_TEST_PRINT_FUNC(map.erase("tom yip"), test_log());
		map.for_each(v);
	}
	{
		gvt_hash_map< gv_string, int, 1 > map;
		GV_TEST_PRINT_FUNC(map["tom yip"] = 33, test_log());
		GV_TEST_PRINT_FUNC(map["kate smith"] = 31, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 66, test_log());
		GV_TEST_PRINT_FUNC(map["bill gates"] = 61, test_log());
		int age;
		GV_TEST_PRINT_FUNC(age = map["tom yip"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 33);
		GV_TEST_PRINT_FUNC(age = map["kate smith"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 31);
		GV_TEST_PRINT_FUNC(age = map["bill gates"], test_log());
		GV_TEST_PRINT_VAR(age, test_log());
		GV_ASSERT(age == 61);
		visitor v;
		map.for_each(v);

		GV_TEST_PRINT_FUNC(map.erase("tom yip"), test_log());
		map.for_each(v);
	}
}
}
