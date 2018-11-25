#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
class gv_com_tasklet : public gv_component, public gv_state_machine
{
public:
	gv_com_tasklet();
	~gv_com_tasklet();
	GVM_DCL_CLASS(gv_com_tasklet, gv_component);

public:
	bool tick(gv_float dt);
	bool set_resource(gv_resource*);
	bool is_tickable();
	void set_time_out(gv_float time);

protected:
	bool task_nop();
	bool task_sleep(gv_float f);
	bool task_wait_next_frame();
	bool task_wait_frame(gv_int frame_count);
	bool reset();
	gv_float m_sleep_time_count_down;
	gv_int m_wait_frame_count_down;
};
}