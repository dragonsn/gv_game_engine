#pragma once
namespace gv
{
class gv_game_event_processor : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gv_game_event_processor, gv_event_processor)
	gv_game_event_processor()
	{
		m_quit = false;
		GVM_SET_CLASS(gv_game_event_processor);
	};
	~gv_game_event_processor(){};

protected:
	gv_int on_event(gv_object_event* pevent)
	{
		if (m_game)
		{
			if (m_game->process_event(pevent))
			{
				return 1;
			}
		}

		switch (pevent->m_id)
		{

		case gv_object_event_id_add_component:
		{
			gv_object_event_add_component* pe =
				gvt_cast< gv_object_event_add_component >(pevent);
			GV_ASSERT(!m_game);
			m_game = gvt_cast< gv_com_game >(pe->component);
			GV_ASSERT(m_game);
			m_game->initialize();
		}
		break;
		case gv_object_event_id_remove_component:
		{
			gv_object_event_remove_component* pe =
				gvt_cast< gv_object_event_remove_component >(pevent);
			gv_com_game* game = gvt_cast< gv_com_game >(pe->component);
			GV_ASSERT(game);
			GV_ASSERT(game == m_game.ptr());
			game->finalize();
			this->m_game = NULL;
		}
		break;

		case gv_object_event_id_quit_game:
			m_quit = true;
			break;
		}

		return 1;
	}
	virtual bool tick(gv_float dt)
	{
		if (m_quit)
		{
			return false;
		}
		super::tick(dt);
		if (m_game)
			return m_game->tick(dt);
		return true;
	};
	virtual bool do_synchronization()
	{
		super::do_synchronization();
		if (m_game)
			return m_game->do_synchronization();
		return true;
	};

public:
	gvt_ref_ptr< gv_com_game > m_game;
	bool m_quit;
};

GVM_IMP_CLASS(gv_game_event_processor, gv_event_processor)
GVM_END_CLASS
}