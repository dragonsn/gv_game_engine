#include <pthread.h>
namespace gv
{

//==============================================
//============gv_thread_imp======================
//==============================================

static pthread_t get_current_thread_id(void)
{
	return pthread_self();
}

static pthread_key_t alloc_tls_slot(void)
{
	// allocate a per-thread mem slot
	pthread_key_t key = 0;
	GVM_VERIFY(pthread_key_create(&key, NULL) == 0);
	return key;
}

static void set_tls_value(pthread_key_t slot, void* p)
{
	GVM_VERIFY(pthread_setspecific(slot, p) == 0);
}

static void* get_tls_value(pthread_key_t slot)
{
	return pthread_getspecific((pthread_key_t)slot);
}

static void free_tls_slot(pthread_key_t slot)
{
	GVM_VERIFY(pthread_key_delete((pthread_key_t)slot) == 0);
}

pthread_key_t s_this_thread_key = alloc_tls_slot();
;
gv_atomic_count gv_thread::s_thead_count;

static pthread_t& get_pthread(gv_thread* pt)
{
	pthread_t* ret = (pthread_t*)pt->get_handle();
	GV_ASSERT(ret);
	return *ret;
}
gv_thread::gv_thread()
	: m_handle(NULL), m_user_data(NULL), m_priority(E_PRIO_NORMAL),
	  m_current_runnable(NULL), m_stack_size(0) // system default;
	  ,
	  m_is_running(false), m_event_stack_depth(0)
{
	++s_thead_count;
};

gv_thread::gv_thread(const gv_id& _name)
	: m_handle(NULL), m_user_data(NULL), m_priority(E_PRIO_NORMAL),
	  m_current_runnable(NULL), m_stack_size(0) // system default;
	  ,
	  m_name(_name), m_is_running(false), m_event_stack_depth(0)
{
	++s_thead_count;
};

gv_thread::~gv_thread()
{
	close_handle();
	--s_thead_count;
}

void gv_thread::close_handle()
{
	if (this->is_running() && get_handle())
	{
		join();
	}
	if (get_handle())
	{
		delete ((pthread_t*)m_handle);
		m_handle = NULL;
	}
}

const void* gv_thread::get_handle() const
{
	return m_handle;
}

void* gv_thread::get_handle()
{
	return m_handle;
}

const gv_id& gv_thread::get_name() const
{
	return m_name;
};

gv_ulong gv_thread::get_tid() const
{
	return m_tid;
};

gv_int translate_thread_priority(gv_thread::priority my_priority)
{
	switch (my_priority)
	{
	// 0 is the lowest, 31 is the highest possible priority for pthread
	case gv_thread::E_PRIO_HIGHEST:
		return 31;
	case gv_thread::E_PRIO_HIGH:
		return 25;
	case gv_thread::E_PRIO_NORMAL:
		return 15;
	case gv_thread::E_PRIO_LOW:
		return 5;
	case gv_thread::E_PRIO_LOWEST:
		return 1;
	}
	return 0;
}

void gv_thread::set_priority(priority p)
{

	struct sched_param param;
	gvt_zero(param);
	// set the priority appropriately
	param.sched_priority = translate_thread_priority(p);
	pthread_setschedparam(get_pthread(this), SCHED_RR, &param);
}

gv_thread::priority gv_thread::get_priority() const
{
	return m_priority;
};

void gv_thread::set_stack_size(int size)
{
	m_stack_size = size;
};

int gv_thread::get_stack_size() const
{
	return m_stack_size;
};

void gv_thread::start(gv_runnable* runable, bool pending)
{
	GV_PROFILE_EVENT_SLOW(gv_thread_start);
	GV_ASSERT(!get_handle()); // thread can't start twice
	m_handle = new pthread_t;
	pthread_attr_t* attr_ptr = NULL;
	pthread_attr_t stack_attr;
	if (pthread_attr_init(&stack_attr) == 0)
	{
		// we'll use this the attribute if this succeeds, otherwise, we'll wing it
		// without it.
		if (pthread_attr_setstacksize(&stack_attr, m_stack_size) == 0)
		{
			attr_ptr = &stack_attr;
		}
		pthread_attr_setdetachstate(&stack_attr, PTHREAD_CREATE_JOINABLE);
	}
	this->m_current_runnable = runable;
	GVM_VERIFY(pthread_create(&get_pthread(this), attr_ptr, gv_thread::entry,
							  this) == 0);

	if (attr_ptr != NULL)
	{
		pthread_attr_destroy(attr_ptr);
	}
	// pthread_detach(get_pthread(this));  // we will still want to join all the
	// thread..
	set_priority(m_priority);
};

void gv_thread::join()
{
	GV_PROFILE_EVENT(pthread_join, 0);
	pthread_join(get_pthread(this), NULL);
};

void gv_thread::join(long milliseconds)
{
	try_join(milliseconds);
};

bool gv_thread::try_join(long milliseconds)
{
	/*struct timespec ts;
  int s;
  if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
  {
          return false;
  }
  ts.tv_sec += milliseconds/1000;
  ts.tv_nsec+= (milliseconds%1000)*1000*1000;
  s = pthread_timedjoin_np(get_pthread(this), NULL, &ts);
  if (s != 0)
  {
          return false;
  }*/
	join();
	return true;
};

bool gv_thread::is_running() const
{
	return m_is_running;
};

void gv_thread::sleep(long milliseconds)
{
	GV_PROFILE_EVENT(gv_thread_sleep, milliseconds);
	usleep(milliseconds * 1000);
};

void gv_thread::yield()
{
	GV_PROFILE_EVENT_SLOW(gv_thread_yield);
	sleep(0);
};

gv_thread* gv_thread::current(const char* name)
{
	gv_thread* ret = (gv_thread*)get_tls_value(s_this_thread_key);
	if (!ret)
	{
		gv_string_tmp s = name;
		if (!name)
			s = "thread";
		s << current_thread_id();
		gv_thread* pthread = new gv_thread(gv_id(*s));
		set_tls_value(s_this_thread_key, pthread);
		ret = pthread;
	}
	return ret;
};

int gv_thread::get_thread_cout()
{
	return s_thead_count.get();
};

gv_uint_ptr gv_thread::current_thread_id()
{
	return (gv_uint_ptr)((size_t)pthread_self());
};

gv_thread_ret_type GV_STDCALL gv_thread::entry(void* p)
{
	gv_thread* pthread = (gv_thread*)p;
	GVM_DEBUG_OUT("[thread]gv thread " << pthread->get_name()
									   << " start runing!!");
	set_tls_value(s_this_thread_key, pthread);
	pthread->m_is_running = true;
	GV_ASSERT(current() == pthread);
	if (pthread->m_current_runnable)
	{
		GV_PROFILE_EVENT(pthread_entry, 0);
		pthread->m_current_runnable->run();
	}
	else
	{
		GV_PROFILE_EVENT(pthread_entry, 0);
		pthread->defaut_run();
	}
	GV_ASSERT(current() == pthread);
	pthread->m_is_running = false;
	GVM_DEBUG_OUT("[thread]gv thread " << pthread->get_name() << " stoped!!");
	pthread_exit(NULL);
	return 0;
};
void gv_thread::defaut_run()
{
}
// not supported in POSIX
void gv_thread::resume_thread()
{
}
void gv_thread::pause_thread()
{
}
void gv_thread::kill_thread()
{
}
bool gv_thread::apc_call(gv_apc_callback callback, gv_uint param)
{
	return false;
}
};