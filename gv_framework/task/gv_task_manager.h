#pragma once
namespace gv
{

	class gv_task_base : public gv_job_base
	{
	public:
		gv_task_base();
		~gv_task_base();
		virtual bool is_task_stream()
		{
			return true;
		}
		bool finish_one_job()
		{
			gv_int finished_task = m_finished_count.add_atomic(1)+1;
			return finished_task == m_task_count; 
		}
		bool is_done()
		{
			return m_task_count == m_finished_count.get();
		}
		gv_int m_task_count;
		gv_atomic_count m_finished_count;
		gv_atomic_count m_started_count;
		gvt_ptr<gv_event> m_finish_event;
		gvt_ref_ptr<gv_task_base> m_prerequisite;
	};

	template < class type_of_functor, typename type_of_object >
	class gvt_task_object_stream : public gv_task_base
	{
	public:
		gvt_task_object_stream() {};
		gvt_task_object_stream(const type_of_functor& _func, type_of_object ** first_object,
			gv_int count = 1, gv_event * event = nullptr)
			: m_func(_func)
		{
			m_task_count = count;
			m_started_count = 0;
			m_finished_count = 0;
			GV_ASSERT(first_object);
			m_object_pointer_array.init(first_object, count);
			m_finish_event = event;
		};

		virtual void execute()
		{
			if (m_prerequisite && !m_prerequisite->is_done())
			{
				m_prerequisite->execute();
			}
			m_prerequisite = NULL;
			type_of_object * object = get_unstarted();
			while (object)
			{
				m_func(object);
				bool all_is_done=finish_one_job(); 
				if (all_is_done && m_finish_event)
				{
					m_finish_event->set();
					break;
				}
				object = get_unstarted();
			}
		}

		type_of_object * get_unstarted()
		{
			gv_int index = m_started_count++;
			if (index < m_task_count)
			{
				return m_object_pointer_array[index];
			}
			return nullptr;
		}

	private:
		boost::function< void(type_of_object * ) > m_func;
		gvt_array< type_of_object *> m_object_pointer_array;
	};

	template < class type_of_functor >
	class gvt_task_functor_loop : public gv_task_base
	{
	public:
		gvt_task_functor_loop() {};
		gvt_task_functor_loop(const type_of_functor& _func, gv_int loop_count = 1, gv_event * event = nullptr)
			: m_func(_func)
		{
			m_task_count = loop_count;
			m_started_count = 0;
			m_finished_count = 0;
			m_finish_event = event;
		};

		virtual void execute()
		{
			if (m_prerequisite && !m_prerequisite->is_done())
			{
				m_prerequisite->execute();
			}
			m_prerequisite = NULL;
			gv_int index = m_started_count++;
			while (index < m_task_count)
			{
				m_func(index);
				bool all_is_done=finish_one_job(); 
				if (all_is_done && m_finish_event)
				{
					m_finish_event->set();
					break;
				}
				index = m_started_count++;
			}
		}

	private:
		boost::function< void(gv_int) > m_func;
	};

	class gv_task_manager_imp;
	class gv_task_manager : public gv_event_processor
	{
	public:
		GVM_DCL_CLASS(gv_task_manager, gv_event_processor)
		gv_task_manager();
		~gv_task_manager();
		virtual bool tick(gv_float dt);
		virtual void process_job(gv_job_base* job);
		static gve_event_channel static_channel()
		{
			return gve_event_channel_task_queue;
		}
		static gv_task_manager * static_register(gv_sandbox * sandbox);

		template<class type_of_functor, class type_of_object>
		void parallel_process_stream(const type_of_functor& func, type_of_object ** first_object, gv_int count)
		{
			GV_PROFILE_EVENT(parallel_process_stream, 0);
			gvt_ref_ptr < gvt_task_object_stream<type_of_functor, type_of_object> >task;
			task = new gvt_task_object_stream<type_of_functor, type_of_object>(func, first_object, count);
			task->inc_ref();
			this->post_job(task);
			this->signal_incoming_task();
			task->execute();
		}


		template<class type_of_functor>
		void parallel_for(const type_of_functor& func, gv_int count)
		{
			GV_PROFILE_EVENT(parallel_for, 0);
			gvt_ref_ptr < gvt_task_functor_loop<type_of_functor> > task;
			task = new gvt_task_functor_loop<type_of_functor>(func,  count);
			task->inc_ref();
			this->post_job(task);
			this->signal_incoming_task();
			task->execute();
		}

		void signal_incoming_task(); 
		void wait_for_incoming_task();

	public:
		gvt_ref_ptr< gv_task_manager_imp > m_task_mgr_impl;
		gv_atomic_count m_quit;
	};



}
