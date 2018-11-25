#pragma once
namespace gv
{
//=================================================================================>
//
//=================================================================================>
class gv_job_base:public gv_refable
{
public:
	gv_job_base(){};
	virtual ~gv_job_base(){};
	virtual void execute(){};
	virtual void execute_1(gv_object*){};
	void* operator new(size_t size);
	void operator delete(void*);
	virtual bool is_task_stream()
	{
		return false;
	}
	gv_job_base* next;
};
typedef gvt_lock_free_header< gv_job_base* > gv_job_list;
//=================================================================================>
// use boost bind ..
//=================================================================================>
template < class T >
class gvt_object_job : public gv_job_base
{
public:
	gvt_object_job(){};
	gvt_object_job(const T& _t)
		: t(_t){};
	~gvt_object_job(){};
	virtual void execute()
	{
		t();
	}

private:
	T t;
};

template < class T >
class gvt_object_job1 : public gv_job_base
{
public:
	gvt_object_job1(){};
	gvt_object_job1(const T& _t)
		: t(_t){};
	~gvt_object_job1(){};
	virtual void execute_1(gv_object* p)
	{
		t(p);
	}

private:
	T t;
};

template < class T, class T2 >
class gvt_object_job_with_ack : public gv_job_base
{
public:
	gvt_object_job_with_ack(){};
	gvt_object_job_with_ack(const T& _t, gv_int _ack_channel, const T2& t2,
							gv_sandbox* sandbox)
		: t(_t)
	{
		ack = new gvt_object_job< T2 >(t2);
		ack_channel = _ack_channel;
		sandbox_hd.init(sandbox);
	};
	~gvt_object_job_with_ack(){};
	virtual void execute()
	{
		t();
		gvt_ref_ptr< gv_sandbox > sandbox = sandbox_hd.get_sandbox();
		if (sandbox)
		{
			sandbox->post_job(ack, ack_channel);
		}
	}

private:
	T t;
	gvt_object_job< T2 >* ack;
	gv_int ack_channel;
	gv_sandbox_handle sandbox_hd;
};
//============================================================================================
//								:
//============================================================================================
template < class T >
inline gvt_object_job< T >* gvt_create_job(const T& t)
{
	return new gvt_object_job< T >(t);
};

template < class T >
inline gvt_object_job1< T >* gvt_create_job1(const T& t)
{
	return new gvt_object_job1< T >(t);
};

template < class T, class T2 >
inline gv_job_base* gvt_create_job_with_ack(const T& t, gv_int _ack_channel,
											const T2& t2, gv_sandbox* sandbox)
{
	return new gvt_object_job_with_ack< T, T2 >(t, _ack_channel, t2, sandbox);
};
}