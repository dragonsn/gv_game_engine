
//============================================================================================
//								:
//============================================================================================
namespace unit_test_lock_free_list
{

//#pragma GV_REMINDER("!!!SN_TO make 64bit lock free !!")

static gv_log* s_log = NULL;
static gv_atomic_count s_count;
static int max_try_count = 1000;
static int sleep_time = 1;
static int batch_size = 100;
//typedef gvt_lock_free_list<int * ,gvp_memory_16> type_of_buf;
typedef gvt_lock_free_list< int*, gvp_memory_32 > type_of_buf;
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
		while (1 && s_count.get() < max_try_count)
		{
			int* pnew = new int;
			*pnew = ++s_count;
			buf->push(pnew);
			if (i % batch_size == 0)
			{
				if (_event.try_wait(sleep_time))
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

void test4_threads(bool use_try = false, int nb_thread = 4, int in_max_try_count = 1000, int in_sleep_time = 1, int in_batch_size = 100)
{
	batch_size = in_batch_size;
	sleep_time = in_sleep_time;
	max_try_count = in_max_try_count;

	sub_test_timer timer("unit_test_lock_free_double_buffer");
	{
		type_of_buf buf;
		gvt_array< gv_thread > threads;
		gvt_array< MyRunnable > runables;
		threads.resize(nb_thread);
		runables.resize(nb_thread);
		for (int i = 0; i < nb_thread; i++)
		{
			gv_string_tmp s;
			s << "thread" << i;
			threads[i].set_name(gv_id(*s));
			threads[i].set_priority(gv_thread::E_PRIO_LOW);
			runables[i].buf = &buf;
			runables[i].use_try = use_try;
			threads[i].start(&runables[i]);
		}

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
					delete tmp;
				}
			}
			else
			{
				type_of_buf::node* p;
				while (p = buf.unlink_front())
				{
					delete p->data;
					delete p;
				}
			}
			gv_thread::sleep(1);
			if (i++ == loop)
				break;
		}

		for (int i = 0; i < nb_thread; i++)
		{
			runables[i].notify();
		}

		{
			type_of_buf::node* p = buf.unlink_all();
			type_of_buf::node* tmp;
			while (p)
			{
				delete p->data;
				tmp = p;
				p = p->next;
				delete tmp;
			}
			gv_thread::sleep(1);
		}
		for (int i = 0; i < nb_thread; i++)
		{
			threads[i].join();
		}

		{
			type_of_buf::node* p = buf.unlink_all();
			type_of_buf::node* tmp;
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
	if (args.size())
	{
		args[0] >> loop;
	}
	test4_threads(true, 48, 1000000, 1, 100);
	test4_threads(true);
}
}
