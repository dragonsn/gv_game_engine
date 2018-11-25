#pragma once
namespace gv
{
// bt: behavour tree
enum gve_bt_state
{
	gve_bt_not_started,
	gve_bt_runing,
	gve_bt_pending,
	gve_bt_wait_for_signal,
	gve_bt_zombie,
};

typedef gv_byte gv_bt_state_type;
const static int gvc_bt_max_stack_depth = 16;
const static gv_bt_state_type gvc_bt_invalid_ai_state = (gv_bt_state_type)-1;
// TODO  use fence to check the signal is outdated or not...
gv_bool inline gv_check_fence(gv_uint fence, gv_uint check_fence)
{
	gv_bool wrapped = ((check_fence - fence) > (1 << 30)) ? true : false;
	return (fence < check_fence) || (wrapped && check_fence < fence);
}

class gv_state_machine
{
public:
	gv_state_machine();

	~gv_state_machine();

	bool signal(bool time_out = false);

	bool signal(gv_uint signal_fence, bool time_out = false);

	gv_uint get_signal_fence()
	{
		return m_signal_fence;
	};

	bool start();

	bool stop();

	bool is_time_out();

	bool is_signaled();

	bool signal_reset();

	gv_bt_state_type& current_state();

	gv_bool goto_state(gv_bt_state_type b);

	gv_bool goto_state(gv_bt_state_type b, gv_int level);

	gv_bt_state_type get_state(gv_int level = 0) const;

	gve_bt_state get_bt_state();

	void set_bt_state(gve_bt_state state);

	void debug_state(const char* name);

	gv_string_tmp get_state_stack();

	bool nop()
	{
		return true;
	};

	void reset_bt();

	void step_in_state();

	void step_out_state();

protected:
	typedef gvt_array_static< gv_bt_state_type, gvc_bt_max_stack_depth >
		state_stack;
	typedef gvt_array_static< const gv_char*, gvc_bt_max_stack_depth >
		state_debug_stack;



	void backup_state_stack(state_stack& stack);

	void restore_state_stack(const state_stack& stack);

	void set_stack_depth(gv_int i);

	gv_int m_current_stack_depth;
	state_stack m_state_stack;
	gv_atomic_count m_signal;
	gv_bool m_is_time_out;
	gv_uint m_signal_fence;
	gve_bt_state m_bt_state;
	state_debug_stack m_state_stack_debug;
	gv_int m_last_debug_stack_depth;
};
}
