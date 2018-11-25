#pragma once

namespace gv
{
class gv_ani_track;
class gv_ani_sequence : public gv_object
{
public:
	GVM_DCL_CLASS(gv_ani_sequence, gv_object)
	gv_ani_sequence();
	bool post_load();
	float get_duration() const
	{
		return m_duration;
	};
	gv_int get_duration_in_ms();
	void set_duration(gv_float f);
	void reset_duration_in_ms();
	int get_track_number() const
	{
		return m_animation_track.size();
	};
	int get_frame_count();
	gv_ani_track* get_track(int index)
	{
		if (m_animation_track.is_valid(index))
			return m_animation_track[index];
		return NULL;
	}
	const gv_ani_track* get_track(int index) const
	{
		if (m_animation_track.is_valid(index))
			return m_animation_track[index];
		return NULL;
	}
	gv_ani_track* get_track(const gv_id& id);
	void add_track(gv_ani_track* ptrack)
	{
		m_animation_track.push_back(ptrack);
	};
	void compress(gv_float ratio = 1.0f);

protected:
	gv_float m_duration;
	gv_int m_duration_in_ms;
	gvt_array< gvt_ref_ptr< gv_ani_track > > m_animation_track;
};
}