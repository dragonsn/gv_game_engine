#pragma once

namespace gv
{
static const gv_int gvc_size_mutex_container = 16;
class gv_mutex
{
public:
	gv_mutex();
	~gv_mutex();
	void lock();
	void unlock();
	struct mutex_container
	{
		gv_uint pad[gvc_size_mutex_container];
	};

protected:
	mutex_container _mutex;
};
};