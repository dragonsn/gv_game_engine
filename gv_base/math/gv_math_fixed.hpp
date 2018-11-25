namespace gv
{
//============================================================================================
//
//============================================================================================
static gv_float s_to_fixed_factor = 1000.f;
static gv_float s_to_3d_factor = 0.001f;
static gv_int s_fixed_one = 1000;
gv_math_fixed* s_fixed_helper = NULL;
gvt_array< gv_int > gv_math_fixed::m_sin_table;
gvt_array< gv_int > gv_math_fixed::m_tan_table;
gv_math_fixed* gv_math_fixed::static_get()
{
	return s_fixed_helper;
};
gv_math_fixed::gv_math_fixed()
{
	s_fixed_helper = this;
}
gv_math_fixed::~gv_math_fixed()
{
	s_fixed_helper = NULL;
};
void gv_math_fixed::set_to_fixed_factor(gv_float f)
{
	s_to_fixed_factor = f;
	s_to_3d_factor = 1.0f / f;
	s_fixed_one = (gv_int)f;
};
gv_float gv_math_fixed::to_float_factor()
{
	return s_to_3d_factor;
}
gv_float gv_math_fixed::to_fixed_factor()
{
	return s_to_fixed_factor;
}
gv_int gv_math_fixed::one()
{
	return s_fixed_one;
};
gv_int gv_math_fixed::to_fixed(gv_float v)
{
	return (gv_int)(v * to_fixed_factor());
};
gv_float gv_math_fixed::to_float(gv_int v)
{
	return ((gv_float)v) * to_float_factor();
};

gv_float gv_math_fixed::deg_to_rad(gv_int deg)
{
	return gvt_value< gv_float >::pi() * ((gv_float)(deg) / (gv_float)pi());
};
gv_int gv_math_fixed::rad_to_deg(gv_float rad)
{
	return (gv_int)((gv_float)pi() * (rad / gvt_value< gv_float >::pi()));
};

void gv_math_fixed::to_fixed(const gv_vector3& vf, gv_vector3i& vi)
{
	for (int i = 0; i < 3; i++)
		vi[i] = to_fixed(vf[i]);
};
void gv_math_fixed::to_float(const gv_vector3i& vi, gv_vector3& vf)
{
	for (int i = 0; i < 3; i++)
		vf[i] = to_float(vi[i]);
};
void gv_math_fixed::to_float(const gv_boxi& box_i, gv_box& box_f)
{
	to_float(box_i.min_p, box_f.min_p);
	to_float(box_i.max_p, box_f.max_p);
};

gv_vector3i gv_math_fixed::to_fixed(const gv_vector3& vf)
{
	gv_vector3i v; 
	to_fixed(vf, v);
	return v;
};

gv_vector3 gv_math_fixed::to_float(const gv_vector3i& vi)
{
	gv_vector3 v;
	to_float(vi, v);
	return v;
};

void gv_math_fixed::to_fixed(const gv_box& box_f, gv_boxi& box_i)
{
	to_fixed(box_f.min_p, box_i.min_p);
	to_fixed(box_f.max_p, box_i.max_p);
};
void gv_math_fixed::to_fixed(const gv_euler& vf, gv_euleri& vi)
{
	for (int i = 0; i < 3; i++)
		vi.v3[i] = float_rad_to_fixed_deg(vf.v3[i]);
};
void gv_math_fixed::to_float(const gv_euleri& vi, gv_euler& vf)
{
	for (int i = 0; i < 3; i++)
		vf.v3[i] = fixed_deg_to_float_rad(vi.v3[i]);
};

gv_int gv_math_fixed::pi()
{
	return 18000;
}
gv_int gv_math_fixed::half_pi()
{
	return 9000;
};
gv_int gv_math_fixed::two_pi()
{
	return 36000;
}

gv_vector2i gv_math_fixed::normalize2d(const gv_vector2i& v)
{
	gv_int angle = gv_math_fixed::static_get()->dir_to_angle(v);
	return gv_math_fixed::static_get()->angle_to_dir(angle);
}

gv_vector3i gv_math_fixed::normalize3d(const gv_vector3i& v)
{
	gv_vector2i v2d = gv_vector2i(v.x, v.z);
	if (!v2d.abs_sum())
		return gv_vector3i(0, one() * gvt_sign(v.y), 0);
	gv_int angle = gv_math_fixed::static_get()->dir_to_angle(v2d);
	gv_vector2i nv2d = gv_math_fixed::static_get()->angle_to_dir(angle);
	gv_int h = v.y * v.abs_sum() / v2d.abs_sum();
	gv_int angle2 =
		gv_math_fixed::static_get()->dir_to_angle(gv_vector2i(one(), h));
	gv_vector2i nv2d2 = gv_math_fixed::static_get()->angle_to_dir(angle2);
	return gv_vector3i(mul(nv2d.x, nv2d2.x), mul(nv2d.y, nv2d2.y),
					   mul(nv2d.y, nv2d2.x));
};

gv_vector2i
gv_math_fixed::normalize_quick_and_not_precise(const gv_vector2i& v)
{
	gv_int l = v.abs_sum();
	if (l != 0)
	{
		gv_vector2i ret = v * s_fixed_one / l;
		return ret;
	}
	else
		return gv_vector2i(s_fixed_one, 0);
}

gv_vector2i gv_math_fixed::reflect(const gv_vector2i& normal,
								   const gv_vector2i& vector)
{
	gv_int project = vector.dot(normal) / one();
	gv_vector2i f = normal * project * 2 / one();
	return vector - f;
}

gv_vector3i gv_math_fixed::reflect3d(const gv_vector3i& normal,
									 const gv_vector3i& vector)
{
	gv_int project = vector.dot(normal) / one();
	gv_vector3i f = normal * project * 2 / one();
	return vector - f;
}

gv_vector2i gv_math_fixed::get_random_position_in_radius(gv_random_uint& ru,
														 gv_int radius)
{
	gv_int random_angle = ru.get_ranged(0, 36000);
	gv_vector2i random_dir =
		gv_math_fixed::static_get()->angle_to_dir(random_angle);
	if (radius <= 1)
		radius = 2;
	gv_int random_radius = ru.get_ranged(1, radius);
	gv_vector2i cpos = random_dir * random_radius / gv_math_fixed::one();

	return cpos;
}

gv_bool gv_math_fixed::test_random_value_in_range(gv_random_uint& ru, gv_int tv,
												  gv_int range)
{
	gv_int rv = ru.get_ranged(0, range);
	return rv < tv;
}

gv_int gv_math_fixed::normalize_deg_angle(gv_int a)
{
	if (a >= 0)
	{
		return a % two_pi();
	}
	gv_int ret = a - (a / two_pi() - 1) * two_pi();
	GV_ASSERT(ret >= 0 && ret < two_pi());
	return ret;
};

gv_int gv_math_fixed::delta_angle(gv_int source, gv_int target)
{
	if (target >= source && target - source <= pi())
		return target - source;
	if (target < source && target - source >= -pi())
		return target - source;
	if (target + two_pi() - source <= pi())
		return target + two_pi() - source;
	if (target - two_pi() - source >= -pi())
		return target - two_pi() - source;
	GV_ASSERT(0);
	return target - source;
}

bool gv_math_fixed::segment_cast(const gv_vector2i& start,
								 const gv_vector2i& end, gv_recti& rect)
{
	gv_vector2i center, extent;
	center[0] = (gv_int)((rect.min_p[0] + rect.max_p[0]) / 2);
	center[1] = (gv_int)((rect.min_p[1] + rect.max_p[1]) / 2);
	extent[0] = (gv_int)((rect.max_p[0] - rect.min_p[0]) / 2);
	extent[1] = (gv_int)((rect.max_p[1] - rect.min_p[1]) / 2);

	// convert to rect coordinate.
	gv_vector2i transformedP0 = start - center;
	gv_vector2i transformedP1 = end - center;
	gv_vector2i segCenter =
		gv_vector2i((gv_int)((transformedP0.x + transformedP1.x) / 2),
					(gv_int)((transformedP0.y + transformedP1.y) / 2));
	gv_vector2i segDirection = transformedP1 - transformedP0;
	for (int i = 0; i < 2; ++i)
	{
		if (std::abs(segCenter[i]) >
			extent[i] + gv_int(std::abs(segDirection[i] / 2)))
			return false;
	}
	segDirection = normalize2d(segDirection);
	gv_int LHS = std::abs((gv_int)(segDirection[0] * segCenter[1] +
								   segDirection[1] * (-segCenter[0])));
	gv_int RHS = extent[0] * std::abs(segDirection[1]) +
				 extent[1] * std::abs(segDirection[0]);
	return LHS <= RHS;
}

bool gv_math_fixed::intersect_ray(const gv_vector2i& v, gv_int dist,
								  const gv_recti& rect, gv_int& result_distance,
								  gv_int dim, gv_int skin)
{
	gv_int dim2 = 1 - dim;
	result_distance = dist;
	if (dist == 0)
		return false; // no move at all
	if (rect.inside(v))
	{
		return false;
	}; // the algorithm need v out of the rect .
	if (v[dim2] < rect.min_p[dim2] || v[dim2] >= rect.max_p[dim2])
		return false;
	gv_int s = 0;
	if (dist > 0)
	{
		s = rect.min_p[dim] - v[dim];
	}
	else
	{
		s = v[dim] - rect.max_p[dim];
	}
	if (s < 0)
		return false;
	s -= skin;
	if (s < 0)
		s = 0;
	// if (s> gvt_abs(dist)  ) return false;
	result_distance = s * gvt_sign(dist);
	return true;
}

bool gv_math_fixed::intersect_ray(const gv_vector3i& v, gv_int speed,
								  const gv_boxi& in_box,
								  gv_int& result_distance, gv_int dim,
								  gv_int skin)
{
	gv_int dim2 = (dim+1)%3;
	result_distance = speed;
	if (speed == 0)
		return false; // no move at all
	
	if (v[dim2] < in_box.min_p[dim2] || v[dim2] >= in_box.max_p[dim2])
		return false;

     dim2 = (dim2 + 1) % 3;
	 if (v[dim2] < in_box.min_p[dim2] || v[dim2] >= in_box.max_p[dim2])
		 return false;

	gv_int s = 0;
	if (speed > 0)
	{
		s = in_box.min_p[dim] - v[dim];
	}
	else
	{
		s = v[dim] - in_box.max_p[dim];
	}
	if (s < 0)
		return false;
	s -= skin;
	if (s < 0)
		s = 0;
	result_distance = s * gvt_sign(speed);
	return true;

	/*
	//methminkowski_difference
	result_distance = speed;
	gv_boxi box = in_box;
	if (box.is_point_inside(v))
	{
		result_distance = 0;
		return false;
	}
	// find minkowski_difference;
	box.move(-v);
	if (speed > 0)
	{
		box.min_p[dim] -= speed;
	}
	else
	{
		box.max_p[dim] -= speed;
	}
	gv_vector3i ve(1,1,1); 
	ve[dim] = 1;
	//box.extend(ve);
	if (box.is_point_inside(gv_vector3i::get_zero_vector()))
	{
		// must penetrate the box
		if (speed > 0)
		{
			result_distance = speed-gvt_clamp(-box.min_p[dim]+2 ,0, speed);
		}
		else
		{
			result_distance = speed-gvt_clamp(-box.max_p[dim]-2 , speed ,0);
		}
		
		return true;
	}
	return false;
	*/
};

bool gv_math_fixed::sweep(const gv_recti& mover, const gv_recti& being_tested,
						  gv_int speed, gv_int& distance, gv_vector2i& contact,
						  gv_vector2i& normal, int dim, gv_int skin)
{
	gv_recti r = being_tested;
	r.left -= mover.width();
	r.top -= mover.height();
	if (intersect_ray(mover.min_p, speed, r, distance, dim, skin))
	{
		gv_int dim2 = 1 - dim;
		gv_int d0, d1;
		bool overlap = gvt_overlap_range(mover.min_p[dim2], mover.max_p[dim2],
										 being_tested.min_p[dim2],
										 being_tested.max_p[dim2], d0, d1);
		GV_ASSERT(overlap);
		normal[dim] = -one() * gvt_sign(speed);
		normal[dim2] = 0;
		if (speed > 0)
		{
			contact[dim] = being_tested.min_p[dim];
		}
		else
		{
			contact[dim] = being_tested.max_p[dim];
		}
		contact[dim2] = (d0 + d1) / 2;
		return true;
	}
	return false;
}

gv_bool gv_math_fixed::sweep(const gv_boxi& mover, const gv_boxi& being_tested,
							 gv_int speed, gv_int& distance,
							 gv_vector3i& contact, gv_vector3i& normal,
							 gv_int dim, gv_int skin)
{
	if (speed == 0) return false;
	gv_boxi r = being_tested;
	r.min_p -= mover.get_size();
	r.min_p += 1;
	
	if (intersect_ray(mover.min_p, speed, r, distance, dim, skin))
	{
		normal = gv_vector3i::get_zero_vector();
		normal[dim] = -one() * gvt_sign(speed);
		contact = (mover.get_center() + being_tested.get_center()) / 2;
		if (speed > 0)
		{
			contact = being_tested.min_p;
		}
		else
		{
			contact = being_tested.max_p;
		}
		return true;
	}
	return false;
}

bool gv_math_fixed::test_if_hit_with_distance(const gv_vector2i& normal_dir,
											  gv_int speed, gv_int& distance,
											  gv_int skin,
											  gv_vector2i& normal)
{
	gv_int speed_dir_sign = gvt_sign(speed);
	if ((distance == 0) || (gvt_sign(distance) == speed_dir_sign &&
							gvt_abs(distance) < gvt_abs(speed) + skin))
	{
		// we hit
		normal = normalize2d(normal_dir);
		distance =
			gvt_abs(distance) > skin ? (distance - skin * speed_dir_sign) : 0;
		return true;
	}
	if (gvt_sign(distance) * speed_dir_sign == -1)
	{
		return false;
	}
	return false;
}

bool gv_math_fixed::sweep(const gv_recti& mover, gv_vector2i start,
						  gv_vector2i end, gv_int speed, gv_int& distance,
						  gv_vector2i& contact, gv_vector2i& normal, gv_int dim,
						  gv_int skin)
{
	if (speed == 0)
		return false;
	gv_int dim2 = 1 - dim;
	gv_int range = end[dim2] - start[dim2];
	// gv_int range2 = end[dim] - start[dim];
	if (range == 0)
	{
		// the same direction
		gv_int dir_speed = gvt_sign(speed);
		gv_vector2i closest_p =
			gvt_sign(end[dim] - start[dim]) == dir_speed ? start : end;
		if (intersect_ray(closest_p, -speed, mover, distance, dim, skin))
		{
			normal[dim] = -one() * gvt_sign(speed);
			normal[dim2] = 0;
			distance = -distance;
			contact = closest_p;
			return true;
		}
		return false;
	}
	if (range < 0)
		gvt_swap(start, end);
	gv_vector2i half_size = mover.get_size() / 2;
	gv_vector2i center = mover.center();
	// move the line toward the box.
	gv_vector2i dir = end - start;
	gv_int speed_dir_sign = gvt_sign(speed);
	dir = normalize_quick_and_not_precise(dir);
	gv_vector2i h = dir.ortho();
	if (h.dot(center - start) < 0)
	{
		h = -h;
	}
	gv_vector2i offset_1 = half_size;
	offset_1[dim] *= -speed_dir_sign;
	gv_vector2i offset_2 = offset_1;
	offset_2[dim2] *= -1;
	if (gvt_abs(offset_1.dot(h)) < gvt_abs(offset_2.dot(h)))
	{
		gvt_swap(offset_1, offset_2);
	}
	if (gvt_between(center[dim2], start[dim2] + offset_1[dim2],
					end[dim2] + offset_1[dim2]))
	{
		contact = gv_get_point_on_line_fixed(dim2, start + offset_1, end + offset_1,
											 center[dim2]);
		distance = contact[dim] - center[dim];
		contact[dim] -= offset_1[dim];
	}
	else if (gvt_between(center[dim2], start[dim2] - half_size[dim2],
						 start[dim2] + half_size[dim2]))
	{
		gv_vector2i v;
		v[dim] = start[dim] + offset_1[dim];
		v[dim2] = center[dim2];
		distance = v[dim] - center[dim];
	}
	else if (gvt_between(center[dim2], end[dim2] - half_size[dim2],
						 end[dim2] + half_size[dim2]))
	{
		gv_vector2i v;
		v[dim] = end[dim] + offset_1[dim];
		v[dim2] = center[dim2];
		distance = v[dim] - center[dim];
	}
	else
		return false;
#pragma GV_REMINDER( \
	"[PITFALL]    large error will occure if the line is almost parallax to an axis , and will have penatration in that case , just don't allow those lines. , to be solve more nice")
	if (test_if_hit_with_distance(h, speed, distance, skin + 12, normal))
		return true;
	return false;
};

gv_int gv_math_fixed::sini(gv_int angle)
{
	int sign_i = 1;
	if (angle < 0)
	{
		sign_i *= -1;
		angle = -angle;
	}
	angle = normalize_deg_angle(angle);
	if (angle > pi())
	{
		sign_i *= -1;
		angle -= pi();
	}
	if (angle > half_pi())
	{
		angle = pi() - angle;
	}
	gv_int ret = m_sin_table[angle];
	ret *= sign_i;
	return ret;
};
gv_int gv_math_fixed::cosi(gv_int angle)
{
	return sini(half_pi() - angle);
};
gv_int gv_math_fixed::tani(gv_int angle)
{
	int sign_i = 1;
	if (angle < 0)
	{
		sign_i *= -1;
		angle = -angle;
	}
	angle = normalize_deg_angle(angle);
	if (angle >= pi() + half_pi())
	{
		angle = two_pi() - angle;
		sign_i *= -1;
	}
	else if (angle >= pi())
	{
		angle = angle - pi();
	}
	else if (angle > half_pi())
	{
		angle = pi() - angle;
		sign_i *= -1;
	}
	gv_int ret = m_tan_table[angle];
	ret *= sign_i;
	return ret;
};

gv_vector2i gv_math_fixed::angle_to_dir(gv_int angle)
{
	return gv_vector2i(cosi(angle), sini(angle));
};

gv_int gv_math_fixed::length(const gv_vector2i& v)
{
	gv_vector2i n = normalize2d(v);
	return v.abs_sum() * s_fixed_one / n.abs_sum();
}

bool gv_math_fixed::is_target_within_distance(const gv_vector2i& source,
											  const gv_vector2i& target,
											  gv_int distance)
{
	bool ret = false;
	gv_vector2i distanceVector = target - source;

	//////////////////////////////////////////////////////////////////////////
	//			B
	//		   /|
	//		  / |
	//		 /  |
	//	   A-----C
	//
	// AB <= distance means target is within distance.
	//		ret = true;
	//
	// AB = sqrt(AC^2 + BC^2)
	//
	// 1.
	// AC < AB
	// BC < AB
	//		if distance < AC
	//		=>
	//		distance < AB
	// Same:
	//		if distance < BC
	//		=>
	//		distance < AB
	// 2.
	// (AC + BC)^2 >= AC^2 + BC^2
	//		if distance > AC+BC
	//		distance^2 > (AC + BC)^2 >= AC^2 + BC^2
	//		=>
	//		distance^2 > AB^2
	if (abs(distanceVector.x) > distance || abs(distanceVector.y) > distance)
	{
		ret = false;
	}
	else if (abs(distanceVector.x) + abs(distanceVector.y) < distance)
	{
		ret = true;
	}
	else
	{
		ret = distanceVector.length_squared() <= distance * distance;
	}

	return ret;
}

gv_int gv_math_fixed::atani(gv_int tan_value)
{
	// std::vector<int>::iterator low,up;
	// low=std::lower_bound (v.begin(), v.end(), 20);
	GV_ASSERT(tan_value >= 0);
	gvt_array< gv_int >::iterator it;
	it = std::lower_bound(m_tan_table.begin(), m_tan_table.end(), tan_value);
	if (it >= m_tan_table.end())
	{
		return half_pi();
	}
	return (gv_int)(gv_int_ptr)(it - m_tan_table.begin());
}
gv_int gv_math_fixed::dir_to_angle(const gv_vector2i& v)
{
	if (v.x == 0)
	{
		return v.y >= 0 ? half_pi() : -half_pi();
	}
	if (v.y == 0)
	{
		return v.x >= 0 ? 0 : pi();
	}
	gv_int t = v.y * one() / v.x;
	gv_int angle = atani(gvt_abs(t));
	if (v.x > 0 && v.y < 0)
	{
		return -angle;
	}
	else if (v.x < 0 && v.y < 0)
	{
		return -pi() + angle;
	}
	else if (v.x < 0 && v.y > 0)
	{
		return pi() - angle;
	}
	return angle;
};
void gv_math_fixed::rebuild_sin_table()
{
	m_sin_table.resize(half_pi() + 1);
	for (int i = 0; i <= half_pi(); i++)
	{
		m_sin_table[i] = to_fixed(sinf(deg_to_rad(i)));
	}
	m_tan_table.resize(half_pi() + 1);
	for (int i = 0; i < half_pi(); i++)
	{
		m_tan_table[i] = to_fixed(tanf(deg_to_rad(i)));
	}
	m_tan_table[half_pi()] = gvt_value< gv_int >::max();
};

void gv_math_fixed::init_from_array(int sin_length, gv_int* sin_table,
									int tan_length, gv_int* tan_table)
{
	m_sin_table.init(sin_table, sin_length);
	m_tan_table.init(tan_table, tan_length);
};

gv_int gv_math_fixed::ray_intersect_plane(const gv_int* ray_vector,
										  gv_int nb_dim, gv_int test_dim,
										  gv_int plane_d, gv_int skin)
{
	GV_ASSERT(nb_dim > test_dim);
	gv_int speed = ray_vector[test_dim];
	if (speed * plane_d < 0)
	{ // not the same direction , or on the plan
		return -1;
	}
	gv_int dist = gvt_abs(plane_d);
	dist = gvt_max(dist - skin, 0);
	speed = gvt_abs(speed);
	if (speed < dist)
	{ // can't reach
		return -1;
	}
	gv_int time = dist * s_fixed_one / speed;
	return time;
};

gv_int gv_math_fixed::ray_intersect_box(const gv_vector3i& ray_vector,
										const gv_boxi& collider,
										gv_vector3i& contact,
										gv_vector3i& normal, gv_int skin)
{
	gv_int time = s_fixed_one + 1;
	gv_vector3i zero_speed = ray_vector;
	zero_speed.abs();
	zero_speed = (zero_speed) / gv_math_fixed::one();
	for (int dim = 0; dim < 3; dim++)
	{
		gv_int d[2];
		gv_int speed = ray_vector[dim];
		if (speed)
		{
			d[0] = collider.min_p[dim];
			d[1] = collider.max_p[dim];
			if (speed * d[0] <= 0 ||
				speed * d[1] <= 0)
			{ // whether inside or in the wrong direction
				// try other dimentiion
				continue;
			}
			gv_vector3i ext;
			ext = 0;
			ext[dim] = zero_speed[dim]; // to prevent speed too big..
			gv_boxi bb = collider;
			bb.extend(gv_vector3i(skin + 2) + ext);

			for (int i = 0; i < 2; i++)
			{
				gv_int cu_time =
					ray_intersect_plane(&ray_vector[0], 3, dim, d[i], skin);
				if (cu_time >= 0 && cu_time < time)
				{
					contact = ray_vector * cu_time / s_fixed_one;
					if (bb.is_point_inside(contact))
					{
						time = cu_time;
						normal = 0;
						normal[dim] = -gvt_sign(speed) * gv_math_fixed::one();
					}
				}
			}
		}
	} // next dimention
	if (time >= 0 && time < s_fixed_one)
		return time;
	return -1;
};
gv_vector2i gv_math_fixed::project(const gv_vector3i& v, gv_int project_dim)
{
	gv_vector2i r;
	r.x = v[(project_dim + 1) % 3];
	r.y = v[(project_dim + 2) % 3];
	return r;
}

gv_recti gv_math_fixed::project(const gv_boxi& box, gv_int project_dim)
{
	GV_ASSERT(project_dim < 3);
	gv_recti r;
	r.min_p = project(box.min_p, project_dim);
	r.max_p = project(box.max_p, project_dim);
	return r;
};

gv_vector3i gv_math_fixed::project_in_direction(const gv_vector3i& vec,
												const gv_vector3i& dir,
												gv_int& project)
{
	project = vec.dot(dir) / one();
	gv_vector3i f = dir * project / one();
	return f;
};

gv_int gv_math_fixed::mul(gv_int a, gv_int b)
{
	return a * b / one();
};

gv_int gv_math_fixed::div(gv_int a, gv_int b)
{
	return a * one() / b;
};

gv_vector3i gv_math_fixed::mul(const gv_vector3i& a, gv_int b)
{
	return a * b / one();
};
gv_vector3i gv_math_fixed::div(const gv_vector3i& a, gv_int b)
{
	return a * one() / b;
};
}