#pragma once
namespace gv
{
class gv_shared_mutex_imp;
static const gv_int gvc_size_shared_mutex_imp = 64;
class gv_shared_mutex
{
public:
	gv_shared_mutex();
	~gv_shared_mutex();

	void lock_shared();
	bool try_lock_shared();
	void unlock_shared();

	void lock();
	bool try_lock();
	void unlock();

	void lock_upgrade();
	void unlock_upgrade();

protected:
	gv_shared_mutex_imp* m_impl;
	gv_char m_shadow[gvc_size_shared_mutex_imp];
};

class gv_scope_read_lock
{
	gv_shared_mutex& the_lock;
	gv_scope_read_lock(const gv_scope_read_lock& l);

public:
	gv_scope_read_lock(gv_shared_mutex& t)
		: the_lock(t)
	{
		the_lock.lock_shared();
	}
	~gv_scope_read_lock()
	{
		the_lock.unlock_shared();
	}
};

class gv_scope_write_lock
{
	gv_shared_mutex& the_lock;
	gv_scope_write_lock(const gv_scope_write_lock& l);

public:
	gv_scope_write_lock(gv_shared_mutex& t)
		: the_lock(t)
	{
		the_lock.lock();
	}
	~gv_scope_write_lock()
	{
		the_lock.unlock();
	}
};
}