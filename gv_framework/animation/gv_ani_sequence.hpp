
namespace gv
{

gv_ani_sequence::gv_ani_sequence()
{
	GVM_SET_CLASS(gv_ani_sequence);
	m_duration_in_ms = -1;
	m_duration = 0;
}

gv_ani_track* gv_ani_sequence::get_track(const gv_id& id)
{
	for (int i = 0; i < this->m_animation_track.size(); i++)
	{
		if (this->m_animation_track[i]->get_name_id() == id)
			return m_animation_track[i];
	}
	return NULL;
};

void gv_ani_sequence::compress(gv_float ratio)
{
	for (int i = 0; i < this->m_animation_track.size(); i++)
	{
		m_animation_track[i] = m_animation_track[i]->compress(ratio);
	}
};

void gv_ani_sequence::set_duration(gv_float f)
{
	m_duration = f;
	for (int i = 0; i < this->m_animation_track.size(); i++)
	{
		m_animation_track[i]->set_duration(f);
	}
};

gv_int gv_ani_sequence::get_duration_in_ms()
{
	if (m_duration_in_ms == -1)
		reset_duration_in_ms();
	return m_duration_in_ms;
};

void gv_ani_sequence::reset_duration_in_ms()
{
	m_duration_in_ms = (gv_int)(get_duration() * 1000.f);
}
bool gv_ani_sequence::post_load()
{
	super::post_load();
	if (m_duration_in_ms == -1)
		reset_duration_in_ms();
	return true;
}; // call after load .

int gv_ani_sequence::get_frame_count()
{
	GV_ASSERT(m_animation_track.size());
	return m_animation_track[0]->get_frame_count();
}
GVM_IMP_CLASS(gv_ani_sequence, gv_object)
GVM_VAR(gv_float, m_duration)
GVM_VAR(gv_int, m_duration_in_ms)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_ani_track > >, m_animation_track)
GVM_END_CLASS
}