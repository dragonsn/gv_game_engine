#pragma once
namespace gv
{

template < class T >
class gvt_rect
{
public:
	typedef gvt_value< T > value_trait;
	typedef T type_of_value;
	typedef gvt_vector2< T > type_of_v2;
	typedef gvt_vector4< T > type_of_v4;
	typedef gvt_quaternion< T > type_of_quaternion;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	union {
		struct
		{
			type_of_value left, top, right, bottom;
		};
		struct
		{
			type_of_v2 min_p;
			type_of_v2 max_p;
		};
		struct
		{
			type_of_v2 p[2];
		};
		struct
		{
			type_of_v4 v4;
		};
	};
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	inline gvt_rect()
	{
		min_p.set(value_trait::max(), value_trait::max());
		max_p.set(value_trait::min(), value_trait::min());
		;
	};
	gvt_rect(const gvt_rect& rect)
	{
		*this = rect;
	}
	inline gvt_rect& operator=(const gvt_rect& rect)
	{
		v4 = rect.v4;
		return *this;
	}
	inline gvt_rect(const type_of_v2& _min, const type_of_v2& _max)
		: min_p(_min), max_p(_max){};
	inline gvt_rect(type_of_value x, type_of_value y, type_of_value x1,
					type_of_value y1)
	{
		this->set(x, y, x1, y1);
	}
	inline type_of_value get_min_x() const
	{
		return left;
	}
	inline type_of_value get_min_y() const
	{
		return top;
	}
	inline type_of_value get_max_x() const
	{
		return right;
	}
	inline type_of_value get_max_y() const
	{
		return bottom;
	}
	inline void set(type_of_value x, type_of_value y, type_of_value x1,
					type_of_value y1)
	{
		v4.set(x, y, x1, y1);
	};
	inline void set(const type_of_v2& v0, const type_of_v2& v1)
	{
		min_p = v0;
		max_p = v1;
	};
	inline void enlarge(type_of_value x, type_of_value y)
	{
		right += x;
		bottom += y;
	};
	inline void set_size(type_of_value x, type_of_value y)
	{
		right = left + x;
		bottom = top + y;
	};
	inline void set_height(type_of_value y)
	{
		bottom = top + y;
	}
	inline void set_width(type_of_value x)
	{
		right = left + x;
	}
	inline void set_size(const type_of_v2& v)
	{
		set_size(v.x, v.y);
	};
	inline void get_size(type_of_value& x, type_of_value& y) const
	{
		x = right - left;
		y = bottom - top;
	};
	inline type_of_v2 get_size() const
	{
		type_of_v2 v;
		get_size(v.x, v.y);
		return v;
	};
	inline void scale_size(type_of_value _sx, type_of_value _sy) const
	{
		type_of_value x, y;
		get_size(x, y);
		set_size(x * _sx, y * _sy);
	};
	inline void scale_x(type_of_value _s)
	{
		left *= _s;
		right *= _s;
	}
	inline void scale_y(type_of_value _s)
	{
		top *= _s;
		bottom *= _s;
	}
	inline void scale(type_of_value _s)
	{
		v4 *= _s;
	}
	inline void scale_by_center(const type_of_v2& _s)
	{
		gvt_rect< type_of_value > r;
		type_of_v2 v = center();
		type_of_value e_x = width() / value_trait::two() * _s.x;
		type_of_value e_y = height() / value_trait::two() * _s.y;
		r.left = v.x - e_x;
		r.right = v.x + e_x;
		r.top = v.y - e_y;
		r.bottom = v.y + e_y;
		*this = r;
	}
	inline type_of_v2 center() const
	{
		type_of_v2 v;
		v.x = (right + left) / value_trait::two();
		v.y = (top + bottom) / value_trait::two();
		return v;
	}
	inline void move(const type_of_v2& v)
	{
		this->min_p += v;
		this->max_p += v;
	};
	inline void move(type_of_value x, type_of_value y)
	{
		type_of_v2 v(x, y);
		this->min_p += v;
		this->max_p += v;
	};
	inline void move_to(type_of_value x, type_of_value y)
	{
		type_of_v2 v = get_size();
		this->min_p.set(x, y);
		this->max_p = min_p + v;
	};
	inline void move_to(const type_of_v2 v)
	{
		move_to(v.x, v.y);
	};
	inline void move_center_to(const type_of_v2 v)
	{
		move_to(v.x - width() / value_trait::two(),
				v.y - height() / value_trait::two());
	};
	inline type_of_v2 get_pos() const
	{
		return min_p;
	}
	inline bool intersect(const type_of_v2& p) const
	{
		if (p.get_x() < left)
			return false;
		if (p.get_x() >= right)
			return false;
		if (p.get_y() < top)
			return false;
		if (p.get_y() >= bottom)
			return false;
		return true;
	};
	inline bool inside(const type_of_v2& p) const
	{
		if (p.get_x() <= left)
			return false;
		if (p.get_x() >= right)
			return false;
		if (p.get_y() <= top)
			return false;
		if (p.get_y() >= bottom)
			return false;
		return true;
	};
	inline type_of_value width() const
	{
		return right - left;
	}
	inline type_of_value height() const
	{
		return bottom - top;
	}
	inline bool is_empty() const
	{
		return (left >= right) || (top >= bottom);
	}
	inline bool is_overlap(const gvt_rect& r) const
	{
		gvt_rect ret = clip(r);
		return !ret.is_empty();
	};
	inline bool is_inside(const gvt_rect& outer) const
	{
		gvt_rect ret = outer.clip(*this);
		return ret == *this;
	};
	inline gvt_rect clip(const gvt_rect& r) const
	{
		gvt_rect rr;
		rr.min_p.x = gvt_max(min_p.x, r.min_p.x);
		rr.min_p.y = gvt_max(min_p.y, r.min_p.y);
		rr.max_p.x = gvt_min(max_p.x, r.max_p.x);
		rr.max_p.y = gvt_min(max_p.y, r.max_p.y);
		return rr;
	};
	inline type_of_value area() const
	{
		return width() * height();
	};
	inline void add(const type_of_v2& v)
	{
		min_p.x = gvt_min(min_p.x, v.x);
		min_p.y = gvt_min(min_p.y, v.y);
		max_p.x = gvt_max(max_p.x, v.x);
		max_p.y = gvt_max(max_p.y, v.y);
	};
	inline void add(const gvt_rect& r)
	{
		this->add(r.min_p);
		this->add(r.max_p);
	};
	inline void extend(type_of_value f)
	{
		min_p.x -= f;
		min_p.y -= f;
		max_p.x += f;
		max_p.y += f;
	};
	inline void extend(const type_of_v2& v)
	{
		extend(v.x, v.y);
	};
	inline void extend(type_of_value sx, type_of_value sy)
	{
		min_p.x -= sx;
		min_p.y -= sy;
		max_p.x += sx;
		max_p.y += sy;
	};
	inline void extend(const gvt_rect& r)
	{
		extend(r.width() / value_trait::two(), r.height() / value_trait::two());
	}
	inline bool operator==(const gvt_rect& r) const
	{
		return (max_p == r.max_p && min_p == r.min_p);
	}
	inline bool operator!=(const gvt_rect& r) const
	{
		return !((*this) == r);
	}
	// if the rect contains the point, returns the point. Otherwise returns the
	// closest point on the perimeter of the rect.
	inline type_of_v2 get_closest_point(const type_of_v2& point) const
	{
		if (inside(point))
		{
			return point;
		}

		type_of_value x = point.x;
		type_of_value y = point.y;

		x = gvt_clamp< type_of_value >(x, left, right);
		y = gvt_clamp< type_of_value >(y, top, bottom);

		type_of_value dLeft = gvt_abs(x - left);
		type_of_value dRight = gvt_abs(x - right);
		type_of_value dTop = gvt_abs(y - top);
		type_of_value dBottom = gvt_abs(y - bottom);

		type_of_value leftRightMin = gvt_min(dLeft, dRight);
		type_of_value topBottomMin = gvt_min(dTop, dBottom);

		type_of_value min = gvt_min(leftRightMin, topBottomMin);

		if (min == dTop)
		{
			y = top;
		}
		else if (min == dBottom)
		{
			y = bottom;
		}
		else if (min == dLeft)
		{
			x = left;
		}
		else
		{
			x = right;
		}

		return type_of_v2(x, y);
	}
	template < class rect >
	inline gvt_rect& operator=(const rect& r)
	{
		min_p = r.min_p;
		max_p = r.max_p;
		return *this;
	}

	type_of_value get_bounding_r_sq()
	{
		type_of_value t = value_trait::two();
		type_of_v2 s = get_size();
		s /= t;
		return s.x * s.x + s.y * s.y;
	}
};

typedef gvt_rect< gv_float > gv_rect;
typedef gvt_rect< gv_int > gv_recti;
};