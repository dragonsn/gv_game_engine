#pragma once
namespace gv
{
class gv_com_game : public gv_com_tasklet
{
	friend class gv_game_event_processor;

public:
	GVM_DCL_CLASS(gv_com_game, gv_com_tasklet);
	gv_com_game();
	virtual ~gv_com_game();

public:
	virtual bool load_world(const gv_id& world_name);
	virtual bool load_world_async(const gv_id& world_name);
	virtual bool unload_world(const gv_id& world_name);
	virtual bool tick(gv_float dt);
	virtual bool initialize();
	virtual bool finalize();
	virtual void on_attach();
	virtual void on_detach();
	virtual bool do_synchronization();

protected:
	virtual bool process_event(gv_object_event* event);
};
}