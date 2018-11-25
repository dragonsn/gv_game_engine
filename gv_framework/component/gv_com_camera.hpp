namespace gv
{
gv_com_camera::gv_com_camera()
{
	link_class(gv_com_camera::static_class());
	m_use_smooth_follow = false;
	set_fov(45, 1.333f, 1.f, 3000.f);
	set_look_at(gv_vector3(1, 0, 300.f), gv_vector3(0, 0, 0));
	m_world_entity_matrix.set_identity();
	m_entity_world_matrix.set_identity();
	update_projection_view_matrix();
	// update_world_matrix();
}

void gv_com_camera::set_current_camera(){
	// GVM_POST_EVENT(render_set_camera,render,(pe->camera=this) );
};

void gv_com_camera::set_look_at(const gv_vector3& eye, const gv_vector3& lookat,
								const gv_vector3 up)
{
	m_eye_offset = eye;
	m_lookat_offset = lookat;
	m_up_vector = up;
};

void gv_com_camera::get_look_at(gv_vector3& eye, gv_vector3& lookat,
								gv_vector3& up)
{
	eye = m_eye_offset;
	lookat = m_lookat_offset;
	up = m_up_vector;
};

gv_float gv_com_camera::get_eye_distance()
{
	gv_vector3 eye_dir = m_eye_offset - m_lookat_offset;
	float dist = eye_dir.normalize();
	return dist;
};

void gv_com_camera::set_eye_distance(gv_float dist)
{
	gv_vector3 eye_dir = m_eye_offset - m_lookat_offset;
	eye_dir.normalize();
	m_eye_offset = m_lookat_offset + eye_dir * dist;
};

void gv_com_camera::set_fov(gv_float fov, gv_float ratio, gv_float z_near,
							gv_float z_far)
{
	m_fov = fov;
	m_ratio = ratio;
	m_near_z = z_near;
	m_far_z = z_far;
}
void gv_com_camera::set_fov(gv_float fov)
{
	m_fov = fov;
};

void gv_com_camera::update_projection_view_matrix()
{

#pragma GV_REMINDER( \
	"[PITFALL]    |------>[rendering] to switch left hand & right hand system here,issuse: frumstum need to be updated, the face culling need to be updated ")
	// has to be left handed,for dx font and some other issue.
	// m_projection_matrix.set_camera_set_perspective_left_hand(m_fov,m_ratio,
	// m_near_z, m_far_z);
	// m_entity_view_matrix.set_camera_lookat_left_hand(m_eye_offset,m_lookat_offset,m_up_vector);
	// m_frustum.set_fov(m_fov, m_ratio, m_near_z, m_far_z);

	m_frustum.set_fov_right_hand(m_fov, m_ratio, m_near_z, m_far_z);
	m_projection_matrix.set_camera_set_perspective(m_fov, m_ratio, m_near_z,
												   m_far_z);
	m_entity_view_matrix.set_camera_lookat(m_eye_offset, m_lookat_offset,
										   m_up_vector);
	this->update_world_matrix();
};

bool gv_com_camera::sync_to_entity(gv_entity* entity)
{
	GV_ASSERT(entity == get_entity());
	entity->update_matrix();
	this->m_world_entity_matrix = entity->get_inv_tm();
	this->m_entity_world_matrix = entity->get_tm();
	this->update_world_matrix();
	return true;
}

void gv_com_camera::update_world_matrix()
{
	this->m_world_view_matrix =
		this->m_world_entity_matrix * this->m_entity_view_matrix;
	this->m_view_world_matrix = this->m_world_view_matrix.get_inverse();
	this->m_world_view_project_matrix =
		m_world_view_matrix * this->m_projection_matrix;
	gv_matrix44 mat = this->m_world_view_matrix.get_inverse_noscale_3dtransform();
	this->m_world_frustum = m_frustum.transform(mat);
}

void gv_com_camera::update_camera(gv_float dt){

}; // called from renderer.

const gv_frustum& gv_com_camera::get_local_frustum()
{
	return m_frustum;
};

const gv_frustum& gv_com_camera::get_world_frustum()
{
	return m_world_frustum;
};

gv_vector3 gv_com_camera::get_world_position()
{
	// return get_entity()->get_position();
	return m_view_world_matrix.get_trans();
};

gv_matrix44 gv_com_camera::get_world_rotation()
{
	gv_matrix44 m;
	m_view_world_matrix.get_rotation(m);
	return m;
}

gv_vector3 gv_com_camera::get_world_dir()
{
	// return -get_entity()->get_tm().axis_z;
	// the matrix is lef handed!!
	return m_view_world_matrix.axis_z;
};

gv_vector3 gv_com_camera::window_to_world(const gv_vector2i& win_pos)
{
	gv_vector4 v((gv_float)win_pos.x, (gv_float)win_pos.y, -1.f, 1);
	gv_vector2i vsize = get_sandbox()->get_debug_renderer()->get_window_size();
	gv_vector4 ret;
	v.x -= (gv_float)vsize.x / 2.f;
	v.x /= (gv_float)vsize.x / 2.f;
	v.y = (gv_float)vsize.y / 2.f - v.y;
	v.y /= (gv_float)vsize.y / 2.f;
	gv_matrix44 mat = this->m_world_view_project_matrix;
	gv_matrix44 mat2 = mat.get_inverse();
	ret = mat2.mul_by(v);
	ret /= ret.w;
	return ret;
};

gv_vector2i gv_com_camera::world_to_window(const gv_vector3& world_pos)
{
	gv_matrix44 mat = this->m_world_view_project_matrix;
	gv_vector4 v = mat.mul_by(gv_vector4(world_pos));
	gv_vector2i ret;
	v /= v.w;
	gv_vector2i vsize = get_sandbox()->get_debug_renderer()->get_window_size();
	v.x = (v.x + 1.0f) * 0.5f * vsize.x;
	v.y = (1.0f - v.y) * 0.5f * vsize.y;
	ret = gv_vector2i((gv_int)v.x, (gv_int)v.y);
	return ret;
};
}

namespace gv
{
GVM_IMP_CLASS(gv_com_camera, gv_component)
GVM_VAR(gv_float, m_fov)
GVM_VAR(gv_float, m_near_z)
GVM_VAR(gv_float, m_far_z)
GVM_VAR(gv_vector3, m_eye_offset)
GVM_VAR(gv_vector3, m_lookat_offset)
GVM_VAR(gv_vector3, m_up_vector)
GVM_VAR(gv_bool, m_use_smooth_follow)
GVM_VAR(gv_float, m_ratio)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gv_frustum, m_frustum)
GVM_VAR(gv_matrix44, m_projection_matrix)
GVM_VAR(gv_matrix44, m_entity_view_matrix)
GVM_VAR(gv_matrix44, m_world_view_matrix)
GVM_VAR(gv_matrix44, m_view_world_matrix)
GVM_VAR(gv_matrix44, m_world_entity_matrix)
GVM_VAR(gv_matrix44, m_entity_world_matrix)
GVM_VAR(gv_frustum, m_world_frustum)
GVM_VAR(gv_matrix44, m_world_view_project_matrix)
GVM_END_CLASS
}