#pragma once
namespace gv
{
// a template class for 2d image , consider the any dimension
template < class T >
class gvt_image_2d : public gvt_vector_dynamic< T >
{

public:
	typedef T (gvt_image_2d::*sampler_call_back)(const gv_vector2& pos,
												 bool do_clamp) const;
	typedef void (gvt_image_2d::*plot_call_back)(const gv_vector2& pos,
												 const T& color, bool do_clamp);

	gvt_image_2d()
		: m_image_size(0)
	{
	}
	gvt_image_2d(const gvt_image_2d& img)
	{
		v = img.v;
		m_image_size = img;
	}
	//============================================================================================
	//								:
	//============================================================================================
	void init_image(gv_vector2i image_size, const T& color)
	{
		set_image_size(image_size);
		(*this) = color;
	}

	void set_image_size(gv_vector2i image_size)
	{
		m_image_size = image_size;
		set_vector_size(image_size.get_x() * image_size.get_y());
	}

	gv_vector2i get_image_size()
	{
		return m_image_size;
	}

	gv_int get_area() const
	{
		return m_image_size.get_x() * m_image_size.get_y();
	}

	template < class container >
	void import_image(gv_vector2i image_size, const container& c)
	{
		set_image_size(image_size);
		for (int i = 0; i < get_area(); i++)
		{
			(*this)[i] = c[i];
		}
	}

	template < class container >
	void export_image(gv_vector2i& image_size, container& c) const
	{
		image_size = m_image_size;
		c.resize(get_area());
		for (int i = 0; i < get_area(); i++)
		{
			c[i] = (*this)[i];
		}
	}
	static gv_vector2 pos_to_tex_coord(const gv_vector2& pos)
	{
		return pos / m_image_size;
	}
	static gv_vector2 tex_coord_to_pos(const gv_vector2& tex_coord)
	{
		return tex_coord * m_image_size;
	}
	//============================================================================================
	//								:get pixel
	//method
	//============================================================================================
	T get_pixel(const gv_vector2i& pos, bool do_clamp = true) const
	{
		gv_vector2i p2 = pos;
		if (do_clamp)
		{
			p2.x = gvt_clamp(p2.x, 0, m_image_size.x - 1);
			p2.y = gvt_clamp(p2.y, 0, m_image_size.y - 1);
		}
		else
		{
			p2.x = p2.x % m_image_size.x;
			p2.y = p2.y % m_image_size.y;
		}
		return get_pixel_unsafe(p2);
	}

	T get_pixel_unsafe(const gv_vector2i& pos) const
	{
		return (*this)[pos.get_x() + pos.get_y() * m_image_size.get_x()];
	}

	T get_pixel_point(const gv_vector2& pos, bool do_clamp = true) const
	{
		gv_vector2 fractional = gvt_floor(pos);
		gv_vector2i pos_i = fractional;
		return get_pixel(pos_i + gv_vector2i(0, 1), do_clamp);
	}

	T get_pixel_linear(const gv_vector2& pos, bool do_clamp = true) const
	{
		gv_vector2 fractional = gvt_floor(pos);
		gv_vector2i pos_i;
		pos_i = fractional;
		fractional = pos - fractional;

		T f1, f2;
		T ff1, ff2;
		// linear interpolate
		f1 = get_pixel(pos_i, do_clamp);
		f2 = get_pixel(pos_i + gv_vector2i(1, 0), do_clamp);
		ff1 = gvt_interpolate(fractional.get_x(), f1, f2);

		// right interpolate
		f1 = get_pixel(pos_i + gv_vector2i(0, 1), do_clamp);
		f2 = get_pixel(pos_i + gv_vector2i(1, 1), do_clamp);
		ff2 = gvt_interpolate(fractional.get_x(), f1, f2);

		// interpolate the result of left&right interpolate
		return gvt_interpolate(fractional.get_y(), ff1, ff2);
	}
	//============================================================================================
	//								:
	//============================================================================================
	void set_pixel(const gv_vector2i& pos, const T& color, bool do_clamp = true)
	{
		gv_vector2i p2 = pos;
		if (do_clamp)
		{
			p2.x = gvt_clamp(p2.x, 0, m_image_size.x - 1);
			p2.y = gvt_clamp(p2.y, 0, m_image_size.y - 1);
		}
		else
		{
			p2.x = p2.x % m_image_size.x;
			p2.y = p2.y % m_image_size.y;
		}
		set_pixel_unsafe(p2, color);
	}

	const T set_pixel_unsafe(const gv_vector2i& pos, const T& color)
	{
		return (*this)[pos.get_x() + pos.get_y() * m_image_size.get_x()] = color;
	}
	//============================================================================================
	//								:
	//============================================================================================
	template < class type_of_new_image >
	void
	resize_image(type_of_new_image& result, gv_vector2i new_size,
				 bool use_clamp = true,
				 sampler_call_back call_back = &gvt_image_2d::get_pixel_linear)
	{
		result.set_image_size(new_size);
		gv_vector2 delta;
		delta = m_image_size;
		gv_vector2 new_s;
		new_s = new_size;
		delta /= new_s;
		gv_vector2 cu_pos(0);
		gv_int idx = 0;
		for (int iy = 0; iy < new_size.get_y(); iy++)
		{
			cu_pos.x = 0;
			for (int ix = 0; ix < new_size.get_x(); ix++, idx++)
			{
				result[idx] = ((this->*call_back)(cu_pos, use_clamp));
				cu_pos.x += delta.x;
			}
			cu_pos.y += delta.y;
		}
	}
	//============================================================================================
	//								:
	//============================================================================================
	// http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
	/*void Line( const float x1, const float y1, const float x2, const float y2,
  const Color& color )
  {
  // Bresenham's line algorithm
  const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
  if(steep)
  {
  std::swap(x1, y1);
  std::swap(x2, y2);
  }

  if(x1 > x2)
  {
  std::swap(x1, x2);
  std::swap(y1, y2);
  }

  const float dx = x2 - x1;
  const float dy = fabs(y2 - y1);

  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;

  const int maxX = (int)x2;

  for(int x=(int)x1; x<maxX; x++)
  {
  if(steep)
  {
  SetPixel(y,x, color);
  }
  else
  {
  SetPixel(x,y, color);
  }

  error -= dy;
  if(error < 0)
  {
  y += ystep;
  error += dx;
  }
  }
  }*/
	/*import image: Color, Image;

  void circle(Image img, int x0, int y0, int radius, Color color=Color.white) {
  int f = 1 - radius;
  int ddf_x = 1;
  int ddf_y = -2 * radius;
  int x = 0;
  int y = radius;
  img[y0 + radius][x0] = color;
  img[y0 - radius][x0] = color;
  img[y0][x0 + radius] = color;
  img[y0][x0 - radius] = color;

  while (x < y) {
  if (f >= 0) {
  y--;
  ddf_y += 2;
  f += ddf_y;
  }
  x++;
  ddf_x += 2;
  f += ddf_x;
  img[y0 + y][x0 + x] = color;
  img[y0 + y][x0 - x] = color;
  img[y0 - y][x0 + x] = color;
  img[y0 - y][x0 - x] = color;
  img[y0 + x][x0 + y] = color;
  img[y0 + x][x0 - y] = color;
  img[y0 - x][x0 + y] = color;
  img[y0 - x][x0 - y] = color;
  }
  }

  void main() {
  auto img = new Image(25, 25);
  circle(img, 12, 12, 12);
  img.textView();
  }
  */

	// http://members.chello.at/~easyfilter/bresenham.html
	// A simple example of Bresenham's line algorithm.

protected:
	gv_vector2i m_image_size;
};
}