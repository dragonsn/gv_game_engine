#pragma once
namespace gv
{
//======================================================================
class gv_effect;
class gv_material_tex
{
public:
	gv_material_tex()
	{
		m_synced = false;
		m_use_tiling = false;
		m_use_mipmap = false;
	};
	gv_material_tex(const gv_material_tex& s)
	{
		*this = s;
	};
	~gv_material_tex(){};
	gv_material_tex& operator=(const gv_material_tex& m)
	{
		m_texture = m.m_texture;
		m_id = m.m_id;
		m_use_tiling = m.m_use_tiling;
		m_use_mipmap = m.m_use_mipmap;
		m_file_name = m.m_file_name;
		return *this;
	}
	gvt_ref_ptr< gv_texture > m_texture;
	gv_id m_id;
	gv_bool m_synced;
	gv_bool m_use_tiling;
	gv_bool m_use_mipmap;
	gv_text m_file_name;
};

//======================================================================
class gv_material_param_float4
{
public:
	gv_material_param_float4()
	{
		m_synced = false;
		m_data = gv_vector4::get_zero_vector();
		m_count = 4;
	};
	gv_material_param_float4(const gv_material_param_float4& s)
	{
		*this = s;
	};
	gv_material_param_float4& operator=(const gv_material_param_float4& m)
	{
		m_id = m.m_id;
		m_data = m.m_data;
		m_count = m_count;
		return *this;
	}
	gv_id m_id;
	gv_vector4 m_data;
	gv_int m_count;
	gv_bool m_synced;
};
//======================================================================
class gv_material : public gv_resource
{
	friend class gv_effect;
	friend class gv_renderer;

public:
	GVM_DCL_CLASS(gv_material, gv_resource);
	gv_material();
	~gv_material();
	gv_material_tex* get_material_texture(const gv_id& texture);
	void add_material_texture(gv_material_tex& p)
	{
		m_material_textures.push_back(p);
	}
	gv_material* clone(gv_sandbox* sandbox);
	void update_material_texture(gv_material_tex& p);
	void bind_effect(gv_effect*);
	void unbind_effect();
	gv_effect* get_effect();
	gv_effect* get_effect_instance();
	void set_effect(gv_effect* e);
	void create_effect_instance();
	bool precache(gv_renderer*);
	void uncache();
	bool is_precached();
	void add_texture(const char* file_name, const char* id = NULL);
	gv_int get_nb_sub_material();
	void set_nb_sub_material(gv_int i);
	void add_sub_material(gv_material*);
	gv_material* get_sub_material(gv_int idx);
	void set_sub_material(gv_int idx, gv_material*);
	gv_id get_default_texture_id(gv_int idx);
	gv_material_param_float4* get_param_float4(const gv_id& name);
	void update_param_float4(gv_material_param_float4 param);

public:
	gvt_array< gv_material_tex > m_material_textures;
	gvt_array< gv_material_param_float4 > m_material_float_params;
	gvt_ref_ptr< gv_effect > m_effect;
	gvt_array< gvt_ref_ptr< gv_material > > m_sub_material;
	// color for old none shader render;
	gv_colorf m_diffuse_color;
	gv_colorf m_specular_color;
	gv_colorf m_ambient_color;
	gv_float m_opacity;
	gv_bool m_is_precached;
};
}
