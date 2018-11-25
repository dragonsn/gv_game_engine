#include "gv_base\math\gvt_image_2d.h"
namespace gv
{
}

namespace unit_test_image_process
{
/*
	GVM_DCL_MMO_CELL_TYPE(road        ,gv_color(255,255,255,255),true )
	GVM_DCL_MMO_CELL_TYPE(cliff		  ,gv_color(126, 107,90,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(grass		  ,gv_color(0,255, 0   ,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(sand		  ,gv_color(255,255, 0 ,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(building	  ,gv_color(255,0,  255,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(water		  ,gv_color(0,0,    255,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(lava		  ,gv_color(255,0,   0 ,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(mud		  ,gv_color(128,64,  64,255),true ) 
	GVM_DCL_MMO_CELL_TYPE(max_type	  ,gv_color(0,  0,   0 ,255),true ) 
	*/

void main(gvt_array< gv_string >& args)
{
	using namespace std;
	gv_unit_test_context_guard context;
	gv_image_2d image;
	image.set_sandbox(m_sandbox);
	if (image.import_file(FILE_TEX_C_BMP))
	{
		gv_image_2d image_mask;
		image_mask.set_sandbox(m_sandbox);
		image_mask.init_image(gv_color(0, 0, 0, 0), image.get_image_size());

		for (int iy = 0; iy < image.get_image_size().get_y(); iy++)
			for (int ix = 0; ix < image.get_image_size().get_x(); ix++)
			{
				gv_color color = image.get_pixel(gv_vector2i(ix, iy));
				gv_color mask(0, 0, 0, 0);
				if (color == gv_color(126, 107, 90, 255))
				{
					mask.r = 255;
				}
				if (color == gv_color(0, 255, 0, 255))
				{
					mask.g = 255;
				}
				if (color == gv_color(128, 64, 64, 255))
				{
					mask.b = 255;
				}
				if (color == gv_color(255, 255, 0, 255))
				{
					mask.a = 255;
				}
				image_mask.get_pixel(gv_vector2i(ix, iy)) = mask;
			}
		image_mask.blur_self();
		image_mask.blur_self();
		image_mask.export_file("map_mask_o.tga");
		gvt_image_2d< gv_colorf > float_mask;
		gvt_export_to_image(image_mask, float_mask);
		gvt_image_2d< gv_colorf > float_mask2;
		float_mask.resize_image(float_mask2, gv_vector2i(512, 512), true);
		gvt_import_from_image(image_mask, float_mask2);
		image_mask.export_file("map_mask.tga");
		//============================================================================================
		//								:
		//============================================================================================
		//build height map;
		gvt_image_2d< gv_colorf > height_map;
		height_map.set_image_size(image.get_image_size());
		for (int iy = 0; iy < image.get_image_size().get_y(); iy++)
			for (int ix = 0; ix < image.get_image_size().get_x(); ix++)
			{
				gv_color color = image.get_pixel(gv_vector2i(ix, iy));
				gv_colorf height(0, 0, 0, 1);
				if (color == gv_color(126, 107, 90, 255)) //cliff
				{
					height = 0.8f;
				}
				if (color == gv_color(0, 255, 0, 255))
				{
					height = 0.2f;
				}
				if (color == gv_color(128, 64, 64, 255))
				{
					height = 0.205f;
				}
				if (color == gv_color(255, 255, 0, 255))
				{
					height = 0.19f;
				}
				if (color == gv_color(255, 255, 255, 255))
				{
					height = 0.21f;
				}
				height.a = 1.0f;
				height_map.set_pixel(gv_vector2i(ix, iy), height);
			}
		gvt_import_from_image(image_mask, height_map);
		image_mask.export_file("height_map.tga");
	}
}
}