namespace gv
{
//==============================================
//============gv_mutex_imp======================
//==============================================
/*
class gv_event_pthread
{
         enum e_trigger_type
        {
                TRIGGERED_NONE,
                TRIGGERED_ONE,
                TRIGGERED_ALL,
        } ;

        bool m_is_initialized;
        bool m_is_manual_reset;
        volatile e_trigger_type m_trigger_type;
        volatile gv_int			m_waiting_threads;
        pthread_mutex_t			m_mutex;
        pthread_cond_t			m_condition;

        inline void lock_event_mutex()
        {
                const int rc = pthread_mutex_lock(&m_mutex);
                GV_ASSERT(rc == 0);
        }

        inline void unlock_event_mutex()
        {
                const int rc = pthread_mutex_unlock(&m_mutex);
                GV_ASSERT(rc == 0);
        }

        static inline void subtract_timevals(const struct timeval *FromThis,
struct timeval *SubThis, struct timeval *Difference)
        {
                if (FromThis->tv_usec < SubThis->tv_usec)
                {
                        int nsec = (SubThis->tv_usec - FromThis->tv_usec) /
1000000 + 1;
                        SubThis->tv_usec -= 1000000 * nsec;
                        SubThis->tv_sec += nsec;
                }

                if (FromThis->tv_usec - SubThis->tv_usec > 1000000)
                {
                        int nsec = (FromThis->tv_usec - SubThis->tv_usec) /
1000000;
                        SubThis->tv_usec += 1000000 * nsec;
                        SubThis->tv_sec -= nsec;
                }

                Difference->tv_sec = FromThis->tv_sec - SubThis->tv_sec;
                Difference->tv_usec = FromThis->tv_usec - SubThis->tv_usec;
        }

public:

        gv_event_pthread()
        {
                m_is_initialized = false;
                m_is_manual_reset = false;
                m_trigger_type = TRIGGERED_NONE;
                m_waiting_threads = 0;
        }

        virtual ~gv_event_pthread()
        {
                // Safely destructing an Event is VERY delicate, so it can
handle badly-designed
                //  calling code that might still be waiting on the event.
                if (m_is_initialized)
                {
                        // try to flush out any waiting threads...
                        lock_event_mutex();
                        m_is_manual_reset = true;
                        unlock_event_mutex();
                        trigger();  // any waiting threads should start to wake
up now.

                        lock_event_mutex();
                        m_is_initialized = false;  // further incoming calls to
this object will now crash in check().
                        while (m_waiting_threads)  // spin while waiting threads
wake up.
                        {
                                unlock_event_mutex();  // cycle through waiting
threads...
                                lock_event_mutex();
                        }
                        // No threads are currently waiting on m_condition and
we hold the m_mutex. Kill it.
                        pthread_cond_destroy(&m_condition);
                        // Unlock and kill the mutex, since nothing else can
grab it now.
                        unlock_event_mutex();
                        pthread_mutex_destroy(&m_mutex);
                }
        }

        virtual bool create(bool _bIsManualReset = false)
        {
                GV_ASSERT(!m_is_initialized);
                bool RetVal = false;
                m_trigger_type = TRIGGERED_NONE;
                m_is_manual_reset = _bIsManualReset;

                if (pthread_mutex_init(&m_mutex, NULL) == 0)
                {
                        if (pthread_cond_init(&m_condition, NULL) == 0)
                        {
                                m_is_initialized = true;
                                RetVal = true;
                        }
                        else
                        {
                                pthread_mutex_destroy(&m_mutex);
                        }
                }
                return RetVal;
        }

        virtual void trigger()
        {
                GV_ASSERT(m_is_initialized);

                lock_event_mutex();

                if (m_is_manual_reset)
                {
                        // Release all waiting threads at once.
                        m_trigger_type = TRIGGERED_ALL;
                        GVM_VERIFY(pthread_cond_broadcast(&m_condition)==0);
                }
                else
                {
                        // Release one or more waiting threads (first one to get
the mutex
                        //  will reset m_trigger_type, rest will go back to
waiting again).
                        m_trigger_type = TRIGGERED_ONE;
                        GVM_VERIFY(pthread_cond_signal(&m_condition));  // may
release multiple threads anyhow!
                }

                unlock_event_mutex();
        }

        virtual void reset()
        {
                GV_ASSERT(m_is_initialized);
                lock_event_mutex();
                m_trigger_type = TRIGGERED_NONE;
                unlock_event_mutex();
        }

        virtual bool wait(gv_uint WaitTime = (gv_uint)-1)
        {
                struct timeval StartTime;

                // We need to know the start time if we're going to do a timed
wait.
                if ( (WaitTime > 0) && (WaitTime != ((gv_uint)-1)) )  // not
polling and not infinite wait.
                {
                        gettimeofday(&StartTime, NULL);
                }

                lock_event_mutex();

                bool bRetVal = false;

                // loop in case we fall through the m_condition signal but
someone else claims the event.
                do
                {
                        // See what state the event is in...we may not have to
wait at all...

                        // One thread should be released. We saw it first, so
we'll take it.
                        if (m_trigger_type == TRIGGERED_ONE)
                        {
                                m_trigger_type = TRIGGERED_NONE;  // dibs!
                                bRetVal = true;
                        }

                        // manual reset that is still signaled. Every thread
goes.
                        else if (m_trigger_type == TRIGGERED_ALL)
                        {
                                bRetVal = true;
                        }

                        // No event signalled yet.
                        else if (WaitTime != 0)  // not just polling, wait on
the condition variable.
                        {
                                m_waiting_threads++;
                                if (WaitTime == ((gv_uint)-1)) // infinite wait?
                                {
                                        GVM_VERIFY(
pthread_cond_wait(&m_condition, &m_mutex)==0);  // unlocks m_mutex while
blocking...
                                }
                                else  // timed wait.
                                {
                                        struct timespec TimeOut;
                                        const gv_uint ms = (StartTime.tv_usec /
1000) + WaitTime;
                                        TimeOut.tv_sec = StartTime.tv_sec + (ms
/ 1000);
                                        TimeOut.tv_nsec = (ms % 1000) * 1000000;
// remainder of milliseconds converted to nanoseconds.
                                        int rc =
pthread_cond_timedwait(&m_condition, &m_mutex, &TimeOut);    // unlocks m_mutex
while blocking...
                                        GV_ASSERT((rc == 0) || (rc ==
ETIMEDOUT));
                                        // Update WaitTime and StartTime in case
we have to go again...
                                        struct timeval Now, Difference;
                                        gettimeofday(&Now, NULL);
                                        subtract_timevals(&Now, &StartTime,
&Difference);
                                        const gv_int DifferenceMS =
((Difference.tv_sec * 1000) + (Difference.tv_usec / 1000));
                                        WaitTime = ((DifferenceMS >= WaitTime) ?
0 : (WaitTime - DifferenceMS));
                                        StartTime = Now;
                                }
                                m_waiting_threads--;
                                GV_ASSERT(m_waiting_threads >= 0);
                        }

                } while ((!bRetVal) && (WaitTime != 0));

                unlock_event_mutex();
                return bRetVal;

        };

};*/

struct gv_event_pthread
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool triggered;
	bool auto_reset;

	void create(bool _auto_reset)
	{
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&cond, 0);
		triggered = false;
		auto_reset = _auto_reset;
	}
	void trigger()
	{
		pthread_mutex_lock(&mutex);
		triggered = true;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	void reset()
	{
		pthread_mutex_lock(&mutex);
		triggered = false;
		pthread_mutex_unlock(&mutex);
	}
	bool wait(gv_uint ms = 0)
	{
		pthread_mutex_lock(&mutex);
		while (!triggered)
			pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
		if (auto_reset)
			reset();
		return true;
	}
};

gv_event::gv_event(bool auto_reset)
{
	gv_event_pthread* p_event = new gv_event_pthread;
	p_event->create(auto_reset);
	_event = p_event;
}

gv_event::~gv_event()
{
	delete ((gv_event_pthread*)_event);
}

void gv_event::set()
{
	GV_PROFILE_EVENT_SLOW(gv_event_set);
	((gv_event_pthread*)_event)->trigger();
}

void gv_event::reset()
{
	GV_PROFILE_EVENT_SLOW(gv_event_reset);
	((gv_event_pthread*)_event)->reset();
}

void gv_event::wait()
{
	GV_PROFILE_EVENT(gv_event_wait, 0);
	((gv_event_pthread*)_event)->wait();
}

bool gv_event::try_wait(int milliseconds)
{
	GV_PROFILE_EVENT(gv_event_try_wait, milliseconds);
	return ((gv_event_pthread*)_event)->wait(milliseconds);
}
}