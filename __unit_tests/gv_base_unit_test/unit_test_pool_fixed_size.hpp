namespace unit_test_pool_fixed_size
{
void main(gvt_array< gv_string >& args)
{
	{ //static
		gvt_pool_fixed_size< int, 100 > pool;
		int* pint;
		for (int i = 0; i < 100; i++)
		{
			pint = pool.allocate();
			(*pint) = i;
			GV_ASSERT(pool.get_index(pint) == i);
		}
		int nb_free = 0;
		int* array[100];
		gvf_stdout< int > ppp;
		pool.for_each(ppp);
		test_log() << gv_endl;
		gvt_pool_fixed_size< int, 1024 >::iterator it = pool.begin();
		while (it != pool.end())
		{
			if ((*it) % 5 == 0)
				array[nb_free++] = &(*it);
			++it;
		}
		for (int i = 0; i < nb_free; i++)
		{
			pool.free(array[i]);
		}
		pool.for_each(gvf_stdout< int >());
	}

	{ //static
		gvt_pool_resizable< int > pool;
		pool.reset(100);
		int* pint;
		for (int i = 0; i < 100; i++)
		{
			pint = pool.allocate();
			(*pint) = i;
			GV_ASSERT(pool.get_index(pint) == i);
		}
		int nb_free = 0;
		int* array[100];
		pool.for_each(gvf_stdout< int >());
		test_log() << gv_endl;
		gvt_pool_fixed_size< int, 1024 >::iterator it = pool.begin();
		while (it != pool.end())
		{
			if ((*it) % 5 == 0)
				array[nb_free++] = &(*it);
			++it;
		}
		for (int i = 0; i < nb_free; i++)
		{
			pool.free(array[i]);
		}
		pool.for_each(gvf_stdout< int >());
	}
}
}
