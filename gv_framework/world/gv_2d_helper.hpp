
namespace gv
{
void gv_2d_helper::to_2d(const gv_vector3& v, gv_vector2i& v_2d,
						 gv_int& height)
{
	v_2d.set(gv_math_fixed::to_fixed(v.x), gv_math_fixed::to_fixed(v.z));
	height = gv_math_fixed::to_fixed(v.y);
};

void gv_2d_helper::to_2d(const gv_vector3& v, gv_vector2i& v_2d)
{
	v_2d.set(gv_math_fixed::to_fixed(v.x), gv_math_fixed::to_fixed(v.z));
};

gv_vector2i gv_2d_helper::to_2d(const gv_vector3& v)
{
	gv_vector2i v_2d;
	to_2d(v, v_2d);
	return v_2d;
};

gv_vector2i gv_2d_helper::to_2d(const gv_vector3i& v)
{
	return gv_vector2i(v.x, v.z);
};

gv_recti gv_2d_helper::to_2d(const gv_boxi& box)
{
	return gv_recti(to_2d(box.min_p), to_2d(box.max_p));
};

void gv_2d_helper::to_3d(const gv_vector2i& v_2d, gv_int height,
						 gv_vector3& v)
{
	v.set(gv_math_fixed::to_float(v_2d.x), gv_math_fixed::to_float(height),
		  gv_math_fixed::to_float(v_2d.y));
};

void gv_2d_helper::to_3d(const gv_vector2i& v_2d, gv_vector3& v)
{
	v.set(gv_math_fixed::to_float(v_2d.x), 0, gv_math_fixed::to_float(v_2d.y));
};

void gv_2d_helper::to_2d(const gv_box& box, gv_recti& rect)
{
	gv_int h = 0;
	gv_vector2i s;
	to_2d(box.get_size(), s, h);
	to_2d(box.min_p, rect.min_p, h);
	to_2d(box.max_p, rect.max_p, h);
	// make sure size is 2 times
	rect.min_p = rect.min_p / 2 * 2;
	rect.max_p = rect.max_p / 2 * 2;
};

void gv_2d_helper::to_3d(const gv_recti& rect, gv_box& box)
{
	gv_int h = 0;
	to_3d(rect.min_p, h, box.min_p);
	to_3d(rect.max_p, h, box.max_p);
};
}