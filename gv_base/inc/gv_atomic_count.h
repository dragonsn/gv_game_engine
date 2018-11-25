#pragma once

namespace gv
{
class gv_atomic_count
{

public:
	inline gv_atomic_count()
		: value(0)
	{
		GV_ASSERT(gvt_is_aligned(this, 4));
	};
	inline gv_atomic_count(gv_int i)
		: value(i)
	{
		GV_ASSERT(gvt_is_aligned(this, 4));
	};
	inline gv_atomic_count(const gv_atomic_count& c)
	{
		GV_ASSERT(gvt_is_aligned(this, 4));
		*this = c;
	}
	// return the initial value before add
	inline gv_int add_atomic(gv_int i)
	{
		return gv::gv_atomic_add(&value, i);
	}
	inline gv_int set(gv_int i)
	{
		return gv::gv_atomic_exchange(&value, i);
	};
	inline gv_int get() const
	{
		return ((gv_atomic_count*)this)->add_atomic(0);
	}
	inline gv_int exchange(gv_int i)
	{
		return gv::gv_atomic_exchange(&value, i);
	}
	inline gv_int operator++() // preincrement
	{
		// The function returns the resulting incremented value.
		return gv::gv_atomic_increment(&value);
	}
	inline gv_int operator++(int) // postincrement
	{
		return gv::gv_atomic_increment(&value) - 1;
	}
	inline gv_int operator--() // predecrement
	{
		// The function returns the resulting decremented value.
		return gv::gv_atomic_decrement(&value);
	}
	inline gv_int operator--(int) // postdecrement
	{
		return gv::gv_atomic_decrement(&value) + 1;
	}
	inline gv_int operator=(const gv_atomic_count& c)
	{
		set(c.value);
		return value;
	}
	inline gv_int if_equal_exchange(gv_int compare_value, gv_int exchange_value)
	{
		return gv::gv_atomic_if_equal_exchange(&value, compare_value,
											   exchange_value);
	}
	template < class visitor >
	inline gv_int interlock_any(visitor& v)
	{
		gv_int old_value = value;
		gv_int cur_value = old_value;
		gv_int new_value;
		for (;;)
		{
			// calculate the function
			new_value = v(old_value);
			// set the new value if the current value is still the expected one
			cur_value = if_equal_exchange(old_value, new_value);
			// we found the expected value: the exchange happened
			if (cur_value == old_value)
				break;
			// recalculate the function on the unexpected value
			old_value = cur_value;
		} // success
		return old_value;
	}

	// for ring buffer.
	int add_and_round(gv_int _step, gv_int _max)
	{
		gvf_add_and_round< gv_int > v(_step, _max);
		return this->interlock_any(v);
	}
	gv_int current() const
	{
		return value;
	}
	gv_int& to_int()
	{
		return *(gv_int*)(&value);
	}

private:
	volatile gv_int value;
};

template < class T >
class gvt_atomic
{
public:
	// this not work ? GV_STATIC_ASSERT(sizeof(T) == 4);
	gvt_atomic()
	{
		GV_ASSERT(sizeof(T) == 4);
	}
	gvt_atomic(const T& t)
	{
		count.set((gv_int)t);
	}
	inline T set(T i)
	{
		return (T)count.set((size_t)i);
	};
	inline T get() const
	{
		return (T)count.get();
	}
	inline T exchange(T i)
	{
		return (T)count.exchange((gv_int)i);
	}
	inline T operator=(const gvt_atomic< T >& c)
	{
		count = c.count;
		return (T)count;
	}
	inline T if_equal_exchange(T compare_value, T exchange_value)
	{
		return (T)count.if_equal_exchange((size_t)compare_value,
										  (size_t)exchange_value);
	}
	gv_atomic_count count;
};

GV_STATIC_ASSERT(sizeof(gv_atomic_count) == 4);
}