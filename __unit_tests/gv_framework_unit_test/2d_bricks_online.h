enum actor_state
{
	e_actor_state_alive,
	e_actor_state_offline,
	e_actor_state_dead,
};

static const   int cell_size_x   =100;
static const int cell_size_y   =50;
static const int actor_size	=7;

inline bool intersect_test(const gv_vector2 &  ray, const gv_rect& rect, float & time, gv_vector2 & normal)
{
	float enter_time = FLT_MIN, leave_time = FLT_MAX;
	if (gvt_is_almost_zero(ray.x))
	{
		if (0 < rect.min_p.x || 0 > rect.max_p.x)
			return false;
	}
	else
	{
		float s = (rect.min_p.x) / ray.x;
		float t = (rect.max_p.x) / ray.x;
		//test if inside or ray facing to wrong side
		normal = gv_vector2(-1, 0);
		if (s > t)
		{
			gvt_swap(s, t);
			normal = gv_vector2(1, 0);
		}
		enter_time = s;
		leave_time = t;
		time = s;
	}
	// y 
	if (gvt_is_almost_zero(ray.y))
	{
		if (0 < rect.min_p.y || 0 > rect.max_p.y)
			return false;
	}
	else
	{
		float s = (rect.min_p.y) / ray.y;
		float t = (rect.max_p.y) / ray.y;
		//test if inside or ray facing to wrong side
		gv_vector2 n2 = gv_vector2(0, -1);
		if (s > t)
		{
			gvt_swap(s, t);
			n2 = gv_vector2(0, 1);
		}
		if (s > enter_time)
		{
			enter_time = s;
			time = s;
			normal = n2;
		}
		if (t < leave_time)
		{
			leave_time = t;
		}
	}
	if (enter_time > leave_time)
		return false;
	if (time < 0 || time > 1)
		return false;
	return true;
}
/*sweep a moving aabb in 2d space*/
inline bool sweep_test(const gv_rect & mover, const gv_rect & being_tested,
	gv_vector2 velocity, float & time_of_impact, gv_vector2 & normal,
	float min_time = 0)
{
	if (gvt_is_almost_zero(velocity.length_squared()))
	{
		return false;
	}
	//Minkowskidifference 
	gv_rect r = being_tested;
	r.min_p -= mover.max_p;
	r.max_p -= mover.min_p;
	if (r.inside(gv_vector2::get_zero_vector()))
	{
		//already overlap!
		return false;
	}
	bool hit = intersect_test(velocity, r, time_of_impact, normal);
	if (!hit)
		return false;

	if (time_of_impact >= min_time)
	{
		time_of_impact -= min_time;
	}
	else
	{
		time_of_impact = 0;
	}
	return true;
}

/*sweep a moving aabb in 2d space*/
inline bool sweep_test2(const gv_rect & mover, const gv_rect & being_tested,
	gv_vector2 velocity, gv_sweep_result& result,
	float skin = 0)
{
	gv_vector2 n;
	bool ret=sweep_test(mover, being_tested, velocity, result.time_of_impact,n , 0.01f);
	result.normal = n;
	return ret;
}

class actor:public gv_refable
{
public:
	actor():m_speed(0,0),m_rotation(0),m_size((gv_float)actor_size)
	{
		m_aabb.add(gv_vector2(0, 0));
		m_aabb.set_size((gv_float)actor_size,(gv_float)actor_size);
		m_is_hit=0;
		m_hit_actor=0;
		m_score=0;
		m_log_in=false; 
		m_index =0 ; 
		m_rotation=0;
	}
	bool		m_log_in;
	gv_ushort	m_index;
	gv_string32 m_name;
	gv_rect		m_aabb; //smaller than the cell size;
	gv_color	m_color; 
	gv_vector2	m_speed;
	gv_vector2	m_location;
	gv_float	m_rotation;
	gv_float	m_size;
	gv_uint		m_test_tag;
	gv_uint		m_is_hit;
	gv_byte		m_state;
	gv_vector2	m_hit_normal;
	actor	*	m_hit_actor;
	gv_float	m_in_love_value;
	gv_int		m_score;
};
class		cell
{
public:
	typedef gvt_array_cached<actor * ,256 > actor_cache;
	gv_rect		m_rect; 
	bool		m_can_move_on;
	gv_color	m_color;
	actor_cache m_actor_cache;
	void add_actor( actor * a)
	{
		m_actor_cache.add(a);
	};
	void remove_actor( actor * a)
	{
		m_actor_cache.erase_item_fast(a);
	};
};

typedef gvt_array_cached<actor * ,1024 > actor_cache_1024;

class	map
{
public:
	
	typedef gvt_array_cached<cell * ,1024 > cell_cache;
	typedef gvt_array< gvt_ref_ptr<actor> > actor_list;
	gv_vector2	m_camera_pos;
	gv_float m_camera_zoom; 
	gv_vector2  m_display_window_size;
	actor_list	 m_actor_list;
	gvt_array< gvt_array <cell >  > m_cells; 
	gv_int		m_map_width,m_map_height;
	gvt_random<gv_float> m_random;
	map()
	{
		m_camera_zoom=1;
	}
	~map()
	{
		m_cells.clear();
		m_actor_list.clear();
	}
	gv_uint		get_unique_tag()
	{
		static gv_uint tag=0; 
		return ++tag;
	}
	gv_vector2	world_to_window(const gv_vector2 & w_v)
	{
		gv_vector2 v=w_v;
		v-=m_camera_pos;
		v*=m_camera_zoom;
		v+=m_display_window_size/2.0f;
		v.x=floorf(v.x);
		v.y=floorf(v.y);
		return v;
	}
	gv_vector2	window_to_world(const gv_vector2 & w_v)
	{
		gv_vector2 v=w_v;
		v-=m_display_window_size/2.0f;
		v/=m_camera_zoom;
		v+=m_camera_pos;
		return v;
	}
	gv_rect		world_to_window(const gv_rect & r)
	{
		return gv_rect( world_to_window(r.min_p) ,world_to_window(r.max_p));
	}
	gv_rect		window_to_world(const gv_rect & r)
	{
		return gv_rect( window_to_world(r.min_p) ,window_to_world(r.max_p));
	}

	inline gvt_vector2<gv_ushort> compress_pos(const gv_vector2 & pos)
	{
		gv_vector2 v=pos; 
		v.x=v.x/(m_map_width*cell_size_x) * 65535.f;
		v.x=gvt_clamp(v.x, 0.f,65535.f); 
		v.y=v.y/(m_map_height*cell_size_y)* 65535.f;
		v.y=gvt_clamp(v.y, 0.f,65535.f); 
		gvt_vector2<gv_ushort> vs; 
		vs.x=(gv_ushort) v.x; 
		vs.y=(gv_ushort)v.y; 
		return vs;
	};

	inline gv_vector2 uncompress_pos(const gvt_vector2<gv_ushort>  & vs)
	{
		gv_vector2 v; 
		v.x=(gv_float)vs.x*(m_map_width*cell_size_x) / 65535.f;
		v.y=(gv_float)vs.y*(m_map_height*cell_size_y)/ 65535.f;
		return v; 
	};
	
	gv_vector2i get_cell_coordinate ( const gv_vector2 & vector)
	{
		gv_vector2i ret; 
		ret.x=(gv_int)floorf(vector.x /cell_size_x);
		ret.y=(gv_int)floorf(vector.y /cell_size_y);
		return ret; 
	}
	gv_int		collect_cells		(const gv_rect & rect,  cell_cache & result)
	{
		gv_vector2i top_left=get_cell_coordinate(rect.min_p);
		gv_vector2i down_right=get_cell_coordinate(rect.max_p);
		for  ( int i = top_left.y ; i<=down_right.y; i++)
		{
			if (!gvt_between(i,0,m_map_height-1) ) continue;
			for  ( int j =top_left.x; j<= down_right.x; j++)
			{
				if (!gvt_between(j,0,m_map_width-1) ) continue;
				result.add (&m_cells[i][j]); 
			}
		}
		return result.size(); 
	};

	template < class actor_container> 
	gv_int		collect_actors 		(const gv_rect & rect,  actor_container & result)
	{
		cell_cache cells;
		this->collect_cells( rect,cells); 
		gv_uint tag=get_unique_tag();
		int nb_actors=0; 
		for ( int i=0 ; i< cells.size() ; i++)
		{
			if (!cells[i]->m_can_move_on) continue; 
			for (int j=0; j< cells[i]->m_actor_cache.size(); j++)
			{
				actor * pactor2=cells[i]->m_actor_cache[j];
				if (pactor2->m_test_tag==tag) continue;
				pactor2->m_test_tag=tag;
				result.push_back(pactor2); nb_actors++;
			}//next actor
		}//next cell
		return nb_actors;
	}

	// the length of the line is less than a cell  , so it can be simple.
	gv_int		collect_cells		( const gv_line_segment & line , cell_cache & result)
	{
		//not efficient enough for long line, todo ,use brethmhan algorithm later 
		gv_rect rect; 
		rect.move_to(line.start_p.v2); 
		rect.add	(line.end_p.v2);
		return collect_cells(rect,result);
	};
	void init ( const char * map_data, gv_int width, gv_int height )
	{
		m_cells.resize(height); 
		for ( int i=0 ;i< height; i++)
			m_cells[i].resize(width);
		for ( int i=0 ; i< height; i++)
			for (int j=0 ;j< width ; j++)
			{
				char data=map_data[i*width+j];
				cell & c=m_cells[i][j];
				switch (data)
				{
					case ' ': c.m_color=gv_color::BLACK(); c.m_can_move_on=true; break;
					case '1': c.m_color=gv_color::GREEN_B(); c.m_can_move_on=false; break;
					case '2': c.m_color=gv_color::GREY_B(); c.m_can_move_on=false; break;
					case '3': c.m_color=gv_color::BLUE_B(); c.m_can_move_on=false; break;
					default: c.m_color=gv_color::BLUE_B(); c.m_can_move_on=false; break;
				}
				c.m_rect.move_to((gv_float)j*cell_size_x, (gv_float)i*cell_size_y); 
				c.m_rect.set_size((gv_float)cell_size_x,(gv_float) cell_size_y);
			}
		m_map_width=width; 
		m_map_height=height;
	}
	void render(actor & actor)
	{
		gv_rect r=world_to_window(actor.m_aabb);
		if (actor.m_is_hit && actor.m_hit_actor )
		{
			//gv_global::debug_draw->draw_rect(r,gv_color::RED(),0.8f);
			//gv_global::debug_draw->draw_arrow (gv_line_segment(world_to_window(actor.m_location),world_to_window(actor.m_location)+actor.m_hit_normal*30.f ),gv_color::BLACK() );
			//gv_global::debug_draw->draw_heart(gv_vector3(r.center().x,r.center().y,0.5f) , actor_size*m_camera_zoom, gv_color::RED());
			if (actor.m_in_love_value <0)
				gv_global::debug_draw->draw_jiong(gv_vector3(r.center().x,r.center().y,0.5f) , -actor.m_in_love_value*actor_size*m_camera_zoom);
			else
				gv_global::debug_draw->draw_heart(gv_vector3(r.center().x,r.center().y,0.5f) , actor.m_in_love_value*actor_size*m_camera_zoom, gv_color::RED());
		}
		else
		{
			gv_global::debug_draw->draw_rect(r,gv_color::BLACK(),0.8f);
			r.extend(-2);
			gv_global::debug_draw->draw_rect(r,actor.m_color,0.8f);
		}
	}
	void render( cell & cell)
	{
		if (cell.m_can_move_on)
		{
			for ( int i=0; i<cell.m_actor_cache.size() ;i++)
			{
				render(*cell.m_actor_cache[i]);
			}
		}
		else
		{
			gv_rect r=world_to_window(cell.m_rect);
			r.extend(-1);
			gv_global::debug_draw->draw_rect(r,cell.m_color,0.8f);
		}
	}
	gv_int render()
	{
		gv_rect view_window= gv_global::debug_draw->get_window_rect();
		m_display_window_size=view_window.get_size();
		//view_window.move_center_to(this->m_camera_pos);
		view_window=window_to_world(view_window);
		cell_cache cells;
		this->collect_cells(view_window,cells);
		for ( int i =0; i< cells.size(); i++)
		{
			this->render(*cells[i]);
		}
		return cells.size();
	}
	void update(gv_float delta_time)
	{
		GV_PROFILE_EVENT(map_update, 0);
		actor_list::iterator it=this->m_actor_list.begin();
		while(it !=this->m_actor_list.end())
		{
			actor * pactor=*it;
			move_actor(pactor,pactor->m_speed*delta_time);
			++it;
		}
	}
	bool   teleport_actor(actor *  pactor, gv_vector2 pos , bool check_overlap =true , bool check_movable=true)
	{
		cell_cache cells;
		//remove from old pos
		this->collect_cells(pactor->m_aabb,cells);
		for ( int i=0 ; i< cells.size() ; i++)
		{
			cells[i]->m_actor_cache.erase_item_fast(pactor);
		}
		cells.clear();
		pactor->m_aabb.move_center_to(pos);
		pactor->m_location=pos;
		this->collect_cells(pactor->m_aabb,cells);
		if (check_movable)
		{
			gv_uint tag=get_unique_tag();
			for ( int i=0 ; i< cells.size() ; i++)
			{
				if (!cells[i]->m_can_move_on && check_overlap ) return false; 
				for (int j=0; j< cells[i]->m_actor_cache.size(); j++)
				{
					actor * pactor2=cells[i]->m_actor_cache[j];
					if (pactor2->m_test_tag==tag) continue;
					pactor2->m_test_tag=tag;
					if (pactor2->m_aabb.is_overlap(pactor->m_aabb)  && check_overlap)
					{
						return false;
					}
				}//next actor
			}//next cell
		}
		for ( int i=0 ; i< cells.size() ; i++)
		{
			cells[i]->m_actor_cache.push_back(pactor);
		}
		return true;
	}
	bool   move_actor(actor *  pactor, gv_vector2 delta_pos )
	{
		cell_cache cells;
		//remove from old pos
		static int move_tag=0; 
		move_tag++;
		if (move_tag==3676)
		{
			//GV_DEBUG_BREAK;
		}
		this->collect_cells(pactor->m_aabb,cells);
		for ( int i=0 ; i< cells.size() ; i++)
		{
			cells[i]->m_actor_cache.erase_item_fast(pactor);
		}
		cells.clear();

		gv_rect rect_moving_aabb;
		rect_moving_aabb.add(delta_pos);
		rect_moving_aabb.move(pactor->m_aabb.center() );
		rect_moving_aabb.extend(pactor->m_aabb);
		rect_moving_aabb.extend(2);

		gv_uint tag=get_unique_tag();
		this->collect_cells(rect_moving_aabb,cells);
		gv_float time_of_impact =1.0f;
		gv_vector3 normal;
		actor * hit_actor=0;
		cell * hit_cell=0; 
		for ( int i=0 ; i< cells.size() ; i++)
		{
			if (!cells[i]->m_can_move_on)
			{
				gv_sweep_result result;
				//bool hit=gv_geom::sweep(pactor->m_aabb,cells[i]->m_rect,delta_pos,result,0.5);
				//bool hit1 = gv_geom::sweep(pactor->m_aabb, cells[i]->m_rect, delta_pos, result, 0.5);
				bool hit = sweep_test2(pactor->m_aabb, cells[i]->m_rect, delta_pos, result, 0.5);
			/*	if (hit1 != hit) {
					sweep_test2(pactor->m_aabb, cells[i]->m_rect, delta_pos, result, 0.5);
				}*/
				
				if (pactor->m_aabb.top +delta_pos.y<=50.f )
				{
					//GV_DEBUG_BREAK;
				}
				if (hit && result.time_of_impact < time_of_impact)
				{
					hit_actor=NULL;
					hit_cell =cells[i];
					time_of_impact=result.time_of_impact;
					normal=result.normal;
				}
			}
			for (int j=0; j< cells[i]->m_actor_cache.size(); j++)
			{
				actor * pactor2=cells[i]->m_actor_cache[j];
				if (pactor2->m_test_tag==tag) continue;
				pactor2->m_test_tag=tag;
				gv_sweep_result result;
				//bool hit=gv_geom::sweep(pactor->m_aabb,pactor2->m_aabb,delta_pos,result,0.5);
				bool hit = sweep_test2(pactor->m_aabb, pactor2->m_aabb, delta_pos, result, 0.5);
				if (hit && result.time_of_impact < time_of_impact)
				{
					hit_actor=pactor2;
					hit_cell=0; 
					time_of_impact=result.time_of_impact;
					normal=result.normal;
				}
			}//next actor
		}//next cell
		//move and update cell
		cells.clear();
		pactor->m_aabb.move(delta_pos*time_of_impact);
		collect_cells(pactor->m_aabb,cells);
		for ( int i=0 ; i< cells.size() ; i++)
		{
			cells[i]->m_actor_cache.push_back(pactor);
		}

		if (pactor->m_aabb.top<=50.f )
		{
			//GV_DEBUG_BREAK;
		}
		pactor->m_location=pactor->m_aabb.center();
		if (time_of_impact < 1.0f )
		{
			//hit happen;
			if (hit_cell)
			{
				pactor->m_speed=gv_geom::reflect(normal.v2, pactor->m_speed);
			}
			if (hit_actor)
			{
				pactor->m_speed=gv_geom::reflect(normal.v2, pactor->m_speed);
				//gv_vector2 delta_speed=gv_geom::reflect(normal.v2, pactor->m_speed)-pactor->m_speed;
				//hit_actor->m_speed-=delta_speed/2.0f; 
				//pactor->m_speed+=delta_speed/2.0f;
				pactor->m_is_hit+=30;
				hit_actor->m_is_hit+=30;
				hit_actor->m_hit_actor=pactor;

				gv_colorf c0=pactor->m_color;
				gv_colorf c1=hit_actor->m_color;
				gv_vector4 v = c0.v -c1.v;
				c0.v/=(c0.get_grey_scale()+0.01f);
				c1.v/=(c1.get_grey_scale()+0.01f);
				v+=c0.v -c1.v;
				gv_float diff=v.length();
				static float threshold=1.5f;
				if (diff > threshold)
				{
					float value=(diff-threshold)+1.0f;
					pactor->m_in_love_value=-value;
					hit_actor->m_in_love_value=-value;
				}
				else
				{
					float value=threshold/(diff+threshold*0.3f)+1.0f;
					pactor->m_in_love_value    = value;
					hit_actor->m_in_love_value=value;
				}
				pactor->m_score+=(gv_int)pactor->m_in_love_value;
				hit_actor->m_score+=(gv_int)pactor->m_in_love_value;
				if (pactor->m_score<0) pactor->m_score=0; 
				if (hit_actor->m_score<0) hit_actor->m_score=0; 
			}
			
			pactor->m_hit_actor=hit_actor;
			pactor->m_hit_normal=normal.v2;
		}
		else
		{
			
			if (pactor->m_is_hit==1)
			{
				pactor->m_color=gv_colorf (m_random.get_uniform(),m_random.get_uniform(),m_random.get_uniform(),1.f);
			}
			if (pactor->m_is_hit>0) pactor->m_is_hit--;
		}
		return true;
	}
	actor *  spawn_actor()
	{
		actor * pactor=new actor; 
		pactor->m_color=gv_colorf (m_random.get_uniform(),m_random.get_uniform(),m_random.get_uniform(),1.f);
		pactor->m_speed=gv_vector2(m_random.get_uniform()-0.5f,m_random.get_uniform()-0.5f)*100.f;
		while(1)
		{
			gv_vector2 pos;
			pos.x =m_random.get_uniform()*this->m_map_width*cell_size_x;
			pos.y =m_random.get_uniform()*this->m_map_height*cell_size_y;
			if (teleport_actor( pactor , pos)  )  break;
		};
		this->m_actor_list.add(pactor);
		pactor->m_index=(gv_ushort)(this->m_actor_list.size()-1);
		return pactor; 
	};//random pos; 

	void spawn_actors(int number )
	{
		for (int i=0; i< number; i++)
			spawn_actor();
	};
	void destroy()
	{
		m_cells.clear();
		m_actor_list.clear();
	}
};