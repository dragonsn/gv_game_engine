GVM_DOM_ELEMENT(gv_effect_stream_channel)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_int, m_stream_index)
GVM_DOM_ATTRIB(gv_int, m_usage_index)
GVM_DOM_ATTRIB(gv_int, m_usage)
GVM_DOM_ATTRIB(gv_int, m_format)
GVM_DOM_END_E

GVM_DOM_ELEMENT(gv_effect_param_info)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_id, m_semantic)
GVM_DOM_ATTRIB_ENUM(gve_effect_semantic, m_semantic_idx)
GVM_DOM_ATTRIB(gv_text, m_extra_info)
GVM_DOM_ATTRIB(gv_int, m_type)
GVM_DOM_ATTRIB(gv_int, m_sub_type)
GVM_DOM_ATTRIB(gv_int, m_row)
GVM_DOM_ATTRIB(gv_int, m_col)
GVM_DOM_ATTRIB(gv_bool, m_is_static)
GVM_DOM_FLAG(transient)
GVM_DOM_ATTRIB(gv_bool, m_is_dirty)
GVM_DOM_ATTRIB(gv_uint, m_tag)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_stream_map)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gvt_array< gv_effect_stream_channel >, m_map)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_texture)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_bool, m_is_cubemap)
GVM_DOM_ATTRIB(gv_bool, m_is_3dmap)
GVM_DOM_ATTRIB(gv_text, m_file_name)
GVM_DOM_ATTRIB(gv_string, m_semantic)
GVM_DOM_ATTRIB(gvt_ref_ptr< gv_texture >, m_texture)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_renderable_texture)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_vector2i, m_size)
GVM_DOM_ATTRIB(gv_colorf, m_clear_color)
GVM_DOM_ATTRIB(gv_text, m_file_name)
GVM_DOM_ATTRIB(gv_int, m_format)
GVM_DOM_ATTRIB(gv_bool, m_use_mipmap)
GVM_DOM_ATTRIB(gv_bool, m_use_window_size)
GVM_DOM_ATTRIB(gv_float, m_width_ratio)
GVM_DOM_ATTRIB(gv_float, m_height_ratio)
GVM_DOM_FLAG(transient)
GVM_DOM_ATTRIB(gvt_ref_ptr< gv_texture >, m_texture)
GVM_DOM_END_E

//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_model)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_text, m_file_name)
GVM_DOM_ATTRIB(gvt_ptr< gv_model >, m_model)
GVM_DOM_END_E

//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_camera)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_vector3, m_pos)
GVM_DOM_ATTRIB(gv_vector3, m_look_at)
GVM_DOM_ATTRIB(gv_vector3, m_up)
GVM_DOM_ATTRIB(gv_float, m_fov)
GVM_DOM_ATTRIB(gv_float, m_near_clip)
GVM_DOM_ATTRIB(gv_float, m_far_clip)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_pass)
GVM_DOM_ATTRIB(gvt_ptr< gv_effect >, m_owner)
GVM_DOM_ATTRIB(gvt_ref_ptr< gv_effect >, m_pass_effect)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_bool, m_enabled)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_render_target)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_int, m_mip_index)
GVM_DOM_ATTRIB(gv_bool, m_render_to_screen)
GVM_DOM_ATTRIB(gv_bool, m_is_clear_color)
GVM_DOM_ATTRIB(gv_bool, m_is_clear_depth)
GVM_DOM_ATTRIB(gv_colorf, m_clear_color_value)
GVM_DOM_ATTRIB(gv_colorf, m_clear_depth_value)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_render_state)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_int, m_state)
GVM_DOM_ATTRIB(gv_int, m_value)
GVM_DOM_END_E
//============================================================================================
//								:
//============================================================================================
GVM_DOM_ELEMENT(gv_effect_texture_object)
GVM_DOM_ATTRIB(gv_id, m_name)
GVM_DOM_ATTRIB(gv_id, m_texture_name)
GVM_DOM_ATTRIB(gv_int, m_stage)
GVM_DOM_ATTRIB(gv_bool, m_is_vertex_texture)
GVM_DOM_ATTRIB(gvt_array< gv_effect_render_state >, m_texture_states)
GVM_DOM_END_E
