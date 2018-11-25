
GVM_DCL_EVENT(add_component, 100)
GVM_EVENT_VAR(gvt_ref_ptr< gv_component >, component)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(remove_component, 101)
GVM_EVENT_VAR(gvt_ref_ptr< gv_component >, component)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(user_event, 102)
GVM_EVENT_VAR(gv_int, user_event_type)
GVM_EVENT_VAR(gv_uint, param0)
GVM_EVENT_VAR(gv_uint, param1)
GVM_EVENT_VAR(gv_uint, param2)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(quit_game, 103)
GVM_END_DCL_EVENT