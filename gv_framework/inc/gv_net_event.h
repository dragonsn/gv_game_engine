GVM_DCL_EVENT(net_cable_disconnected, 801)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_fatal_error, 802)
GVM_EVENT_VAR(gv_string_tmp, error_string)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_add_host, 803)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, host)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_remove_host, 804)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, host)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_add_net_replica, 805)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, replica)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_remove_net_replica, 806)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, replica)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_remote_call, 808)
GVM_EVENT_VAR(gvt_ref_ptr< gv_refable >, call)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, caller)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_actor_2d_move, 809)
GVM_EVENT_VAR(gv_uint, uuid)
GVM_EVENT_VAR(gv_vector2, old_pos)
GVM_EVENT_VAR(gv_vector2, new_pos)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_actor_2d_state_change, 810)
GVM_EVENT_VAR(gv_uint, uuid)
GVM_EVENT_VAR(gv_byte, old_state)
GVM_EVENT_VAR(gv_byte, new_state)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_actor_2d_facing_change, 811)
GVM_EVENT_VAR(gv_uint, uuid)
GVM_EVENT_VAR(gv_float, old_facing)
GVM_EVENT_VAR(gv_float, new_facing)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(net_actor_input_command, 812)
GVM_EVENT_VAR(gv_uint, uuid)
GVM_EVENT_VAR(gv_byte, old_state)
GVM_END_DCL_EVENT