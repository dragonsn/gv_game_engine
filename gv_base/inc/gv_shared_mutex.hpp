#pragma warning(disable : 4244)
#include "gv_shared_mutex.h"
#include <boost/boost/thread/shared_mutex.hpp>
namespace gv
{

class gv_shared_mutex_imp
{
	friend class gv_shared_mutex;

public:
	gv_shared_mutex_imp(){};
	~gv_shared_mutex_imp(){};
	inline void lock_shared()
	{
		return m_mutex.lock_shared();
	};
	inline bool try_lock_shared()
	{
		return m_mutex.try_lock_shared();
	};
	inline void unlock_shared()
	{
		return m_mutex.unlock_shared();
	};
	inline void lock()
	{
		return m_mutex.lock();
	};
	inline bool try_lock()
	{
		return m_mutex.try_lock();
	};
	inline void unlock()
	{
		return m_mutex.unlock();
	};
	inline void lock_upgrade()
	{
		return m_mutex.lock_upgrade();
	};
	inline void unlock_upgrade()
	{
		return m_mutex.unlock_upgrade();
	};
	boost::shared_mutex m_mutex;
};

GV_STATIC_ASSERT(gvc_size_shared_mutex_imp >= sizeof(gv_shared_mutex_imp));
gv_shared_mutex::gv_shared_mutex()
{
	this->m_impl = (gv_shared_mutex_imp*)this->m_shadow;
	gvt_construct(this->m_impl);
};
gv_shared_mutex::~gv_shared_mutex()
{
	gvt_destroy(this->m_impl);
};
void gv_shared_mutex::lock_shared()
{
	return this->m_impl->lock_shared();
};
bool gv_shared_mutex::try_lock_shared()
{
	return this->m_impl->try_lock_shared();
};
void gv_shared_mutex::unlock_shared()
{
	return this->m_impl->unlock_shared();
};
void gv_shared_mutex::lock()
{
	return this->m_impl->lock();
};
bool gv_shared_mutex::try_lock()
{
	return this->m_impl->try_lock();
};
void gv_shared_mutex::unlock()
{
	return this->m_impl->unlock();
};
void gv_shared_mutex::lock_upgrade()
{
	return this->m_impl->lock_upgrade();
};
void gv_shared_mutex::unlock_upgrade()
{
	return this->m_impl->unlock_upgrade();
};
};