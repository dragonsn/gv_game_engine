namespace gv
{
template < class T >
T gvt_get_pixel(int x, int y, T* pimg, int w, int l)
{
	x = gvt_max(0, x);
	x = gvt_min(x, w - 1);
	y = gvt_max(0, y);
	y = gvt_min(y, l - 1);
	return pimg[x + y * (w)];
};
template < class T >
const T& gvt_get_pixel(int x, int y, const gvt_array< T >& pimg, int w, int l)
{

	x = gvt_max(0, x);
	x = gvt_min(x, w - 1);
	y = gvt_max(0, y);
	y = gvt_min(y, l - 1);
	return pimg[x + y * (w)];
};

template < class T >
T& gvt_get_pixel(int x, int y, gvt_array< T >& pimg, int w, int l)
{

	x = gvt_max(0, x);
	x = gvt_min(x, w - 1);
	y = gvt_max(0, y);
	y = gvt_min(y, l - 1);
	return pimg[x + y * (w)];
};

template < class T >
void gvt_set_pixel(T t, int x, int y, T* pimg, int w, int l)
{
	x = gvt_max(0, x);
	x = gvt_min(x, w - 1);
	y = gvt_max(0, y);
	y = gvt_min(y, l - 1);
	pimg[x + y * w] = t;
};

template < class T >
void gvt_set_pixel(T t, int x, int y, gvt_array< T >& pimg, int w, int l)
{
	x = gvt_max(0, x);
	x = gvt_min(x, w - 1);
	y = gvt_max(0, y);
	y = gvt_min(y, l - 1);
	pimg[x + y * w] = t;
};

template < class T >
void gvt_get_gaussian_kernal(T& mask_data, int mask_width, int mask_height,
							 float coeff = -0.35f)
{
	mask_data.resize(mask_height * mask_width);

	gv_float mult = 0.0f;
	for (int y = 0; y < mask_height; y++)
	{
		for (int x = 0; x < mask_width; x++)
		{
			gv_float cx = (gv_float)x - (gv_float)(mask_width - 1) / 2.0f;
			gv_float cy = (gv_float)y - (gv_float)(mask_height - 1) / 2.0f;
			gv_float r = cx * cx + cy * cy;
			mult += expf(coeff * r);
		}
	}
	mult = 1.0f / mult;
	for (int y = 0; y < mask_height; y++)
	{
		for (int x = 0; x < mask_width; x++)
		{
			gv_float cx = (gv_float)x - (gv_float)(mask_width - 1) / 2.0f;
			gv_float cy = (gv_float)y - (gv_float)(mask_height - 1) / 2.0f;
			gv_float r = cx * cx + cy * cy;
			mask_data[y * mask_width + x] = mult * expf(coeff * r);
		}
	}
}

template < class T >
void gvt_gaussian_blur(T* img_input, T* img_output, int w, int h,
					   int mask_width = 5, int mask_height = 5,
					   float coeff = -0.35f)
{
	gv_float* mask_data = new gv_float[mask_width * mask_height];
	gv_float mult = 0.0f;
	for (int y = 0; y < mask_height; y++)
	{
		for (int x = 0; x < mask_width; x++)
		{
			gv_float cx = (gv_float)x - (gv_float)(mask_width - 1) / 2.0f;
			gv_float cy = (gv_float)y - (gv_float)(mask_height - 1) / 2.0f;
			gv_float r = cx * cx + cy * cy;
			mult += expf(coeff * r);
		}
	}
	mult = 1.0f / mult;
	for (int y = 0; y < mask_height; y++)
	{
		for (int x = 0; x < mask_width; x++)
		{
			gv_float cx = (gv_float)x - (gv_float)(mask_width - 1) / 2.0f;
			gv_float cy = (gv_float)y - (gv_float)(mask_height - 1) / 2.0f;
			gv_float r = cx * cx + cy * cy;
			mask_data[y * mask_width + x] = mult * expf(coeff * r);
		}
	}
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			T d;
			d = 0;
			for (int yy = 0; yy < mask_height; yy++)
			{
				for (int xx = 0; xx < mask_width; xx++)
				{
					T src = gvt_get_pixel(x + xx - mask_width / 2,
										  y + yy - mask_height / 2, img_input, w, h);
					d += src * mask_data[xx + yy * mask_width];
				}
			}

			gvt_set_pixel(d, x, y, img_output, w, h);
		}
	}
	delete[](mask_data);
}
}