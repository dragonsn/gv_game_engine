#pragma once
namespace gv
{

template < class T >
class gvt_box
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_v2;
	typedef gvt_vector3< T > type_of_v3;
	typedef gvt_vector4< T > type_of_v4;
	typedef gvt_quaternion< T > type_of_quaternion;
	typedef gvt_matrix43< T > type_of_matrix43;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// data member
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	type_of_v3 min_p;
	type_of_v3 max_p;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// functions
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_box()
	{
		min_p.set(value_trait::max(), value_trait::max(), value_trait::max());
		max_p.set(value_trait::min(), value_trait::min(), value_trait::min());
	};

	inline explicit gvt_box(const type_of_v3& v)
	{
		this->max_p = this->min_p = v;
	};
	inline gvt_box(const type_of_v3& vmin, const type_of_v3& vmax)
	{
		this->max_p = vmax, this->min_p = vmin;
	};
	inline gvt_box(const gvt_box& oldbox)
	{
		this->max_p = oldbox.max_p;
		this->min_p = oldbox.min_p;
	};

	inline void add(type_of_v3 v)
	{
		max_p.x = gvt_max(v.x, max_p.x);
		max_p.y = gvt_max(v.y, max_p.y);
		max_p.z = gvt_max(v.z, max_p.z);

		min_p.x = gvt_min(v.x, min_p.x);
		min_p.y = gvt_min(v.y, min_p.y);
		min_p.z = gvt_min(v.z, min_p.z);
	};

	inline void add(const gvt_box& b)
	{
		max_p.x = gvt_max(b.max_p.x, max_p.x);
		max_p.y = gvt_max(b.max_p.y, max_p.y);
		max_p.z = gvt_max(b.max_p.z, max_p.z);

		min_p.x = gvt_min(b.min_p.x, min_p.x);
		min_p.y = gvt_min(b.min_p.y, min_p.y);
		min_p.z = gvt_min(b.min_p.z, min_p.z);
	};

	inline void add(int number_points, const type_of_v3* point_array)
	{
		for (int i = 0; i < number_points; i++)
		{
			this->add(point_array[i]);
		}
	};

	inline void create(type_of_v3 corners[8])
	{
		for (int i = 0; i < 8; i++)
		{
			this->add(corners[i]);
		}
	};

	inline void set(const type_of_v3& new_min_point,
					const type_of_v3& new_max_point)
	{
		this->min_p = new_min_point, this->max_p = new_max_point;
	};

	inline void get(type_of_v3& result_min_point,
					type_of_v3& result_max_point) const
	{
		result_min_point = this->min_p;
		result_max_point = this->max_p;
	};

	inline type_of_v3 get_center() const
	{
		return (this->min_p + this->max_p) / value_trait::two();
	};

	inline const type_of_v3& get_min_point() const
	{
		return this->min_p;
	};

	inline const type_of_v3& get_max_point() const
	{
		return this->max_p;
	};

	inline type_of_v3 get_size() const
	{
		return this->max_p - this->min_p;
	};

	inline type_of_value get_height() const
	{
		return this->max_p.z - this->min_p.z;
	};

	inline type_of_value get_width() const
	{
		return this->max_p.x - this->min_p.x;
	};

	inline type_of_value get_length() const
	{
		return this->max_p.y - this->min_p.y;
	};

	inline type_of_value get_volumn() const
	{
		return get_width() * get_height() * get_length();
	}

	inline type_of_value get_xdelta() const
	{
		return this->get_width();
	};

	inline type_of_value get_ydelta() const
	{
		return this->get_length();
	};
	inline type_of_value get_zdelta() const
	{
		return this->get_height();
	};
	inline type_of_value get_diagonal() const
	{
		return min_p.distance_to(max_p);
	}

	inline void scale_box(const type_of_v3& scale)
	{
		this->min_p *= scale;
		this->max_p *= scale;
	};

	inline void scale_box(type_of_value scale)
	{
		this->min_p *= scale;
		this->max_p *= scale;
	};

	inline void get_tranform(const gvt_box& target, type_of_v3& scale,
							 type_of_v3& offset) const
	{
		scale = target.get_size() / get_size();
		offset = target.get_center() - get_center();
	}

	inline void clear_z_extents()
	{
		min_p.z = value_trait::max();
		max_p.z = value_trait::min();
	};

	inline void add_to_z(type_of_value z)
	{
		min_p.z = gv::gvt_min(z, min_p.z);
		max_p.z = gv::gvt_max(z, max_p.z);
	};

	inline bool is_point_inside(const type_of_v3& point,
								type_of_value epsilon = 0) const
	{
		if (min_p.x - point.x <= epsilon && point.x - max_p.x < epsilon &&
			min_p.y - point.y <= epsilon && point.y - max_p.y < epsilon &&
			min_p.z - point.z <= epsilon && point.z - max_p.z < epsilon)
		{
			return true;
		}
		return false;
	};

	inline gvt_box& operator=(const gvt_box& b)
	{
		this->min_p = b.min_p;
		this->max_p = b.max_p;
		return *this;
	};

	inline bool operator==(const gvt_box& b) const
	{
		return (this->min_p == b.min_p && this->max_p == b.max_p);
	}

	inline bool operator!=(const gvt_box& b) const
	{
		return !(*this == b);
	};

	inline type_of_v3 get_extend()
	{
		return get_size() / value_trait::two();
	}

	inline void extend(const type_of_v3& amount)
	{
		this->max_p += amount;
		this->min_p -= amount;
	};

	inline void extend(const type_of_value& amount)
	{
		extend(type_of_v3(amount, amount, amount));
	};

	inline void set_extend(type_of_v3 ext)
	{
		type_of_v3 c = get_center();
		(*this) = gvt_box(c - ext, c + ext);
	}

	inline void set_extend(type_of_value x, type_of_value y, type_of_value z)
	{
		set_extend(type_of_v3(x, y, z));
	}

	inline void move_center_to_origin()
	{
		this->max_p =
			this->max_p - (this->min_p + this->max_p) / value_trait::two();
		this->min_p =
			this->min_p - (this->min_p + this->max_p) / value_trait::two();
	};

	inline void reset()
	{
		min_p.set(value_trait::max(), value_trait::max(), value_trait::max());
		max_p.set(value_trait::min(), value_trait::min(), value_trait::min());
	}

	inline void empty()
	{
		this->reset();
	}
	inline bool is_empty()
	{
		return this->get_height() <= 0 || this->get_width() <= 0 ||
			   this->get_length() <= 0;
	}

	inline void move(const type_of_v3& v)
	{
		this->min_p += v;
		this->max_p += v;
	};

	inline void move_to(const type_of_v3& v)
	{
		type_of_v3 vc = this->get_center();
		type_of_v3 vd = v - vc;
		this->move(vd);
	};

	inline bool intersect(const gvt_box& b, gvt_box* result = NULL,
						  type_of_value epsilon = value_trait::epsilon()) const
	{
		if (min_p.x > b.get_max_point().x + epsilon)
			return false;
		if (max_p.x < b.get_min_point().x - epsilon)
			return false;
		if (min_p.y > b.get_max_point().y + epsilon)
			return false;
		if (max_p.y < b.get_min_point().y - epsilon)
			return false;
		if (min_p.z > b.get_max_point().z + epsilon)
			return false;
		if (max_p.z < b.get_min_point().z - epsilon)
			return false;
		if (result)
		{
			for (int i = 0; i < 3; i++)
			{
				(*result).min_p[i] = gvt_max(min_p[i], b.min_p[i]);
				(*result).max_p[i] = gvt_min(max_p[i], b.max_p[i]);
			}
		}
		return true;
	}

	inline void get_sphere(type_of_v3& pos, type_of_value& r) const
	{
		type_of_v3 box_dimensions;
		box_dimensions = max_p - min_p;
		pos = min_p + (box_dimensions / value_trait::two());
		r = (pos - min_p).length();
	}

	inline gvt_box clip(const gvt_box& b) const
	{
		gvt_box r;
		r.min_p.x = gvt_max(min_p.x, b.min_p.x);
		r.min_p.y = gvt_max(min_p.y, b.min_p.y);
		r.min_p.z = gvt_max(min_p.z, b.min_p.z);

		r.max_p.x = gvt_min(max_p.x, b.max_p.x);
		r.max_p.y = gvt_min(max_p.y, b.max_p.y);
		r.max_p.z = gvt_min(max_p.z, b.max_p.z);
		return r;
	}

	inline gvt_box minkowski_difference(const gvt_box& b) const
	{
		gvt_box r;
		r.min_p = min_p - b.max_p;
		r.max_p = max_p - b.min_p;
		return r;
	}

	inline gvt_box minkowski_sum(const gvt_box& b) const
	{
		gvt_box r;
		r.min_p = min_p + b.min_p;
		r.max_p = max_p + b.max_p;
		return r;
	}

	gv_bool is_overlap(const gvt_box& b) const
	{
		gvt_box r = this->clip(b);
		return !r.is_empty();
	}

	// Minkowski difference

	// transformed by an  matrix
	gvt_box transform(const type_of_matrix43& mat)
	{
		gvt_box rotated_box;
		type_of_v3 temp;

		temp = get_max_point();
		temp *= mat;
		rotated_box.add(temp);
		temp = get_min_point();
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_max_point().x;
		temp.y = get_max_point().y;
		temp.z = get_min_point().z;
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_max_point().x;
		temp.y = get_min_point().y;
		temp.z = get_max_point().z;
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_max_point().x;
		temp.y = get_min_point().y;
		temp.z = get_min_point().z;
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_min_point().x;
		temp.y = get_max_point().y;
		temp.z = get_max_point().z;
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_min_point().x;
		temp.y = get_max_point().y;
		temp.z = get_min_point().z;
		temp *= mat;
		rotated_box.add(temp);

		temp.x = get_min_point().x;
		temp.y = get_min_point().y;
		temp.z = get_max_point().z;
		temp *= mat;
		rotated_box.add(temp);
		return rotated_box;
	};

	static const gvt_box& static_unit_box()
	{
		static gvt_box box(-type_of_v3(value_trait::half(), value_trait::half(),
									   value_trait::half()),
						   type_of_v3(value_trait::half(), value_trait::half(),
									  value_trait::half()));
		return box;
	}

}; // box

typedef gvt_box< gv_float > gv_box;
typedef gvt_box< gv_int > gv_boxi;

} // gv