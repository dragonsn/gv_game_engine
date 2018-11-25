#pragma once
// http://www.chaoticmind.net/~hcb/projects/boost.atomic/doc/atomic/usage_examples.html
// still use 32bit ptr in 64 bit os, since there is no reliable 128bit CAS2
//

namespace gv
{
//============================================================================================
//								:
//============================================================================================

class gv_lock_free : public gvp_memory_base
{
public:
	static const gv_int max_try_count = 100;
	void* alloc(size_t size);
	void free(void* p);
	void* realloc(void* p, size_t size);

	static void* static_to_ptr(const gv_atomic_count& a);
	static gv_atomic_count static_from_ptr(const void* src);
	static void static_init(const gv_int initial_lock_free_size = 16 * 1024 *
																  1024);
	static void static_destroy();
};
#define GVM_LOCK_FREE_FREEZE_CHECK \
	GV_ASSERT((++count) < gv_lock_free::max_try_count);

template < typename T_node_ptr >
struct gvt_lock_free_header
{
	gvt_lock_free_header()
	{
		GV_ASSERT(sizeof(gvt_lock_free_header) == 8);
		GV_ASSERT(!((gv_int_ptr) this % 8));
	}
	gvt_lock_free_header(gv_long l)
	{
		gv_long c = l;
		memcpy(this, &c, sizeof(gv_long));
	}
	T_node_ptr get_header()
	{
		return (T_node_ptr)gv_lock_free::static_to_ptr(head_);
	}
	void set_header(T_node_ptr ptr)
	{
		head_ = gv_lock_free::static_from_ptr(ptr);
	};
	inline T_node_ptr unlink_all()
	{
		T_node_ptr head = NULL;
		int count = 0;
		for (;;)
		{
			// the problem here is the head is not aligned !! .interlock_copy();
			head = get_header();
			gv_int c = counter.current();
			if (!head)
				break;
			gv_int head_int = gv_lock_free::static_from_ptr(head).current();
			if (gv_CAS2(this, head_int, c, 0, c + 1))
				break;
			if ((++count) > gv_lock_free::max_try_count)
			{
				gv_sleep(0);
			};
		}
		return head;
	}
	inline T_node_ptr if_equal_exchange(gv_int compare_value,
										gv_int exchange_value)
	{
		gv_int i = head_.if_equal_exchange(compare_value, exchange_value);
		return (T_node_ptr)gv_lock_free::static_to_ptr(i);
	}
	inline T_node_ptr if_equal_exchange(T_node_ptr compare_value,
										T_node_ptr exchange_value)
	{
		return if_equal_exchange(
			gv_lock_free::static_from_ptr(compare_value).current(),
			gv_lock_free::static_from_ptr(exchange_value).current());
	}
	// http://preshing.com/20120612/an-introduction-to-lock-free-programming/
	// lock free head must be read & save at the same time , otherwise ,can't be
	// 100% safe
	// e.g. c=h.head_; c2=h.counter in 2 state can be completely wrong !!!!
	// problem ? the head->next may become invalid , when the same head is  push
	// and pop quickly!!
	gvt_lock_free_header interlock_copy()
	{
		return gv_atomic_add64((gv_long volatile*)this, 0);
	}

public:
	GVM_ALIGN_VAR(8, gv_atomic_count head_);
	gv_atomic_count counter;
};

template < typename T_node_ptr >
inline void gvt_lock_free_list_push_list(gvt_lock_free_header< T_node_ptr >& list,
										 T_node_ptr node,
										 T_node_ptr node_last)
{

	/*T_node_ptr stale_head;
  gv_int count = 0;
  for (;;)
  {
          stale_head = list.get_header();
          node_last->next = stale_head;
          T_node_ptr cur_value;
          GV_ASSERT(stale_head != node);
          cur_value = list.if_equal_exchange(stale_head, node);
          if (cur_value == stale_head) break;
          if ((++count) > gv_lock_free::max_try_count)
          {
                  gv_thread::sleep(0);
          };
  }
  */
	T_node_ptr head = NULL;
	int count = 0;
	for (;;)
	{
		gvt_lock_free_header< T_node_ptr > h = list.interlock_copy();
		head = h.get_header();
		gv_int counter = h.counter.current();
		node_last->next = head;
		//make sure this list is not a circle
		GV_ASSERT(head != node);
		gv_int head_int = gv_lock_free::static_from_ptr(head).current();
		gv_int next_head_int = gv_lock_free::static_from_ptr(node).current();
		if (gv_CAS2(&list, head_int, counter, next_head_int, counter + 1))
			break;
		if ((++count) > gv_lock_free::max_try_count)
		{
			gv_sleep(0);
		};
	}
}

template < typename T_node_ptr >
inline void gvt_lock_free_list_push(T_node_ptr node,
									gvt_lock_free_header< T_node_ptr >& list)
{
	return gvt_lock_free_list_push_list(list, node, node);
}

template < typename T_node_ptr >
inline T_node_ptr
gvt_lock_free_list_pop_all_reverse(gvt_lock_free_header< T_node_ptr >& list)
{
	T_node_ptr last = list.unlink_all();
	return last;
}

template < typename T_node_ptr >
inline T_node_ptr
gvt_lock_free_list_pop_list(gvt_lock_free_header< T_node_ptr >& list,
							T_node_ptr& first, T_node_ptr& last)
{
	last = list.unlink_all();
	T_node_ptr _last = last;
	first = 0;
	while (_last)
	{
		T_node_ptr tmp = _last;
		_last = _last->next;
		tmp->next = first;
		first = tmp;
	}
	return first;
}

template < typename T_node_ptr >
inline T_node_ptr
gvt_lock_free_list_pop_all(gvt_lock_free_header< T_node_ptr >& list)
{
	T_node_ptr first, last;
	return gvt_lock_free_list_pop_list(list, first, last);
}

template < typename T_node_ptr >
inline T_node_ptr
gvt_lock_free_list_pop_front(gvt_lock_free_header< T_node_ptr >& list)
{
	T_node_ptr head = NULL, next_head = NULL;
	int count = 0;
	for (;;)
	{
		gvt_lock_free_header< T_node_ptr > h = list.interlock_copy();
		head = h.get_header();
		gv_int counter = h.counter.current();
		if (!head)
			break;
		next_head = head->next;
		gv_int head_int = gv_lock_free::static_from_ptr(head).current();
		gv_int next_head_int = gv_lock_free::static_from_ptr(next_head).current();
		if (gv_CAS2(&list, head_int, counter, next_head_int, counter + 1))
			break;
		if ((++count) > gv_lock_free::max_try_count)
		{
			gv_sleep(0);
		};
	}
	return head;
}

template < typename T, class policy_memory >
class gvt_lock_free_list
{
public:
	class node
	{
	public:
		node()
		{
			next = 0;
		}
		T data;
		node* next;

		static policy_memory& memory()
		{
			static policy_memory mem;
			return mem;
		};
		void operator delete(void* p)
		{
			memory().free(p);
		};
		void* operator new(size_t size)
		{
			return memory().alloc(size);
		};
	};

	typedef node type_of_node;

	gvt_lock_free_list()
	{
	}

	T* push()
	{
		node* n = new node;
		push(n);
		return &n->data;
	}

	node* push(const T& data)
	{
		node* n = new node;
		n->data = data;
		push(n);
		return n;
	}

	void push(node* n)
	{
		gvt_lock_free_list_push(n, header);
	}

	void push_list(node* first, node* last)
	{
		gvt_lock_free_list_push_list(header, first, last);
	}

	node* unlink_all(void)
	{
		return gvt_lock_free_list_pop_all(header);
	}

	node* unlink_front(void)
	{
		return gvt_lock_free_list_pop_front(header);
	}

	// alternative interface if ordering is of no importance
	node* unlink_all_reverse(void)
	{
		return header.unlink_all();
	}

	void clear()
	{
		node* last = unlink_all_reverse();
		while (last)
		{
			node* tmp = last;
			last = last->next;
			delete tmp;
		}
	}

protected:
	MS_ALIGN(16)
	gvt_lock_free_header< node* > header;
};
}
