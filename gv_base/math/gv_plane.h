#pragma once
namespace gv
{
template < class T >
class gvt_plane
{
public:
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	// plane formula: ax+by+cz+d=0;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_v2;
	typedef gvt_vector3< T > type_of_v3;
	typedef gvt_vector4< T > type_of_v4;
	union {
		struct
		{
			type_of_v3 normal;	// normal of the plane
			type_of_value offset; // Offset from origin, i.e., d
		};
		struct
		{
			type_of_value a, b, c, d;
		};
		struct
		{
			type_of_v4 v4;
		};
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_plane(){};

	gvt_plane(const gvt_plane& plane)
	{
		*this = plane;
	}

	inline gvt_plane(type_of_value x, type_of_value y, type_of_value z,
					 type_of_value d)
	{
		this->normal.set(x, y, z);
		this->offset = d;
	};

	inline gvt_plane(const type_of_v3& normal, type_of_value d)
	{
		this->normal = normal;
		this->offset = d;
	};

	inline gvt_plane(const type_of_v3& normal, const type_of_v3& point)
	{
		this->normal = normal;
		this->offset = -normal.dot(point);
	};

	inline gvt_plane(const type_of_v3& v1, const type_of_v3& v2,
					 const type_of_v3& v3)
	{
		this->normal = (v2 - v1).cross(v3 - v1);
		this->normal.normalize();
		this->offset = -this->normal.dot(v1);
	};

	inline bool operator==(const gvt_plane& plane) const
	{
		return this->normal.is_almost_equal(plane.normal) &&
			   value_trait::is_almost_equal(this->offset, plane.offset);
	};

	inline gvt_plane& operator=(const gvt_plane& plane)
	{
		v4 = plane.v4;
		return *this;
	};

	inline bool operator!=(const gvt_plane& plane) const
	{
		return !(*this == plane);
	};

	inline gvt_plane operator-() const
	{
		gvt_plane plane(-this->normal, -this->offset);
		return plane;
	};

	inline void set(const type_of_v3& normal, type_of_value d)
	{
		this->normal = normal;
		this->offset = d;
	};

	inline void set(const type_of_v3& v1, const type_of_v3& v2,
					const type_of_v3& v3)
	{
		gvt_plane a(v1, v2, v3);
		(*this) = a;
		// return true;
	};

	inline void set(const type_of_v3& normal, const type_of_v3& point)
	{
		gvt_plane p(normal, point);
		*this = p;
	};

	inline type_of_value test_point(const type_of_v3& point) const
	{
		return point.dot(this->normal) + this->offset;
	};

	inline const type_of_v3& get_normal() const
	{
		return normal;
	};

	inline type_of_value get_offset() const
	{
		return offset;
	};

	inline void set_normal(const type_of_v3& vector)
	{
		normal = vector;
	}

	inline void set_offset(type_of_value f)
	{
		offset = f;
	}

	inline void normalize()
	{
		type_of_value f = normal.normalize();
		offset *= f;
	}

	inline type_of_v3 project(const type_of_v3& point) const
	{
		return point - this->normal * (this->normal.dot(point) + this->offset);
	}

	template < class type_of_matrix >
	inline gvt_plane transform(type_of_matrix& mat) const
	{
		// the plane need to be normalized!!
		GV_ASSERT_SLOW(normal.is_normlaized());
		gvt_plane plane;
		type_of_v3 point(normal.x * (-offset), normal.y * (-offset),
						 normal.z * (-offset));
		plane.normal = mat.rotate(this->normal);
		point = point * mat;
		plane.offset = -plane.normal.dot(point);
		return plane;
	}
};

typedef gvt_plane< gv_float > gv_plane;
}
