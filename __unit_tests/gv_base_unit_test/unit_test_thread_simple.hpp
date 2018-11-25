namespace unit_test_thread_simple
{
//the test is from poco
class MyRunnable : public gv_runnable
{
public:
	MyRunnable()
		: _ran(false)
	{
	}

	void run()
	{
		gv_thread* pThread = gv_thread::current();
		if (pThread)
			_threadName = pThread->get_name();
		_ran = true;
		test_log() << "wait for event" << gv_endl;
		_event.wait();
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

private:
	bool _ran;
	gv_id _threadName;
	gv_event _event;
};
int MyRunnable::_staticVar = 0;

void testThreads()
{
	sub_test_timer timer("testThreads");
	{

		gv_thread thread(gv_id_one);
		MyRunnable r;
		assert(!thread.is_running());
		thread.start(&r);
		gv_thread::sleep(100);
		GV_ASSERT(thread.is_running());
		r.notify();
		gv_thread::sleep(100);
		//thread.join();
		GV_ASSERT(!thread.is_running());
		GV_ASSERT(r.ran());
		GV_ASSERT(r.threadName() == gv_id_one);
	}
	gv_id::static_purge();
}

void test4_threads()
{
	sub_test_timer timer("test4_threads");
	{

		gv_thread thread1(gv_id_one);
		gv_thread thread2(gv_id_two);
		gv_thread thread3(gv_id_three);
		gv_thread thread4(gv_id_four);

		MyRunnable r1;
		MyRunnable r2;
		MyRunnable r3;
		MyRunnable r4;
		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running());

		thread1.start(&r1);
		gv_thread::sleep(200);
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running());
		thread2.start(&r2);
		thread3.start(&r3);
		thread4.start(&r4);
		gv_thread::sleep(200);
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(thread2.is_running());
		GV_ASSERT(thread3.is_running());
		GV_ASSERT(thread4.is_running());
		r4.notify();
		thread4.join();
		GV_ASSERT(!thread4.is_running())
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(thread2.is_running());
		GV_ASSERT(thread3.is_running());
		r3.notify();
		thread3.join();
		GV_ASSERT(!thread3.is_running());
		r2.notify();
		thread2.join();
		GV_ASSERT(!thread2.is_running());
		r1.notify();
		thread1.join();
		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(r1.ran());
		GV_ASSERT(r1.threadName() == gv_id_one);
		GV_ASSERT(r2.ran());
		GV_ASSERT(r2.threadName() == gv_id_two);
		GV_ASSERT(r3.ran());
		GV_ASSERT(r3.threadName() == gv_id_three);
		GV_ASSERT(r4.ran());
		GV_ASSERT(r4.threadName() == gv_id_four);
	}
	gv_id::static_purge();
}

bool stop_flag = false;
void run()
{
	gv_thread* pThread = gv_thread::current();
	while (!stop_flag)
	{
		gv_string_tmp s;
		s << "this thread is the smartest: " << pThread->get_name() << gv_endl;
		test_log() << *s;
		gv_thread::sleep(100);
	}
}
class runable_test : public gv_refable
{
public:
	runable_test()
	{
	}
	~runable_test()
	{
	}
	static void run(gvt_ref_ptr< gv_refable > self)
	{
		gv_thread* pThread = gv_thread::current();
		while (!stop_flag)
		{
			gv_string_tmp s;
			s << "this thread is the smartest: " << pThread->get_name() << gv_endl;
			test_log() << *s;
			gv_thread::sleep(100);
		}
	}
};

void test4_thread_with_boost()
{
	sub_test_timer timer("test4_threads");
	{

		gv_thread thread1(gv_id_one);
		gv_thread thread2(gv_id_two);
		gv_thread thread3(gv_id_three);
		gv_thread thread4(gv_id_four);

		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running());
		thread1.start(boost::bind(run));
		thread2.start(boost::bind(run));
		thread3.start(boost::bind(run));
		thread4.start(boost::bind(run));
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(thread2.is_running());
		GV_ASSERT(thread3.is_running());
		GV_ASSERT(thread4.is_running())
		gv_thread::sleep(900);
		stop_flag = true;
		gv_thread::sleep(900);
		thread1.join();
		thread2.join();
		thread3.join();
		thread4.join();
		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running())
	}
}

void test4_thread_with_boost2()
{
	sub_test_timer timer("test4_threads");
	{
		stop_flag = false;
		gv_thread thread1(gv_id_one);
		gv_thread thread2(gv_id_two);
		gv_thread thread3(gv_id_three);
		gv_thread thread4(gv_id_four);

		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running());
		thread1.start(boost::bind(runable_test::run, (new runable_test)));
		thread2.start(boost::bind(runable_test::run, (new runable_test)));
		thread3.start(boost::bind(runable_test::run, (new runable_test)));
		thread4.start(boost::bind(runable_test::run, (new runable_test)));
		//thread2.start(boost::bind(runable_test::run, new runable_test));
		//thread3.start(boost::bind(runable_test::run, new runable_test));
		//thread4.start(boost::bind(runable_test::run, new runable_test));
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(thread2.is_running());
		GV_ASSERT(thread3.is_running());
		GV_ASSERT(thread4.is_running())
		gv_thread::sleep(900);
		stop_flag = true;
		gv_thread::sleep(900);
		thread1.join();
		thread2.join();
		thread3.join();
		thread4.join();
		GV_ASSERT(!thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running())
	}
}

void main(gvt_array< gv_string >& args)
{
	testThreads();
	test4_threads();
	test4_thread_with_boost();
	test4_thread_with_boost2();
}
}
