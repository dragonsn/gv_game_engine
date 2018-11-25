#pragma once

#include "gv_mutex.h"

namespace gv
{
//-----------------class---------------------------------------------------
class gv_mutex_null
{
public:
	gv_mutex_null(){};
	void lock(){};
	void unlock(){};
};
class gv_mutex;
//-----------------class---------------------------------------------------

//-----------------class---------------------------------------------------
class gvp_single_thread
{
public:
	typedef gv_mutex_null mutex_holder;
};

//-----------------class---------------------------------------------------
class gvp_multi_thread
{
public:
	typedef gv_mutex mutex_holder;
};

typedef gvt_scope_lock< gv_mutex > gv_thread_lock;

template < class T >
class gvt_policy_thread_mode : public gvp_single_thread
{
};

//-----------------class---------------------------------------------------
template < class policy_thread >
class gvt_lockable_object_level
{
public:
	typedef typename policy_thread::mutex_holder type_of_mutex;
	void lock()
	{
		object_lock.lock();
	};
	void unlock()
	{
		object_lock.unlock();
	}

private:
	type_of_mutex object_lock;
};

} // namespace
