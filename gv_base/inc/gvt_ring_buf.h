#pragma once

#include <boost/boost/circular_buffer.hpp>
namespace gv
{
// plug in our memory managerment ,and make it guardable!
template < class T, class policy_memory = gvt_policy_memory_default< T >,
		   class policy_thread_mode = gvt_policy_thread_mode< T > >
class gvt_ring_buf
	: public boost::circular_buffer< T, gvt_allocator< T, policy_memory > >,
	  public gvt_lockable_object_level< policy_thread_mode >
{
public:
	typedef T type_of_data;
	typedef gvt_ring_buf< type_of_data, policy_memory, policy_thread_mode >
		type_of_ring;
	typedef boost::circular_buffer< T, gvt_allocator< T, policy_memory > > super;
	gvt_ring_buf()
	{
	}
	gvt_ring_buf(int size)
		: super(size)
	{
	}
};

} // namespace gv