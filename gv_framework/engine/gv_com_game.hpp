namespace gv
{
gv_com_game::gv_com_game()
{
	GVM_SET_CLASS(gv_com_game);
}
gv_com_game::~gv_com_game()
{
}
bool gv_com_game::load_world(const gv_id& world_name)
{
	return true;
};
bool gv_com_game::load_world_async(const gv_id& world_name)
{
	return true;
};
bool gv_com_game::unload_world(const gv_id& world_name)
{
	return true;
};
bool gv_com_game::tick(gv_float dt)
{
	super::tick(dt);
	return true;
};

bool gv_com_game::initialize()
{
	return true;
};

bool gv_com_game::finalize()
{
	return true;
};

bool gv_com_game::process_event(gv_object_event* event)
{
	return false;
};

void gv_com_game::on_attach()
{
	GVM_POST_EVENT(add_component, game_logic, (pe->component = this));
}
void gv_com_game::on_detach()
{
	GVM_POST_EVENT(remove_component, game_logic, (pe->component = this));
}

bool gv_com_game::do_synchronization()
{
	return true;
}

GVM_IMP_CLASS(gv_com_game, gv_com_tasklet)
GVM_END_CLASS
}
