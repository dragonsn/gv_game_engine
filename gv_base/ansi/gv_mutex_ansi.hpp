namespace gv
{
//==============================================
//============gv_mutex_imp======================
//==============================================

GV_STATIC_ASSERT(sizeof(gv_mutex::mutex_container) >= sizeof(pthread_mutex_t));
class gv_mutex_imp
{
public:
	gv_mutex_imp(gv_mutex::mutex_container& p)
		: m_cs(*((pthread_mutex_t*)&p))
	{
	}
	void init()
	{
		gvt_construct(&m_cs);
		pthread_mutexattr_t MutexAttributes;
		pthread_mutexattr_init(&MutexAttributes);
		pthread_mutexattr_settype(&MutexAttributes, PTHREAD_MUTEX_RECURSIVE);
		GVM_VERIFY(pthread_mutex_init(&m_cs, &MutexAttributes) == 0);
	}
	void destroy()
	{
		GVM_VERIFY(pthread_mutex_destroy(&m_cs) == 0);
		gvt_destroy(&m_cs);
	}
	void lock()
	{
		// GV_PROFILE_EVENT_SLOW(gv_mutex_lock);
		pthread_mutex_lock(&m_cs);
	};
	void unlock()
	{
		// GV_PROFILE_EVENT_SLOW(gv_mutex_unlock);
		pthread_mutex_unlock(&m_cs);
	}

private:
	pthread_mutex_t& m_cs;
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
}