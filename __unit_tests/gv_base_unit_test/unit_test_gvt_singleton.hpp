

namespace unit_test_gvt_singleton
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
	static void static_check()
	{
		for (int i = 1; i < max_test_class; i++)
		{
			GV_ASSERT(s_init_table[i] == 0);
		}
	}
};
int test_class::s_index = 0;
int test_class::s_init_table[max_test_class];

void main(gvt_array< gv_string >& args)
{
	test_class& c = gvt_singleton< test_class >::instance();
	test_class& c2 = gvt_singleton< test_class >::instance();
	test_class& c3 = gvt_singleton< test_class >::instance();
	gvt_global< test_class > p;
	p->static_check();
}
}