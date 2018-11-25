#pragma once

namespace gv
{

gvi_ui_manager::gvi_ui_manager()
{
	GVM_SET_CLASS(gvi_ui_manager);
};
gvi_ui_manager::~gvi_ui_manager(){

};

void gvi_ui_manager::register_events()
{
	super::register_events();
	return;
};
void gvi_ui_manager::unregister_events()
{
	super::unregister_events();
	return;
};
bool gvi_ui_manager::tick(gv_float dt)
{
	super::tick(dt);
	return true;
};
// none virtual utility
bool gvi_ui_manager::do_synchronization()
{
	return super::do_synchronization();
}

GVM_IMP_CLASS(gvi_ui_manager, gv_event_processor);
GVM_END_CLASS
}