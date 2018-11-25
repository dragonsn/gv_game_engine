namespace gv
{
static const gv_uint gvc_invalid_network_uuid_rpg2d_actor = (gv_uint)-1;

gv_com_rpg2d_actor_online::gv_com_rpg2d_actor_online()
	: m_location_net(0, 0), m_state_net(0), m_facing_net(0),
	  m_uuid(gvc_invalid_network_uuid_rpg2d_actor), m_flush_dt(0.1f),
	  m_cu_flush_time(0)
{
	GVM_SET_CLASS(gv_com_rpg2d_actor_online);
};

gv_com_rpg2d_actor_online::~gv_com_rpg2d_actor_online(){

};

bool gv_com_rpg2d_actor_online::tick(gv_float dt)
{
	super::tick(dt);
	m_cu_flush_time += dt;
	if (m_cu_flush_time <= m_flush_dt)
		return true;
	m_cu_flush_time = 0;
	if (m_uuid != gvc_invalid_network_uuid_rpg2d_actor)
	{
		if (!this->m_location.is_almost_equal(this->m_location_net))
		{
			GVM_POST_EVENT(net_actor_2d_move, net, pe->uuid = m_uuid;
						   pe->old_pos = this->m_location_net;
						   pe->new_pos = this->m_location;);
			this->m_location_net = this->m_location;
		}
		if (this->m_state_net != this->get_state())
		{
			GVM_POST_EVENT(net_actor_2d_state_change, net, pe->uuid = m_uuid;
						   pe->old_state = this->m_state_net;
						   pe->new_state = this->get_state(););
			this->m_state_net = this->get_state();
		}
		if (this->m_facing_net != this->m_facing)
		{
			GVM_POST_EVENT(net_actor_2d_facing_change, net, pe->uuid = m_uuid;
						   pe->old_facing = this->m_facing_net;
						   pe->new_facing = this->m_facing;);
			this->m_facing_net = this->m_facing;
		}
	}
	return true;
};

gv_color gv_com_rpg2d_actor_online::get_state_color()
{
	if (this->m_uuid != gvc_invalid_network_uuid_rpg2d_actor)
	{
		return gv_color::RED();
	}
	return gv_color::GREY_B();
}

GVM_IMP_CLASS(gv_com_rpg2d_actor_online, gv_com_rpg2d_actor)
GVM_VAR(gv_vector2, m_location_net)
GVM_VAR(gv_float, m_facing_net)
GVM_VAR(gv_byte, m_state_net)
GVM_END_CLASS
}