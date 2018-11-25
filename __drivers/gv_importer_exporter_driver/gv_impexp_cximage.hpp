#undef INTMAX_C
#undef UINTMAX_C

#if !(GVM_NOT_SUPPORT_XIMAGE)

#include "cximage/CxImage/ximage.h"

namespace gv
{
/*
CXIMAGE_FORMAT_UNKNOWN = 0,
CXIMAGE_FORMAT_BMP = 1,
CXIMAGE_FORMAT_GIF = 2,
CXIMAGE_FORMAT_JPG = 3,
CXIMAGE_FORMAT_PNG = 4,
CXIMAGE_FORMAT_ICO = 5,
CXIMAGE_FORMAT_TIF = 6,
CXIMAGE_FORMAT_TGA = 7,
CXIMAGE_FORMAT_PCX = 8,
CXIMAGE_FORMAT_WBMP = 9,
CXIMAGE_FORMAT_WMF = 10,
CXIMAGE_FORMAT_JP2 = 11,
CXIMAGE_FORMAT_JPC = 12,
CXIMAGE_FORMAT_PGX = 13,
CXIMAGE_FORMAT_PNM = 14,
CXIMAGE_FORMAT_RAS = 15,
CXIMAGE_FORMAT_JBG = 16,
CXIMAGE_FORMAT_MNG = 17,
CXIMAGE_FORMAT_SKA = 18,
CXIMAGE_FORMAT_RAW = 19,
CXIMAGE_FORMAT_PSD = 20,*/

static gv_int get_cx_image_type_from_file_name(const char* name)
{
	gv_file_manager* pfm = gv_global::fm.get();
	gv_string s = pfm->get_extension(name);
	s.to_upper();
	if (s == ".TGA")
	{
		return CXIMAGE_FORMAT_TGA;
	}
	else if (s == ".BMP")
	{
		return CXIMAGE_FORMAT_BMP;
	}
	else if (s == ".JPG")
	{
		return CXIMAGE_FORMAT_JPG;
	}
	else if (s == ".PNG")
	{
		return CXIMAGE_FORMAT_PNG;
	}
	else if (s == ".PSD")
	{
		return CXIMAGE_FORMAT_PSD;
	}
	return CXIMAGE_FORMAT_UNKNOWN;
}

class gv_impexp_cximage : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_cximage, gv_importer_exporter);
	gv_impexp_cximage()
	{
		link_class(gv_impexp_cximage::static_class());
	};
	~gv_impexp_cximage(){};

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_jpg);
		result.push_back(gv_id_tga);
		result.push_back(gv_id_png);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_image_2d::static_class();
	}

	bool do_import(const gv_string_tmp& fn, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_cximage__do_import, 0)
		gv_image_2d* pimage = gvt_cast< gv_image_2d >(target);
		if (!pimage)
			return false;
		GV_ASSERT(pimage);
		const char* name = *fn;
		gv_byte* buffer = NULL;
		gv_int type = get_cx_image_type_from_file_name(name);
		gv_string_tmp file_name;
		file_name = target->get_sandbox()->get_physical_resource_path(name);
		CxImage* image = new CxImage(*file_name, type);
		int size = image->GetWidth() * image->GetHeight() * 4;
		if (image->Encode2RGBA(buffer, size, true))
		{
			pimage->m_image_size.x = image->GetWidth();
			pimage->m_image_size.y = image->GetHeight();
			gv_byte* p = buffer;
			gv_int pixel_size = pimage->m_image_size.x * pimage->m_image_size.y;
			pimage->m_pixels.resize(pixel_size);
			for (int i = 0; i < pixel_size; i++)
			{
				pimage->m_pixels[i].r = *p++;
				pimage->m_pixels[i].g = *p++;
				pimage->m_pixels[i].b = *p++;
				pimage->m_pixels[i].a = *p++;
			}
		}
		else
		{
			GVM_ERROR("failed to import image 2d " << name << gv_endl);
			return false;
		}
		delete image;
		if (buffer)
			free(buffer);
		return true;
	};

	void get_export_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_jpg);
		result.push_back(gv_id_tga);
		result.push_back(gv_id_png);
	};
	gv_class_info* get_export_source_class()
	{
		return gv_image_2d::static_class();
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{

		GV_PROFILE_EVENT(gv_impexp_cximage__do_export, 0)
		gv_image_2d* pimage = gvt_cast< gv_image_2d >(source);
		GV_ASSERT(pimage);
		CxImage* newImage = new CxImage(0);
		// newImage->AlphaCreate();	// Create the alpha layer
		// newImage->IncreaseBpp(32);	// set image to 32bit
		gvt_array< gv_color > temp;
		temp = pimage->m_pixels;
		for (int i = 0; i < temp.size(); i++)
			temp[i].swap_red_blue();

		newImage->CreateFromArray((gv_byte*)&temp[0], pimage->m_image_size.x,
								  pimage->m_image_size.y, 32,
								  (pimage->m_image_size.x * 4), true);
		gv_byte* buffer = NULL;
		int size = 0;
		int type = get_cx_image_type_from_file_name(*file_name);
		newImage->Encode(buffer, size, type);
		gvt_array< gv_byte > the_array;
		the_array.init(buffer, size);
		gv_save_array_to_file(*file_name, the_array);
		if (buffer)
			free(buffer);
		newImage->Destroy();
		delete newImage;
		return true;
	};
};

GVM_IMP_CLASS(gv_impexp_cximage, gv_importer_exporter)
GVM_END_CLASS
}

#endif