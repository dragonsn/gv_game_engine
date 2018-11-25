namespace unit_test_log
{

//the test is from poco
static gv_log* s_log = NULL;
class MyRunnable : public gv_runnable
{
public:
	MyRunnable()
		: _ran(false)
	{
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
			gv_string_tmp s;
			s << gv_global::time->get_microsec_from_start() << "  " << *_threadName << " " << i++ << " \r\n";
			(*s_log) << s;
			if (i % 100 == 0)
			{
				if (_event.try_wait(1))
					return;
			}
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

private:
	bool _ran;
	gv_id _threadName;
	gv_event _event;
};
int MyRunnable::_staticVar = 0;

void test4_threads()
{
	sub_test_timer timer("multi_thread_log");
	{

		s_log = gv_global::log->create_log("test_log()", 65536 * 10);

		gv_thread thread1(gv_id_one);
		gv_thread thread2(gv_id_two);
		gv_thread thread3(gv_id_three);
		gv_thread thread4(gv_id_four);

		MyRunnable r1;
		MyRunnable r2;
		MyRunnable r3;
		MyRunnable r4;

		thread1.start(&r1);
		gv_thread::sleep(2);
		thread2.start(&r2);
		thread3.start(&r3);
		thread4.start(&r4);
		gv_thread::sleep(2000);
		r4.notify();
		thread4.join();
		r3.notify();
		gv_thread::sleep(200);
		thread3.join();
		r2.notify();
		thread2.join();
		r1.notify();
		thread1.join();

		gv_global::log->close_log(s_log);
	}
	gv_id::static_purge();
}

void main(gvt_array< gv_string >& args)
{
	GVM_LOG(main, "first !!!!" << 12323 << "\r\n");
	GVM_LOG(main, "first !!!!" << 12323 << "\r\n");
	GVM_LOG(main, "first !!!!" << 12323 << "\r\n");
	test4_threads();
}
}
