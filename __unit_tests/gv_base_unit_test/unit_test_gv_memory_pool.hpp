namespace unit_test_gv_memory_pool
{
class malloc_with_pool
{
public:
	void* alloc(size_t size)
	{
		if (!size)
			return NULL;
		void* p = fam.try_alloc(size);
		if (!p)
		{
			return ::malloc(size);
		}
		return p;
	}

	void free(void* p)
	{
		if (!p)
			return;
		bool r = fam.try_free(p);
		if (!r)
		{
			::free(p);
		}
	}
	void* realloc(void* p, size_t size)
	{
		void* pold = p;
		void* pnew = NULL;
		if (!size)
		{
			free(p);
			return NULL;
		}
		if (!p)
		{
			return alloc(size);
		}

		size_t old_size = 0;
		bool r = fam.try_realloc(pold, pnew, size, old_size);
		if (!r && !old_size)
		{
			pnew = ::realloc(pold, size);
		}
		else if (!old_size)
		{
			assert(0); //should not happen
		}
		else if (!r)
		{
			pnew = ::malloc(size);
			memcpy(pnew, pold, min(size, old_size));
			r = fam.try_free(pold);
			assert(r);
		}
		return pnew;
	}

private:
	gvt_memory_pool_manager fam;
};

class unit_test
{
public:
	struct pointer_checker
	{

		pointer_checker()
		{
			assert(0);
		}
		pointer_checker(void* _p)
		{
			p = _p;
		}
		union {
			void* p;
			unsigned char uc[4];
		};

		int get_sum()
		{
			return uc[0] ^ uc[1] ^ uc[2] ^ uc[3];
		}
		void set(int size)
		{
			if (!p)
				return;
			int i = get_sum();
			memset(p, i, size);
		};

		void ptr_verify(int size)
		{
			if (!p)
				return;
			int sum = get_sum();
			unsigned char* pc = (unsigned char*)p;
			for (int i = 0; i < size; i++)
			{
				assert(*pc == sum);
				pc++;
			}
		};
	};

	void run()
	{

		srand(0);
		{ //paired allocation, free
			malloc_with_pool mem;
			const int test_loop = 1024;
			void** pv = new void*[test_loop];
			for (int i = 0; i < test_loop; i++)
			{
				pv[i] = mem.alloc(rand() % 1024);
			}
			for (int i = 0; i < test_loop; i++)
			{
				mem.free(pv[i]);
			}
			delete[] pv;
		}

		{ //paired allocation, free
			malloc_with_pool mem;
			const int test_loop = 1024;
			void** pv = new void*[test_loop];
			for (int i = 0; i < test_loop; i++)
			{
				pv[i] = mem.alloc(rand() % 1024);
			}
			for (int i = 0; i < test_loop; i++)
			{
				pv[i] = mem.realloc(pv[i], rand() % 1024);
			}
			for (int i = 0; i < test_loop; i++)
			{
				mem.free(pv[i]);
			}
			delete[] pv;
		}

		{ //random test  allocation, free
			malloc_with_pool mem;
			const int test_size = 1024;
			const int test_loop = 1024 * 10;
			void* tab_pointer[test_size];
			int tab_size[test_size];
			memset(tab_pointer, 0, sizeof(tab_pointer));
			memset(tab_size, 0, sizeof(tab_size));
			for (int i = 0; i < test_loop; i++)
			{
				int index = rand() % test_size;
				int size = rand() % 512;
				int size_old = tab_size[index];
				void* ptr = tab_pointer[index];
				void* ptr_new = NULL;
				if (!ptr)
				{
					int dice = rand() % 2;
					if (dice)
					{
						ptr = mem.alloc(size);
					}
					else
					{
						ptr = mem.realloc(NULL, size);
					}
					tab_pointer[index] = ptr;
					tab_size[index] = size;
					pointer_checker c(ptr);
					c.set(size);
				}
				else
				{
					{
						pointer_checker c(ptr);
						c.ptr_verify(size_old);
					}
					int dice = rand() % 2;
					if (dice)
					{
						mem.free(ptr);
						size = 0;
						ptr = NULL;
					}
					else
					{
						ptr = mem.realloc(ptr, size);
						pointer_checker c(ptr);
						c.set(size);
					}
					tab_pointer[index] = ptr;
					tab_size[index] = size;
				}
			} //test_loop

			for (int i = 0; i < test_size; i++)
			{
				int size_old = tab_size[i];
				void* ptr = tab_pointer[i];
				pointer_checker c(ptr);
				c.ptr_verify(size_old);
				mem.free(ptr);
			}
		} //end test
	};	//run
};

void main(gvt_array< gv_string >& args)
{
#if GV_64
	return;
#endif
	unit_test a;
	a.run();
}
};
