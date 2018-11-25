#pragma once
namespace gv
{
class gv_component_manager : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gv_component_manager, gv_event_processor);
	gv_component_manager();
	~gv_component_manager();

public:
	virtual bool tick(gv_float dt);
	virtual bool do_synchronization();
	virtual gv_int on_event(gv_object_event* event);

public:
	gvt_array< gvt_ref_ptr< gv_component > > m_components;
};
};