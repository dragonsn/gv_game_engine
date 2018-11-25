
GVM_DOM_ELEMENT(scp_add_grass_layer_param)
GVM_DOM_ATTRIB(gv_terrain_grass_layer_info, info)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_add_mesh_layer_param)
GVM_DOM_ATTRIB(gv_terrain_mesh_layer_info, info)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_add_fur_layer_param)
GVM_DOM_ATTRIB(gv_terrain_fur_layer_info, info)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_remove_grass_layer_param)
GVM_DOM_ATTRIB(gv_int, index)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_remove_mesh_layer_param)
GVM_DOM_ATTRIB(gv_int, index)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_remove_fur_layer_param)
GVM_DOM_ATTRIB(gv_int, index)
GVM_DOM_END_E

GVM_DOM_ELEMENT(scp_create_grass_mesh_param)
GVM_DOM_ATTRIB(gvt_ptr< gv_material >, material)
GVM_DOM_ATTRIB(gv_float, density)
GVM_DOM_ATTRIB(gv_float, normal_factor)
GVM_DOM_ATTRIB(gv_vector2, height_range)
GVM_DOM_ATTRIB(gv_float, grass_height)
GVM_DOM_ATTRIB(gv_float, grass_width)
GVM_DOM_END_E