namespace gv
{
class gv_event_processor_data : public gv_refable
{
public:
	friend class gv_event_processor;
	typedef gvt_lock_free_header< gv_object_event* > event_queue;
	typedef gvt_dlist< gvi_object_event_handler_info* > handler_list;
	typedef gvt_hash_map< gv_id, handler_list, 4096 > named_event_handler_map;
	typedef gvt_hash_map< gv_object_event_id, handler_list, 4096 >
		event_handler_map;

	gv_event_processor_data(gv_event_processor* p)
		: m_runnable(*p, &gv_event_processor::tick_autonomous)
	{
		m_quit = false;
		m_is_autonomous = false;
	}

	named_event_handler_map m_named_event_handler_map;
	event_handler_map m_event_handler_map;
	event_queue m_event_queue;
	gv_mutex m_lock;

	gv_event m_start_tick_event;
	gv_event m_end_tick_event;
	gv_thread m_run_thread;
	gvt_runnable< gv_event_processor > m_runnable;
	gvt_array< gv_event_processor* > m_children;
	gv_job_list m_job_list;
	gv_job_list m_job1_list;
	gv_job_list m_job1_list_for_read;
	gv_bool m_quit;
	gv_bool m_is_autonomous;
};
//============================================================================================
//								:
//============================================================================================
void* gv_object_event::operator new(size_t size)
{
	GV_ASSERT(size < 256);
	return gv_global::pool_256.allocate();
};
void gv_object_event::operator delete(void* p)
{
	gv_global::pool_256.free(p);
};
void* gv_job_base::operator new(size_t size)
{
	GV_ASSERT(size < 256);
	return gv_global::pool_256.allocate();
};
void gv_job_base::operator delete(void* p)
{
	gv_global::pool_256.free(p);
};
//============================================================================================
//								:
//============================================================================================
gv_event_processor::gv_event_processor()
{
	link_class(gv_event_processor::static_class());
	m_default_channel = 0;
	m_pimpl = new gv_event_processor_data(this);
	m_synchronized = false;
	m_paused = false;
	m_last_tick_time = 0;
	m_delta_time = 0;
	m_synchronize_continue = true;
	m_use_fixed_delta_time = false;
	m_default_channel = m_base_channel = m_current_channel = 0;
	m_fixed_delta_time = 0.4f;
	m_tick_count = 0;
}

gv_event_processor::~gv_event_processor()
{
	this->set_autonomous(false);
	gvt_safe_delete(m_pimpl);
}

bool gv_event_processor::tick(gv_float dt)
{
	// default do nothing ,but process the message.
	GV_PROFILE_EVENT(gv_event_processor_tick, (gv_int)(dt * 1000));
	this->flush_events();
	this->flush_jobs();
	this->flush_jobs1();
	
	if (m_base_channel)
	{
		// give the slave a break;
		gv_thread::sleep(1);
	}
	m_tick_count++;
	// for children
	if (this->m_pimpl->m_children.size())
	{
		for (int i = 0; i < this->m_pimpl->m_children.size(); i++)
		{
			gv_event_processor* p = this->m_pimpl->m_children[i];
			if (!p->tick_as_child(dt))
				return false;
		}
	}
	return true;
};

bool gv_event_processor::tick_as_child(gv_float dt)
{
	return true;
}
bool gv_event_processor::is_autonomous()
{
	return m_pimpl->m_is_autonomous;
};

bool gv_event_processor::set_autonomous(bool enable)
{
	if (enable == is_autonomous())
		return false;
	if (is_autonomous())
	{
		if (m_pimpl->m_run_thread.is_running())
		{
			m_pimpl->m_quit = true;
			this->signal_frame_start();
			m_pimpl->m_run_thread.join();
		}
	}
	else
	{
		m_pimpl->m_quit = false;
		gv_string_tmp s;
		s << this->get_name();
		this->m_pimpl->m_run_thread.set_name(gv_id(*s));
		this->m_pimpl->m_run_thread.start(&this->m_pimpl->m_runnable);
	}
	this->m_pimpl->m_is_autonomous = enable;
	return true;
};

void gv_event_processor::tick_autonomous()
{
	while (1)
	{
		gv_float real_delta_time = 0;
		m_last_tick_time = get_sandbox()->get_time_in_seconds();
		if (is_synchronized())
		{
			GV_PROFILE_EVENT(gv_event_processor_wait_start, 0)
			this->m_pimpl->m_start_tick_event.wait();
		}
		if (m_pimpl->m_quit)
			break;

		real_delta_time =
			(gv_float)(get_sandbox()->get_time_in_seconds() - m_last_tick_time);
		m_delta_time = corrected_delta_time(real_delta_time);
		if (!this->tick(m_delta_time))
		{
			// should send a message;
			this->m_synchronized = true;
			this->m_synchronize_continue = false;
			this->m_pimpl->m_end_tick_event.set();
			return;
		};
		double cu_time = get_sandbox()->get_time_in_seconds();
		if (m_use_fixed_delta_time)
		{
			// real_delta_time < (this->m_fixed_delta_time-0.001f)
			long sleep_time = (long)((this->m_fixed_delta_time -
									  (float)(cu_time - m_last_tick_time)) *
									 1000);
			if (sleep_time > 0)
			{
				gv_thread::sleep(gvt_min< long >(sleep_time, 1000));
			}
		}
		if (is_synchronized())
		{
			this->m_pimpl->m_end_tick_event.set();
		}
	}
};

void gv_event_processor::signal_frame_start()
{
	this->m_pimpl->m_start_tick_event.set();
};

void gv_event_processor::wait_signal_frame_end()
{
	if (m_synchronize_continue)
		this->m_pimpl->m_end_tick_event.wait();
};

void gv_event_processor::wait_signal_frame_start()
{
	this->m_pimpl->m_start_tick_event.wait();
};

bool gv_event_processor::do_synchronization()
{
	return m_synchronize_continue;
};

gv_int gv_event_processor::on_event(gv_object_event* event)
{
	return 0;
}
bool gv_event_processor::process_event(gv_object_event* event)
{
	if (this->on_event(event))
	{
		if (!event->is_mute())
		{
			GVM_DEBUG_LOG(event, "process_event id["
									 << event->m_id << "]"
									 << "name [" << event->get_id_string()
									 << "] by :" << this->get_name() << gv_endl)
		}
		return true;
	}
	if (this->m_pimpl->m_children.size())
	{
		for (int i = 0; i < this->m_pimpl->m_children.size(); i++)
		{
			gv_event_processor* p = this->m_pimpl->m_children[i];
			GVM_DEBUG_LOG(event, "process_event id["
									 << event->m_id << "]"
									 << "name [" << event->get_id_string()
									 << "] by :" << p->get_name() << gv_endl)
			if (p->on_event(event))
				return true;
		}
	}
#pragma GV_REMINDER( \
	"TO_REMOVE:this is older way to register new event handler (listener) , but too complicated for some simple usable.")
	if (event->m_id == gv_object_event_id_named)
	{
		return this->process_named_event(event);
	}
	gv_event_processor_data::handler_list* plist;
	plist = m_pimpl->m_event_handler_map.find(event->m_id);
	if (!plist)
	{
		GVM_DEBUG_LOG(main, "[warning ]not handled event! "
								<< event->m_id <<":"<< event->get_id_string()<< gv_endl);
		return false;
	}
	gv_event_processor_data::handler_list::iterator pit;
	pit = plist->begin();
	bool is_broadcast = !event->m_reciever;
	while (pit != plist->end())
	{
		gvi_object_event_handler_info* prec = (*pit);
		if (is_broadcast || event->m_reciever.ptr() == prec->get_handler())
		{

			if (!event->is_mute()) // hide ui event
			{
				if (prec->handler_is_object())
					GVM_DEBUG_LOG(event,
								  "process_event id["
									  << event->m_id << "]"
									  << "name [" << event->get_id_string() << "] by :"
									  << prec->get_handler()->get_name() << gv_endl)
				else
					GVM_DEBUG_LOG(event,
								  "process_event id["
									  << event->m_id << "]"
									  << "name [" << event->get_id_string() << "] by :"
									  << (gv_int_ptr)prec->get_handler() << gv_endl)
			}
			prec->process(event);
		}
		++pit;
	};
	return true;
};

bool gv_event_processor::process_named_event(gv_object_event* event)
{
	GV_ASSERT(event->m_id == gv_object_event_id_named);
	gv_event_processor_data::handler_list* plist;
	plist = m_pimpl->m_named_event_handler_map.find(event->m_name);
	if (!plist)
	{
		GVM_DEBUG_LOG(main, "[warning] ]not handled event! "
								<< event->m_id << event->m_name << gv_endl);
		return false;
	}
	gv_event_processor_data::handler_list::iterator pit;
	pit = plist->begin();
	bool is_broadcast = !event->m_reciever;
	while (pit != plist->end())
	{
		gvi_object_event_handler_info* prec = (*pit);
		if (is_broadcast || event->m_reciever.ptr() == prec->get_handler())
		{
			if (prec->handler_is_object())
				GVM_DEBUG_LOG(event, "process_named_event id["
										 << event->m_id << "]"
										 << "name [" << event->get_id_string()
										 << "] by :" << prec->get_handler()->get_name()
										 << gv_endl)
			else
				GVM_DEBUG_LOG(event, "process_named_event id["
										 << event->m_id << "]"
										 << "name [" << event->get_id_string()
										 << "] by :" << (gv_int_ptr)prec->get_handler()
										 << gv_endl)
			prec->process(event);
		}
		++pit;
	};
	return true;
};

bool gv_event_processor::post_event(gv_object_event* event)
{
	gvt_lock_free_list_push(event, m_pimpl->m_event_queue);
	return true;
};

void gv_event_processor::post_job(gv_job_base* job)
{
	gvt_lock_free_list_push(job, m_pimpl->m_job_list);
};

void gv_event_processor::post_job1(gv_job_base* job)
{
	gvt_lock_free_list_push(job, m_pimpl->m_job1_list);
};

gv_job_base* gv_event_processor::pop_job1()
{
	gv_job_base* pjob =
		gvt_lock_free_list_pop_front(m_pimpl->m_job1_list_for_read);
	return pjob;
};

void gv_event_processor::process_job(gv_job_base* job)
{
	job->execute();
};

void gv_event_processor::flush_jobs()
{
	gvt_ref_ptr<gv_job_base> p = gvt_lock_free_list_pop_all(m_pimpl->m_job_list);
	gv_job_base* tmp;
	while (p)
	{
		process_job(p);
		tmp = p->next;
		p = tmp;
	}
};

void gv_event_processor::flush_jobs1()
{
	gv_event_processor* job_source = this;
	if (m_base_channel)
	{
		job_source = get_sandbox()->get_event_processor(m_base_channel);
	}
	else
	{
		// i am job source
		gv_job_base *first, *last;
		gvt_lock_free_list_pop_list(m_pimpl->m_job1_list, first, last);
		if (first)
			gvt_lock_free_list_push_list(m_pimpl->m_job1_list_for_read, first, last);
	}
	if (job_source)
	{
		gv_job_base* pjob = job_source->pop_job1();
		while (pjob)
		{
			pjob->execute_1(this);
			delete pjob;
			pjob = job_source->pop_job1();
		}
	}
}

void gv_event_processor::flush_events()
{
	gv_object_event* it = gvt_lock_free_list_pop_all(m_pimpl->m_event_queue);
	while (it)
	{
		GV_ASSERT(it);
		this->process_event(it);
		gv_object_event* bak = it;
		it = it->next;
		delete bak;
	}
};

void gv_sandbox_bind_event_hander(gv_event_processor_data::handler_list* plist,
								  gvi_object_event_handler_info* prec)
{
	gv_event_processor_data::handler_list::iterator it = plist->begin();
	gv_event_processor_data::handler_list::iterator it_end = plist->end();
	gv_event_processor_data::handler_list::iterator it2;
	while (it != it_end)
	{
		if ((*it)->get_handler() == prec->get_handler())
		{
			GVM_DEBUG_LOG(main, "already registered the event!"
									<< "with object "
									<< (gv_int_ptr)prec->get_handler() << "\r\n");
			(*it)->release();
			it2 = it;
			++it;
			plist->erase(it2);
			continue;
		}
		++it;
	}
	plist->push_back(prec);
}

void gv_sandbox_unbind_event_hander(
	gv_event_processor_data::handler_list* plist, gv_object* object)
{
	gv_event_processor_data::handler_list::iterator it = plist->begin();
	gv_event_processor_data::handler_list::iterator it_end = plist->end();
	gv_event_processor_data::handler_list::iterator it2;
	while (it != it_end)
	{
		if ((*it)->get_handler() == object)
		{
			(*it)->release();
			it2 = it;
			++it;
			plist->erase(it2);
			continue;
		}
		++it;
	}
}

void gv_event_processor::register_event(gv_object_event_id id,
										gvi_object_event_handler_info* prec)
{
	gv_event_processor_data::handler_list* plist =
		m_pimpl->m_event_handler_map.find(id);
	if (!plist)
		plist = m_pimpl->m_event_handler_map.add(
			id, gv_event_processor_data::handler_list());
	gv_sandbox_bind_event_hander(plist, prec);
};

void gv_event_processor::unregister_event(gv_object_event_id id,
										  gv_object* object)
{
	gv_event_processor_data::handler_list* plist =
		m_pimpl->m_event_handler_map.find(id);
	GV_ASSERT(plist);
	gv_sandbox_unbind_event_hander(plist, object);
	if (plist->is_empty())
		m_pimpl->m_event_handler_map.erase(id);
};

void gv_event_processor::register_named_event(
	gv_id id, gvi_object_event_handler_info* prec)
{
	gv_event_processor_data::handler_list* plist =
		m_pimpl->m_named_event_handler_map.find(id);
	if (!plist)
		plist = m_pimpl->m_named_event_handler_map.add(
			id, gv_event_processor_data::handler_list());
	gv_sandbox_bind_event_hander(plist, prec);
};

void gv_event_processor::unregister_named_event(gv_id id, gv_object* object)
{
	gv_event_processor_data::handler_list* plist =
		m_pimpl->m_named_event_handler_map.find(id);
	GV_ASSERT(plist);
	gv_sandbox_unbind_event_hander(plist, object);
	if (plist->is_empty())
		m_pimpl->m_named_event_handler_map.erase(id);
};

void gv_event_processor::set_thread_priority(gv_thread::priority p)
{
	m_pimpl->m_run_thread.set_priority(p);
};

void gv_event_processor::add_child(gv_event_processor* child)
{
	m_pimpl->m_children.add_unique(child);
};
void gv_event_processor::remove_child(gv_event_processor* child)
{
	m_pimpl->m_children.remove(child);
};

//-------------------------------------------------------------
GVM_IMP_CLASS(gv_event_processor, gv_object)
GVM_VAR(gv_ushort, m_default_channel)
GVM_END_CLASS
}