

namespace unit_test_slist
{
using namespace std;
class test_class
{
	static const int max_test_class = 200;
	static int s_index;
	static int s_init_table[max_test_class];

public:
	int index;
	test_class()
	{

		index = s_index++;
		GV_ASSERT(s_init_table[index] == 0);
		test_log() << gv_endl << " test object " << index << " constructed" << gv_endl;
		s_init_table[index] = -1;
	}
	~test_class()
	{
		GV_ASSERT(s_init_table[index] == -1);
		test_log() << gv_endl << " test object " << index << " destroyed" << gv_endl;
		s_init_table[index] = 0;
	}

	const test_class& operator=(const test_class& c)
	{
		return *this;
	}
	const bool operator==(const test_class& c)
	{
		return true;
	}

	static void static_check()
	{
		for (int i = 0; i < max_test_class; i++)
		{
			GV_ASSERT(s_init_table[i] == 0);
		}
	}
};
int test_class::s_index = 0;
int test_class::s_init_table[max_test_class];
class printer
{
public:
	void operator()(const test_class& c)
	{
		test_log() << "object" << c.index << " ";
	}
} the_printer;

class printer_int
{
public:
	void operator()(const int& c)
	{
		test_log() << c << " ";
	}
} the_int_printer;
void main(gvt_array< gv_string >& args)
{

	{
		gvt_slist< int > list;
		GV_TEST_PRINT_FUNC(list.add(2), test_log());
		GV_TEST_PRINT_FUNC(list.add(1), test_log());
		GV_TEST_PRINT_FUNC(list.add(3), test_log());
		GV_TEST_PRINT_FUNC(list.add(22), test_log())
		list.for_each(the_int_printer);

		//			GV_TEST_PRINT_FUNC(pret=list.find(3) ,test_log())
		//GV_TEST_PRINT_VAR(pret,test_log());
		//GV_TEST_PRINT_FUNC(list.erase(1)  ,test_log())
		//list.for_each(the_int_printer);
	}
	/*

		{
			gvt_slist<test_class> list; 
			GV_TEST_PRINT_FUNC(list.add(test_class()) ,test_log());
			GV_TEST_PRINT_FUNC(list.add(test_class()) ,test_log());
			GV_TEST_PRINT_FUNC(list.add(test_class()) ,test_log());
			GV_TEST_PRINT_FUNC(list.add(test_class()) ,test_log());
			list.for_each(the_printer);
			list.erase(test_class());
			list.for_each(the_printer);
			gvt_slist<test_class> list2=list; 
			list2+=list2;
			list2.for_each(the_printer);
		}*/
	test_class::static_check();
}
}
