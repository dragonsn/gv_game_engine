#pragma once
namespace gv
{
class gv_ani_track : public gv_object
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_ani_track, gv_object);
	~gv_ani_track(){};
	// all the time is in seconds.
	virtual gv_float get_frequency() const
	{
		return 30.f;
	}
	virtual void set_frequency(gv_float f)
	{
	}
	virtual void set_duration(gv_float f){};
	virtual gv_float get_duration() const
	{
		return 0;
	};
	virtual gv_int get_frame_count() const
	{
		return (gv_int)(get_duration() * get_frequency());
	};
	virtual gv_int
	get_frame_number(gv_float time,
					 bool do_loop) const // get the frame count at a specific time
	{
		gv_int nb_key = get_frame_count();
		gv_int cu_key = (gv_int)(time * get_frequency());
		if (do_loop)
			cu_key = cu_key % nb_key;
		cu_key = gvt_clamp(cu_key, 0, nb_key - 1);
		return (gv_ushort)cu_key;
	}
	virtual void get_trans_rot(float time, gv_vector3& pos, gv_quat& rot,
							   gv_vector3& scale, bool do_loop) const
	{
		pos = gv_vector3::get_zero_vector();
		rot = gv_quat::get_zero_rotation();
		scale = gv_vector3(1, 1, 1);
	};

	virtual gv_matrix44 get_world_tm(float time, bool do_loop) const
	{
		return gv_matrix44::get_identity();
	}
	virtual void insert_rot_key(float time, const gv_quat& q){};
	virtual void insert_trans_key(float time, const gv_vector3& pos){};
	virtual gv_ani_track* compress(float ratio = 1.0f)
	{
		return NULL;
	};
};

class gv_ani_track_raw : public gv_ani_track
{
public:
	GVM_DCL_CLASS(gv_ani_track_raw, gv_ani_track);
	gv_ani_track_raw();
	~gv_ani_track_raw(){};

	virtual gv_float get_frequency() const
	{
		return m_frequency;
	}
	virtual void set_frequency(gv_float f)
	{
		m_frequency = f;
	}
	virtual void set_duration(gv_float f);
	virtual gv_float get_duration() const;
	virtual gv_int get_frame_count() const;
	virtual void get_trans_rot(float time, gv_vector3& pos, gv_quat& rot,
							   gv_vector3& scale, bool do_loop) const;
	virtual void insert_rot_key(float time, const gv_quat& q);
	virtual void insert_trans_key(float time, const gv_vector3& pos);
	virtual gv_ani_track* compress(float ratio);

public:
	gv_float m_frequency;
	gvt_array< gv_vector3 > m_pos;
	gvt_array< gv_quat > m_rot;
};

class gv_ani_track_with_key : public gv_ani_track
{
public:
	GVM_DCL_CLASS(gv_ani_track_with_key, gv_ani_track);
	gv_ani_track_with_key();
	~gv_ani_track_with_key(){};
	virtual gv_float get_frequency() const
	{
		return m_frequency;
	}
	virtual void set_frequency(gv_float f)
	{
		m_frequency = f;
	}
	virtual void set_duration(gv_float f)
	{
		m_duration = f;
	};
	virtual gv_float get_duration() const
	{
		return m_duration;
	};
	virtual void get_trans_rot(float time, gv_vector3& pos, gv_quat& rot,
							   gv_vector3& scale, bool do_loop) const;
	virtual void insert_rot_key(float time, const gv_quat& q);
	virtual void insert_trans_key(float time, const gv_vector3& pos);
	virtual void insert_scale_key(float time, const gv_vector3& scale);
	virtual gv_ani_track* compress(float ratio = 1.0f);
	virtual void init(gv_ani_track_raw* raw);
	virtual gv_matrix44 get_world_tm(float time, bool do_loop) const;

public:
	gv_int m_father_track;
	gv_float m_duration;
	gv_float m_frequency;
	gv_vector3 m_initial_pos;
	gv_vector3 m_initial_scale;
	gv_quat m_initial_rot;
	gv_ani_pos_track m_pos_track;
	gv_ani_rot_track m_rot_track;
	gv_ani_scale_track m_scale_track;
};
};