//============================================================================================
//								:
//============================================================================================
namespace unit_test_lock_free_list_function
{
static gv_log* s_log = NULL;
static gv_atomic_count s_count;
struct int_ptr_node : public gv_pooled_struct_32
{
	int* data;
	int_ptr_node* next;
};
typedef gvt_lock_free_header< int_ptr_node* > type_of_buf;
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
		while (1 && s_count.get() < 1000)
		{
			int* pnew = new int;
			int_ptr_node* pnode = new int_ptr_node;
			pnode->data = pnew;
			*pnew = ++s_count;
			gvt_lock_free_list_push(pnode, *buf);
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
	bool use_try;
};
int MyRunnable::_staticVar = 0;

void test4_threads(bool use_try = false)
{
	sub_test_timer timer("unit_test_lock_free_double_buffer");
	{
		type_of_buf buf;

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

		thread1.start(&r1);
		thread2.start(&r2);
		thread3.start(&r3);
		thread4.start(&r4);
		int i = 0;
		while (1)
		{
			GV_PROFILE_EVENT(loop_free, 0)
			int_ptr_node* p = gvt_lock_free_list_pop_all(buf);
			int_ptr_node* tmp;
			while (p)
			{
				delete p->data;
				tmp = p;
				p = p->next;
				delete tmp;
			}
			gv_thread::sleep(1);
			if (i++ == 20)
				break;
		}
		r4.notify();
		r3.notify();
		r2.notify();
		r1.notify();

		thread4.join();
		thread3.join();
		thread2.join();
		thread1.join();
		{
			int_ptr_node* p = gvt_lock_free_list_pop_all(buf);
			int_ptr_node* tmp;
			while (p)
			{
				delete p->data;
				tmp = p;
				p = p->next;
				delete tmp;
			}
		}
	}
	gv_id::static_purge();
}
void main(gvt_array< gv_string >& args)
{
	test4_threads();
	test4_threads(true);
}
}
