#pragma once

namespace gv
{
class gv_ani_set;
class gv_model_node : public gv_object
{
public:
	GVM_DCL_CLASS(gv_model_node, gv_object);
	gv_model_node();
	~gv_model_node();
	inline gv_model_node* get_parent()
	{
		return m_parent;
	};
	inline gv_int get_nb_child()
	{
		return m_children.size();
	};
	inline gv_model_node* get_nth_child(int i)
	{
		return m_children[i];
	}
	inline gv_matrix44 get_local_tm()
	{
		return m_local_tm;
	}
	inline gv_matrix44 get_world_tm()
	{
		return m_world_tm;
	}
	inline void set_local_tm(const gv_matrix44& m)
	{
		m_local_tm = m;
	}
	inline void set_world_tm(const gv_matrix44& m)
	{
		m_world_tm = m;
	}

	void set_parent(gv_model_node* p);
	void update_world_tm();
	void update_local_tm();
	gv_model_node* find_node(const gv_id& node_name);

public:
	gvt_ptr< gv_model_node > m_parent;
	gv_matrix44 m_local_tm;
	gv_matrix44 m_world_tm;

public:
	gvt_array< gvt_ref_ptr< gv_model_node > > m_children;
};

//
class gv_model : public gv_resource
{
	// the model is actually a  scene;
	friend class gv_impexp_3ds;
	friend class gv_impexp_fbx;

public:
	GVM_DCL_CLASS(gv_model, gv_resource);
	gv_model();
	~gv_model();
	int get_nb_static_mesh()
	{
		return m_static_mesh.size();
	};
	int get_nb_skeletal_mesh()
	{
		return m_skeletal_mesh.size();
	};
	gv_static_mesh* get_static_mesh(int idx)
	{
		return m_static_mesh[idx];
	};
	gv_skeletal_mesh* get_skeletal_mesh(int idx)
	{
		return m_skeletal_mesh[idx];
	};
	gv_material* get_material(const gv_id& id)
	{
		for (int i = 0; i < m_materials.size(); i++)
			if (m_materials[i]->get_name_id() == id)
				return m_materials[i];
		return NULL;
	}
	gv_texture* get_texture_by_file_name(const gv_string_tmp& s)
	{
		for (int i = 0; i < this->m_textures.size(); i++)
		{
			if (this->m_textures[i]->get_file_name() == s)
				return this->m_textures[i];
		}
		return NULL;
	};
	void add_texture(gv_texture* pset)
	{
		m_textures.push_back(pset);
	}
	void add_animation(gv_ani_set* pset)
	{
		m_skeletal_ani_set.push_back(pset);
	}
	gv_ani_set* get_animation(gv_int idx)
	{
		if (idx >= m_skeletal_ani_set.size())
			return NULL;
		return m_skeletal_ani_set[idx];
	}
	gv_int get_nb_animation()
	{
		return m_skeletal_ani_set.size();
	}
	void add_material(gv_material* mat)
	{
		this->m_materials.push_back(mat);
	}
	void add_static_mesh(gv_static_mesh* mesh)
	{
		this->m_static_mesh.push_back(mesh);
	}
	void add_skeletal_mesh(gv_skeletal_mesh* mesh)
	{
		this->m_skeletal_mesh.push_back(mesh);
	}

public:
	// bake the skeletal animation data into textures
	bool build_morph_texture(
		gv_int ani_index, gv_int skeletal_mesh_index,
		gvt_array< gv_image_2d >& output, gvt_array< gv_int >& output_ani_length,
		gvt_array< gv_string >& output_ani_name, int image_size,
		gvt_array< gv_vector3 >& output_original_vertex,
		gvt_array< gv_vector3 >& output_original_normal, bool output_normal_offset,
		gv_string_tmp& error_message, float debug_offset = 0,
		float old_diff_factor = 1, bool remove_root_motion = false);

public:
	gvt_ref_ptr< gv_model_node > m_root_node;
	gvt_array< gvt_ref_ptr< gv_static_mesh > > m_static_mesh;
	gvt_array< gvt_ref_ptr< gv_skeletal_mesh > > m_skeletal_mesh;
	gvt_array< gvt_ref_ptr< gv_ani_set > > m_skeletal_ani_set;
	gvt_array< gv_matrix44 > m_camera_model_tm;
	gvt_array< gvt_ref_ptr< gv_texture > > m_textures;
	gvt_array< gvt_ref_ptr< gv_material > > m_materials;
	gv_float m_max_half_size;
};
}