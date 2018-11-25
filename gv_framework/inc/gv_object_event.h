#pragma once
namespace gv
{
typedef gv_ushort gv_object_event_id;
class gv_object;
class gv_func_info;

static const gv_ushort gvc_invalid_object_event_id = 0xffff;
static const gv_ushort gv_object_event_id_named =
	0; // reserved id for name event , check name for real event id
static const gv_ushort gv_object_event_id_first =
	1; // reserved id for the first event id , don't use, only for test

#define GV_REGISTER_EVENT_ID(name, num)                     \
	static const gv_ushort gv_object_event_id_##name = num; \
	static const gv_ushort GV_MACRO_CONCATE(gv_object_event_, num) = num;

//=================================================================================>
//	the event used to communicate between objects locally or remotely
//	it's very important for a remote game editor or multilayer game
//=================================================================================>
static const int gvc_max_event_size = 256;
class gv_object_event : public gv_refable_with_factory
{
public:
	gv_object_event()
	{
		m_flag = 0;
		m_id = gvc_invalid_object_event_id;
	}
	virtual ~gv_object_event(){};

	bool is_valid();
	void init(){};
	virtual const char* get_id_string()
	{
		if (m_id == gv_object_event_id_named)
			return *m_name.string();
		else
			return "gv_object_event";
	}
	virtual bool is_mute()
	{
		return false;
	}
	//============================================================================================
	//								:
	//============================================================================================
	void* operator new(size_t size);
	void operator delete(void*);
	//============================================================================================
	//								:
	//============================================================================================
	gv_object_event_id m_id;
	gv_id m_name; // will be use , if is a named event
	gvt_ref_ptr< gv_object > m_sender;
	gvt_ref_ptr< gv_object > m_reciever;
	gv_uint m_flag;

	gv_object_event* next;
};

template < class type_of_event >
inline type_of_event* gvt_cast(gv_object_event* pevent)
{
	if (pevent && pevent->m_id == type_of_event::static_event_id())
		return (type_of_event*)pevent;
	return NULL;
}
//=================================================================================>
// info used to register corresponding handler & event pair;
//=================================================================================>
class gvi_object_event_handler_info
{
public:
	gvi_object_event_handler_info(){};
	virtual ~gvi_object_event_handler_info(){};
	virtual void release()
	{
		delete this;
	};
	virtual gv_int process(gv_object_event* pevent) = 0;
	virtual gv_object* get_handler() = 0;
	virtual bool handler_is_object()
	{
		return true;
	};
};
//=================================================================================>
// hook the event with a c++ function
//=================================================================================>

template < class T >
class gv_object_event_handler_c : public gvi_object_event_handler_info
{
public:
	typedef gv_int (T::*gv_event_callback)(gv_object_event* event);

	gv_object_event_handler_c(){};
	~gv_object_event_handler_c(){};
	gv_object_event_handler_c(T* p, gv_event_callback func)
	{
		m_func = func;
		m_obj = p;
		m_is_object = (boost::is_base_of< gv_object, T >::value);
	}
	virtual gv_int process(gv_object_event* pevent)
	{
		return (m_obj->*m_func)(pevent);
	}
	virtual gv_object* get_handler()
	{
		return m_is_object ? (gv_object*)m_obj : NULL;
	}
	virtual bool handler_is_object()
	{
		return m_is_object;
	};

private:
	gv_event_callback m_func;
	T* m_obj;
	bool m_is_object;
};
//=================================================================================>
// hook the event with a script function
//=================================================================================>
class gv_object_event_handler_script : public gvi_object_event_handler_info
{
private:
	gv_object_event_handler_script();
	~gv_object_event_handler_script();

public:
	virtual gv_int process(gv_object_event* pevent);
	virtual gv_object* get_handler()
	{
		return m_obj;
	}

protected:
	gv_func_info* m_func;
	gv_object* m_obj;
};

#include "gv_framework_event_ids.h"
};