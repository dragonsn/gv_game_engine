namespace gv
{
gv_model_node::gv_model_node()
{
	GVM_SET_CLASS(gv_model_node);
	m_local_tm.set_identity();
	m_world_tm.set_identity();
}

gv_model_node::~gv_model_node()
{
}

void gv_model_node::update_world_tm()
{
	if (this->m_parent)
	{
		this->m_world_tm = this->m_local_tm * this->m_parent->m_world_tm;
	}
	for (int i = 0; i < this->m_children.size(); i++)
	{
		this->m_children[i]->update_world_tm();
	}
};

void gv_model_node::update_local_tm()
{
	if (this->m_parent)
	{
		this->m_local_tm =
			this->m_world_tm * this->m_parent->m_world_tm.get_inverse();
	}
	for (int i = 0; i < this->m_children.size(); i++)
	{
		this->m_children[i]->update_local_tm();
	}
};

gv_model_node* gv_model_node::find_node(const gv_id& node_name)
{
	if (this->get_name_id() == node_name)
		return this;
	for (int i = 0; i < this->m_children.size(); i++)
	{
		gv_model_node* node = this->m_children[i]->find_node(node_name);
		if (node)
			return node;
	}
	return NULL;
};

void gv_model_node::set_parent(gv_model_node* p)
{
	if (m_parent && m_parent.ptr() != p)
	{
		m_parent->m_children.erase_item_fast(p);
	}
	m_parent = p;
	if (!p)
		return;
	m_parent->m_children.add_unique(this);
};

//==============================================================================
gv_model::gv_model()
{
	GVM_SET_CLASS(gv_model);
	m_max_half_size = 512.f;
}

gv_model::~gv_model(){};

bool gv_model::build_morph_texture(
	gv_int ani_index, gv_int skeletal_mesh_index,
	gvt_array< gv_image_2d >& output, gvt_array< gv_int >& output_ani_length,
	gvt_array< gv_string >& output_ani_name, int image_size,
	gvt_array< gv_vector3 >& output_original_vertex,
	gvt_array< gv_vector3 >& output_original_normal, bool output_normal_offset,
	gv_string_tmp& error_message, float debug_offset, float old_diff_factor,
	bool ignore_root_motion)
{
	gv_ani_set* pani = get_animation(ani_index);
	if (!pani)
		return 0;
	gv_skeletal_mesh* pskin = get_skeletal_mesh(skeletal_mesh_index);
	if (!pskin)
		return 0;
	gv_static_mesh* pmesh = pskin->get_t_mesh();
	output_original_vertex.clear();
	output_original_normal.clear();
	// init image
	if (image_size != 0)
	{
		if (pmesh->get_nb_vertex() > image_size)
		{
			GVM_WARNING("image size too small , can't hold all ther vertex in one "
						"line!!, enlarge it");
			image_size = gv_next_power_of_two(pmesh->get_nb_vertex());
			return 0;
		}
		output.resize(4);
		for (int i = 0; i < 4; i++)
		{
			output[i].set_sandbox(get_sandbox());
			output[i].init_image(gv_color::GREY(),
								 gv_vector2i(image_size, image_size));
		}
	}
	else
	{
		return 0;
	}
	// filling the images	TO_DO:compress data
	gvt_ref_ptr< gv_skeletal > my_skeletal =
		pskin->get_skeletal()->clone(get_sandbox());
	float fps = 30;
	gv_int line = 0;
	for (int sq = 0; sq < pani->get_nb_sequence(); sq++)
	{
		gv_ani_sequence* pseq = pani->get_sequence(sq);
		float ani_time = pseq->get_duration();
		float frame_count = ani_time * fps;
		output_ani_length.push_back(line);
		for (int frame = 0; frame < frame_count; frame++, line++)
		{
			// animate the mesh
			if (line >= image_size)
			{
				GVM_WARNING("image size too small , can't hold all the animations!, "
							"enlarge it");
				break;
			}
			my_skeletal->m_root_tm.set_identity();
			float time = (float)frame / fps;
			// set matrix
			int nb_track = pseq->get_track_number();
			if (!nb_track)
				continue;
			gv_float d = pseq->get_duration();
			gv_vector3 pos, scale;
			gv_quat q;
			for (int track = 0; track < nb_track; track++)
			{
				gv_ani_track* ptrack = pseq->get_track(track);
				ptrack->get_trans_rot((float)time, pos, q, scale, true);
				my_skeletal->set_bone_local_rotation_trans(ptrack->get_name_id(), q,
														   pos, scale);
			}

			my_skeletal->m_ignore_root_motion = ignore_root_motion;
			if (ignore_root_motion)
				my_skeletal->m_ignore_hierarchy = 1;
			my_skeletal->update_world_matrix();
			// start filling image.
			gv_vertex_buffer* vb = pmesh->m_vertex_buffer;
			const gvt_array< gv_vector3 >& original_pos = vb->m_raw_pos;
			const gvt_array< gv_vector3 >& original_normal = vb->m_raw_normal;
			for (int i = 0; i < vb->m_raw_pos.size(); i++)
			{
				gv_vector3 v3(0);
				gv_vector3 n3(0);
				gv_quat rot;
				rot.set_identity();
				float accumulate_weight = 0;
				for (int j = 0; j < 4; j++)
				{
					gv_short bone_index = (gv_short)vb->m_raw_blend_index[i][j];
					if (bone_index == -1)
						continue;
					gv_bone* pbone = NULL;
					if (!pskin->m_bone_mapping.size())
					{
						pbone = &my_skeletal->m_bones[bone_index];
					}
					else
					{
						pbone =
							&my_skeletal->m_bones[pskin->m_bone_inv_mapping[bone_index]];
					}
					gv_bone& bone = *pbone;
					float weight = vb->m_raw_blend_weight[i][j];
					gv_matrix44 mat = bone.m_matrix_model_to_bone * bone.m_tm * weight;
					v3 += original_pos[i] * mat;
					gv_matrix44 mat_r;
					mat.get_rotation(mat_r);
					n3 += original_normal[i] * mat_r;
					gv_math::convert(q, mat_r);
					rot = rot.slerp(q,
									1 - accumulate_weight / (accumulate_weight + weight));
					accumulate_weight += weight;
				} // next bone
				n3.normalize();
				if (line == 0) // the first line is reference line !!!
				{
					output_original_vertex.push_back(v3);
					output_original_normal.push_back(n3);
				}
				rot.normalize();
				v3 += v3 * debug_offset * (v3.length()) / m_max_half_size;
				v3 -= output_original_vertex[i] * old_diff_factor;
				// n3 -= output_original_normal[i];
				gv_byte rg[2];
				gv_byte ba[2];
				// set the xy offset map
				gvt_encode_zero_to_one_into_byte(
					gvt_normalize_value< gv_float >(v3.x, -m_max_half_size,
													m_max_half_size),
					rg);
				gvt_encode_zero_to_one_into_byte(
					gvt_normalize_value< gv_float >(v3.y, -m_max_half_size,
													m_max_half_size),
					ba);
				output[0].set_pixel(gv_vector2i(i, line),
									gv_color(rg[0], rg[1], ba[0], ba[1]));
				// set the z offset map, w is not used so far and save for later usage
				gvt_encode_zero_to_one_into_byte(
					gvt_normalize_value< gv_float >(v3.z, -m_max_half_size,
													m_max_half_size),
					rg);
				gvt_encode_zero_to_one_into_byte(
					gvt_normalize_value< gv_float >(v3.z, -m_max_half_size,
													m_max_half_size),
					ba);
				output[1].set_pixel(gv_vector2i(i, line),
									gv_color(rg[0], rg[1], ba[0], ba[1]));
				// set the rotation xy
				if (output_normal_offset)
				{
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(n3.x, -1, 1), rg);
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(n3.y, -1, 1), ba);
					output[2].set_pixel(gv_vector2i(i, line),
										gv_color(rg[0], rg[1], ba[0], ba[1]));
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(n3.z, -1, 1), rg);
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(n3.z, -1, 1), ba);
					output[3].set_pixel(gv_vector2i(i, line),
										gv_color(rg[0], rg[1], ba[0], ba[1]));
				}
				else
				{
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(rot.x, -1, 1), rg);
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(rot.y, -1, 1), ba);
					output[2].set_pixel(gv_vector2i(i, line),
										gv_color(rg[0], rg[1], ba[0], ba[1]));
					// set the rotation zw
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(rot.z, -1, 1), rg);
					gvt_encode_zero_to_one_into_byte(
						gvt_normalize_value< gv_float >(rot.w, -1, 1), ba);
					output[3].set_pixel(gv_vector2i(i, line),
										gv_color(rg[0], rg[1], ba[0], ba[1]));
				}
			} // next vertex
		}	 // next frame
		output_ani_name.push_back(*pseq->get_name().get_id().string());
	} // next sequence
	// test code
	output_ani_length.add(line);
	output_ani_name.add("end");

	gv_string_tmp s;
	error_message.write_array(output_ani_length);
	error_message.write_array(output_ani_name);

	return 1;
};

GVM_IMP_CLASS(gv_model_node, gv_object)
GVM_VAR(gvt_ptr< gv_model_node >, m_parent)
GVM_VAR(gv_matrix44, m_local_tm)
GVM_VAR(gv_matrix44, m_world_tm)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_model_node > >, m_children)
GVM_END_CLASS

GVM_IMP_CLASS(gv_model, gv_resource)
GVM_VAR(gvt_ref_ptr< gv_model_node >, m_root_node)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_static_mesh > >, m_static_mesh)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_skeletal_mesh > >, m_skeletal_mesh)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_ani_set > >, m_skeletal_ani_set)
GVM_VAR(gvt_array< gv_matrix44 >, m_camera_model_tm)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_texture > >, m_textures)
GVM_VAR(gv_float, m_max_half_size)
GVM_VAR_TOOLTIP("this is used to compress model &  data , including position "
				"after apply animation , should be the value at least "
				"max(max_coord, abs(min_coord))")
GVM_END_CLASS;
}