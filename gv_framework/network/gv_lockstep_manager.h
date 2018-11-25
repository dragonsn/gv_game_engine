#pragma once
namespace gv
{
// max frame=2^18, if 30 fps, this is equal to 8738 seconds  , more than 2 hours
static const gv_uint gvc_max_lockstep_frame_number = 262143;
// max command code =2^9-1
static const gv_uint gvc_max_command_code = 512;
// max command
static const gv_uint gvc_max_command_param_size = 16;

struct gv_lockstep_command
{
	gv_ushort command_code;
};

// a class used to process lock step command
class gv_com_lockstep_command_executor : public gv_component
{
public:
	virtual bool exec_command(const gv_lockstep_command& command)
};

class gv_lockstep_manager_data;
class gv_lockstep_manager : public gv_event_processor
{
public:
	virtual void reset();

	virtual void init(int nb_player, int local_player_index);

	virtual void register_command(gv_ushort command_code,
								  gv_string_tmp param_type_name);

	virtual void queue_command(gv_byte player_index, gv_int frame_count,
							   const gv_lockstep_command& command);

	virtual void bind_executor(gv_com_lockstep_command_executor* executor);

protected:
	gvt_ref_ptr< gv_lockstep_manager_data > m_impl;
};
}