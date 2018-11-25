
#include "stdafx.h"
using namespace gv; 

namespace game_2d_bricks
{

	#include	"2d_bricks_maps.h"
	#include	"2d_bricks_online.h"
	//======================================================================
	class gv_game_2d_bricks:public gv_unit_test_with_renderer
	{
	public:
		virtual gv_string  name()
		{ 
			return "game_2d_bricks"; 
		}
		map			m_map;
		gv_uint		m_max_frame;
		gv_double	m_last_tick_time;
		gv_double	m_delta_time;
		gv_uint		m_tick_count;
		actor*		m_main_actor;
		bool		m_is_drag;
		gv_double   m_start_drag_time;
		gv_vector2i	m_start_drag_pos;
		bool		m_pause;
		gv_game_2d_bricks()
		{}
		virtual void initialize()		
		{
			if(gv_global::command_line_options.size() )
			{
				gv_global::command_line_options[0]>>m_max_frame;
			}
			else m_max_frame=1000;
			m_last_tick_time=gv_global::time->get_sec_from_start();
			m_delta_time=0; 
			m_map.init(the_map,map_width,map_height);
			m_map.spawn_actors(2000);
			//m_map.init(the_map2,map_width2,map_height2);
			//m_map.spawn_actors(10);
			m_main_actor=NULL;
			m_is_drag=false;
			m_tick_count=0;
			m_pause=false;
		};
		bool  update_map_view_mode( float dt)
		{
			gv_string_tmp title="MAP VIEW MODE ====>>";
			gv_global::debug_draw.get()->draw_string(*title,gv_vector2i(60,120),gv_color::RED());

			float step=200.0f*dt;
			if (gv_global::input->is_key_down(e_key_up) )
			{
				m_map.m_camera_pos.y-=step;
			}
			if (gv_global::input->is_key_down(e_key_down) )
			{
				m_map.m_camera_pos.y+=step;
			}
			if (gv_global::input->is_key_down(e_key_left))
			{
				m_map.m_camera_pos.x-=step;
			}
			if (gv_global::input->is_key_down(e_key_right))
			{
				m_map.m_camera_pos.x+=step;
			}
			return true;
		}

		bool  update_play_ball_view_mode( float dt)
		{
			
			{
				bool down=	gv_global::input->is_key_down(e_key_lbutton);
				if(m_is_drag && !down)
				{
					gv_global::debug_draw->release_mouse();
					gv_double time=gv_global::time->get_sec_from_start();
					time-=m_start_drag_time;
					gv_vector2i cu_mouse_pos;
					gv_global::input->get_mouse_pos(cu_mouse_pos);
					cu_mouse_pos-=m_start_drag_pos; 
					m_main_actor->m_speed.x+=(gv_float)(cu_mouse_pos.x*0.5f)/(gv_float)time;
					m_main_actor->m_speed.y+=(gv_float)(cu_mouse_pos.y*0.5f)/(gv_float)time;
					m_main_actor->m_speed.x=gvt_clamp(m_main_actor->m_speed.x, -100.f, 100.f);
					m_main_actor->m_speed.y=gvt_clamp(m_main_actor->m_speed.y, -100.f, 100.f);
				}
				if (down && !m_is_drag )
				{
					gv_global::debug_draw->capture_mouse();
					m_start_drag_time=gv_global::time->get_sec_from_start();
					gv_global::input->get_mouse_pos(m_start_drag_pos);
				}
				m_is_drag=down;
			}
			return true;
		}

		virtual void render()	
		{
			GV_PROFILE_EVENT(render_all,0 );
			gv_double cu=gv_global::time->get_sec_from_start();
			m_delta_time=gvt_clamp ( cu-m_last_tick_time, 0.01, 0.1); 
			m_last_tick_time=cu;
			int cell_drawed=0;

			gv_string_tmp s="[BRICK2D]>>";
			s<< "fps: "<<1.0/m_delta_time;
			if (!m_pause)
			{//UPDATE
				GV_PROFILE_EVENT(m_map_update,0 );
			    //update_map_view_mode((gv_float) m_delta_time);
				update_play_ball_view_mode((gv_float) m_delta_time);
				m_map.update((gv_float) m_delta_time);
				//m_map.update(0.03f);
			}
			{//RENDER
				GV_PROFILE_EVENT(m_map_render,0 );
				if (!m_main_actor) m_main_actor=*m_map.m_actor_list.begin(); 
				if (m_main_actor)
				{
					m_map.m_camera_pos=m_main_actor->m_location; 
					m_map.m_camera_zoom=2;
					gv_string_tmp title="GAME SCORE:";
					title<<m_main_actor->m_score;
					gv_global::debug_draw.get()->draw_string(*title,gv_vector2i(60,120),gv_color::RED());
				}
				cell_drawed=m_map.render();
			}
			s<<" cell in view "<<cell_drawed;
			gv_global::debug_draw.get()->draw_string(*s,gv_vector2i(60,60),gv_color::RED());
			if (gv_global::input->is_key_down(e_key_return))
			{
				m_tick_count=m_max_frame;
			}
			static bool bool_is_space_down=false;
			if (gv_global::input->is_key_down(e_key_space) && !bool_is_space_down)
			{
				m_pause=!m_pause;
			}
			bool_is_space_down=gv_global::input->is_key_down(e_key_space);
			m_tick_count++;
		};
		virtual bool is_finished  ()	
		{ 
			if (m_tick_count<m_max_frame) return false;
			return true; 
		}
		virtual void destroy()
		{
			m_map.destroy();
		}
	}test;
	gv_unit_test_with_renderer* ptest=&test;
};

#include "2d_bricks_online.hpp"