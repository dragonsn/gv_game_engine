#pragma once
#include <stack>
namespace gv
{
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_stack
	: public std::stack< T, std::deque< T, gvt_allocator< T, policy_memory > > >,
	  public gvt_lockable_object_level< policy_thread_mode >
{
public:
	typedef T type_of_data;
	typedef gvt_stack< type_of_data, policy_memory, policy_thread_mode >
		type_of_stack;
	typedef std::stack< T, std::deque< T, gvt_allocator< T, policy_memory > > > super;
};
} // namespace gv
