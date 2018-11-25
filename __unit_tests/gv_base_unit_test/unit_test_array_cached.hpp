namespace unit_test_array_cached
{

void main(gvt_array< gv_string >& args)
{

	{
		sub_test_timer timer("unit_test_array_cached_add_16");
		gvt_array_cached< int, 16 > array;
		for (int i = 0; i < 16; i++)
			array.add(i);
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
	}

	{
		sub_test_timer timer("unit_test_array_cached_add_17");
		gvt_array_cached< int, 16 > array;
		for (int i = 0; i < 17; i++)
			array.add(i);
		//std::copy(array.begin(), array.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
	}
}
}
