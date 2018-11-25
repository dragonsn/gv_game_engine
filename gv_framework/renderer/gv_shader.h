#pragma once
namespace gv
{
class gv_shader;
//============================================================================================
//								:
//============================================================================================
enum gve_shader_data_type
{
	e_shader_data_float,
	e_shader_data_bool,
	e_shader_data_int,
	e_shader_data_vector2,
	e_shader_data_vector3,
	e_shader_data_vector4,
	e_shader_data_colorf,
	e_shader_data_matrix4,
	e_shader_data_matrix43,
	e_shader_data_sampler_2d,
	e_shader_data_sampler_1d,
	e_shader_data_sampler_3d,
	e_shader_data_sampler_cubemap,
	e_shader_data_dynamic,
	e_shader_data_text, // only for some debug info
};
//============================================================================================
//								:
//============================================================================================
struct gv_shader_const
{
	gv_shader_const();
	~gv_shader_const();
	GVM_IMP_STRUCT_ASSIGN(gv_shader_const);
	gv_id m_name;
	gve_shader_data_type m_type;
	gv_int m_register;
	gv_int m_register_count;
	gv_int m_semantic_idx;
	gv_int m_cached_effect_param_index;
};



class gv_shader : public gv_resource
{
public:
	
	GVM_DCL_CLASS(gv_shader, gv_resource);
	gv_shader();
	~gv_shader();
	enum shader_type
	{
		gve_shader_type_vertex,
		gve_shader_type_pixel,
		gve_shader_type_compute,
		gve_shader_type_count,
	};
	//============================================================================================
	//								:
	//============================================================================================
	virtual void set_file_name(const gv_string_tmp& s) override;
	gv_int get_nb_constant_var();
	gv_shader_const* get_constant_var(gv_int index);
	gv_shader_const* get_constant_var(const gv_id& name);
	void add_constant_var(const gv_shader_const& var);

	gv_int get_nb_sampler();
	gv_shader_const* get_sampler(gv_int index);
	gv_int get_sampler_index(const gv_id& name);
	void add_sampler(const gv_shader_const& var);

	const char* get_compiler_name();
	void set_compiler_name(const gv_id& name);
	const char* get_compile_target();
	void set_compile_target(const gv_id& name);

	const char* get_compile_entry();
	void set_compile_entry(const gv_id& name);

	const gv_text& get_source_code();
	void set_source_code(const gv_text& text);

	bool is_pixel_shader()
	{
		return m_is_pixel_shader;
	}
	void set_pixel_shader(gv_bool b)
	{
		m_is_pixel_shader = b;
		if (b) m_shader_type = gve_shader_type_pixel;
	}

	bool is_asm()
	{
		return m_is_asm;
	}
	void set_is_asm(gv_bool b)
	{
		m_is_asm = b;
	}

	//============================================================================================
	//								:member
	//============================================================================================
protected:
	gvt_array< gv_shader_const > m_constant_table;
	gvt_array< gv_shader_const > m_sampler_table;
	gvt_ptr< gv_shader >m_origin; 
	gvt_ptr< gv_effect > m_effect;
	gv_text m_source_code;
	gv_text m_compile_option;
	gv_id m_compile_entry;
	gv_id m_compiler_name;
	gv_id m_compile_target;
	shader_type m_shader_type; 
	gvt_array< gv_byte > m_binary_code;
	gvt_array< gv_byte > m_constant_data;
	gv_ulong m_compiled_key;
	gv_bool m_is_pixel_shader;
	gv_bool m_is_asm;
};
};