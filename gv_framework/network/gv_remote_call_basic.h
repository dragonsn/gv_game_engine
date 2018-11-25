GVM_DCL_REMOTE_CALL(client_login, 0)
GVM_REMOTE_CALL_PARAM(gv_string, user_name)
GVM_REMOTE_CALL_PARAM(gv_string, password)
GVM_REMOTE_CALL_RETURN(gv_bool, is_ok)
GVM_REMOTE_CALL_RETURN(gv_int, error_code)
GVM_REMOTE_CALL_RETURN(gv_uint, global_user_id)
GVM_END_DCL_REMOTE_CALL

GVM_DCL_REMOTE_CALL(client_logout, 1)
GVM_REMOTE_CALL_PARAM(gv_uint, global_user_id)
GVM_REMOTE_CALL_RETURN(gv_bool, done)
GVM_END_DCL_REMOTE_CALL
