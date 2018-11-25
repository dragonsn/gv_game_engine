namespace gv
{
// gp gv good friend .
class gv_gp_helper
{
public:
	static gameplay::Matrix to_gp(const gv_matrix44& m);
	static gameplay::Quaternion to_gp(const gv_quat& q);
	static gameplay::Vector3 to_gp(const gv_vector3& v);
	static gameplay::Vector4 to_gp(const gv_vector4& v);
	static gameplay::BoundingBox to_gp(const gv_box& box);
	static gameplay::BoundingSphere to_gp(const gv_sphere& sphere);
	static gameplay::Touch::TouchEvent to_gp(gve_touch_event);

	static gv_matrix44 to_gv(const gameplay::Matrix& m);
	static gv_quat to_gv(const gameplay::Quaternion& q);
	static gv_vector3 to_gv(const gameplay::Vector3& v);
	static gv_vector4 to_gv(const gameplay::Vector4& v);
	static gv_box to_gv(const gameplay::BoundingBox& b);
	static gv_sphere to_gv(const gameplay::BoundingSphere& b);
	static gve_touch_event to_gv(gameplay::Touch::TouchEvent);
};
}
