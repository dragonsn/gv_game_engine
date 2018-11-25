#pragma once
namespace gv
{
class gv_com_rpg2d_actor_online : public gv_com_rpg2d_actor
{
	friend class gv_world_rpg2d;

public:
	GVM_DCL_CLASS(gv_com_rpg2d_actor_online, gv_com_rpg2d_actor);
	gv_com_rpg2d_actor_online();
	~gv_com_rpg2d_actor_online();
	bool tick(gv_float dt);
	gv_color get_state_color();

public:
	gv_vector2 m_location_net;
	gv_float m_facing_net;
	gv_byte m_state_net;
	gv_uint m_uuid;
	gv_float m_flush_dt;
	gv_float m_cu_flush_time;
};
}