namespace gv
{
gv_state_machine::gv_state_machine()
{
	GVM_ZERO_ME;
	m_bt_state = gve_bt_runing;
}

gv_state_machine::~gv_state_machine()
{
}

bool gv_state_machine::start()
{
	GV_ASSERT(this->m_bt_state != gve_bt_wait_for_signal);
	this->m_bt_state = gve_bt_runing;
	return true;
};

bool gv_state_machine::stop()
{
	GV_ASSERT(this->m_bt_state != gve_bt_wait_for_signal);
	this->m_bt_state = gve_bt_pending;
	return true;
}

bool gv_state_machine::signal(bool is_time_out)
{
	// GV_ASSERT(this->m_bt_state==gve_bt_wait_for_signal);
	++this->m_signal;
	m_is_time_out = is_time_out;
	return true;
};

bool gv_state_machine::is_time_out()
{
	return m_is_time_out;
};

bool gv_state_machine::signal(gv_uint signal_fence, bool is_time_out)
{
	if (this->m_bt_state == gve_bt_wait_for_signal &&
		this->m_signal_fence == signal_fence)
	{
		++this->m_signal;
		m_is_time_out = is_time_out;
		return true;
	};
	return false;
};

bool gv_state_machine::is_signaled()
{
	return this->m_signal.get() != 0;
};

bool gv_state_machine::signal_reset()
{
	this->m_signal = 0;
	return true;
};

gv_bt_state_type& gv_state_machine::current_state()
{
	return this->m_state_stack[m_current_stack_depth];
};

gv_bool gv_state_machine::goto_state(gv_bt_state_type b)
{
	this->m_state_stack[m_current_stack_depth] = b;
	for (int i = m_current_stack_depth + 1; i < gvc_bt_max_stack_depth; i++)
	{
		this->m_state_stack[i] = 0; //
	}
	return true;
};

gv_bool gv_state_machine::goto_state(gv_bt_state_type b, gv_int level)
{
	if (level < 0)
		level = m_current_stack_depth + level;
	this->m_state_stack[level] = b;
	for (int i = level + 1; i < gvc_bt_max_stack_depth; i++)
	{
		this->m_state_stack[i] = 0; //
	}
	return true;
};

gv_bt_state_type gv_state_machine::get_state(gv_int level) const
{
	if (level < 0)
		level = m_current_stack_depth + level;
	return this->m_state_stack[level];
};

void gv_state_machine::step_in_state()
{
	m_current_stack_depth++;
};

void gv_state_machine::step_out_state()
{
	m_current_stack_depth--;
};

gve_bt_state gv_state_machine::get_bt_state()
{
	return m_bt_state;
};

void gv_state_machine::set_bt_state(gve_bt_state state)
{
	m_bt_state = state;
};

void gv_state_machine::debug_state(const char* name)
{
	m_state_stack_debug[m_current_stack_depth] = name;
	m_last_debug_stack_depth = m_current_stack_depth;
};

void gv_state_machine::set_stack_depth(gv_int i)
{
	m_current_stack_depth = i;
};

void gv_state_machine::backup_state_stack(state_stack& stack)
{
	stack = m_state_stack;
};

void gv_state_machine::restore_state_stack(const state_stack& stack)
{
	m_state_stack = stack;
};

gv_string_tmp gv_state_machine::get_state_stack()
{
	gv_string_tmp s;
	s << "[state_stack]:";
	for (int i = 0; i <= m_last_debug_stack_depth; i++)
	{
		if (m_state_stack_debug[i])
		{
			s << "=>[" << i << "]" << m_state_stack_debug[i];
		}
	}
	return s;
};

void gv_state_machine::reset_bt()
{
	m_current_stack_depth = 0;
	m_state_stack = 0;
	m_signal.set(0);
	m_is_time_out = false;
	m_signal_fence = 0;
	m_bt_state = gve_bt_runing;
	m_state_stack_debug = 0;
};
}