#pragma once
namespace gv
{

class gv_texture : public gv_resource
{
	friend class gv_image_2d;

public:
	GVM_DCL_CLASS(gv_texture, gv_resource);
	gv_texture();
	~gv_texture();
	virtual gv_int import_from_external_file(const char* file_name,
											 gv_text& error);
	const gv_byte* get_texture_data();
	const gv_int get_texture_data_size();
	bool load_into_memory();

	inline bool is_file_in_memory()
	{
		return m_is_file_in_memory;
	}
	inline bool is_volume_texture()
	{
		return m_is_volume_texture;
	}
	inline bool is_cube_texture()
	{
		return m_is_cube_texture;
	}
	inline void set_volume_texture(gv_bool b)
	{
		m_is_volume_texture = b;
	}
	inline void set_cube_texture(gv_bool b)
	{
		m_is_cube_texture = b;
	}

protected:
	gve_pixel_format m_format;
	gv_bool m_is_file_in_memory;
	gvt_array< gv_byte > m_file_image;
	gv_bool m_is_volume_texture;
	gv_bool m_is_cube_texture;
};
}