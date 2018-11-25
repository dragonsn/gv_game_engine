#include <vector>
namespace unit_test_array_simple
{
using namespace std;
class test_class
{
	static const int max_test_class = 200;
	static int s_index;
	static int s_init_table[max_test_class];

public:
	int index;

	test_class(const test_class& c)
	{
		index = s_index++;
		GV_ASSERT(s_init_table[index] == 0);
		test_log() << gv_endl << " test object " << index << " constructed" << gv_endl;
		s_init_table[index] = -1;
		*this = c;
	}

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
		for (int i = 0; i < max_test_class; i++)
		{
			GV_ASSERT(s_init_table[i] == 0);
		}
	}
};
int test_class::s_index = 0;
int test_class::s_init_table[max_test_class];
void main(gvt_array< gv_string >& args)
{

	GV_ASSERT(gvt_get_alignment< gv_char >() == 1);
	GV_ASSERT(gvt_get_alignment< gv_ushort >() == 2);
	GV_ASSERT(gvt_get_alignment< gv_time >() == 8);
	GV_ASSERT(gvt_get_alignment< gv_long >() == 8);
	GV_ASSERT(gvt_get_alignment< gv_double >() == 8);
	GV_ASSERT(gvt_get_alignment< gv_float >() == 4);
	GV_ASSERT(gvt_get_alignment< gv_int >() == 4);

	{
		sub_test_timer timer("gvt_pool_array");
		gvt_pool_array< int > pool;
		pool.reset(12);
		int* p0 = pool.allocate();
		*p0 = 0;
		int* p1 = pool.allocate();
		*p1 = 0;
		pool.free(p0);
	}
	{
		sub_test_timer timer("gvt_array_gv_int");
		gvt_array< gv_int > array;
		array.push_back(2);
		array.push_back(1);
		array.push_back(3);
		array.push_back(22);
		array.sort(std::less< gv_int >());
		gv_int idx;
		bool c = array.find(3, idx);
		array.find(3, idx, std::equal_to< gv_int >());
		test_log() << "done " << gv_endl;
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		test_log() << gv_endl << "array.erase(0,2) " << gv_endl;
		array.erase(0, 2);
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		array.push_back(5);
		array.push_back(6);
		array.push_back(11);
		array.push_back(100);
		array.push_back(102);
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		array.sort();
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(11, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(12, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(1, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(0, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(102, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(3, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());
		GV_TEST_PRINT_FUNC(array.find_in_sorted_array(103, idx), test_log());
		GV_TEST_PRINT_VAR(idx, test_log());

		{
			sub_test_timer timer("gvt_array_default_push_back_benchmark");
			gvt_array< gv_int > array;
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}
		{
			sub_test_timer timer("std_array_default_push_back_benchmark");
			std::vector< int > array;
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}
		{
			sub_test_timer timer("gvt_array_with_gvp_array_size_calculator_string_push_back_benchmark");
			gvt_array< gv_int, gvp_memory_default, gvt_policy_thread_mode< int >, gvp_array_size_calculator_string > array;
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}
		{
			sub_test_timer timer("std_vector_with_my_allocator__push_back_benchmark");
			std::vector< int, gvt_allocator< int > > array;
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}

		{
			sub_test_timer timer("gvt_array_with_gvp_array_size_calculator_string__reserve__first__push_back__benchmark");
			gvt_array< gv_int, gvp_memory_default, gvt_policy_thread_mode< int >, gvp_array_size_calculator_string > array;
			array.reserve(3000);
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}

		{
			sub_test_timer timer("std::vector_with_my_allocator_reserve_first_push_back_benchmark");
			std::vector< int, gvt_allocator< int > > array;
			array.reserve(3000);
			for (int i = 0; i < 3000; i++)
				array.push_back(i);
			for (int i = 0; i < 3000; i++)
				array.pop_back();
		}
	}

	{
		sub_test_timer timer("test_sort");
		gvt_array< int > array;
		srand(11);
		for (int i = 0; i < 3000; i++)
		{
			array.insert_in_sorted_array(rand());
		}
		GV_ASSERT(array.check_is_sorted());
	}

	{
		sub_test_timer timer("test_sort");
		gvt_array< int > array;
		srand(11);
		for (int i = 0; i < 3000; i++)
		{
			array.insert_in_sorted_array(rand(), true);
		}
		GV_ASSERT(array.check_is_sorted(true));
		GV_ASSERT(array.size() == 3000);
	}

	{
		gvt_array< test_class > array;
		GV_TEST_PRINT_FUNC(array.resize(10), test_log());
		GV_TEST_PRINT_FUNC(array.erase(0, 5), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(test_class()), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(test_class()), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(test_class()), test_log());
		GV_TEST_PRINT_FUNC(array.push_back(test_class()), test_log());
		gvt_array< test_class > array2;
		GV_TEST_PRINT_FUNC(array2 = array, test_log());
		GV_TEST_PRINT_FUNC(array2.insert(array.begin(), 2, 3), test_log());
		GV_TEST_PRINT_FUNC(array2 += array2, test_log());
	}
	test_class::static_check();

	{
		gvt_array< float > a_f;
		gvt_array< int > a_i;
		a_f.add(123.0f);
		a_f.add(12.0f);
		a_f.add((float)1.111);

		a_i = a_f;
		gvt_array< int, gvt_memory_cached< 1024 > > a_i2;
		a_i2 = a_i;
	}

	{
		sub_test_timer timer("gvt_array_test_insert_to_sorted");
		gvt_array< int > a_i;
		srand((gv_int)gv_global::time->get_performance_counter());
		a_i.reserve(1000);
		for (int i = 0; i < 1000; i++)
		{
			a_i.push_back(rand() % 3000);
		}
		a_i.sort();
		for (int i = 0; i < 3000; i++)
		{
			int c = rand() % 3000;
			gv_int idx;
			if (a_i.find_in_sorted_array(c, idx))
			{
				GV_ASSERT(a_i[idx] == c);
			}
			else
			{
				int pos = -idx - 1;
				GV_ASSERT(pos >= 0);
				if (pos < a_i.size())
				{
					GV_ASSERT(c <= a_i[pos]);
				}
				else
				{
					GV_ASSERT(pos == a_i.size());
					GV_ASSERT(c >= *a_i.last());
				}
			}

		} //==3000 test
	}
}
};
