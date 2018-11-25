#include "gv_base.h"
#include "gvt_array.h"
#include "gvt_slist.h"
#include "gv_string.h"
#include "gv_id.h"
#include "gv_thread.h"
#include "gv_profiler.h"
#include <windows.h>
#pragma warning(disable : 4244)
#pragma warning(disable : 4245)
#define GV_PROFILE_EVENT_0(x) GV_PROFILE_EVENT(x, 0)
namespace gv
{
GV_THREAD_LOCAL_STATIC gv_thread* s_current_thread;
//==============================================
//============gv_mutex_imp======================
//==============================================
GV_STATIC_ASSERT(sizeof(gv_mutex::mutex_container) >= sizeof(CRITICAL_SECTION));
class gv_mutex_imp
{
public:
	gv_mutex_imp(gv_mutex::mutex_container& p)
		: m_cs(*((CRITICAL_SECTION*)&p))
	{
	}
	void init()
	{
		gvt_construct(&m_cs);
		InitializeCriticalSection(&m_cs);
	}
	void destroy()
	{
		DeleteCriticalSection(&m_cs);
		gvt_destroy(&m_cs);
	}
	void lock()
	{
		// GV_PROFILE_EVENT_0(gv_mutex_lock);
		EnterCriticalSection(&m_cs);
	};
	void unlock()
	{
		// GV_PROFILE_EVENT_0(gv_mutex_unlock);
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION& m_cs;
};

gv_mutex::gv_mutex()
{
	gv_mutex_imp(_mutex).init();
}

gv_mutex::~gv_mutex()
{
	gv_mutex_imp(_mutex).destroy();
}

void gv_mutex::lock()
{
	gv_mutex_imp(_mutex).lock();
}

void gv_mutex::unlock()
{
	gv_mutex_imp(_mutex).unlock();
}
//==============================================
//============gv_mutex_imp======================
//==============================================
gv_event::gv_event(bool auto_reset)
{
	_event = CreateEventA(NULL, auto_reset ? FALSE : TRUE, FALSE, NULL);
	GV_ASSERT(_event);
}

gv_event::~gv_event()
{
	CloseHandle(_event);
}

void gv_event::set()
{
	GV_PROFILE_EVENT_0(gv_event_set);
	if (!SetEvent(_event))
	{
		GV_ASSERT(0 && "cannot signal event");
	}
}

void gv_event::reset()
{
	GV_PROFILE_EVENT_0(gv_event_reset);
	if (!ResetEvent(_event))
	{
		GV_ASSERT(0 && "cannot reset event");
	}
}

void gv_event::wait()
{
	GV_PROFILE_EVENT_0(gv_event_wait);

	// switch (WaitForSingleObject(_event, INFINITE))
	DWORD ret = WaitForSingleObjectEx(_event, INFINITE, TRUE);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		return;
	case WAIT_IO_COMPLETION:
		return;
	default:
		GV_ASSERT(0);
	}
}

bool gv_event::try_wait(int milliseconds)
{
	GV_PROFILE_EVENT_0(gv_event_try_wait);
	// switch (WaitForSingleObject(_event, milliseconds ))
	switch (WaitForSingleObjectEx(_event, milliseconds, TRUE))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		GV_ASSERT(0);
	}
	return false;
}

//==============================================
//============gv_mutex_imp======================
//==============================================
gv_atomic_count gv_thread::s_thead_count;

gv_thread::gv_thread()
	: m_handle(NULL), m_user_data(NULL), m_priority(E_PRIO_NORMAL),
	  m_current_runnable(NULL), m_stack_size(0) // system default;
	  ,
	  m_event_stack_depth(0)
{
	++s_thead_count;
};

gv_thread::gv_thread(const gv_id& _name)
	: m_handle(NULL), m_user_data(NULL), m_priority(E_PRIO_NORMAL),
	  m_current_runnable(NULL), m_stack_size(0) // system default;
	  ,
	  m_name(_name), m_event_stack_depth(0)
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
	if (m_handle)
		CloseHandle(m_handle);
	m_handle = NULL;
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

// THREAD_PRIORITY_LOWEST
// THREAD_PRIORITY_BELOW_NORMAL
// THREAD_PRIORITY_NORMAL
// THREAD_PRIORITY_HIGHEST
// THREAD_PRIORITY_ABOVE_NORMAL
int get_win32_thread_priority(int _pri)
{
	switch (_pri)
	{
	case gv_thread::E_PRIO_LOWEST:
		return THREAD_PRIORITY_LOWEST;
	case gv_thread::E_PRIO_LOW:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case gv_thread::E_PRIO_NORMAL:
		return THREAD_PRIORITY_NORMAL;
	case gv_thread::E_PRIO_HIGH:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case gv_thread::E_PRIO_HIGHEST:
		return THREAD_PRIORITY_HIGHEST;
	default:
		GV_ASSERT(0 && "invalid thread priority!");
	}
	return -1;
};

void gv_thread::set_priority(priority p)
{
	if (m_priority != p)
	{
		m_priority = p;
	}
	if (m_handle)
	{
		if (SetThreadPriority(m_handle, get_win32_thread_priority(p)) == 0)
			GV_ASSERT(0 && "cannot set thread priority!");
	}
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
	GV_PROFILE_EVENT_0(gv_thread_start);
	m_current_runnable = runable;
	DWORD tid;
	m_handle = CreateThread(NULL, m_stack_size, entry, this,
							pending ? CREATE_SUSPENDED : 0, &tid);
	m_tid = (gv_ulong)tid;
	GV_ASSERT(m_handle && "cannot create thread!");
	set_priority(m_priority);
};

void gv_thread::join()
{
	GV_PROFILE_EVENT_0(gv_thread_join);
	if (!m_handle)
		return;
	switch (WaitForSingleObject(m_handle, INFINITE))
	{
	case WAIT_OBJECT_0:
		close_handle();
		return;
	default:
		GV_ASSERT(0 && "cannot join thread");
	}
};

void gv_thread::join(long milliseconds)
{
	bool r = try_join(milliseconds);
	GV_ASSERT(r && "try join thread time out!");
};
bool gv_thread::try_join(long milliseconds)
{
	GV_PROFILE_EVENT_0(gv_thread_try_join);
	if (!m_handle)
		return true;
	switch (WaitForSingleObject(m_handle, milliseconds + 1))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		close_handle();
		return true;
	default:
		GV_ASSERT(0 && "cannot join thread");
	}
	return false;
};

bool gv_thread::is_running() const
{
	if (m_handle)
	{
		DWORD ec = 0;
		return GetExitCodeThread(m_handle, &ec) && ec == STILL_ACTIVE;
	}
	return false;
};
void gv_thread::sleep(long milliseconds)
{
	GV_PROFILE_EVENT(gv_thread_sleep, milliseconds);
	Sleep(DWORD(milliseconds));
};
void gv_thread::yield()
{
	GV_PROFILE_EVENT_0(gv_thread_yield);
	Sleep(0);
};
gv_thread* gv_thread::current(const char* name)
{
	gv_thread* ret = s_current_thread;
	if (!ret)
	{
		gv_string_tmp s = name;
		if (!name)
			s = "thread";
		s << current_thread_id();
		gv_thread* pthread = new gv_thread(gv_id(*s));
		s_current_thread = pthread;
		ret = pthread;
	}
	return ret;
};

gv_uint_ptr gv_thread::current_thread_id()
{
	return (gv_uint_ptr)GetCurrentThreadId();
};
int gv_thread::get_thread_cout()
{
	return s_thead_count.get();
};
gv_thread_ret_type GV_STDCALL gv_thread::entry(void* p)
{
	gv_thread* pthread = (gv_thread*)p;
	s_current_thread = pthread;
	GV_ASSERT(current() == pthread);
	if (pthread->m_current_runnable)
	{
		GV_PROFILE_EVENT_0(pthread_entry);
		pthread->m_current_runnable->run();
	}
	else
	{
		GV_PROFILE_EVENT_0(pthread_entry);
		pthread->defaut_run();
	}
	GV_ASSERT(current() == pthread);
	return 1;
};

void gv_thread::defaut_run()
{
}

void gv_thread::resume_thread()
{
	GV_PROFILE_EVENT_0(gv_thread_resume);
	ResumeThread(m_handle);
};

void gv_thread::pause_thread()
{
	GV_PROFILE_EVENT_0(gv_thread_pause);
	SuspendThread(m_handle);
};

void gv_thread::kill_thread()
{
	GV_PROFILE_EVENT_0(gv_thread_kill);
	TerminateThread(m_handle, 0);
};

#if GV_64
bool gv_thread::apc_call(gv_apc_callback callback, gv_ulong_ptr param)
#else
bool gv_thread::apc_call(gv_apc_callback callback, gv_uint param)
#endif
{
	return QueueUserAPC(callback, get_handle(), param) != 0;
};
};
