#pragma once
namespace gv
{
//============================================================================================
//								:old terrain
//treasure
//============================================================================================
#define VARIANCE_DEPTH \
	10 // 2^0------->2^12 , but the last variance is not useful
#define VARIANCE_FULL_TREE_SIZE ((1 << VARIANCE_DEPTH))
#define GV_ROAM_PATCH_SIZE 32 // should change after adjust size of patch
#define GV_ROAM_PATCH_SIZE_SQUARED (GV_ROAM_PATCH_SIZE * GV_ROAM_PATCH_SIZE)
#define ROAM_MAX_DISPLAY_NODE 65536
#define GV_ROAM_PATCH_VTX_SIZE \
	(GV_ROAM_PATCH_SIZE + 1) * (GV_ROAM_PATCH_SIZE + 1)
#define GV_TERRAIN_FUR_PATCH_SIZE 16
//============================================================================================
//								:
//============================================================================================
class gv_terrain_grass_layer_info
{
public:
	gv_terrain_grass_layer_info(){};
	gv_terrain_grass_layer_info(const gv_terrain_grass_layer_info& i)
	{
		*this = i;
	}
	gv_terrain_grass_layer_info& operator=(const gv_terrain_grass_layer_info& i)
	{
		grass_material = i.grass_material;
		fade_out_radius = i.fade_out_radius;
		is_look_at = i.is_look_at;
		grass_size = i.grass_size;
		return *this;
	}
	~gv_terrain_grass_layer_info(){};
	//==================================================================================
	gvt_ref_ptr< gv_material > grass_material;
	gv_float fade_out_radius;
	gv_bool is_look_at;
	gv_vector2 grass_size;
};
//============================================================================================
//								:
//============================================================================================
class gv_terrain_mesh_layer_info
{
public:
	gv_terrain_mesh_layer_info()
	{
	}
	gv_terrain_mesh_layer_info(const gv_terrain_mesh_layer_info& layer)
	{
		*this = layer;
	}
	gv_terrain_mesh_layer_info&
	operator=(const gv_terrain_mesh_layer_info& layer)
	{
		mesh = layer.mesh;
		is_colidable = layer.is_colidable;
		return *this;
	}
	//==================================================================================
	gvt_ref_ptr< gv_com_static_mesh > mesh;
	gv_bool is_colidable;
};
//============================================================================================
//								:
//============================================================================================
class gv_terrain_fur_layer_info
{
public:
	gv_terrain_fur_layer_info()
	{
		fur_pass = 0;
	}
	gv_terrain_fur_layer_info(const gv_terrain_fur_layer_info& i)
	{
		*this = i;
	}
	gv_terrain_fur_layer_info& operator=(const gv_terrain_fur_layer_info& layer)
	{
		fur_material = layer.fur_material;
		fur_density_tex = layer.fur_density_tex;
		fur_density_tex_src = layer.fur_density_tex_src;
		fur_effect = layer.fur_effect;
		fur_pass = layer.fur_pass;
		return *this;
	}
	gvt_ref_ptr< gv_material > fur_material;
	gvt_ref_ptr< gv_effect > fur_effect;
	gvt_ref_ptr< gv_texture > fur_density_tex;
	gv_text fur_density_tex_src;
	gv_int fur_pass;
	gvt_array< gv_bool > patch_mask;
};

//============================================================================================
//								:
//============================================================================================
#define GV_DOM_FILE "../component/gv_com_terrain_structs.h"
#define GVM_DOM_DECL
#define GVM_DOM_IMP_XML
#define GVM_DOM_IMP_SERIALIZE
#define GVM_DOM_STATIC_CLASS
#include "..\inc\gv_data_model_ex.h"
//============================================================================================
//								:
//============================================================================================
void clear_roam_node_stack();
inline gv_vector3 roam_to_world(const gv_vector3& v)
{
	return gv_vector3(v.y, v.z, v.x);
}

inline gv_vector3 world_to_roam(const gv_vector3& v)
{
	return gv_vector3(v.z, v.x, v.y);
}
class gv_roam_chunk;
class gv_material;
class gv_com_camera;
class gv_roam_patch;
class gv_static_mesh;
class gv_com_terrain_roam : public gv_com_graphic
{

public:
	GVM_DCL_CLASS(gv_com_terrain_roam, gv_com_graphic);
	gv_com_terrain_roam();
	~gv_com_terrain_roam();

public:
	virtual bool sync_to_entity(gv_entity*);
	virtual gv_box get_local_bbox();
	virtual gv_sphere get_local_bsphere();

	bool generator_from_height_map(const gv_float* pheightmap, gv_int w,
								   gv_int l);
	bool is_empty()
	{
		return m_map.size() == 0;
	}
	gv_float get_height_world(const gv_vector3& world_pos);
	inline const gv_vector2i& get_size_in_point()
	{
		return m_size_in_point;
	}
	inline const gv_vector2i& get_size_in_patch()
	{
		return m_size_in_patch;
	}
	inline void set_init_height_scale(gv_float f)
	{
		m_init_height_scale = f;
	}

	gv_float get_height(gv_int x, gv_int y);
	gv_float get_heightf(gv_float x, gv_float y);
	gv_float get_height_interpolated(gv_float x, gv_float y);
	gv_float get_height_smooth(gv_float x, gv_float y);

	void get_normal(gv_int x, gv_int y, gv_vector4b& norm, gv_vector4b& binormal,
					gv_vector4b& tangent);
	void get_normal(gv_int x, gv_int y, gv_vector3& norm);
	void get_normalf(gv_float x, gv_float y, gv_vector3& norm);
	void get_normal_interpolated(gv_float x, gv_float y, gv_vector3& norm);
	gv_vector3 get_normal_world(const gv_vector3& world_pos);

	bool intersect_world_line_seg(const gv_line_segment& line_seg, float& s);
	gv_byte* gen_light_map_with_sun(const gv_vector3& sun_dir, gv_int w,
									gv_int h);
	void set_wire_frame_on(bool b);
	void set_view_dependence(bool b);
	bool build_static_mesh(int detail_level = 1, bool keep_terrain = true);

	virtual bool set_resource(gv_resource* resource);
	bool init(gvt_array< gv_float >& pheightmap, gv_int w, gv_int l);
	bool init_from_raw_file(const gv_string_tmp& name, gv_int w = 0,
							gv_int h = 0);
	bool init_from_image_file(const gv_string_tmp& name);
	bool init_from_byte_array(gvt_array< gv_byte >& array, gv_int w = 0,
							  gv_int h = 0);
	void on_visible(class gv_com_camera*, gv_ulong visible_test_tag);

	bool fill_index(gv_int* pindex, gv_int& length, gv_int max_length);
	gv_int get_nb_visible_patch();
	gv_roam_patch* get_nth_visible_patch(gv_int idx);
	void get_nth_patch_index_buffer(gv_int idx, gv_int& offset, gv_int& size);
	gv_int get_nb_tri()
	{
		return m_tri_number;
	}

	inline void enable_wireframe_mode(gv_bool b)
	{
		m_is_wireframe_mode = b;
	};
	inline gv_bool is_wireframe_mode()
	{
		return m_is_wireframe_mode;
	};
	inline void set_lod_threshold(gv_float f)
	{
		m_lod_threshold = f;
	}
	inline gv_float get_lod_threshold()
	{
		return m_lod_threshold;
	}
	void rebuild_normal(gvt_array< gv_float >& height_map, gv_int w, gv_int h,
						gvt_array< gv_vector3 >& normals,
						gvt_array< gv_vector3 >& binormals,
						gvt_array< gv_vector3 >& tangents);
	void enable_island_generation(gv_vector2i center, gv_float height = 1.0f,
								  gv_float power = 2.0f);

	void attach_a_renderable(gv_com_graphic* com);
	void detach_a_renderable(gv_com_graphic* com);
	void update_attached_renderable(gv_com_graphic* com);

	// decoration layer
	gv_terrain_grass_layer_info* get_grass_layer(gv_int idx);
	gv_terrain_mesh_layer_info* get_mesh_layer(gv_int idx);
	gv_terrain_fur_layer_info* get_fur_layer(gv_int idx);
	inline gv_int get_nb_grass_layer()
	{
		return m_grass_layers.size();
	};
	inline gv_int get_nb_mesh_layer()
	{
		return m_mesh_layers.size();
	};
	inline gv_int get_nb_fur_layer()
	{
		return m_fur_layers.size();
	};
	gv_int add_grass_layer(const gv_terrain_grass_layer_info& info);
	gv_int add_mesh_layer(const gv_terrain_mesh_layer_info& info);
	gv_int add_fur_layer(const gv_terrain_fur_layer_info& info);
	gv_int remove_grass_layer(gv_int idx);
	gv_int remove_mesh_layer(gv_int idx);
	gv_int remove_fur_layer(gv_int idx);
	void update_grass_layer(gv_int layer, const gv_rect& position,
							gvt_array< gv_float >& pattern, gv_int operation,
							gv_float power = 1.f);
	void update_mesh_layer(gv_int layer, const gv_rect& position,
						   gvt_array< gv_float >& pattern, gv_int operation,
						   gv_float power = 1.f);
	void update_fur_layer(gv_int layer, const gv_rect& position,
						  gvt_array< gv_float >& pattern, gv_int operation,
						  gv_float power = 1.f);

	void add_grass_layer(gv_material* material, gv_float density,
						 gv_float normal_factor, gv_vector2 height_range,
						 float grass_height, float grass_width);
	inline void enable_export_normal_map(bool b = true)
	{
		m_do_export_normal_map = b;
	}
	gv_bool is_dynamic_LOD_on()
	{
		return m_enable_dynamic_LOD;
	}
	void enable_dynamic_LOD(gv_bool b = true)
	{
		m_enable_dynamic_LOD = b;
	}

	void select_all_patch();
	void unselect_all_patch();
	void select_patch(gv_roam_patch* patch, bool b);

	gv_vector3 world_to_terrain_local(const gv_vector3& pos);
	gv_vector3 terrain_local_to_world(const gv_vector3& pos);
	gv_rect world_to_terrain_local(const gv_box& world_aabb);
	gv_box terrain_local_to_world(const gv_rect& terrain_rect);

protected:
	void clear();
	void reset();
	void link_patch();
	void release_map();
	void tessellate(gv_com_camera* pcamera);
	void build_island(gvt_array< gv_float >& pheightmap, gv_int w, gv_int l);
	gv_int build_grass_mesh(int nb_grass, gv_static_mesh* static_mesh,
							gv_roam_patch* patch, float grass_height,
							float grass_width, float normal_factor,
							gv_vector2 height_range);

public:
	class gv_roam_patch* get_patch(const gv_vector2i& pos_in_patch);
	gv_roam_patch* get_patch(gv_int ix, gv_int iy);
	gv_roam_patch* get_patch_world(const gv_vector3& world_pos);
	gv_roam_patch* get_patch_unsafe(gv_int ix, gv_int iy);
	class gv_roam_patch* get_patch_by_point_pos(const gv_vector2i& pos_in_point);
	void collect_patch(const gv_line_segment& line,
					   gvt_array_cached< gv_roam_patch*, 256 >& result);
	void collect_patch(const gv_box& box,
					   gvt_array_cached< gv_roam_patch*, 256 >& result);
	void collect_patch(const gv_rect& terrain_rect,
					   gvt_array_cached< gv_roam_patch*, 256 >& result);
	bool intersect_patch_line(const gv_line_segment& line_seg,
							  gv_vector3* hit_point, gv_bool* hit, gv_int ix,
							  gv_int iy);
	virtual void switch_wireframe();

public:
	void update_heightmap(const gv_rect& position, gvt_array< gv_float >& pattern,
						  gv_int operation, gv_float power = 1.f);
	void rebuild_editor_data();
	void rebuild_normal_texture();

private:
	//-----------------------persistent-----------------------
	gvt_array< gv_roam_patch > m_map;
	gv_bool m_enable_dynamic_LOD;
	gv_vector2i m_size_in_point;
	gv_vector2i m_size_in_patch;
	gv_box m_world_aabb;
	gv_float m_lod_threshold;
	//-----------------------transient------------------------
	gv_bool m_updated;
	gv_int m_tri_number;
	gv_bool m_is_wireframe_mode;
	gv_float m_init_height_scale;
	gv_vector2i m_island_center;
	gv_bool m_island_generation_enabled;
	gv_bool m_do_export_normal_map;
	gv_float m_island_height;
	gv_float m_island_height_power;
	gvt_array_cached< gvt_ptr< gv_roam_patch >, 512 >
		m_visible_image; // just support one camera only by now.!!
	gvt_array< gvt_ptr< gv_object > > m_attached_renderable;

	gvt_array< gv_terrain_grass_layer_info > m_grass_layers;
	gvt_array< gv_terrain_mesh_layer_info > m_mesh_layers;
	gvt_array< gv_terrain_fur_layer_info > m_fur_layers;
	gvt_ref_ptr< gv_texture > m_normal_texure;

	//-----------------------transient and for editor only----
	gvt_array< gv_float > m_editor_height_map;
	gvt_array< gv_vector3 > m_editor_normal;
	gvt_array< gv_vector3 > m_editor_binormal;
	gvt_array< gv_vector3 > m_editor_tangent;

public:
	GVM_DCL_FUNC(scp_add_grass_layer)
	GVM_DCL_FUNC(scp_create_grass_mesh)
	GVM_DCL_FUNC(scp_add_mesh_layer)
	GVM_DCL_FUNC(scp_add_fur_layer)
	GVM_DCL_FUNC(scp_remove_grass_layer)
	GVM_DCL_FUNC(scp_remove_mesh_layer)
	GVM_DCL_FUNC(scp_remove_fur_layer)
};

//============================================================================================
//								:
//============================================================================================
class gv_roam_node
{
public:
	GVM_DCL_STRUCT(gv_roam_node);
	gv_roam_node();
	~gv_roam_node();

public:
	inline gv_roam_node* get_left_neighbour()
	{
		return m_left_neighbour;
	}
	inline gv_roam_node* get_right_neighbour()
	{
		return m_right_neighbour;
	}
	inline gv_roam_node* get_basic_neighbour()
	{
		return m_basic_neighbour;
	}
	inline gv_roam_node* get_left_child()
	{
		return m_left_child;
	};
	inline gv_roam_node* get_right_child()
	{
		return m_right_child;
	};

	inline void set_left_neighbour(gv_roam_node* p)
	{
		m_left_neighbour = p;
	}
	inline void set_right_neighbour(gv_roam_node* p)
	{
		m_right_neighbour = p;
	}
	inline void set_basic_neighbour(gv_roam_node* p)
	{
		m_basic_neighbour = p;
	}
	inline void set_left_child(gv_roam_node* p)
	{
		m_left_child = p;
	}
	inline void set_right_child(gv_roam_node* p)
	{
		m_right_child = p;
	}

	void release();
	void make_root(gv_vector2i& v0, gv_vector2i& v1, gv_vector2i& v2);
	void destroy_root();
	void force_split();
	void split();
	void reset();
	bool fill_index(gv_int* pindex, gv_int& length, gv_int max_length);

	inline gv_int get_idx_in_full_tree()
	{
		return m_idx_in_full_tree;
	}
	inline void set_idx_in_full_tree(gv_int idx)
	{
		m_idx_in_full_tree = idx;
	}

	void* operator new(size_t size);
	void operator delete(void*);

private:
	gvt_ptr< gv_roam_node > m_left_neighbour;
	gvt_ptr< gv_roam_node > m_right_neighbour;
	gvt_ptr< gv_roam_node > m_basic_neighbour;
	gvt_ptr< gv_roam_node > m_left_child;
	gvt_ptr< gv_roam_node > m_right_child;
	gv_int m_vi[3];
	gv_int m_idx_in_full_tree;
};

//============================================================================================
//								:
//============================================================================================
class gv_roam_chunk : public gv_resource
{
public:
	friend class gv_com_terrain_roam;
	friend class gv_roam_patch;
	friend class gv_com_roam_d3d;
	GVM_DCL_CLASS(gv_roam_chunk, gv_resource);
	gv_roam_chunk();
	~gv_roam_chunk();

protected:
	gv_float
	get_height(gv_int x,
			   gv_int y); // get height element from the local  xy coordinate
	gv_float
	get_height_unsafe(gv_int x,
					  gv_int y); // might out of range. no clamp , faster version
	void get_normal(gv_int x, gv_int y, gv_vector4b& norm, gv_vector4b& binormal,
					gv_vector4b& tangent);
	const gv_box& get_local_aabb();
	gv_int initialize(const gv_float* pheightmap, gv_int stride,
					  bool need_dynamic_lod);
	gv_int initialize_normal(const gv_vector3* normals,
							 const gv_vector3* binormals,
							 const gv_vector3* tangents, gv_int stride);
	void build_aabb();
	void compute_variance();
	gv_float compute_variance_recursively(const struct tri2d&,
										  gvt_array< gv_float >&);

protected:
	gvt_array< gv_float > m_variance_l;
	gvt_array< gv_float > m_variance_r;

public:
	gvt_array< gv_float > m_height_map;
	gvt_array< gv_vector4b > m_normals;
	gvt_array< gv_vector4b > m_binormals;
	gvt_array< gv_vector4b > m_tangents;
	gvt_ptr< gv_material > m_material;
	gv_box m_aabb;
	gv_bool m_is_cache_dirty;
	gv_uint m_pass_flag;
};
//============================================================================================
//								:
//============================================================================================

class gv_roam_patch
{
public:
	GVM_DCL_STRUCT(gv_roam_patch);
	friend class gv_com_terrain_roam;
	gv_roam_patch();
	~gv_roam_patch();
	gv_roam_patch(const gv_roam_patch& c)
	{
		(*this) = c;
	}
	gv_roam_patch& operator=(const gv_roam_patch& c)
	{
		m_index_buffer_offset = c.m_index_buffer_offset;
		m_index_buffer_size = c.m_index_buffer_size;
		m_offset = c.m_offset;
		m_world_aabb = c.m_world_aabb;
		m_chunk = c.m_chunk;
		m_chunk_name = c.m_chunk_name;
		m_is_visible = c.m_is_visible;
		m_terrain = c.m_terrain;
		m_grass_layer_instances = c.m_grass_layer_instances;
		m_mesh_layer_instances = c.m_mesh_layer_instances;
		return (*this);
	};

public:
	void tessellate(gv_float max_tolerate_err);
	void reset();
	gv_float get_height(gv_int x, gv_int y)
	{
		return this->get_chunk()->get_height(x, y);
	};
	void get_normal(gv_int x, gv_int y, gv_vector4b& norm, gv_vector4b& binormal,
					gv_vector4b& tangent)
	{
		return this->get_chunk()->get_normal(x, y, norm, binormal, tangent);
	};
	;
	bool fill_index_buffer(gv_int* pindex, gv_int& length, gv_int max_length);
	void update_world_aabb();
	inline gv_box& get_world_aabb()
	{
		return m_world_aabb;
	}
	inline gv_box get_local_aabb();
	inline bool is_visible()
	{
		return m_is_visible;
	}
	inline gv_roam_node* get_rroot()
	{
		return m_right_root;
	}
	inline gv_roam_node* get_lroot()
	{
		return m_left_root;
	}
	inline gv_roam_chunk* get_chunk()
	{
		return m_chunk;
	}
	inline void set_visibility(gv_bool b)
	{
		m_is_visible = b;
	}
	inline gv_bool get_visibility()
	{
		return m_is_visible;
	}
	inline const gv_vector2i& get_offset()
	{
		return m_offset;
	}
	gv_bool attach_a_renderable(gv_obj_node_ptr node);
	gv_bool detach_a_renderable(gv_obj_node_ptr node);
	void set_visible_tag(gv_ulong tag);
	gv_int get_index_in_terrain();

public:
	gv_bool m_is_selected;
	gvt_array< gvt_array< gv_vector4 > > m_grass_layer_instances;
	gvt_array< gvt_array< gv_vector4 > > m_mesh_layer_instances;

protected:
	void tessellate(gv_roam_node* proot, gvt_array< gv_float >& variance,
					gv_float max_tolerate_err);

protected:
	gv_int m_index_buffer_offset;
	gv_int m_index_buffer_size;
	gvt_ptr< gv_roam_node > m_left_root;
	gvt_ptr< gv_roam_node > m_right_root;
	gv_vector2i m_offset;
	gv_box m_world_aabb;
	gvt_ref_ptr< gv_roam_chunk > m_chunk;
	gv_object_name m_chunk_name;
	gv_bool m_is_visible;
	gvt_ptr< gv_com_terrain_roam > m_terrain;
	gv_obj_list m_renderables;
};
}