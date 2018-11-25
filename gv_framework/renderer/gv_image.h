#pragma once

namespace gv
{
enum gve_pixel_format
{
#define GVM_DCL_PIXEL_FORMAT_MAKE_ENUM
#include "gv_pixel_format.h"
};

template < class type_of_string >
inline gve_pixel_format gv_pixel_format_from_string(const type_of_string& s)
{
#define GVM_DCL_PIXEL_FORMAT_FROM_STRING
#include "gv_pixel_format.h"
	return gve_pixel_A8R8G8B8;
};

inline const char* get_pixel_format_name(gve_pixel_format f)
{
	switch (f)
	{
#define GVM_DCL_PIXEL_FORMAT_GET_STRING
#include "gv_pixel_format.h"
	}
	return "Unknown";
}

class gv_texture;
class gv_image_2d : public gv_resource
{
	friend class gv_com_terrain_roam;

public:
	GVM_DCL_CLASS(gv_image_2d, gv_resource);
	gv_image_2d();

	~gv_image_2d();

	bool init_image(const gv_color& init_color, const gv_vector2i& image_size);

	virtual bool import_file(const char* name);

	virtual bool export_file(const char* name);

	gv_vector2i get_image_size();

	const gv_color& get_pixel(const gv_vector2i& pos) const;

	gv_color& get_pixel(const gv_vector2i& pos);

	void set_pixel(const gv_vector2i& pos, const gv_color& col);

	virtual gv_texture* create_texture();

	void blur_self(int mask_width = 5, int mask_height = 5, float coeff = -0.35f);

	void copy_image(const gv_image_2d& image);

	void clear_alpha_channel(gv_byte value);

public:
	gv_vector2i m_image_size;
	gvt_array< gv_color > m_pixels;
};

template < class T >
class gvt_image_2d;

template < class T >
inline void gvt_import_from_image(gv_image_2d& i,
								  const gvt_image_2d< T >& image)
{
	image.export_image(i.m_image_size, i.m_pixels);
};

template < class T >
inline void gvt_export_to_image(gv_image_2d& i, gvt_image_2d< T >& image)
{
	image.import_image(i.m_image_size, i.m_pixels);
}
};
