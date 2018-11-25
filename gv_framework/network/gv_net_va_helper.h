#pragma once
namespace gv
{
namespace va_helper
{
struct gv_remote_call_client_login
{
	gv_string user_name;
	gv_string password;
};

struct gv_remote_call_client_login_ack
{
	gv_bool is_ok;
	gv_int error_code;
	gv_uint global_user_id;
};

struct gv_remote_call_client_logout
{
	gv_uint global_user_id;
};

struct gv_remote_call_client_logout_ack
{
	gv_bool done;
};

struct gv_net_cmd_host_chat
{
	gv_string content;
};

struct gv_net_cmd_cmd_generic_0
{
	gv_ushort code_id;
};

struct gv_net_cmd_cmd_generic_1
{
	gv_ushort code_id;
	gv_uint arg_1;
};

struct gv_net_cmd_cmd_generic_2
{
	gv_ushort code_id;
	gv_uint arg_1;
	gv_uint arg_2;
};

struct gv_net_cmd_cmd_generic_3
{
	gv_ushort code_id;
	gv_uint arg_1;
	gv_uint arg_2;
	gv_uint arg_3;
};

struct gv_net_cmd_cmd_generic_4
{
	gv_ushort code_id;
	gv_uint arg_1;
	gv_uint arg_2;
	gv_uint arg_3;
	gv_uint arg_4;
};
}
}