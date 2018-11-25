#pragma once
namespace gv
{
#define gvc_max_cocurrent_animation 4
#define GV_DOM_FILE "../animation/gv_ani_struct.h"
#define GVM_DOM_DECL
#define GVM_DOM_IMP_IS_EQUAL
#define GVM_DOM_IMP_IS_LESS
#define GVM_DOM_IMP_XML
#define GVM_DOM_IMP_SERIALIZE
#include "../inc/gv_data_model_ex.h"
//============================================================================================
//								:this is a simpler version of
//animation component , need time to build a animation blend tree component to
//suppot more complexed animation play.
//============================================================================================
class gv_com_tasklet;
;
class gv_com_animation : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_animation, gv_component)
	gv_com_animation();
	~gv_com_animation();
	void set_ani_set(gv_ani_set* set);
	gv_ani_set* get_ani_set();
	bool play_animation(const gv_id& animation_id, gv_float blending_time = 0.0f,
						bool loop = false, bool clear_list = false,
						gv_com_tasklet* on_ani_finished = NULL);
	bool play_new_animation(const gv_id& animation_id,
							gv_float blending_time = 0.0f, bool loop = false,
							bool clear_list = false,
							gv_com_tasklet* on_ani_finished = NULL);
	void clear_all_animation(bool keep_first = false);
	const gv_id& get_animation_name();

	bool is_animation_finished();
	bool is_blending_start();
	void enable_update_root_only(bool enable = true);
	void enable_root_motion(bool enable = true);

	bool tick(gv_float dt);
	bool set_resource(gv_resource*);
	bool is_tickable();
	bool sync_to_entity(gv_entity*);

protected:
	void check_play_info_tail();
	void push_back_play_info(gv_ani_play_info*);
	void pop_front_play_info();
	void remove_following_animation(gv_ani_play_info*);
	gv_ani_play_info* get_cu_ani()
	{
		return m_play_info_list;
	}
	gv_ani_play_info* alloc_play_info();
	void delete_play_info(gv_ani_play_info*);

protected:
	gv_bool m_is_root_motion_enabled;
	gv_bool m_is_update_root_only;
	gvt_ptr< gv_ani_set > m_cu_animation_set;
	gvt_ptr< gv_ani_play_info > m_play_info_list;
	gvt_ptr< gv_ani_play_info > m_play_info_tail;
	//
	gv_matrix44 m_root_tm_ref;
	gv_matrix44 m_root_tm_ani;
};
}
