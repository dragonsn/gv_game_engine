#pragma once

#include "gvt_array_static.h"

namespace gv
{

template < class T >
int gvt_load_callstack(T* p, int max_call_stack_depth)
{
	GV_ASSERT(0);
	return false;
};
extern template <>
int gvt_load_callstack< gv_uint >(gv_uint* p, int max_call_stack_depth);
extern template <>
int gvt_load_callstack< gv_ulong >(gv_ulong* p, int max_call_stack_depth);

template < class type_of_eip, int max_call_stack_depth >
class gvt_callstack
{
public:
	typedef gvt_array_not_resizable< type_of_eip, max_call_stack_depth >
		type_of_stack;

	inline void load()
	{
		gvt_load_callstack< type_of_eip >(stack.begin(), stack.size());
	};
	inline const type_of_stack& get() const
	{
		return stack;
	}

protected:
	type_of_stack stack;
};
};