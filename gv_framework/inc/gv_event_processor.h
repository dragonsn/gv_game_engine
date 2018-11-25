#pragma once
namespace gv
{
class gv_event_processor_data;

//============================================================================================
//		the processor to recieve event and process
//:
//============================================================================================
class gv_event_processor : public gv_object
{
	friend class gv_sandbox;
	friend class gv_sandbox_data;

public:
	GVM_DCL_CLASS(gv_event_processor, gv_object);
	gv_event_processor();
	~gv_event_processor();

public:
	inline gv_uint get_tick_count()
	{
		return m_tick_count;
	};
	inline bool is_paused()
	{
		return m_paused;
	}
	inline void pause(bool enable = true)
	{
		m_paused = enable;
	}
	inline bool is_fixed_delta_time()
	{
		return m_use_fixed_delta_time;
	}
	// TO fix:now only take effect when in another thread
	inline void set_fixed_delta_time(bool enable, gv_float dt = 0.03f)
	{
		m_use_fixed_delta_time = true;
		m_fixed_delta_time = m_delta_time = dt;
	};
	inline gv_float corrected_delta_time(gv_float delta_time)
	{
		gv_float dt = is_paused() ? 0 : is_fixed_delta_time() ? m_fixed_delta_time
															  : delta_time;
		dt = gvt_clamp(dt, 0.f, 0.1f);
		return dt;
	}
	bool is_autonomous();
	bool set_autonomous(bool enable = true);
	void tick_autonomous();
	void set_thread_priority(gv_thread::priority p);
	void post_job(gv_job_base* job);
	//FIX me: this is just bad name , see macro GVM_POST_JOB1 for detail , 
	//pass the processor as parameter, used for worker thread queue
	void post_job1(gv_job_base* job); 
	void add_child(gv_event_processor* child);
	void remove_child(gv_event_processor* child);
	inline void set_base_channel(gv_ushort channel)
	{
		m_base_channel = channel;
	};
	inline gv_ushort get_base_channel()
	{
		return m_base_channel;
	}
	inline void set_current_channel(gv_ushort channel)
	{
		m_current_channel = channel;
	};
	inline gv_ushort get_current_channel()
	{
		return m_current_channel;
	}
	inline bool is_synchronized()
	{
		return m_synchronized;
	}
	inline void set_synchronization(bool enable = true)
	{
		m_synchronized = enable;
		m_synchronize_continue = true;
	}
	virtual void signal_frame_start();
	virtual bool execute(const char* cmd)
	{
		return false;
	};
	gve_event_channel static_channel()
	{
		return gve_event_channel_default;
	}
protected:
	// functions need to be derived
	virtual bool tick(gv_float dt);
	virtual bool tick_as_child(gv_float dt);
	virtual bool do_synchronization();
	virtual void register_events(){};
	virtual void unregister_events(){};
	virtual void wait_signal_frame_end();
	virtual void wait_signal_frame_start();
protected:
	// by default all the event send to processor will be processed by this
	// function first., return true means processed, no need to continue.
	virtual gv_int on_event(gv_object_event* event);
	virtual void process_job(gv_job_base* job);
	gv_job_base* pop_job1();
	void flush_jobs();
	void flush_jobs1();
	bool process_event(gv_object_event* event);
	bool post_event(gv_object_event* event);
	bool process_named_event(gv_object_event* event);
	void flush_events();
	// the register event  function s are not thread safe, it must be called in
	// the sandbox main thread.
	void register_event(gv_object_event_id id,
						gvi_object_event_handler_info* handler);
	void unregister_event(gv_object_event_id id, gv_object* object);
	void register_named_event(gv_id id, gvi_object_event_handler_info* handler);
	void unregister_named_event(gv_id id, gv_object* object);

protected:
	gv_ushort m_default_channel;
	gv_ushort m_base_channel;
	gv_ushort m_current_channel;
	gv_bool m_synchronized;
	gv_bool m_paused;
	gv_bool m_synchronize_continue;
	gv_bool m_use_fixed_delta_time;
	gv_double m_last_tick_time;
	gv_float m_delta_time;
	gv_float m_fixed_delta_time;
	gv_uint m_tick_count;
	gvt_ptr< gv_event_processor_data > m_pimpl;
};
}