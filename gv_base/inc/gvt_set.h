#pragma once
#include <set>
namespace gv
{
template < class T, class policy_compare = std::less< T >,
		   class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_set
	: public std::set< T, policy_compare, gvt_allocator< T, policy_memory > >,
	  public gvt_lockable_object_level< policy_thread_mode >
{
public:
	gvt_set(){};
	typedef T type_of_data;
	typedef gvt_set< type_of_data, policy_compare, policy_memory,
					 policy_thread_mode >
		type_of_set;
	typedef std::set< T, policy_compare, gvt_allocator< T, policy_memory > > super;

public:
};
} // namespace gv