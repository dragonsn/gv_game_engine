namespace unit_test_profiler_simple
{
//the test is from poco
class MyRunnable : public gv_runnable
{
public:
	MyRunnable()
		: _ran(false)
	{
		m_index = _staticVar;
		staticFunc();
	}

	void do_something(int time)
	{
		GV_PROFILE_EVENT(do_something, m_index);
		for (int i = 0; i < time; i++)
		{
			float a = sinf((float)i);
		}
	};

	void func_1()
	{
		GV_PROFILE_EVENT(func_1, m_index);
		do_something(200);
		func_1_1();
		func_1_2();
	}

	void func_1_1()
	{
		GV_PROFILE_EVENT(func_1_1, m_index);
		do_something(100);
	}
	void func_1_2()
	{
		GV_PROFILE_EVENT(func_1_2, m_index);
		do_something(100);
	}
	void func_2()
	{
		GV_PROFILE_EVENT(func_2, m_index);
		func_2_1();
		do_something(200);
		func_2_2();
	}
	void func_2_1()
	{
		GV_PROFILE_EVENT(func_2_1, m_index);
		do_something(10);
	}
	void func_2_2()
	{
		GV_PROFILE_EVENT(func_2_2, m_index);
		func_2_2_1();
		func_2_2_2();
		do_something(200);
		func_2_2_3();
	}

	void func_2_2_1()
	{
		GV_PROFILE_EVENT(func_2_2_1, m_index);
		do_something(200);
	}
	void func_2_2_2()
	{
		GV_PROFILE_EVENT(func_2_2_2, m_index);
		do_something(200);
	}

	void func_2_2_3()
	{
		GV_PROFILE_EVENT(func_2_2_3, m_index);
		do_something(100);
	}

	void func_3()
	{
		GV_PROFILE_EVENT(func_3, m_index);
		do_something(500);
	}

	void run()
	{
		bool signaled = false;
		gv_thread* pThread = gv_thread::current();
		if (pThread)
			_threadName = pThread->get_name();
		_ran = true;

		while (!signaled)
		{
			GV_PROFILE_EVENT(run, m_index);
			func_1();
			func_2();
			func_3();
			int wait_time = rand() % 10 + 1;
			{
				GV_PROFILE_EVENT(event_try_wait, m_index);
				//test_log()<<"thread "<<*_threadName<<"do someting using "<< wait_time<<"ms"<<gv_endl;
				signaled = _event.try_wait(10);
			}
		};
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
	int m_index;
};
int MyRunnable::_staticVar = 0;

void test4_threads()
{
	sub_test_timer timer("test4_threads");
	{
		GV_PROFILE_EVENT(test_profile, 0);
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
		gv_thread::sleep(100);
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(!thread2.is_running());
		GV_ASSERT(!thread3.is_running());
		GV_ASSERT(!thread4.is_running());
		thread2.start(&r2);
		thread3.start(&r3);
		thread4.start(&r4);
		gv_thread::sleep(300);
		GV_ASSERT(thread1.is_running());
		GV_ASSERT(thread2.is_running());
		GV_ASSERT(thread3.is_running());
		GV_ASSERT(thread4.is_running());
		r4.notify();
		thread4.join();
		GV_ASSERT(!thread4.is_running());
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

void main(gvt_array< gv_string >& args)
{
	test4_threads();
}
}
