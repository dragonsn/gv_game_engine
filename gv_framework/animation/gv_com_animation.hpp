#pragma once

namespace gv
{
//============================================================================================
//								:
//============================================================================================
gv_com_animation::gv_com_animation()
{
	m_is_root_motion_enabled = false;
	m_is_update_root_only = false;
	link_class(gv_com_animation::static_class());
};

gv_com_animation::~gv_com_animation()
{
	this->clear_all_animation();
};
//============================================================================================
//								:
//============================================================================================
void gv_com_animation::set_ani_set(gv_ani_set* set)
{
	this->m_cu_animation_set = set;
};
gv_ani_set* gv_com_animation::get_ani_set()
{
	return this->m_cu_animation_set;
};
//============================================================================================
//								:
//============================================================================================
bool gv_com_animation::play_animation(const gv_id& animation_id,
									  gv_float blending_time, bool loop,
									  bool clear,
									  gv_com_tasklet* on_ani_finished)
{
	if (!this->m_cu_animation_set)
		return false;
	gv_ani_sequence* pani = this->get_ani_set()->get_sequence(animation_id);
	if (!pani)
	{
		GVM_WARNING("can't find animation " << animation_id << " in "
											<< m_cu_animation_set->get_location()
											<< gv_endl);
		return false;
	}

	if (this->m_play_info_tail.ptr() &&
		this->m_play_info_tail->m_ani_sequence == pani)
	{
		if (get_cu_ani()->m_current_time > get_cu_ani()->m_end_time ||
			this->m_play_info_tail->m_loop)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	if (clear) // clear the playing queue
	{
		if (this->get_cu_ani())
		{
			if (!gv_float_trait::is_almost_zero(blending_time))
			{
				clear_all_animation(true);
				get_cu_ani()->m_end_time = get_cu_ani()->m_current_time + blending_time;
				get_cu_ani()->m_blending_start = true;
			}
			else
			{
				clear_all_animation();
			}
		}
	}
	gv_ani_play_info* info = this->alloc_play_info();
	info->m_ani_sequence = pani;
	info->m_current_time = 0.0f;
	info->m_blending_time = blending_time;
	info->m_loop = loop;
	info->m_next = NULL;
	// info->m_task_on_ani_stop=on_ani_finished;
	if (!loop)
		info->m_end_time = pani->get_duration();
	else
		info->m_end_time = gv_float_trait::max();
	this->push_back_play_info(info);
	return true;
};

void gv_com_animation::clear_all_animation(bool keep_first)
{
	if (!get_cu_ani())
		return;
	if (keep_first)
	{
		this->remove_following_animation(get_cu_ani());
	}
	else
	{
		while (get_cu_ani())
			this->pop_front_play_info();
	}
};

const gv_id& gv_com_animation::get_animation_name()
{
	if (this->get_cu_ani())
	{
		if (this->get_cu_ani()->m_blending_start && this->get_cu_ani()->m_next)
		{
			return this->get_cu_ani()->m_next->m_ani_sequence->get_name_id();
		}
		else
			return get_cu_ani()->m_ani_sequence->get_name_id();
	}
	return gv_id_null;
};
//============================================================================================
//								:
//============================================================================================

bool gv_com_animation::is_animation_finished()
{
	return !get_cu_ani();
};

bool gv_com_animation::is_blending_start()
{
	if (get_cu_ani())
	{
		return get_cu_ani()->m_blending_start;
	}
	return false;
};
//============================================================================================
//								:
//============================================================================================
void gv_com_animation::enable_update_root_only(bool enable)
{
	this->m_is_update_root_only = enable;
};

void gv_com_animation::enable_root_motion(bool enable)
{
	this->m_is_root_motion_enabled = enable;
	if (enable)
	{
		this->m_root_tm_ref = get_entity()->get_tm();
	}
};
//============================================================================================
//								:
//============================================================================================
bool gv_com_animation::tick(gv_float dt)
{
	if (!m_cu_animation_set)
		return false;
	if (!get_cu_ani())
		return true;
	const gv_com_skeletal_mesh* com_sk_mesh =
		get_entity()->get_component< gv_com_skeletal_mesh >();
	gv_skeletal* my_skeletal = NULL;
	if (com_sk_mesh && !com_sk_mesh->is_visible_last_frame())
	{
		get_cu_ani()->m_current_time += dt;
		if (get_cu_ani()->m_current_time < get_cu_ani()->m_end_time)
		{
			return true;
		}
	}
	if (com_sk_mesh)
	{
		my_skeletal = com_sk_mesh->get_writable_skeletal();
	}
	if (!my_skeletal)
	{
		gv_ani_play_info* pani = get_cu_ani();
		gv_ani_sequence* pseq = get_cu_ani()->m_ani_sequence;
		if (pseq)
		{
			int nb_track = pseq->get_track_number();
			if (nb_track)
			{
				gv_vector3 pos, scale;
				gv_quat q;
				gv_ani_track* ptrack = pseq->get_track(0);
				ptrack->get_trans_rot(pani->m_current_time, pos, q, scale,
									  pani->m_loop);
				gv_math::convert(this->m_root_tm_ani, q);
				this->m_root_tm_ani.set_trans(pos);
				this->m_root_tm_ani.scale_by(scale);
			}
		}
	}
	else if (m_is_update_root_only)
	{
		gv_ani_play_info* pani = get_cu_ani();
		gv_ani_sequence* pseq = get_cu_ani()->m_ani_sequence;
		if (pseq)
		{
			int nb_track = pseq->get_track_number();
			if (nb_track)
			{
				gv_vector3 pos, scale;
				gv_quat q;
				gv_ani_track* ptrack = pseq->get_track(0);
				ptrack->get_trans_rot(pani->m_current_time, pos, q, scale,
									  pani->m_loop);
				my_skeletal->set_bone_local_rotation_trans(ptrack->get_name_id(), q,
														   pos, scale);
			}
		}
	}
	else if (!is_blending_start() ||
			 !get_cu_ani()->m_next) // one animation take place
	{
		gv_ani_play_info* pani = get_cu_ani();
		gv_ani_sequence* pseq = get_cu_ani()->m_ani_sequence;
		if (pseq)
		{
			int nb_track = pseq->get_track_number();
			for (int i = 0; i < nb_track; i++)
			{
				gv_vector3 pos, scale;
				gv_quat q;
				gv_ani_track* ptrack = pseq->get_track(i);
				ptrack->get_trans_rot(pani->m_current_time, pos, q, scale,
									  pani->m_loop);
				my_skeletal->set_bone_local_rotation_trans(ptrack->get_name_id(), q,
														   pos, scale);
			}
		}
	}
	else // blending 2 animations
	{
		gv_ani_play_info* pani1 = get_cu_ani();
		gv_ani_play_info* pani2 = get_cu_ani()->m_next;
		gv_float alpha =
			1.f -
			(pani1->m_end_time - pani1->m_current_time) / pani1->m_blending_time;
		alpha = gvt_clamp(alpha, 0.f, 1.f);
		gv_ani_sequence* pseq1 = pani1->m_ani_sequence;
		gv_ani_sequence* pseq2 = pani2->m_ani_sequence;
		if (pseq1 && pseq2 &&
			pseq1->get_track_number() == pseq2->get_track_number())
		{
			int nb_track = pseq2->get_track_number();
			for (int i = 0; i < nb_track; i++)
			{
				gv_vector3 pos, pos1, pos2, s, s1, s2;
				gv_quat q, q1, q2;
				gv_ani_track* ptrack1 = pseq1->get_track(i);
				gv_ani_track* ptrack2 = pseq2->get_track(i);
				if (ptrack1->get_name_id() != ptrack2->get_name_id())
				{
					ptrack1 = pseq1->get_track(ptrack2->get_name_id());
				}
				if (ptrack1)
				{
					ptrack1->get_trans_rot(pani1->m_current_time, pos1, q1, s1,
										   pani1->m_loop);
					ptrack2->get_trans_rot(pani2->m_current_time, pos2, q2, s2,
										   pani2->m_loop);
					q = q1.slerp(q2, alpha);
					pos = gvt_lerp(pos1, pos2, alpha);
					s = gvt_lerp(s1, s2, alpha);
				}
				else
				{
					ptrack2->get_trans_rot(pani2->m_current_time, pos, q, s,
										   pani2->m_loop);
				}
				my_skeletal->set_bone_local_rotation_trans(ptrack2->get_name_id(), q,
														   pos, s);
			}
		}
		pani2->m_current_time += dt;
	}
	if (my_skeletal)
	{
		if (m_is_root_motion_enabled)
		{
			this->m_root_tm_ani = my_skeletal->get_bone(0)->m_tm;
			my_skeletal->m_root_tm = m_root_tm_ani.get_inverse();
		}
		else
		{
			// my_skeletal->m_root_tm.set_identity();
		}
		my_skeletal->update_world_matrix();
	}
	get_cu_ani()->m_current_time += dt;
	if (get_cu_ani()->m_current_time > get_cu_ani()->m_end_time)
	{
		if (get_cu_ani()->m_task_on_ani_stop)
		{
			get_cu_ani()->m_task_on_ani_stop->signal();
			get_cu_ani()->m_task_on_ani_stop = NULL;
		};
		if (get_cu_ani()->m_next)
			this->pop_front_play_info();
	}
	return true;
};

bool gv_com_animation::play_new_animation(const gv_id& animation_id,
										  gv_float blending_time, bool loop,
										  bool clear_list,
										  gv_com_tasklet* on_ani_finished)
{
	if (get_animation_name() != animation_id)
	{
		return play_animation(animation_id, blending_time, loop, clear_list,
							  on_ani_finished);
	}
	return true;
};
//============================================================================================
//								:
//============================================================================================
bool gv_com_animation::set_resource(gv_resource* pres)
{
	gv_ani_set* set = gvt_cast< gv_ani_set >(pres);
	if (!set)
		return false;
	this->set_ani_set(set);
	return true;
};
bool gv_com_animation::is_tickable()
{
	return true;
};
bool gv_com_animation::sync_to_entity(gv_entity* entity)
{
	if (m_is_root_motion_enabled)
	{
		// sync the root motion to the object, but it's a little complexed
		//, need to remove the motion movement from skeletal.
		entity->set_tm(this->m_root_tm_ref * this->m_root_tm_ani);
	}
	return true;
};
//============================================================================================
//								:
//============================================================================================
gv_ani_play_info* gv_com_animation::alloc_play_info()
{
	return new gv_ani_play_info;
};

void gv_com_animation::delete_play_info(gv_ani_play_info* pani)
{
	delete pani;
};

void gv_com_animation::push_back_play_info(gv_ani_play_info* pinfo)
{
	this->check_play_info_tail();
	if (this->m_play_info_tail)
		this->m_play_info_tail->m_next = pinfo;
	else
		this->m_play_info_list = pinfo;
	this->m_play_info_tail = pinfo;
};

void gv_com_animation::pop_front_play_info()
{
	if (!this->m_play_info_list)
		return;
	this->check_play_info_tail();
	gv_ani_play_info* pinfo = this->m_play_info_list;
	this->m_play_info_list = pinfo->m_next;
	if (this->m_play_info_tail == pinfo)
		this->m_play_info_tail = NULL;
	delete_play_info(pinfo);
};

void gv_com_animation::remove_following_animation(gv_ani_play_info* pani)
{
	this->m_play_info_tail = pani;
	pani = pani->m_next;
	while (pani)
	{
		gv_ani_play_info* pnext = pani->m_next;
		delete_play_info(pani);
		pani = pnext;
	}
	if (m_play_info_tail)
		m_play_info_tail->m_next = NULL;
};

void gv_com_animation::check_play_info_tail()
{
	if (this->m_play_info_list && !this->m_play_info_tail)
	{
		gv_ani_play_info* pani = m_play_info_list;
		while (pani->m_next)
			pani = pani->m_next;
		this->m_play_info_tail = pani;
	}
};
//============================================================================================
//								:
//============================================================================================
GVM_IMP_CLASS(gv_com_animation, gv_component)
GVM_VAR(gvt_ptr< gv_ani_set >, m_cu_animation_set)
GVM_VAR(gvt_ptr< gv_ani_play_info >, m_play_info_list)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gvt_ptr< gv_ani_play_info >, m_play_info_tail)
GVM_END_CLASS

#define GV_DOM_FILE "../animation/gv_ani_struct.h"
#define GVM_DOM_RTTI
#define GVM_DOM_IMP_COPY
#include "gv_data_model_ex.h"
}