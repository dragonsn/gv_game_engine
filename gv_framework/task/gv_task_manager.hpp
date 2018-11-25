//#pragma   GV_DISABLE_OPTIMIZATION
namespace gv
{

void  gv_do_one_task()
{
}

class gv_worker_runable : public gv_runnable
{
public:
	gv_worker_runable():m_start_event(true)	
	{	
		m_quit = false;
	}
	void run()
	{
		while (!m_quit)
		{
			//m_start_event.reset();
			m_start_event.wait(); 
			do_work();
		}
	}

	void do_work()
	{
		while (!m_quit)
		{
			gv_job_base * job = m_jobs.unlink_all();
			if (!job) break;
			while (job && !m_quit )
			{ 
				GV_PROFILE_EVENT(do_one_task,0)
				gv_job_base * next = job->next;
				job->execute();
				job->dec_ref();
				job = next;
			}
		}
	}

	void notify()
	{
		m_start_event.set();
	}

	void quit()
	{
		m_quit = true; 
		notify();
	}
	gv_job_list m_jobs;
protected:
	gv_event m_start_event;
	volatile gv_bool m_quit;
};

class gv_task_manager_imp:public gv_refable
{
public:
	gv_task_manager_imp()
		:m_incoming_event(false)
	{
		int nb_thread = get_num_workers();
		m_worker_threads.resize(nb_thread);
		m_worker_runables.resize(nb_thread);
		for (int i = 0; i < nb_thread; i++)
		{
			gv_string_tmp s;
			s << "task_worker_" << i;
			m_worker_threads[i].set_name(gv_id(*s));
			m_worker_threads[i].set_priority(gv_thread::E_PRIO_HIGH);
			m_worker_threads[i].start(&m_worker_runables[i]);
		}
	}

	~gv_task_manager_imp()
	{
		gv_int num_thread = get_num_workers();
		for (int i = 0; i < num_thread; i++)
		{
			m_worker_runables[i].quit();
		}
		for (int i = 0; i < num_thread; i++)
		{
			m_worker_threads[i].join();
		}
	}
	gv_uint get_num_workers()
	{
		//TODO get CPU count 
		return 4;
	}
	
	void distribute_task(gv_task_base * task)
	{
		static gv_uint total_task_count = 1;
		gv_uint num_core = get_num_workers(); 
		for (gv_uint i = 0; i < gvt_min((gv_uint)task->m_task_count-1 , num_core ); i++)
		{
			gv_uint index = ( total_task_count++) % num_core;
			task->inc_ref();
			gvt_lock_free_list_push((gv_job_base*)task, m_worker_runables[index].m_jobs);
			m_worker_runables[index].notify();
			
		}
	}

	gvt_array< gv_thread > m_worker_threads;
	gvt_array< gv_worker_runable > m_worker_runables;
	gv_event m_incoming_event;
};

gv_task_base::gv_task_base()
{
};

gv_task_base::~gv_task_base()
{
};

gv_task_manager::gv_task_manager()
{
	m_task_mgr_impl = new gv_task_manager_imp; 
	GVM_SET_CLASS(gv_task_manager);
};

gv_task_manager::~gv_task_manager()
{
	
	m_quit.set(1);
	signal_incoming_task();
	m_task_mgr_impl = nullptr;
};

bool gv_task_manager::tick(gv_float dt)
{
	bool ret=true; 
	signal_frame_start();
	while (ret)
	{
		wait_for_incoming_task(); 
		if (m_quit.get() ) break;
		ret= super::tick(dt); 
	}
	return false;
};

void gv_task_manager::process_job(gv_job_base* job)
{
	if (job->is_task_stream())
	{
		gvt_ref_ptr< gv_task_base> task = (gv_task_base*)job; 
		m_task_mgr_impl->distribute_task(task); 
		task->execute(); //me help too .
		task->dec_ref();
	}
	else
	{
		super::process_job(job);
	}
};


void gv_task_manager::signal_incoming_task()
{
	m_task_mgr_impl->m_incoming_event.set();
};

void gv_task_manager::wait_for_incoming_task()
{
	m_task_mgr_impl->m_incoming_event.reset();
	m_task_mgr_impl->m_incoming_event.wait();
};

gv_task_manager *  gv_task_manager::static_register(gv_sandbox * sandbox)
{
	sandbox->register_processor(gv_task_manager::static_class(), gv_task_manager::static_channel());
	gv_task_manager* mgr = gvt_cast< gv_task_manager >(sandbox->get_event_processor(gv_task_manager::static_channel()));
	mgr->set_synchronization(false);
	mgr->set_autonomous(true);
	mgr->set_thread_priority(gv_thread::E_PRIO_HIGHEST);
	mgr->wait_signal_frame_start();
	return mgr; 
};

GVM_IMP_CLASS(gv_task_manager,gv_event_processor)
GVM_END_CLASS

}