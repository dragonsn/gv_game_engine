#pragma once
namespace gv
{

gv_texture::gv_texture()
{
	link_class(gv_texture::static_class());
	m_is_file_in_memory = false;
	m_is_volume_texture = false;
	m_is_cube_texture = false;
}

gv_texture::~gv_texture(){

};

const gv_byte* gv_texture::get_texture_data()
{
	return m_file_image.begin();
}
const gv_int gv_texture::get_texture_data_size()
{
	return m_file_image.size();
}

gv_int gv_texture::import_from_external_file(const char* file_name,
											 gv_text& error)
{
	gvt_array< gv_byte > array;
	if (!gv_load_file_to_array(file_name, array))
	{
		error = "failed to load the file; ";
		return 0;
	}
	this->m_is_file_in_memory = true;
	this->m_file_image = array;
	this->m_file_name = file_name;
	this->uncache();
	return 1;
};

bool gv_texture::load_into_memory()
{
	gv_text err;
	return this->import_from_external_file(*m_file_name, err) != 0;
};

GVM_IMP_CLASS(gv_texture, gv_resource)
GVM_VAR(gv_bool, m_is_file_in_memory)
GVM_VAR(gv_int, m_format)
GVM_VAR(gv_bool, m_is_volume_texture)
GVM_VAR(gv_bool, m_is_cube_texture)
GVM_VAR_ATTRIB_SET(hide_in_editor)
GVM_VAR(gvt_array< gv_byte >, m_file_image)
GVM_END_CLASS;
}