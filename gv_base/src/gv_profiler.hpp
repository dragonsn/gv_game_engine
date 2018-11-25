#include "gv_base_internal.h"

#define DEBUG_CALL_GRAPH_SLOW \
	0 // turn it on for android & other platform that can't debug

namespace gv
{

class gv_time_mark
{
public:
	gv_time_mark()
	{
		GVM_ZERO_ME;
	}
	enum type
	{
		e_nop,
		e_profile_start,
		e_profile_end,
		e_tracking_float,
		e_tracking_int,
	};
	gv_ulong m_time_stamp;
	gv_id m_thread_id;
	const char* m_marker_name;
	union {
		struct
		{
			gv_uint m_user_data_uints[3];
			type m_type;
		};
		gv_int m_user_data_ints[3];
		gv_float m_user_data_floats[3];
		gv_double m_user_data_double;
		gv_long m_user_data_long;
		gv_ulong m_user_data_ulong;
	};
};

#pragma GV_REMINDER( \
	"[gv_profiler_marker]the index parameter is used for dynamic part of the name information,e.g. the index of object")
static const int gv_profiler_version = 1;
gv_int s_event_stack_depth = 0;
gv_int& static_event_stack_depth()
{
	if (gv_thread::current())
	{
		return gv_thread::current()->get_event_stack_depth();
	}
	return s_event_stack_depth;
}

gv_profiler_marker::gv_profiler_marker(const char* name, int index)
{
	m_name = NULL;
	m_stack_depth = static_event_stack_depth()++;
	if (!name)
		return;
	gv_time_mark* pmark = gv_global::profiler->alloc();
	if (!pmark)
		return;
	m_name = name;
	m_index = index;
	pmark->m_marker_name = m_name;
	pmark->m_user_data_ints[0] = m_index;
	pmark->m_user_data_ints[1] = m_stack_depth;
	pmark->m_type = gv_time_mark::e_profile_start;
#if DEBUG_CALL_GRAPH_SLOW
	{
		gv_string_tmp s;
		s << "[" << pmark->m_thread_id << "]";
		for (int i = 0; i < m_stack_depth; i++)
			s << "\t";
		s << "|----------->>" << m_name << " [STARTED] ";
		GVM_DEBUG_OUT(s);
	}
#endif
};

gv_profiler_marker::~gv_profiler_marker()
{
	if (!m_name)
		return;
	gv_time_mark* pmark = gv_global::profiler->alloc();
	if (!pmark)
		return;
	pmark->m_marker_name = m_name;
	pmark->m_user_data_ints[0] = m_index;
	pmark->m_user_data_ints[1] = m_stack_depth;
	pmark->m_type = gv_time_mark::e_profile_end;
	static_event_stack_depth()--;
#if DEBUG_CALL_GRAPH_SLOW
	{
		gv_string_tmp s;
		s << "[" << pmark->m_thread_id << "]";
		for (int i = 0; i < m_stack_depth; i++)
			s << "\t";
		s << "<<------------|" << m_name << " [FINISHED] ";
		GVM_DEBUG_OUT(s);
	}
#endif
};

class gv_profiler_manager_imp
{
protected:
	friend class gv_profiler_manager;
	gv_profiler_manager_imp(int nb_marks)
	{
		m_mark_ring_buffer = new gv_time_mark[nb_marks];
		m_mark_ring_buffer_size = nb_marks;
		m_is_dumping = false;
	};
	~gv_profiler_manager_imp()
	{
		delete[] m_mark_ring_buffer;
	};
	gv_time_mark* alloc(bool wait)
	{
		while (m_is_dumping)
		{
			if (wait)
				gv_thread::sleep(100);
			else
				return NULL;
		};
		int index = m_current_free.add_and_round(1, m_mark_ring_buffer_size);
		gv_time_mark* p = m_mark_ring_buffer + index;
		gv_thread* ps = gv_thread::current();
		if (ps)
			p->m_thread_id = ps->get_name();
		else
		{
			p->m_thread_id = gv_id_main;
		}
		p->m_time_stamp = gv_global::time->get_performance_counter_from_start();
		return p;
	};

	// write all the event in ring buffer to log
	template < class type_of_stream >
	void dump_mark(gv_time_mark& mark, type_of_stream* ps)
	{
		//"<Item priority=\"1\" distance='close'> Go to the <bold>Toy
		//store!</bold></Item>"
		if (mark.m_type == gv_time_mark::e_nop)
			return;
		gvt_xml_write_element_open(*ps, "mark", false);
		gvt_xml_write_attribute(*ps, "time", mark.m_time_stamp);
		gvt_xml_write_attribute(*ps, "thread", *mark.m_thread_id.string());
		gvt_xml_write_attribute(*ps, "type", mark.m_type);
		gvt_xml_write_attribute(*ps, "name", mark.m_marker_name);
		gvt_xml_write_attribute(*ps, "user_data0", mark.m_user_data_uints[0]);
		gvt_xml_write_attribute(*ps, "user_data1", mark.m_user_data_uints[1]);
		gvt_xml_write_attribute(*ps, "user_data2", mark.m_user_data_uints[2]);
		gvt_xml_write_element_close(*ps);
	}

	void dump_snap_shot(const char* prefix)
	{
		m_is_dumping = true;
		{
			gv_thread_lock lock(this->m_mutex);
			int event_count = 0;
			for (int i = 0; i < this->m_mark_ring_buffer_size; i++)
				if (this->m_mark_ring_buffer[i].m_type != gv_time_mark::e_nop)
					event_count++;
			gv_string file_name;
			file_name << prefix << gv_global::time->get_local_time_string() << ".xml";
			file_name.replace_all(":", "_");
			file_name.replace_all(" ", "@");
			gvi_stream* pss = gv_global::fm->open_text_file_for_write(file_name);
			//
			gv_string_tmp s_buffer;
			;
			gv_string_tmp* ps = &s_buffer;
			gv_string_tmp s_helper;
			// s<<"<gv_performance_snap_shot  version
			// =\""<<this->gv_profiler_version<<"\""
			gvt_xml_write_element_open(*ps, "gv_performance_snap_shot", false);
			gvt_xml_write_attribute(*ps, "version", gv_profiler_version);
			gvt_xml_write_attribute(*ps, "event_count", event_count);
			gv_ulong perf_frequency = gv_global::time->get_performance_frequency();
			gvt_xml_write_attribute(*ps, "perf_frequency", perf_frequency);
			gvt_xml_write_element_open(*ps);
			for (int i = 0; i < this->m_mark_ring_buffer_size; i++)
				dump_mark(this->m_mark_ring_buffer[i], ps);
			gvt_xml_write_element_close(*ps, "gv_performance_snap_shot");
			*pss << *s_buffer;
			delete pss;
		}
		m_is_dumping = false;
	};

	// TODO automaticly write to a log file with all the information.
	void enable_async_log(bool enable){

	};
	void reset()
	{
	}
	// data member
	gv_atomic_count m_current_free;
	gv_mutex m_mutex;
	volatile bool m_is_dumping;
	gv_time_mark* m_mark_ring_buffer;
	int m_mark_ring_buffer_size;
};

gv_profiler_manager::gv_profiler_manager()
{
	m_imp = NULL;
};

gv_profiler_manager::~gv_profiler_manager()
{
	GVM_SAFE_DELETE(m_imp);
};

void gv_profiler_manager::init(int ring_buffer_size)
{
	GV_ASSERT(!m_imp);
	m_imp = new gv_profiler_manager_imp(ring_buffer_size);
};

gv_time_mark* gv_profiler_manager::alloc(bool wait)
{
	if (m_imp)
		return m_imp->alloc(wait);
	return NULL;
};

// write all the event in ring buffer to log
void gv_profiler_manager::dump_snap_shot(const char* prefix)
{
	if (m_imp)
		return m_imp->dump_snap_shot(prefix);
};

// write all the event in ring buffer to log
void gv_profiler_manager::reset()
{
	if (m_imp)
		return m_imp->reset();
};
// TODO automatically write to a log file with all the information.
void gv_profiler_manager::enable_async_log(bool enable)
{
	if (m_imp)
		return m_imp->enable_async_log(enable);
};
};