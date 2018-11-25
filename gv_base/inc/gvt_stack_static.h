#pragma once
#include "gvt_stack.h"
namespace gv
{
template < class T, int fixed_size >
class gvt_stack_static
{
public:
	inline gvt_stack_static()
	{
		cu_size = 0;
	}
	inline gvt_stack_static(const gvt_stack_static& s)
	{
		*this = s;
	}
	inline const T& top() const
	{
		GV_ASSERT(cu_size);
		return data[cu_size - 1];
	}
	inline int pop()
	{
		GV_ASSERT(cu_size);
		return --cu_size;
	}
	inline void push(const T& t)
	{
		GV_ASSERT(cu_size < fixed_size);
		data[cu_size++] = t;
	}
	inline void push()
	{
		GV_ASSERT(cu_size < fixed_size);
		cu_size++;
	}
	inline bool is_empty()
	{
		return cu_size == 0;
	}
	inline void pop_all()
	{
		cu_size = 0;
	}
	inline gv_int size()
	{
		return cu_size;
	}
	inline T& operator[](gv_int i)
	{
		GV_ASSERT(i < cu_size);
		return data[i];
	}
	inline gvt_stack_static& operator=(const gvt_stack_static& s)
	{
		if (this != &s)
		{
			cu_size = s.cu_size;
			for (int i = 0; i < cu_size; i++)
				data[i] = s.data[i];
		}
		return *this;
	}

private:
	gv_int cu_size;
	T data[fixed_size];
};
} // namespace gv