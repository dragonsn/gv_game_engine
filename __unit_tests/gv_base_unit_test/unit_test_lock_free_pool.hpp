
//============================================================================================
//								:
//============================================================================================
namespace unit_test_lock_free_pool
{
static gv_atomic_count s_count;
typedef gvt_lock_free_list< int*, gvp_memory_32 > type_of_buf;
typedef gvt_lock_free_list< int*, gvp_memory_32 >::node type_of_node;
typedef gvt_lock_free_pool< type_of_node, 1024 > type_of_pool;

static int loop = 20;
class MyRunnable : public gv_runnable
{
public:
	MyRunnable()
		: _ran(false)
	{
		use_try = false;
	}

	void run()
	{
		GV_PROFILE_EVENT(MyRunnable, 0);
		gv_thread* pThread = gv_thread::current();
		if (pThread)
			_threadName = pThread->get_name();
		int i = 0;
		while (1)
		{
			if (s_count.get() > 100000)
				return;
			type_of_node* node = pool->allocate();
			node->data = new int;
			*node->data = ++s_count;
			buf->push(node);
			if (i % 100 == 0)
			{
				if (_event.try_wait(1))
					return;
			}
			i++;
		}
	}

	bool ran() const
	{
		return _ran;
	}

	const gv_id& threadName() const
	{
		return _threadName;
	}

	void notify()
	{
		_event.set();
	}

	static void staticFunc()
	{
		++_staticVar;
	}

	static int _staticVar;

	bool _ran;
	gv_id _threadName;
	gv_event _event;
	type_of_buf* buf;
	type_of_pool* pool;
	bool use_try;
};
int MyRunnable::_staticVar = 0;

void test4_threads(bool use_try = false)
{
	sub_test_timer timer("unit_test_lock_free_pool");
	{
		type_of_buf buf;
		type_of_pool pool;

		gv_thread thread1(gv_id_one);
		gv_thread thread2(gv_id_two);
		gv_thread thread3(gv_id_three);
		gv_thread thread4(gv_id_four);

		MyRunnable r1;
		MyRunnable r2;
		MyRunnable r3;
		MyRunnable r4;

		r1.buf = r2.buf = r3.buf = r4.buf = &buf;
		r1.use_try = r2.use_try = r3.use_try = r4.use_try = use_try;
		r1.pool = r2.pool = r3.pool = r4.pool = &pool;

		thread1.start(&r1);
		thread2.start(&r2);
		thread3.start(&r3);
		thread4.start(&r4);
		int i = 0;
		while (1)
		{
			GV_PROFILE_EVENT(loop_free, 0)
			if (!use_try)
			{
				type_of_buf::node* p = buf.unlink_all();
				type_of_buf::node* tmp;
				while (p)
				{
					delete p->data;
					tmp = p;
					p = p->next;
					pool.free(tmp);
				}
			}
			else
			{
				type_of_buf::node* p;
				while (p = buf.unlink_front())
				{
					delete p->data;
					pool.free(p);
				}
			}
			gv_thread::sleep(1);
			if (i++ == loop)
				break;
		}
		r4.notify();
		r3.notify();
		r2.notify();
		r1.notify();
		{
			type_of_buf::node* p = buf.unlink_all();
			type_of_buf::node* tmp;
			while (p)
			{
				delete p->data;
				tmp = p;
				p = p->next;
				pool.free(tmp);
			}
			gv_thread::sleep(1);
		}
		thread4.join();
		thread3.join();
		thread2.join();
		thread1.join();
		{
			type_of_buf::node* p = buf.unlink_all();
			type_of_buf::node* tmp;
			while (p)
			{
				delete p->data;
				tmp = p;
				p = p->next;
				pool.free(tmp);
			}
		}
	}
	gv_id::static_purge();
}
void main(gvt_array< gv_string >& args)
{
	if (args.size())
	{
		args[0] >> loop;
	}
	test4_threads();
	test4_threads(true);
}
}
