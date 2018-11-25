#define GV_DOM_FILE "gv_func_param_structs.h"
#define GVM_DOM_RTTI
#define GVM_DOM_STATIC_CLASS
#include "gv_data_model_ex.h"

GVM_BGN_FUNC(gv_object, scp_debug_break)
GVM_PARAM(gv_int, condition)
if (condition)
{
	GV_DEBUG_BREAK;
	return 0;
}
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_object, scp_log)
GVM_PARAM(gv_id, tag)
GVM_PARAM(gv_string, s)
GVM_PARAM(gv_byte, need_time_stamp)
GVM_LOG(script,
		"[" << tag << "]"
			<< (need_time_stamp ? gv_global::time->get_local_time_string() : "")
			<< "=======:" << s);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_object, scp_import_file)
GVM_PARAM(gv_text, external_file_name)
GVM_PARAM(gv_text, error)
return this->import_from_external_file(*external_file_name, error);
GVM_END_FUNC

GVM_BGN_FUNC(gv_object, scp_rename)
GVM_PARAM(gv_id, name)
GVM_PARAM(gv_int, idx)
rename(gv_object_name(name, idx));
return 1;
GVM_END_FUNC
