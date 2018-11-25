#include "gv_framework_private.h"
#include "gv_render_resource.h"
#include "gv_render_option.h"
#include "gv_renderer.h"
#include "gv_component_renderer.h"
#include "../renderer/gv_image.h"
#include "../renderer/gv_texture.h"

namespace gv
{

gv_image_2d::gv_image_2d()
{
	GVM_SET_CLASS(gv_image_2d);
};

gv_image_2d::~gv_image_2d(){};

bool gv_image_2d::init_image(const gv_color& init_color,
							 const gv_vector2i& image_size)
{
	m_image_size = image_size;
	this->m_pixels.resize(image_size.x * image_size.y);
	this->m_pixels = init_color;
	return true;
}

void gv_image_2d::clear_alpha_channel(gv_byte value)
{
	for (int i = 0; i < this->m_pixels.size(); i++)
	{
		m_pixels[i].a = value;
	}
};

bool gv_image_2d::import_file(const char* name)
{
	return m_sandbox->import_external_format(this, name);
};

bool gv_image_2d::export_file(const char* name)
{
	return m_sandbox->export_external_format(this, name);
};

gv_vector2i gv_image_2d::get_image_size()
{
	return m_image_size;
};

void gv_image_2d::set_pixel(const gv_vector2i& pos, const gv_color& col)
{
	gvt_set_pixel(col, pos.x, pos.y, m_pixels, m_image_size.x, m_image_size.y);
};

const gv_color& gv_image_2d::get_pixel(const gv_vector2i& pos) const
{
	return gvt_get_pixel(pos.x, pos.y, m_pixels, m_image_size.x, m_image_size.y);
};

gv_color& gv_image_2d::get_pixel(const gv_vector2i& pos)
{
	return gvt_get_pixel(pos.x, pos.y, m_pixels, m_image_size.x, m_image_size.y);
};

gv_texture* gv_image_2d::create_texture()
{
	gv_string_tmp temp_file = "temp.tga";
	gv_texture* ptex = this->get_sandbox()->create_object< gv_texture >(this);
	this->export_file(*temp_file);
	ptex->set_file_name(*temp_file);
	ptex->load_into_memory();
	return ptex;
};

void gv_image_2d::blur_self(int mask_width, int mask_height, float coeff)
{
	gvt_array< gv_colorf > img_copy;
	gvt_array< gv_colorf > img_blur;
	img_copy = this->m_pixels;
	img_blur.resize(img_copy.size());
	gvt_gaussian_blur((gv_vector4*)img_copy.begin(),
					  (gv_vector4*)img_blur.begin(), m_image_size.get_x(),
					  m_image_size.get_y(), mask_width, mask_height, coeff);
	this->m_pixels = img_blur;
};

void gv_image_2d::copy_image(const gv_image_2d& image)
{
	m_pixels = image.m_pixels;
	m_image_size = image.m_image_size;
};

GVM_IMP_CLASS(gv_image_2d, gv_resource)
GVM_VAR(gv_vector2i, m_image_size)
GVM_VAR(gvt_array< gv_color >, m_pixels)
GVM_END_CLASS
}