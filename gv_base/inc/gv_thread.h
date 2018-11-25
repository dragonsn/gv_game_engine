#pragma once

namespace gv
{
class gv_event
{
public:
	gv_event(bool auto_reset = true);
	~gv_event();
	void set();
	void wait();
	bool try_wait(int milliseconds);
	void reset();

protected:
	void* _event;
};

class gv_runnable : public gv_refable
{
public:
	gv_runnable(){};
	virtual ~gv_runnable(){};
	virtual void run(){};
};

template < class T >
class gvt_runnable : public gv_runnable
{
public:
	typedef void (T::*gv_runnable_callback)();
	gvt_runnable()
	{
		m_method = NULL;
		m_pobject = NULL;
	}
	gvt_runnable(T& object, gv_runnable_callback method)
		: m_pobject(&object), m_method(method)
	{
	}
	gvt_runnable(const gvt_runnable& ra)
		: m_pobject(ra.m_pobject), m_method(ra.m_method)
	{
	}
	~gvt_runnable()
	{
	}
	void init(T* pobj, gv_runnable_callback method)
	{
		m_pobject = pobj;
		m_method = method;
	}
	void set_object(T* pobj)
	{
		m_pobject = pobj;
	}
	void set_callback(gv_runnable_callback method)
	{
		m_method = method;
	}
	gvt_runnable& operator=(const gvt_runnable& ra)
	{
		m_pobject = ra.m_pobject;
		m_method = ra.m_method;
		return *this;
	}
	void run()
	{
		(m_pobject->*m_method)();
	}

private:
	T* m_pobject;
	gv_runnable_callback m_method;
};

typedef void(GV_STDCALL* gv_apc_callback)(gv_ulong_ptr param);

#if defined(WIN32)
typedef unsigned long gv_thread_ret_type;
#else
typedef void* gv_thread_ret_type;
#endif
class gv_thread : public gv_runnable
{
public:
	enum priority
	{
		E_PRIO_LOWEST,
		E_PRIO_LOW,
		E_PRIO_NORMAL,
		E_PRIO_HIGH,
		E_PRIO_HIGHEST,
	};
	gv_thread();
	gv_thread(const gv_id&);
	virtual ~gv_thread();

	const void* get_handle() const;
	void* get_handle();
	const gv_id& get_name() const;
	void set_name(const gv_id& id)
	{
		m_name = id;
	};
	gv_ulong get_tid() const;
	void set_priority(priority pri);
	;
	priority get_priority() const;
	void set_stack_size(int size);
	int get_stack_size() const;
	void start(gv_runnable* p, bool pending = false);
	void start(boost::function< void() > call, bool pending = false)
	{
		m_thread_main = call;
		start(this, pending);
	};
	void join();
	void join(long milliseconds);
	bool try_join(long milliseconds);
	bool is_running() const;
	void* get_user_data() const
	{
		return m_user_data;
	};
	void set_user_data(void* data)
	{
		m_user_data = data;
	}
	void close_handle();
	void resume_thread();
	void pause_thread();
	void kill_thread();
#if GV_64
	bool apc_call(gv_apc_callback callback, gv_ulong_ptr param);
#else
	bool apc_call(gv_apc_callback callback, gv_uint param);
#endif
	gv_int& get_event_stack_depth()
	{
		return m_event_stack_depth;
	};
	// can overrider defaut_run ,if you don't use runable.
	virtual void defaut_run();
	static void sleep(long milliseconds = 0);
	static void yield();
	static gv_thread* current(const char* name = 0);
	static int get_thread_cout();
	static gv_uint_ptr current_thread_id();
	static void static_create_main_thread_context();
	virtual void run()
	{
		m_thread_main();
	};

protected:
	static gv_atomic_count s_thead_count;
	static gv_thread_ret_type GV_STDCALL entry(void* pThread);
	boost::function< void() > m_thread_main;
	gv_id m_name;
	void* m_handle;
	void* m_user_data;
	priority m_priority;
	gv_runnable* m_current_runnable;
	int m_stack_size;
	gv_ulong m_tid;
	gv_bool m_is_running;
	gv_int m_event_stack_depth;
	gv_runnable m_default_runable;
};
}
