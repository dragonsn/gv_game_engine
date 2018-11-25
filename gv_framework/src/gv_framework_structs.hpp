namespace gv
{
GVM_IMP_STRUCT(gv_vector2i)
{
	GVM_VAR(gv_int, x)
	GVM_VAR(gv_int, y)
}
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_vector4b)
{
	GVM_VAR(gv_byte, x)
	GVM_VAR(gv_byte, y)
	GVM_VAR(gv_byte, z)
	GVM_VAR(gv_byte, w)
}
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_box)
GVM_VAR(gv_vector3, min_p)
GVM_VAR(gv_vector3, max_p)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_sphere)
GVM_VAR(gv_vector3, pos)
GVM_VAR(gv_float, r)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_plane)
GVM_VAR(gv_vector3, normal)
GVM_VAR(gv_float, offset)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_object_handle)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_object_event)
GVM_VAR(gv_ushort, m_id)
GVM_VAR(gv_id, m_name)
GVM_VAR(gvt_ref_ptr< gv_object >, m_sender)
GVM_VAR(gvt_ref_ptr< gv_object >, m_reciever)
GVM_VAR(gv_uint, m_flag)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_frustum)
GVM_STATIC_ARRAY(gv_plane, 6, planes)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_rect)
GVM_VAR(gv_vector2, min_p)
GVM_VAR(gv_vector2, max_p)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_recti)
GVM_VAR(gv_vector2i, min_p)
GVM_VAR(gv_vector2i, max_p)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_boxi)
GVM_VAR(gv_vector3i, min_p)
GVM_VAR(gv_vector3i, max_p)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_euleri)
GVM_VAR(gv_int, yaw)
GVM_VAR(gv_int, pitch)
GVM_VAR(gv_int, roll)
GVM_END_STRUCT

#if GV_WITH_OS_API
GVM_IMP_STRUCT_WITH_SUPER(gv_ip_address_ipv4, gv_vector4b)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_socket_address)
GVM_VAR(gv_short, m_sin_family)
GVM_VAR(gv_ushort, m_sin_port)
GVM_VAR(gv_ip_address_ipv4, m_sin_addr)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_packet)
GVM_VAR(gv_ulong, m_time_stamp)
GVM_VAR(gv_socket_address, m_address)
GVM_VAR(gv_uint, m_user_param)
GVM_VAR(gv_uint, m_sequence_number)
GVM_VAR(gv_uint, m_resend_times)
GVM_END_STRUCT
#endif

GVM_IMP_STRUCT(gv_framework_config)
GVM_VAR(gv_text, module_path_xml)
GVM_VAR(gv_text, module_path_gvb)
GVM_VAR(gv_text, data_path_root)
GVM_VAR(gv_text, physics_asset_root_path)
GVM_VAR(gv_text, texture_cache_path)
GVM_VAR(gvt_array< gv_string >, driver_list)
GVM_VAR(gv_vector2i, window_size)
GVM_END_STRUCT
GVM_IMP_STATIC_CLASS(gv_framework_config)

GVM_IMP_STRUCT(gv_int_string_pair)
GVM_VAR(gv_int, i)
GVM_VAR(gv_string, s)
GVM_END_STRUCT
};