
namespace gv
{
gv_com_tasklet::gv_com_tasklet()
{
	GVM_SET_CLASS(gv_com_tasklet);
	m_sleep_time_count_down = 0;
	m_current_stack_depth = 0;
	m_state_stack = 0;
	m_state_stack_debug = NULL;
	m_bt_state = gve_bt_runing;
	m_signal_fence = 0;
	m_is_time_out = false;
	m_last_debug_stack_depth = 0;
};

gv_com_tasklet::~gv_com_tasklet(){

};

bool gv_com_tasklet::tick(gv_float dt)
{
	m_is_time_out = false;
	if (m_sleep_time_count_down > 0)
	{
		m_sleep_time_count_down -= dt;
		if (m_sleep_time_count_down <= 0)
		{
			m_sleep_time_count_down = 0;
			if (this->get_bt_state() == gve_bt_wait_for_signal)
			{
				this->signal(true);
			}
			else
				m_is_time_out = true;
		}
	}
	return true;
};
bool gv_com_tasklet::set_resource(gv_resource*)
{
	return true;
};
bool gv_com_tasklet::is_tickable()
{
	return true;
};

bool gv_com_tasklet::task_nop()
{
	return true;
};
void gv_com_tasklet::set_time_out(gv_float time)
{
	m_sleep_time_count_down = time;
};
bool gv_com_tasklet::task_sleep(gv_float f)
{
	m_sleep_time_count_down = f;
	return true;
};
bool gv_com_tasklet::task_wait_next_frame()
{
	signal();
	return true;
};

bool gv_com_tasklet::task_wait_frame(gv_int frame_count)
{
	if (m_wait_frame_count_down == 0)
	{
		m_wait_frame_count_down = frame_count;
		return true;
	}
	m_wait_frame_count_down--; 
	if (!m_wait_frame_count_down) return true; 
	return false;
};

bool gv_com_tasklet::reset()
{
	m_sleep_time_count_down = 0;
	m_wait_frame_count_down = 0;
	reset_bt();
	return true;
};

GVM_IMP_CLASS(gv_com_tasklet, gv_component);
GVM_VAR(gv_int, m_current_stack_depth)
GVM_STATIC_ARRAY(gv_byte, 16, m_state_stack)
GVM_VAR(gv_int, m_signal)
GVM_VAR(gv_int, m_bt_state)
GVM_END_CLASS
}
