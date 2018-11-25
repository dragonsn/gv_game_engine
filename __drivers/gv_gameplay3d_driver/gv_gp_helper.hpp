namespace gv
{
Matrix gv_gp_helper::to_gp(const gv_matrix44& m)
{
	return Matrix(m.m[0]);
};
Quaternion gv_gp_helper::to_gp(const gv_quat& q)
{
	return Quaternion(q.x, q.y, q.z, q.w);
};
Vector3 gv_gp_helper::to_gp(const gv_vector3& v)
{
	return Vector3(v.get_x(), v.get_y(), v.get_z());
};
Vector4 gv_gp_helper::to_gp(const gv_vector4& v)
{
	return Vector4(v.x, v.y, v.z, v.w);
};
gameplay::BoundingBox gv_gp_helper::to_gp(const gv_box& box)
{
	return BoundingBox(to_gp(box.min_p), to_gp(box.max_p));
};
gameplay::BoundingSphere gv_gp_helper::to_gp(const gv_sphere& sphere)
{
	return BoundingSphere(to_gp(sphere.pos), sphere.r);
};
gv_matrix44 gv_gp_helper::to_gv(const Matrix& m)
{
	return gv_matrix44(m.m);
};
gv_quat gv_gp_helper::to_gv(const Quaternion& q)
{
	return gv_quat(q.x, q.y, q.z, q.w);
};
gv_vector3 gv_gp_helper::to_gv(const Vector3& v)
{
	return gv_vector3(v.x, v.y, v.z);
};
gv_vector4 gv_gp_helper::to_gv(const Vector4& v)
{
	return gv_vector4(v.x, v.y, v.z, v.w);
};
gv_box gv_gp_helper::to_gv(const gameplay::BoundingBox& b)
{
	return gv_box(to_gv(b.min), to_gv(b.max));
};
gv_sphere gv_gp_helper::to_gv(const gameplay::BoundingSphere& b)
{
	return gv_sphere(to_gv(b.center), b.radius);
};

gameplay::Touch::TouchEvent gv_gp_helper::to_gp(gve_touch_event e)
{
	switch (e)
	{
	case e_touch_move:
		return gameplay::Touch::TOUCH_MOVE;
	case e_touch_press:
		return gameplay::Touch::TOUCH_PRESS;
	case e_touch_release:
		return gameplay::Touch::TOUCH_RELEASE;
	}
	return gameplay::Touch::TOUCH_MOVE;
};
gve_touch_event gv_gp_helper::to_gv(gameplay::Touch::TouchEvent e)
{
	switch (e)
	{
	case gameplay::Touch::TOUCH_MOVE:
		return e_touch_move;
	case gameplay::Touch::TOUCH_PRESS:
		return e_touch_press;
	case gameplay::Touch::TOUCH_RELEASE:
		return e_touch_release;
	}
	return e_touch_none;
};
};
