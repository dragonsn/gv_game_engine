GVM_DOM_ELEMENT(scp_debug_break_param)
GVM_DOM_ATTRIB(gv_int, condition)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_log_param)
GVM_DOM_ATTRIB(gv_id, tag)
GVM_DOM_ATTRIB_SET(tooltip, "log type tag!")
GVM_DOM_ATTRIB_SET(default, "info")
GVM_DOM_ATTRIB(gv_string, s)
GVM_DOM_ATTRIB_SET(tooltip, "content of log !")
GVM_DOM_ATTRIB_SET(default, "HELLO WORLD!")
GVM_DOM_ATTRIB(gv_byte, need_time_stamp)
GVM_DOM_ATTRIB(gv_int, condition)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rename_param)
GVM_DOM_ATTRIB(gv_id, name)
GVM_DOM_ATTRIB(gv_int, idx)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rebuild_normal_param)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rescale_param)
GVM_DOM_ATTRIB(gv_vector3, scale)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rotate_param)
GVM_DOM_ATTRIB(gv_euler, rotation)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rebuild_bounding_param)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_rebuild_world_matrix_param)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_switch_wireframe_param)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_reinit_world_param)
GVM_DOM_ATTRIB(gv_vector2i, grid_size)
GVM_DOM_ATTRIB(gv_box, bounding)
GVM_DOM_ATTRIB(gv_text, resource_name)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_import_file_param)
GVM_DOM_FLAG(file_name)
GVM_DOM_ATTRIB(gv_text, external_file_name)
GVM_DOM_ATTRIB(gv_text, error)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_export_morph_texture)
GVM_DOM_ATTRIB(gv_text, file_name)
GVM_DOM_ATTRIB_SET(default, "target path and main file name , will generate a "
							"set of files with different postfix")
GVM_DOM_END_E

GVM_DOM_ELEMENT(gv_event_processor_desc)
GVM_DOM_ATTRIB(gv_bool, m_is_synchronization)
GVM_DOM_ATTRIB(gv_bool, m_is_autonomous)
GVM_DOM_ATTRIB(gv_id, m_channel_id) // use name for safe
GVM_DOM_ATTRIB(gv_id, m_processor_cls)
GVM_DOM_END_E
