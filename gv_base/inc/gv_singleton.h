
#pragma once
namespace gv
{
template < typename T >
class gvt_singleton
{
public:
	typedef T type_of_instance;
	typedef gvt_factory< T > type_of_factory;
	static T& instance()
	{
		return *get_ptr();
	};
	static void destroy()
	{
		T*& p = get_ptr();
		if (factory())
		{
			factory()->destroy(p);
			// gvt_safe_delete(factory());
		}
		else
			gvt_safe_delete(p);
		p = NULL;
	}
	static void lock()
	{
		instance.lock();
	};
	static void unlock()
	{
		instance.unlock();
	}
	static void set_factory(type_of_factory* f)
	{
		if (f)
		{
			// GV_ASSERT(factory()==NULL);
			factory() = f;
		}
	}
	static void init(T* t)
	{
		T*& p = get_ptr();
		destroy();
		p = t;
	}

protected:
	gvt_singleton(){};
	static T*& get_ptr(bool create = true)
	{
		static T* s_instance = NULL;
		if (!s_instance && create)
		{
			if (factory())
			{
				s_instance = factory()->create();
			}
			else
				s_instance = new T();
		}
		return s_instance;
	}
	static type_of_factory*& factory()
	{
		static type_of_factory* s_factory = NULL;
		return s_factory;
	}

private:
	// Prohibited actions...this does not prevent hijacking.
	gvt_singleton(const gvt_singleton& s){};
	gvt_singleton& operator=(const gvt_singleton&){};
	// gvt_singleton Helpers
	// data structure
};

template < typename T >
class gvt_global : public gv_empty_class_with_virtual
{
public:
	typedef gvt_singleton< T > type_of_singleton;

	gvt_global()
	{
		ptr = NULL;
	};
	virtual ~gvt_global()
	{
		destroy();
	};
	T* operator->()
	{
		return get();
	}
	void destroy()
	{
		if (ptr)
		{
			type_of_singleton::destroy();
		}
		ptr = NULL;
	}
	T& operator*()
	{
		return (*get());
	}
	T* get()
	{
		if (!ptr)
		{
			ptr = &type_of_singleton::instance();
		}
		return ptr;
	}
	T* try_get()
	{
		return ptr;
	}
	void set(T* p)
	{
		ptr = p;
		type_of_singleton::init(p);
	}
	void lock()
	{
		type_of_singleton::lock();
	};
	void unlock()
	{
		type_of_singleton::unlock();
	}

protected:
	T* ptr;
};
}
