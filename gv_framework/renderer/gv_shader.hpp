namespace gv
{
//============================================================================================
//								:
//============================================================================================
gv_shader_const::gv_shader_const()
{
	GVM_ZERO_ME;
};
gv_shader_const::~gv_shader_const(){};

GVM_IMP_STRUCT(gv_shader_const)
GVM_VAR(gv_id, m_name)
GVM_VAR(gv_int, m_type)
GVM_VAR(gv_int, m_register)
GVM_VAR(gv_int, m_cached_effect_param_index)
GVM_END_STRUCT
//============================================================================================
//								:
//============================================================================================
gv_shader::gv_shader()
{
	GVM_SET_CLASS(gv_shader);
	m_compiled_key = 0;
	m_is_pixel_shader = false;
	m_is_asm = false;
	m_shader_type = gve_shader_type_vertex; 
};
gv_shader::~gv_shader(){};

gv_int gv_shader::get_nb_constant_var()
{
	return m_constant_table.size();
};

void gv_shader::set_file_name(const gv_string_tmp& s)
{
	super::set_file_name(s);
	gv_string main_name = gv_file_manager::get_main_name(*s); 
	if (main_name.strlen())
	{
		switch (main_name[0])
		{
		case 'c': m_shader_type = gv_shader::gve_shader_type_compute;
			break;
		case 'v':m_shader_type = gv_shader::gve_shader_type_vertex;
			break;
		case 'f':m_shader_type = gv_shader::gve_shader_type_pixel;
			break;
		}
	}
}

gv_shader_const* gv_shader::get_constant_var(gv_int index)
{
	return &m_constant_table[index];
};
gv_shader_const* gv_shader::get_constant_var(const gv_id& name)
{
	for (int i = 0; i < m_constant_table.size(); i++)
	{
		if (m_constant_table[i].m_name == name)
			return &m_constant_table[i];
	}
	return NULL;
};
void gv_shader::add_constant_var(const gv_shader_const& var)
{
	m_constant_table.add(var);
};
//============================================================================================
//								:
//============================================================================================
gv_int gv_shader::get_nb_sampler()
{
	return m_sampler_table.size();
};
gv_shader_const* gv_shader::get_sampler(gv_int index)
{
	return &m_sampler_table[index];
};
gv_int gv_shader::get_sampler_index(const gv_id& name)
{
	for (int i = 0; i < m_sampler_table.size(); i++)
	{
		if (m_sampler_table[i].m_name == name)
			return i;
	}
	return -1;
};
void gv_shader::add_sampler(const gv_shader_const& var)
{
	m_sampler_table.add(var);
};
//============================================================================================
//								:
//============================================================================================
const char* gv_shader::get_compiler_name()
{
	return *m_compiler_name;
};
void gv_shader::set_compiler_name(const gv_id& name)
{
	m_compiler_name = name;
};
const char* gv_shader::get_compile_target()
{
	return *m_compile_target;
};
void gv_shader::set_compile_target(const gv_id& name)
{
	m_compile_target = name;
};
const char* gv_shader::get_compile_entry()
{
	return *m_compile_entry;
};
void gv_shader::set_compile_entry(const gv_id& name)
{
	m_compile_entry = name;
};

const gv_text& gv_shader::get_source_code()
{
	return m_source_code;
};
void gv_shader::set_source_code(const gv_text& text)
{
	m_source_code = text;
};
//============================================================================================

GVM_IMP_CLASS(gv_shader, gv_resource)
GVM_VAR(gvt_array< gv_shader_const >, m_constant_table)
GVM_VAR(gvt_array< gv_shader_const >, m_sampler_table)
GVM_VAR(gv_text, m_source_code)
GVM_VAR(gv_text, m_compile_option)
GVM_VAR(gv_id, m_compiler_name)
GVM_VAR(gv_id, m_compile_target)
GVM_VAR(gv_id, m_compile_entry)
GVM_VAR(gvt_array< gv_byte >, m_binary_code)
GVM_VAR(gvt_array< gv_byte >, m_constant_data)
GVM_VAR(gv_ulong, m_compiled_key)
GVM_VAR(gv_bool, m_is_pixel_shader) //deprecated.
GVM_VAR(gv_int ,  m_shader_type)
GVM_VAR(gv_bool, m_is_asm)
GVM_END_CLASS
};