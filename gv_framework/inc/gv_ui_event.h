GVM_DCL_EVENT(ui_load_module, 1000)
GVM_EVENT_VAR(gvt_ref_ptr< gv_object >, module)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_msg_box, 1001)
GVM_EVENT_VAR(gv_string, title)
GVM_EVENT_VAR(gv_string, message)
GVM_EVENT_VAR(gv_bool, need_ok)
GVM_EVENT_VAR(gv_string, need_cancel)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_active, 1002)
GVM_EVENT_VAR(gv_string, module_name)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_char, 1003)
GVM_EVENT_VAR(gv_uint, character)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_move, 1004)
GVM_EVENT_VAR(gv_float, x)
GVM_EVENT_VAR(gv_float, y)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_left_down, 1005)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_left_up, 1006)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_right_down, 1007)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_right_up, 1008)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_middle_down, 1009)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_middle_up, 1010)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_mouse_wheel, 1011)
GVM_EVENT_VAR(gv_float, x)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_key_down, 1012)
GVM_EVENT_VAR(gv_uint, key)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(ui_inject_key_up, 1013)
GVM_EVENT_VAR(gv_uint, key)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_load_layout, 1014)
GVM_EVENT_VAR(gv_string, layout_name)
GVM_EVENT_VAR(gv_string, layout_address)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_load_module_with_cls_id, 1015)
GVM_EVENT_VAR(gv_string, module_class_name)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_add_listener_channel, 1016)
GVM_EVENT_VAR(gv_int, channelid)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_control_event, 1017)
GVM_EVENT_VAR(gv_string, page_name)
GVM_EVENT_VAR(gv_string, control_name)
GVM_EVENT_VAR(gv_string, control_value)
GVM_EVENT_VAR(gv_int, event_flag)
GVM_EVENT_VAR(gv_int, x)
GVM_EVENT_VAR(gv_int, y)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_touch_event, 1018)
GVM_EVENT_VAR(gv_int, event_type)
GVM_EVENT_VAR(gv_int, x)
GVM_EVENT_VAR(gv_int, y)
GVM_EVENT_VAR(gv_int, contact_index)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_pad_event, 1019)
GVM_EVENT_VAR(gv_int, event_flag)
GVM_EVENT_VAR(gv_int, x)
GVM_EVENT_VAR(gv_int, y)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(ui_open_layout, 1020)
GVM_EVENT_VAR(gv_string, layout_name)
GVM_EVENT_VAR(bool, close_old_layout)
GVM_END_DCL_EVENT
