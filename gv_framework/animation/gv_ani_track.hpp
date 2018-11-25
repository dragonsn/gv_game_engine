#pragma once
namespace gv
{
//==================================================================================>>
gv_ani_track_raw::gv_ani_track_raw()
{
	GVM_SET_CLASS(gv_ani_track_raw);
	this->m_frequency = 30;
}
void gv_ani_track_raw::set_duration(gv_float f)
{
	gv_int nb_frame = (gv_int)(f * get_frequency());
	this->m_pos.resize(nb_frame);
	this->m_rot.resize(nb_frame);
};
gv_float gv_ani_track_raw::get_duration() const
{
	return m_pos.size() / get_frequency();
};
gv_int gv_ani_track_raw::get_frame_count() const
{
	return (gv_ushort)m_pos.size();
};
void gv_ani_track_raw::get_trans_rot(float time, gv_vector3& pos, gv_quat& rot,
									 gv_vector3& scale, bool do_loop) const
{
	if (!m_pos.size())
	{
		pos = gv_vector3::get_zero_vector();
		rot = gv_quat::get_zero_rotation();
		scale = gv_vector3(1, 1, 1);
		return;
	}
	gv_int cu_key = get_frame_number(time, do_loop); //(gv_int)(time*m_frequency);
	pos = m_pos[cu_key];
	rot = m_rot[cu_key];
	scale = gv_vector3(1, 1, 1);
};

void gv_ani_track_raw::insert_rot_key(float time, const gv_quat& q)
{
	gv_int nb_key = m_pos.size();
	gv_int cu_key = (gv_int)(time * m_frequency);
	cu_key = gvt_clamp(cu_key, 0, nb_key - 1);
	m_rot[cu_key] = q;
};

void gv_ani_track_raw::insert_trans_key(float time, const gv_vector3& pos)
{
	gv_int nb_key = m_pos.size();
	gv_int cu_key = (gv_int)(time * m_frequency);
	cu_key = gvt_clamp(cu_key, 0, nb_key - 1);
	m_pos[cu_key] = pos;
};

gv_ani_track* gv_ani_track_raw::compress(float ratio)
{
	gv_ani_track_with_key* compressed =
		get_sandbox()->create_object< gv_ani_track_with_key >(this->get_name(),
															  this->get_owner());
	compressed->init(this);
	this->set_owner(NULL);
	return compressed;
};
//==================================================================================>>
gv_ani_track_with_key::gv_ani_track_with_key()
{
	GVM_SET_CLASS(gv_ani_track_with_key);
	this->m_frequency = 30.f;
	this->m_duration = 0;
	m_initial_pos = gv_vector3::get_zero_vector();
	m_initial_scale = gv_vector3(1, 1, 1);
	;
	m_initial_rot = gv_quat(0, 0, 0, 1);
	m_father_track = -1;
}
void gv_ani_track_with_key::get_trans_rot(float time, gv_vector3& pos,
										  gv_quat& rot, gv_vector3& scale,
										  bool do_loop) const
{
	if (m_pos_track.size())
	{
		pos = m_pos_track.get(time, do_loop);
	}
	else
		pos = m_initial_pos;
	if (m_rot_track.size())
	{
		rot = m_rot_track.get(time, do_loop);
	}
	else
		rot = m_initial_rot;
	if (m_scale_track.size())
	{
		scale = m_scale_track.get(time, do_loop);
	}
	else
		scale = m_initial_scale;
}

gv_matrix44 gv_ani_track_with_key::get_world_tm(float time,
												bool do_loop) const
{
	gv_vector3 pos;
	gv_quat rot;
	gv_vector3 scale;
	get_trans_rot(time, pos, rot, scale, do_loop);
	gv_matrix44 mat;
	gv_math::compose(mat, pos, rot, scale);
	if (m_father_track != -1)
	{
		const gv_ani_sequence* ani = gvt_cast< gv_ani_sequence >(this->get_owner());
		mat = ani->get_track(m_father_track)->get_world_tm(time, do_loop) * mat;
	}
	return mat;
}

void gv_ani_track_with_key::insert_rot_key(float time, const gv_quat& q)
{
	m_rot_track.insert_key(time, q);
};
void gv_ani_track_with_key::insert_scale_key(float time,
											 const gv_vector3& scale)
{
	m_scale_track.insert_key(time, scale);
}
void gv_ani_track_with_key::insert_trans_key(float time,
											 const gv_vector3& pos)
{
	m_pos_track.insert_key(time, pos);
};
gv_ani_track* gv_ani_track_with_key::compress(float ratio)
{
	this->m_pos_track.digest_equal_keys();
	this->m_rot_track.digest_equal_keys();
	this->m_scale_track.digest_equal_keys();
	return this;
};

void gv_ani_track_with_key::init(gv_ani_track_raw* raw)
{
	this->set_duration(raw->get_duration());
	this->set_frequency(raw->get_frequency());
	gv_float dt = 1.0f / raw->m_frequency;
	gv_float time = 0;
	for (int i = 0; i < raw->m_pos.size(); i++, time += dt)
	{
		this->m_pos_track.insert_key(time, raw->m_pos[i]);
	}
	time = 0;
	for (int i = 0; i < raw->m_rot.size(); i++, time += dt)
	{
		this->m_rot_track.insert_key(time, raw->m_rot[i]);
	}
	GV_ASSERT(m_pos_track.get_duration() == m_rot_track.get_duration());
	this->m_pos_track.digest_equal_keys();
	this->m_rot_track.digest_equal_keys();
};
//============================================================================================
//								:
//============================================================================================

GVM_IMP_CLASS(gv_ani_track, gv_object)
GVM_END_CLASS

GVM_IMP_CLASS(gv_ani_track_raw, gv_ani_track);
GVM_VAR(gv_float, m_frequency)
GVM_VAR(gvt_array< gv_vector3 >, m_pos)
GVM_VAR(gvt_array< gv_quat >, m_rot)
GVM_END_CLASS

GVM_IMP_STRUCT(gv_ani_pos_key)
GVM_VAR(gv_float, m_time)
GVM_VAR(gv_vector3, m_key)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_ani_scale_key)
GVM_VAR(gv_float, m_time)
GVM_VAR(gv_vector3, m_key)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_ani_rot_key)
GVM_VAR(gv_float, m_time)
GVM_VAR(gv_quat, m_key)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_ani_pos_track)
GVM_VAR(gvt_array< gv_ani_pos_key >, m_keys)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_ani_rot_track)
GVM_VAR(gvt_array< gv_ani_rot_key >, m_keys)
GVM_END_STRUCT

GVM_IMP_STRUCT(gv_ani_scale_track)
GVM_VAR(gvt_array< gv_ani_scale_key >, m_keys)
GVM_END_STRUCT

GVM_IMP_CLASS(gv_ani_track_with_key, gv_ani_track);
GVM_VAR(gv_int, m_father_track)
GVM_VAR(gv_float, m_frequency)
GVM_VAR(gv_float, m_duration)
GVM_VAR(gv_ani_rot_track, m_rot_track)
GVM_VAR(gv_ani_pos_track, m_pos_track)
GVM_VAR(gv_ani_scale_track, m_scale_track)
GVM_VAR(gv_vector3, m_initial_pos)
GVM_VAR(gv_vector3, m_initial_scale)
GVM_VAR(gv_quat, m_initial_rot)
GVM_END_CLASS
}