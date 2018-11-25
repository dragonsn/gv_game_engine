namespace gv
{
gv_roam_patch::gv_roam_patch()
{
	m_is_visible = false;
	m_is_selected = false;
}

gv_roam_patch::~gv_roam_patch()
{
}

void gv_roam_patch::reset()
{
	this->m_left_root = new gv_roam_node;
	this->m_right_root = new gv_roam_node;
	gv_vector2i v0(0, GV_ROAM_PATCH_SIZE);
	gv_vector2i v1(GV_ROAM_PATCH_SIZE, GV_ROAM_PATCH_SIZE);
	gv_vector2i v2(0, 0);
	gv_vector2i v3(GV_ROAM_PATCH_SIZE, 0);
	this->m_left_root->make_root(v0, v1, v2);
	this->m_right_root->make_root(v3, v2, v1);
	this->m_right_root->set_basic_neighbour(this->m_left_root);
	this->m_left_root->set_basic_neighbour(this->m_right_root);
}

void gv_roam_patch::tessellate(gv_float max_tolerate_err)
{
	this->tessellate(m_left_root, m_chunk->m_variance_l, max_tolerate_err);
	this->tessellate(m_right_root, m_chunk->m_variance_r, max_tolerate_err);
}

void gv_roam_patch::tessellate(gv_roam_node* proot,
							   gvt_array< gv_float >& variance,
							   gv_float max_tolerate_err)
{
	if (proot->get_left_child())
	{
		this->tessellate(proot->get_left_child(), variance, max_tolerate_err);
		this->tessellate(proot->get_right_child(), variance, max_tolerate_err);
	}
	else if (proot->get_idx_in_full_tree() < variance.size())
	{
		if (variance[proot->get_idx_in_full_tree()] > max_tolerate_err)
		{
			proot->force_split();
			this->tessellate(proot->get_left_child(), variance, max_tolerate_err);
			this->tessellate(proot->get_right_child(), variance, max_tolerate_err);
		}
	}
}

bool gv_roam_patch::fill_index_buffer(gv_int* pindex, gv_int& length,
									  gv_int max_length)
{
	if (!this->m_left_root->fill_index(pindex, length, max_length))
		return false;
	if (!this->m_right_root->fill_index(pindex, length, max_length))
		return false;
	return true;
};

void gv_roam_patch::update_world_aabb()
{
	gv_box b = m_chunk->get_local_aabb();
	b.move(roam_to_world(
		gv_vector3((gv_float)m_offset.get_x(), (gv_float)m_offset.get_y(), 0)));
	this->m_world_aabb = b.transform(m_terrain->get_entity()->get_tm());
}

gv_int gv_roam_patch::get_index_in_terrain()
{
	return m_offset.get_x() + m_offset.get_y() * m_terrain->get_size_in_patch().x;
};

gv_box gv_roam_patch::get_local_aabb()
{
	gv_box b = m_chunk->get_local_aabb();
	b.move(roam_to_world(
		gv_vector3((gv_float)m_offset.get_x(), (gv_float)m_offset.get_y(), 0)));
	return b;
};

gv_bool gv_roam_patch::attach_a_renderable(gv_obj_node_ptr node)
{
	m_renderables.link(node);
	gv_com_graphic* pcom = gvt_cast< gv_com_graphic >(node->object);
	GV_ASSERT(pcom);
	// this->m_world_aabb.add(pcom->get_entity()->get_world_aabb());
	return true;
};

gv_bool gv_roam_patch::detach_a_renderable(gv_obj_node_ptr node)
{
	m_renderables.unlink(node);
	return true;
};

void gv_roam_patch::set_visible_tag(gv_ulong tag)
{
	gv_obj_node_ptr pnode = this->m_renderables.first;
	while (pnode)
	{
		gv_com_graphic* pcom = gvt_cast< gv_com_graphic >(pnode->object);
		GV_ASSERT(pcom);
		pcom->set_visible_test_tag(tag);
		pnode = pnode->next;
	}
};

GVM_IMP_STRUCT(gv_roam_patch)
GVM_VAR(gv_vector2i, m_offset)
GVM_VAR(gv_box, m_world_aabb)
GVM_VAR(gvt_ref_ptr< gv_roam_chunk >, m_chunk)
GVM_VAR(gv_object_name, m_chunk_name)
GVM_VAR(gvt_ptr< gv_com_terrain_roam >, m_terrain)
GVM_VAR(gvt_array< gvt_array< gv_vector4 > >, m_grass_layer_instances)
GVM_VAR(gvt_array< gvt_array< gv_vector4 > >, m_mesh_layer_instances)
GVM_END_STRUCT
}