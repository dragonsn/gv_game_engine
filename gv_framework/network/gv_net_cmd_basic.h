//============================================================================================
//								:
//============================================================================================
GVM_DCL_NET_CMD(host_ping, 1)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(host_sync_game_time, 2)
GVM_NET_CMD_VAR(gv_ulong, game_start_time)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(host_sync_system_time, 3)
GVM_NET_CMD_VAR(gv_ulong, time_stamp_utc)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(host_chat, 4)
GVM_NET_CMD_VAR(gv_string, content)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_0, 10)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_1, 11)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_2, 12)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_3, 13)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_4, 14)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_5, 15)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_NET_CMD_VAR(gv_uint, arg_5)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_6, 16)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_NET_CMD_VAR(gv_uint, arg_5)
GVM_NET_CMD_VAR(gv_uint, arg_6)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_7, 17)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_NET_CMD_VAR(gv_uint, arg_5)
GVM_NET_CMD_VAR(gv_uint, arg_6)
GVM_NET_CMD_VAR(gv_uint, arg_7)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_8, 18)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_NET_CMD_VAR(gv_uint, arg_5)
GVM_NET_CMD_VAR(gv_uint, arg_6)
GVM_NET_CMD_VAR(gv_uint, arg_7)
GVM_NET_CMD_VAR(gv_uint, arg_8)
GVM_END_DCL_NET_CMD

GVM_DCL_NET_CMD(cmd_generic_9, 19)
GVM_NET_CMD_VAR(gv_ushort, code_id)
GVM_NET_CMD_VAR(gv_uint, arg_1)
GVM_NET_CMD_VAR(gv_uint, arg_2)
GVM_NET_CMD_VAR(gv_uint, arg_3)
GVM_NET_CMD_VAR(gv_uint, arg_4)
GVM_NET_CMD_VAR(gv_uint, arg_5)
GVM_NET_CMD_VAR(gv_uint, arg_6)
GVM_NET_CMD_VAR(gv_uint, arg_7)
GVM_NET_CMD_VAR(gv_uint, arg_8)
GVM_NET_CMD_VAR(gv_uint, arg_9)
GVM_END_DCL_NET_CMD