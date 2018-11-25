#pragma once

#include "boost/bind.hpp"
#include "../component/gv_com_graphic.h"
#if GV_WITH_OS_API
#include "../component/gv_com_terrain_roam.h"
#endif
namespace gv
{
gv_world::gv_world()
{
	link_class(gv_world::static_class());
	this->m_entities.reserve(2048);
	this->m_hided = false;
	m_grid_size = 0;
};

gv_world::~gv_world(){};

void gv_world::init(gv_int layer, const gv_vector2i& grid_width_height,
					const gv_box& world_size, void*)
{
	m_grid_size = grid_width_height;
	m_world_box = world_size;
};

bool gv_world::tick(gv_float dt)
{
	GV_PROFILE_EVENT(gv_world_tick, 0);
	bool ret = super::tick(dt);
	if (!ret)
		return ret;
	for (int i = 0; i < this->m_entities.size(); i++)
	{
		if (!this->m_entities[i] || this->m_entities[i]->is_deleted())
		{
			this->m_entities.erase_fast(i);
		}
	}
	for (int i = 0; i < this->m_entities.size(); i++)
	{
		if (this->m_entities[i]->is_hided())
			continue;
		if (this - m_entities[i]->is_ignore_pause() || !this->is_paused())
		{
			this->m_entities[i]->tick(dt);
		}
	}
	return true;
};

gv_int gv_world::on_event(gv_object_event* pevent)
{
	return 0;
};

bool gv_world::do_synchronization()
{
	bool ret = super::do_synchronization();
	if (!ret)
		return ret;
	for (int i = 0; i < this->m_entities.size(); i++)
	{
		if (this->m_entities[i])
			this->m_entities[i]->synchronize_from_components();
	}
	return true;
};

bool gv_world::add_entity(gv_entity* entity)
{
	gvt_ref_ptr< gv_entity > r(entity);
	if (m_entities.find(r))
	{
		return false;
	};
	this->m_entities.push_back(entity);
	entity->set_world(this);
	entity->set_owner(this);
	return true;
};
void gv_world::delete_entity(gv_entity* entity)
{
	GV_ASSERT(m_entities.find(entity));
	m_entities.erase_item_fast(entity);
	entity->detach_all_component();
	get_sandbox()->delete_object(entity);
}

void gv_world::delete_all_entity()
{

	for (int i = 0; i < this->m_entities.size(); i++)
	{
		gv_entity* e = this->m_entities[i];
		if (e)
		{
			e->detach_all_component();
			get_sandbox()->delete_object(e);
		}
	}
	m_entities.clear();
}
bool gv_world::move_entity(gv_entity* entity, gv_vector3 new_pos,
						   gv_euler new_rot)
{
	entity->set_position(new_pos);
	entity->set_rotation(new_rot);
	entity->update_matrix();
	return true;
};
bool gv_world::teleport_entity(gv_entity* entity, gv_vector3 pos, gv_euler rot,
							   bool check_overlap)
{
	if (!get_world_box().is_point_inside(pos))
		return false;
	entity->set_position(pos);
	entity->set_rotation(rot);
	entity->update_matrix();
	return true;
}
void gv_world::collect_entity(const gv_line_segment& line,
							  gv_world::collect_result& result){

};
void gv_world::collect_entity(const gv_box& box,
							  gv_world::collect_result& result){

};
void gv_world::collect_entity(const gv_frustum& frustum,
							  gv_world::collect_result& result){

};

void gv_world::collect_entity(const gv_rect& rect, collect_result& result){

};

bool gv_world::line_check(const gv_line_segment& line, gv_vector3& pos,
						  gv_entity*& entity)
{
	gv_vector3 dir = line.end_p - line.start_p;
	float dist = dir.normalize();
	gv_line ray(line.start_p, dir);
	// check the ground;
	gv_plane p(0, 1, 0, -m_world_box.min_p.get_y());
	gv_vector3 v;
	bool hit = gv_geom::intersect_ray_plane(ray, p, v, dist);
	gv_float s, t;
	entity = NULL;
	if (dist < 0)
		dist = gv_float_trait::max(); // ignore the wrong direction.
	for (int i = 0; i < m_entities.size(); i++)
	{
		if (!m_entities[i])
			continue;
		if (m_entities[i]->is_hided())
			continue;
		if (m_entities[i]->is_ignore_line_check())
			continue;
		if (gv_geom::intersect_ray_aabb(ray, m_entities[i]->get_world_aabb(), s,
										t))
		{
			//test terrain first
			gv_com_terrain_roam* pterrain =
				m_entities[i]->get_component< gv_com_terrain_roam >();
			if (pterrain)
			{
				if (!pterrain->intersect_world_line_seg(line, s))
				{
					continue;
				}
			}

			if (s > 0 && s < dist)
			{
				dist = s;
				hit = true;
				entity = m_entities[i];
			}
		}
	}
	pos = ray.get_point_on_line(dist);
	return hit;
};

gv_int gv_world::get_nb_entity() const
{
	return m_entities.size();
};

gv_entity* gv_world::get_entity(gv_int idx)
{
	if (idx < m_entities.size())
		return m_entities[idx];
	return NULL;
};

const gv_entity* gv_world::get_entity(gv_int idx) const
{
	return m_entities[idx];
};

gv_entity* gv_world::find_entity(const gv_id& name)
{
	for (int i = 0; i < m_entities.size(); i++)
	{
		if (!m_entities[i])
			continue;
		if (m_entities[i]->get_name_id() == name)
		{
			return m_entities[i];
		}
	}
	return NULL;
};

const gv_box& gv_world::get_world_box() const
{
	return m_world_box;
};

gv_float gv_world::get_ground_height(const gv_vector3& pos)
{
	return 0.0f;
};

bool gv_world::post_load()
{
	// reduce the iterating time to find the world.
	get_module()->set_world(this);
	return true;
};

//============================================================================================
//								:
//============================================================================================
gv_world_light_info::gv_world_light_info()
{
	this->m_ambient_color = gv_color::GREY_D();
	this->m_specular_color = gv_color::GREY_D();
	this->m_sun_dir = gv_vector4(1, 1, 1, 1);
};
}

namespace gv
{
GVM_IMP_CLASS(gv_world, gv_event_processor)
{
	GVM_VAR(gvt_array< gvt_ref_ptr< gv_entity > >, m_entities)
	GVM_VAR(gv_box, m_world_box)
	GVM_VAR(gv_vector2i, m_grid_size)
	GVM_VAR(gvt_ref_ptr< gv_entity >, m_main_entity)
	GVM_VAR(gv_world_light_info, m_light_info)
}
GVM_END_CLASS

GVM_IMP_STRUCT(gv_world_light_info)
{
	GVM_VAR(gv_vector4, m_sun_dir)
	GVM_VAR(gv_color, m_ambient_color)
	GVM_VAR(gv_color, m_specular_color)
}
GVM_END_STRUCT

GVM_BGN_FUNC(gv_world, scp_reinit_world)
GVM_PARAM(gv_vector2i, grid_size)
GVM_PARAM(gv_box, bounding)
GVM_PARAM(gv_text, resource_name)
this->init(1, grid_size, bounding, (void*) * resource_name);
return 1;
GVM_END_FUNC
}