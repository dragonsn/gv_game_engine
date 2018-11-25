namespace unit_test_dlist
{

void main(gvt_array< gv_string >& args)
{
	gvt_dlist< int > l1;
	gvt_list< int > l2;
	int data[] = {1, 2, 3, 4, 5, 6};
	for (int i = 0; i < gvt_array_length(data); i++)
		l1.push_back(data[i]);
	for (int i = 0; i < gvt_array_length(data); i++)
		l2.push_back(data[i]);
	GV_ASSERT(gvt_for_each_is_equal(l1, l2));
	l1.pop_back();
	l2.pop_back();
	GV_ASSERT(gvt_for_each_is_equal(l1, l2));
	l1.push_front(100);
	l2.push_front(100);
	GV_ASSERT(gvt_for_each_is_equal(l1, l2));
	l1.pop_front();
	l2.pop_front();
	GV_ASSERT(gvt_for_each_is_equal(l1, l2));

	{
		gvt_dlist< int >::iterator it1 = l1.begin();
		gvt_list< int >::iterator it2 = l2.begin();
		it1++;
		it2++;
		l1.erase(it1);
		l2.erase(it2);
		GV_ASSERT(gvt_for_each_is_equal(l1, l2));
	}
	{
		gvt_dlist< int >::iterator it1 = l1.begin();
		gvt_list< int >::iterator it2 = l2.begin();
		it1++;
		it2++;
		l1.insert(it1, 123);
		l2.insert(it2, 123);
		GV_ASSERT(gvt_for_each_is_equal(l1, l2));
	}

	{
		gvt_dlist< int > list0;
		gvt_dlist< int > list1;
		int data[] = {1, 2, 3, 4, 5, 6};
		for (int i = 0; i < gvt_array_length(data); i++)
			list0.push_back(data[i]);
		gvt_dlist< int >::iterator it, it1;
		it = list0.unlink_front();
		it1 = list1.link_front(it);
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		GV_ASSERT((*list0.first()) == 2);
		GV_ASSERT((*list1.first()) == 1);
		it = list0.unlink_back();
		it1 = list1.link_back(it);
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		GV_ASSERT((*list0.last()) == 5);
		GV_ASSERT((*list1.last()) == 6);
		it = list1.unlink_back();
		it1 = list0.link_back(it);
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		GV_ASSERT((*list0.last()) == 6);
		GV_ASSERT((*list1.last()) == 1);
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		it = list1.unlink_front();
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		GV_ASSERT(*it == 1);
		it1 = list1.unlink_front();
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list1.link_front(it);
		list1.link_front(it1);
		list1.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		list0.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
	}

	{
		//sort
		srand((gv_uint)gv_global::time->get_performance_counter());
		gvt_dlist< int > list;
		for (int i = 0; i < 1000; i++)
		{
			list.push_back(rand());
		}
		list.sort();
		GV_ASSERT(list.size() == 1000);
		gvt_dlist< int >::iterator it = list.begin();
		for (int i = 1; i < 1000; i++)
		{
			GV_ASSERT(*it <= *(++it));
		}
	}
}
}
