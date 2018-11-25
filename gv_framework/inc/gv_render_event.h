GVM_DCL_EVENT(render_init, 400)
GVM_EVENT_VAR(gv_long, window_handle)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_uninit, 401)
GVM_EVENT_VAR(gv_uint, window_handle)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_enable_pass, 402)
GVM_EVENT_VAR(gv_int, pass)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_disable_pass, 404)
GVM_EVENT_VAR(gv_int, pass)
GVM_END_DCL_EVENT

GVM_DCL_EVENT_MUTE(render_set_camera, 405)
GVM_EVENT_VAR(gvt_ref_ptr< gv_component >, camera)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_set_ui_manager, 406)
GVM_EVENT_VAR(gvt_ptr< gvi_ui_manager >, ui_mgr)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_reload_shader_cache, 407)
GVM_END_DCL_EVENT

GVM_DCL_EVENT(render_resize_window, 408)
GVM_EVENT_VAR(gv_int, sx)
GVM_EVENT_VAR(gv_int, sy)
GVM_END_DCL_EVENT
