#pragma once
namespace gv
{
template < class T >
class gvt_frustum
{
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_v2;
	typedef gvt_vector3< T > type_of_v3;
	typedef gvt_vector4< T > type_of_v4;
	typedef gvt_plane< T > type_of_plane;
	typedef gvt_math< T > math;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//	six plane to represent  the frustum , maybe  5  or 4 is enough?
public:
	type_of_plane planes[6];
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:
	inline gvt_frustum()
	{
	}

	gvt_frustum(const gvt_frustum& f)
	{
		for (int i = 0; i < 6; ++i)
		{
			planes[i] = f.planes[i];
		}
	}

	void set_fov(type_of_value fov, type_of_value ratio_h_by_w,
				 type_of_value znear, type_of_value zfar)
	{
		// fov/=2.0f;//test for clip
		fov = gvt_deg2rad(fov);
		this->planes[0].a =
			math::cos(fov / value_trait::two()) / ratio_h_by_w; // this is left
		this->planes[0].b = 0;
		this->planes[0].c = math::sin(fov / value_trait::two());
		this->planes[0].d = 0;
		this->planes[0].normal.normalize();

		this->planes[1].a =
			-math::cos(fov / value_trait::two()) / ratio_h_by_w; // this is right
		this->planes[1].b = 0;
		this->planes[1].c = math::sin(fov / value_trait::two());
		this->planes[1].d = 0;
		this->planes[1].normal.normalize();

		this->planes[2].a = 0; // this is top
		this->planes[2].b = -math::cos(fov / value_trait::two());
		this->planes[2].c = math::sin(fov / value_trait::two());
		this->planes[2].d = 0;
		this->planes[2].normal.normalize();

		this->planes[3].a = 0; // this is bottom
		this->planes[3].b = math::cos(fov / value_trait::two());
		this->planes[3].c = math::sin(fov / value_trait::two());
		this->planes[3].d = 0;
		this->planes[3].normal.normalize();

		this->planes[4].a = this->planes[4].b = 0;
		this->planes[4].c = 1;
		this->planes[4].d = -znear;

		this->planes[5].a = this->planes[5].b = 0;
		this->planes[5].c = -1;
		this->planes[5].d = zfar;
	}

	void set_fov_right_hand(type_of_value fov, type_of_value ratio_h_by_w,
							type_of_value znear, type_of_value zfar)
	{
		this->set_fov(fov, ratio_h_by_w, znear, zfar);
		gv_matrix44 m;
		m.set_identity();
		m.m[0][0] = -1;
		m.m[2][2] = -1; // rotate 180 by y
		*this = this->transform(m);
	}

	void set_frustum(type_of_value left, type_of_value right, type_of_value top,
					 type_of_value bottom, type_of_value znear,
					 type_of_value zfar)
	{
		type_of_v3 normal;
		type_of_v3 orig(0, 0, 0);

		normal.x = znear;
		normal.y = 0;
		normal.z = -left;
		normal.normalize();
		this->planes[0] = type_of_plane(normal, orig);

		normal.x = -znear;
		normal.y = 0;
		normal.z = right;
		normal.normalize();
		this->planes[1] = type_of_plane(normal, orig);

		normal.x = 0;
		normal.y = -znear;
		normal.z = -top;
		normal.normalize();
		this->planes[2] = type_of_plane(normal, orig);

		normal.x = 0;
		normal.y = znear;
		normal.z = bottom;
		normal.normalize();
		this->planes[3] = type_of_plane(normal, orig);

		normal.x = 0;
		normal.y = 0;
		normal.z = 1;
		orig.z = znear;
		this->planes[4] = type_of_plane(normal, orig);

		normal.x = 0;
		normal.y = 0;
		normal.z = -1;
		orig.z = zfar;
		this->planes[5] = type_of_plane(normal, orig);
	}

	bool operator==(const gvt_frustum& f) const
	{
		int size = gvt_array_length(planes);
		for (int i = 0; i < size; i++)
		{
			if (planes[i] != f.planes[i])
				return false;
		}
		return true;
	}

	template < class type_of_matrix >
	gvt_frustum transform(type_of_matrix& matrix) const
	{
		gvt_frustum f;
		int size = gvt_array_length(planes);
		for (int i = 0; i < size; i++)
		{
			f.planes[i] = planes[i].transform(matrix);
		}
		return f;
	}

	bool intersect(const gvt_box< T >& aabb) const
	{

		type_of_v3 pos;
		type_of_v3 box_min(aabb.min_p);
		type_of_v3 box_max(aabb.max_p);

		for (int i = 0; i < 6; i++)
		{
			if (planes[i].a > 0.0f)
			{
				if (planes[i].b > 0.0f)
				{
					if (planes[i].c > 0.0f)
					{
						pos.x = box_max.x;
						pos.y = box_max.y;
						pos.z = box_max.z;
					}
					else
					{
						pos.x = box_max.x;
						pos.y = box_max.y;
						pos.z = box_min.z;
					}
				}
				else
				{
					if (planes[i].c > 0.0f)
					{
						pos.x = box_max.x;
						pos.y = box_min.y;
						pos.z = box_max.z;
					}
					else
					{
						pos.x = box_max.x;
						pos.y = box_min.y;
						pos.z = box_min.z;
					}
				}
			}
			else
			{
				if (planes[i].b > 0.0f)
				{
					if (planes[i].c > 0.0f)
					{
						pos.x = box_min.x;
						pos.y = box_max.y;
						pos.z = box_max.z;
					}
					else
					{
						pos.x = box_min.x;
						pos.y = box_max.y;
						pos.z = box_min.z;
					}
				}
				else
				{
					if (planes[i].c > 0.0f)
					{
						pos.x = box_min.x;
						pos.y = box_min.y;
						pos.z = box_max.z;
					}
					else
					{
						pos.x = box_min.x;
						pos.y = box_min.y;
						pos.z = box_min.z;
					}
				}
			}
			type_of_v3 normal(this->planes[i].a, this->planes[i].b,
							  this->planes[i].c);
			if (normal.dot(pos) + planes[i].d < 0)
			{
				return false;
			}
		}
		return true;
	}
}; //

typedef gvt_frustum< gv_float > gv_frustum;
}
