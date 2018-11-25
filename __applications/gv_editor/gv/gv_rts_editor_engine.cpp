#include "stdafx.h"
#include "gv_rts_editor_engine.h"
#include "gv_framework\inc\gv_framework_events.h"
#include "gv_base\math\gvt_image_2d.h"

#include "..\resource.h"
#include "..\SelectConfigDialog.h"
#include "..\gv_rts_editorDoc.h"
#include "..\gv_rts_editorView.h"

#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_rts_editor_engine_classes.h"
gv::gv_string_tmp get_new_object_name_dialog();

using namespace gv;


namespace gv
{

class gv_rts_editor_engine_impl : public gv_refable
{
public:
	gv_rts_editor_engine_impl(){};
	~gv_rts_editor_engine_impl()
	{
	}
};
//============================================================================================
//								:
//============================================================================================
static gv_rts_editor_engine* s_peditor = NULL;
;
gv_rts_editor_engine* get_editor()
{
	return s_peditor;
};
gv_rts_editor_engine::gv_rts_editor_engine()
{
	s_peditor = this;
	m_impl = new gv_rts_editor_engine_impl;
	GVM_SET_CLASS(gv_rts_editor_engine);
	m_current_brush = e_brush_height_map;
	m_terrain_paint_layer = 0;
	m_is_selection_locked = false;
	m_clear_color = gv_color::BLACK();

	/*m_test_color_red=gv_color::RED();
  m_test_color_green=gv_color::GREEN();
  m_test_color_blue=gv_color::BLUE();
  m_test_color_blue.a=128;
  int data[]={1,22,333,444,5555,6666,77777};
  m_test_array.init(data,gvt_array_length(data));
  m_test_array_static=13;
  m_test_array_static[2]=26;
  m_test_array_2.resize(3);
  m_test_array_2[0].push_back("tom");
  m_test_array_2[0].push_back("jerry");
  m_test_array_2[0].push_back("jim");

  m_test_array_2[1].push_back("cat");
  m_test_array_2[1].push_back("dog");
  m_test_array_2[1].push_back("pig");
  m_test_array_2[1].push_back("monkey");

  m_test_text="测试文本，测试文本";
  m_test_v3.set(2,4,8);
  m_test_m.set_identity();*/

	m_editor_mode = e_editor_mode_actor;
	m_is_paint_terrain_start = false;
	m_is_drag_move_started = false;
	m_drag_start = false;
}
gv_rts_editor_engine::~gv_rts_editor_engine()
{
	s_peditor = NULL;
}
bool gv_rts_editor_engine::load_default_option(gv_engine_option& option)
{
	super::load_default_option(option);
	/*<array array_size="1" >
          <gv_event_processor_desc number_property="4" >
          <m_is_synchronization type="gv_bool" value="1"	/>
          <m_is_autonomous type="gv_bool" value="0"	/>
          <m_channel_id type="gv_id" value="channel_render"	/>
          <m_processor_cls type="gv_id" value="gv_renderer_d3d9"	/>
          </gv_event_processor_desc>
  </array>*/
	option.m_processors.clear();

	option.m_root_path = "../../../../gv_resource";
	return true;
};
void gv_rts_editor_engine::static_init(int nb_arg, char* args[],
									   char* extra_arg)
{
	gv_framework_config& fc = gv_global::framework_config;
	// to do , load project configuration first.
	// SN_TODO
	// fc.window_size.set(1024, 1024);
	gv_global::framework_config.nb_debug_tri = 10000;
	gv_global::framework_config.nb_debug_line = 10000;
	gv_global::framework_config.is_editor = true;
	gv_string_tmp s_args = extra_arg;
	s_args << " no_profile";
	gv_game_engine::static_init(nb_arg, args, (char*)*s_args);
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_rts_editor_engine_classes.h"
	sandbox->init_classes();
};

void gv_rts_editor_engine::static_destroy()
{
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_rts_editor_engine_classes.h"

	gv_game_engine::static_destroy();
};

gv_material* gv_rts_editor_engine::get_default_material()
{
	return m_default_material;
};

void gv_rts_editor_engine::init()
{
	super::init();
	m_game_entity->set_position(gv_vector3(0.f, -0.f, 0.f));
	m_game_entity->update_matrix();
	gv_rts_editor_engine_option option;
	gvt_load_option_file(option, "gv_rts_editor.ini");
	gv_com_camera* camera = get_sandbox()->create_object< gv_com_camera >(
		gv_id("main_camera"), m_game_entity);
	gv_float ratio = (gv_float)gv_global::framework_config.window_size.get_x() /
					 gv_global::framework_config.window_size.get_y();
	camera->set_fov(60, ratio, 0.1f, 100);
	camera->set_look_at(gv_vector3(1, 1, 1.f) * 3, gv_vector3(0, 0, 0));
	camera->update_projection_view_matrix();
	m_game_entity->add_component(camera);
	GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
	m_game_entity->add_component(gv_id("gv_com_observer_camera_controller"));
	gv_sandbox* my_sandbox = get_sandbox();
	//==================================================================
	{ // set default material
		get_sandbox()->try_load_module(gv_id("mat_engine"));
		gv_material* pmaterial = get_sandbox()->find_object< gv_material >(
			"(2)mat_engine:0/default_material:0");
		GV_ASSERT(pmaterial);
		// m_default_material=get_sandbox()->create_object<gv_material>
		// (gv_id("default_material"),this);
		m_default_material = get_sandbox()->clone_object(pmaterial);
		m_default_material->rename("default_material");
		m_default_material->set_owner(this);
		// gv_effect * my_effect
		// =get_sandbox()->find_object<gv_effect>("(4)mat_engine:0/default:0/base:0/TexturedPhong:0");
		// m_default_material->m_effect=my_effect;
		// m_default_material->m_ambient_color=pmaterial->m_ambient_color;
		// m_default_material->m_diffuse_color=pmaterial->m_diffuse_color;
		// m_default_material->m_specular_color=pmaterial->m_specular_color;
	}

	//==================================================================
	{
		m_cube_mesh = get_sandbox()->find_object< gv_static_mesh >(
			"(3)mat_engine:0/cube_mesh:0/gv_static_mesh:61");
		;
	}
	//==================================================================
	{
		m_sphere_mesh = get_sandbox()->find_object< gv_static_mesh >(
			"(3)mat_engine:0/sphere_mesh:0/gv_static_mesh:56");
		;
	}
	//==================================================================
	{
		gvt_array< gv_class_info* > result;
		get_sandbox()->query_classes_derived_from(
			gv_importer_exporter::static_class()->get_name_id(), result);
		result.erase_item(gv_importer_exporter::static_class());
		this->m_imp_exp.resize(result.size());
		for (int i = 0; i < m_imp_exp.size(); i++)
		{
			this->m_imp_exp[i] = gvt_cast< gv_importer_exporter >(
				get_sandbox()->create_object(result[i], this));
		}
	}
	m_clear_color = gv_color::BLACK();
	m_achitype_editor_entity = m_game_entity;
	//============================================================================================
	//								:create world
	//============================================================================================
	my_sandbox->register_processor(gv_id("gv_world"), gve_event_channel_world);
	gv_world* my_world = gvt_cast< gv_world >(
		my_sandbox->get_event_processor(gve_event_channel_world));
	{
		my_world->init(
			1, gv_vector2i(128, 128),
			gv_box(gv_vector3(-1000, 00, -1000), gv_vector3(1000, 200, 1000)),
			NULL);
		my_world->pause();
		m_current_world = my_world;
		{
			gv_entity* my_entity = my_sandbox->create_object< gv_entity >(
				gv_id("editor_observer"), my_world);
			my_entity->add_component(gv_id("gv_com_cam_fps_fly"));
			my_entity->set_position(gv_vector3(0, 2, 0));
			my_entity->set_rotation(gv_euler(0, 0, -0.7f));
			my_entity->set_ignore_line_check(true);
			camera = my_sandbox->create_object< gv_com_camera >(gv_id("main_camera"),
																my_entity);
			camera->set_fov(60.f, ratio, 0.3f, 5000);
			camera->set_look_at(gv_vector3(0, 0, 4.f), gv_vector3(0, 0, 0));
			camera->update_projection_view_matrix();
			my_entity->add_component(camera);
			m_world_editor_entity = my_entity;
		}
	}
	m_achitype_editor_entity->set_world(my_world);
	//============================================================================================
	//============================================================================================
	//								:
	//============================================================================================
	{
		terrain_set_brush_param param;
		param.operation = 0;
		param.power = 50.f;
		param.size = 5;
		this->terrain_set_brush(&param);
	}
	this->double_click_obj(m_sphere_mesh);
	this->double_click_obj(m_default_material);
	//============================================================================================
	// create the builder list
	gvt_array< gv_class_info* > result;
	get_sandbox()->query_classes_derived_from(gv_id("gv_object_builder"), result);
	for (int i = 0; i < result.size(); i++)
	{
		gv_object* obj = get_sandbox()->create_object(result[i]);
		gv_object_builder* builder = gvt_cast< gv_object_builder >(obj);
		GV_ASSERT(builder);
		m_builders.push_back(builder);
	}
};

gv_world* gv_rts_editor_engine::get_world_for_save()
{
	if (this->m_editor_mode == e_editor_mode_map)
		return m_current_world;
	return NULL;
};

bool gv_rts_editor_engine::tick()
{
	super::tick();
	gv_rect window_rect(0, 0, 0, 0);
	gv_vector2 window_size;
	window_size = gv_global::framework_config.window_size;
	window_rect.set_size(window_size);
	{
		// draw frame;
		gv_rect r = window_rect;
		gv_float d = 1;
		this->get_debug_draw()->draw_rect_ring(r, gv_color::BLUE_B(), d);
		r.extend(-d);
		this->get_debug_draw()->draw_rect_ring(r, gv_color::BLUE_D(), d);
		r.extend(-d);
		this->get_debug_draw()->draw_rect_ring(r, gv_color::RED_B(), d);
	}

	gv_global::rnd_opt.m_clear_color = this->m_clear_color;
	if (!get_game_entity())
		return true;
	//======================================================
	if (m_current_world)
	{
		m_world_editor_entity->set_world(m_current_world);
		m_current_world->refresh_from_editor(get_sandbox()->get_delta_time());
	}

	{
		gv_string_tmp mode_name;
		switch (this->m_editor_mode)
		{
		case e_editor_mode_actor:
		{
			mode_name = "MODE: actor editor:";
			m_game_entity = m_achitype_editor_entity;
			m_achitype_editor_entity->get_world()->set_hided(false);
			this->get_debug_draw()->draw_grid_3d(get_game_entity()->get_position() -
													 gv_vector3(50, 0, 50),
												 100, 100, 1, 1);
			this->get_debug_draw()->draw_axis_with_ani(
				get_game_entity()->get_position() - gv_vector3(0.03f, 0, 0.01f), 200,
				1.f);
			gv_com_camera* camera = get_game_entity()->get_component< gv_com_camera >();
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
			if (get_game_entity())
			{
				get_game_entity()->debug_draw_aabb();
				get_game_entity()->debug_draw_axis();
				this->get_debug_draw()->draw_sphere(
					get_game_entity()->get_world_bounding_sphere(), gv_color::BLUE_B());
			}
			this->get_debug_draw()->draw_arrow(
				gv_line_segment(
					get_game_entity()->get_position() + gv_vector3(0, 10, 0),
					get_game_entity()->get_position() + gv_vector3(0, 10, 0) +
						camera->get_world_dir() * 10),
				gv_color::PINK());
			// DRAW THE SKELETAL
			gv_com_skeletal_mesh* com_mesh =
				this->m_game_entity->get_component< gv_com_skeletal_mesh >();
			if (com_mesh)
			{
				gv_skeletal_mesh* my_skeletal = com_mesh->get_skeletal_mesh();
				const gv_matrix44& world = get_game_entity()->get_tm();
				if (my_skeletal)
				{
					for (int i = 0; i < com_mesh->get_writable_skeletal()->m_bones.size();
						 i++)
					{
						gv_bone& bone = com_mesh->get_writable_skeletal()->m_bones[i];
						if (bone.m_hierachy_depth)
						{
							gv_bone& father =
								com_mesh->get_writable_skeletal()->m_bones[bone.m_parent_idx];
							this->get_debug_draw()->draw_line_3d(
								(bone.m_tm * world).get_trans(),
								(father.m_tm * world).get_trans(), gv_color::WHITE(),
								gv_color::BLACK());
						}
						gv_vector2i pos =
							camera->world_to_window((bone.m_tm * world).get_trans());
						get_debug_draw()->draw_string(*bone.m_name, (float)pos.get_x(),
													  (float)pos.get_y(),
													  gv_color::YELLOW());
					}
				}
			}
			break;
		}

		case e_editor_mode_map:
		{
			mode_name = "MODE: map      editor:";
			this->get_debug_draw()->draw_grid_3d(
				m_current_world->get_world_box().min_p,
				m_current_world->get_world_box().get_xdelta(),
				m_current_world->get_world_box().get_zdelta(), 10, 10);
			this->get_debug_draw()->draw_axis_with_ani(
				gv_vector3(0.01f, 0, 0.01f),
				m_current_world->get_world_box().get_xdelta() * 2, 1.f);
			m_game_entity = m_world_editor_entity;
			gv_com_camera* camera = get_game_entity()->get_component< gv_com_camera >();
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
			get_game_entity()->debug_draw_aabb();
			get_game_entity()->debug_draw_axis();
			this->get_debug_draw()->draw_box_3d(
				gv_box(gv_vector3(-1, -1, -1), gv_vector3(1, 1, 1)),
				gv_color::YELLOW());
			this->get_debug_draw()->draw_box_3d(
				gv_box(gv_vector3(-.5f, -.5f, -.5f), gv_vector3(0.5f, 0.5f, 0.5f)),
				gv_color::PINK());

			gv_box box;
			box.add(m_last_hit_position + gv_vector3(0, 0.6f, 0));
			box.extend(gv_vector3(1, 1, 1));
			this->get_debug_draw()->draw_box_3d(box, gv_color::GREEN());

			if (m_drag_start)
			{
				gv_vector3 hit_pos;
				gv_entity* entity;
				if (this->test_mouse_point(hit_pos, entity))
				{
					gv_box b(hit_pos);
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_D());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_B());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::RED());
					if (entity)
					{
						gv_global::debug_draw->draw_box_3d(entity->get_world_aabb(),
														   gv_color::YELLOW());
					}
				}
			}
			if (m_selected_entity && m_selected_entity->get_world() &&
				!m_selected_entity->get_world()->is_hided() &&
				!m_selected_entity->is_deleted())
			{
				gv_box b = m_selected_entity->get_world_aabb();
				b.min_p.y = 0;
				gv_global::debug_draw->draw_box_3d(b, gv_color::GREY());
				gv_global::debug_draw->draw_box_3d(m_selected_entity->get_world_aabb(),
												   gv_color::YELLOW());
				gv_global::debug_draw->draw_sphere(
					m_selected_entity->get_world_bounding_sphere(), gv_color::PINK());
			}
			break;
		}

		case e_editor_mode_game:
		{
			mode_name = "MODE: game     preview:";
			break;
		}

		case e_editor_mode_material:
		{
			mode_name = "MODE: material editor:";
			break;
		}

		case e_editor_mode_terrain:
		{
			mode_name = "MODE: terrain  editor:";
			;
			this->get_debug_draw()->draw_grid_3d(
				m_current_world->get_world_box().min_p,
				m_current_world->get_world_box().get_xdelta(),
				m_current_world->get_world_box().get_zdelta(), 10, 10);
			this->get_debug_draw()->draw_axis_with_ani(
				gv_vector3(0.01f, 0, 0.01f),
				m_current_world->get_world_box().get_xdelta() * 2, 1.f);
			m_game_entity = m_world_editor_entity;
			gv_com_camera* camera = get_game_entity()->get_component< gv_com_camera >();
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
			get_game_entity()->debug_draw_aabb();
			get_game_entity()->debug_draw_axis();
			this->get_debug_draw()->draw_box_3d(
				gv_box(gv_vector3(-1, -1, -1), gv_vector3(1, 1, 1)),
				gv_color::YELLOW());
			this->get_debug_draw()->draw_box_3d(
				gv_box(gv_vector3(-.5f, -.5f, -.5f), gv_vector3(0.5f, 0.5f, 0.5f)),
				gv_color::PINK());

			gv_box box;
			box.add(m_last_hit_position + gv_vector3(0, 0.6f, 0));
			box.extend(gv_vector3(1, 1, 1));
			this->get_debug_draw()->draw_box_3d(box, gv_color::GREEN());

			if (m_drag_start)
			{
				gv_vector3 hit_pos;
				gv_entity* entity;
				if (this->test_mouse_point(hit_pos, entity))
				{
					gv_box b(hit_pos);
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_D());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::YELLOW_B());
					b.extend(gv_vector3(0.3f));
					gv_global::debug_draw->draw_box_3d(b, gv_color::RED());
					if (entity)
					{
						gv_global::debug_draw->draw_box_3d(entity->get_world_aabb(),
														   gv_color::YELLOW());
					}
				}
			}
			break;
		}
		}

		this->get_debug_draw()->draw_string(mode_name, gv_vector2i(10, 10),
											gv_color::WHITE());
		this->get_debug_draw()->draw_string(mode_name, gv_vector2i(8, 8),
											gv_color::BLACK());
	}
	return true;
};

void gv_rts_editor_engine::double_click_obj(gv_object* object, bool lock)
{
	if (object && object->is_owned_by(m_game_entity))
		return;
	if (object && object->is_a(gv_com_graphic::static_class()))
	{
		gv_com_graphic* pcom = gvt_cast< gv_com_graphic >(object);
		object = pcom->get_resource();
	}
	if (!object)
	{
		m_selected_entity = NULL;
		return;
	}
	if (object->is_a(gv_static_mesh::static_class()) &&
		get_editor_mode() == e_editor_mode_actor)
	{
		gv_static_mesh* pmesh = gvt_cast< gv_static_mesh >(object);
		gv_com_static_mesh* com_mesh =
			m_game_entity->get_component< gv_com_static_mesh >();
		gvt_ref_ptr< gv_material > mat = NULL;
		if (com_mesh)
		{
			mat = com_mesh->get_material();
		}
		m_game_entity->detach_component< gv_com_static_mesh >();
		m_game_entity->detach_component< gv_com_skeletal_mesh >();
		// if (!com_mesh)
		{
			com_mesh = get_sandbox()->create_object< gv_com_static_mesh >();
			com_mesh->set_resource(pmesh);
			com_mesh->set_material(mat ? mat : m_default_material);
			com_mesh->set_renderer_id(gve_render_pass_opaque,
									  gv_id("gv_com_effect_renderer"));
			m_game_entity->add_component(com_mesh);
		}
		// else com_mesh->set_resource(pmesh);
		gv_com_camera* pcamera = m_game_entity->get_component< gv_com_camera >();
		gv_float r = com_mesh->get_local_bsphere().get_radius();
		pcamera->set_eye_distance(r * 2);
		pcamera->set_fov(60, pcamera->get_ratio(), 0.1f * r, 100 * r);
		pcamera->update_projection_view_matrix();
		m_game_entity->reset_bounding_from_components();
	}
	if (object->is_a(gv_skeletal_mesh::static_class()) &&
		get_editor_mode() == e_editor_mode_actor)
	{
		gv_skeletal_mesh* pmesh = gvt_cast< gv_skeletal_mesh >(object);

		gvt_ref_ptr< gv_material > mat = NULL;
		gv_com_skeletal_mesh* com_mesh =
			m_game_entity->get_component< gv_com_skeletal_mesh >();
		if (com_mesh)
		{
			mat = com_mesh->get_material();
		}
		m_game_entity->detach_component< gv_com_static_mesh >();
		m_game_entity->detach_component< gv_com_skeletal_mesh >();
		// if (!com_mesh)
		{
			com_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >();
			com_mesh->set_resource(pmesh);
			// com_mesh->set_material(m_default_material);
			com_mesh->set_material(mat ? mat : m_default_material);
			com_mesh->set_renderer_id(gve_render_pass_opaque,
									  gv_id("gv_com_effect_renderer"));
			m_game_entity->add_component(com_mesh);
		}

		gv_com_camera* pcamera = m_game_entity->get_component< gv_com_camera >();
		m_game_entity->reset_bounding_from_components();
		gv_float r = m_game_entity->get_local_bounding_sphere().r;
		pcamera->set_eye_distance(r * 2);
		pcamera->set_fov(60, pcamera->get_ratio(), 0.1f * r, 100 * r);
		pcamera->update_projection_view_matrix();
		// m_game_entity->reset_bounding_from_components();
	}
	if (object->is_a(gv_ani_set::static_class()) &&
		get_editor_mode() == e_editor_mode_actor)
	{
		gv_com_animation* pani = m_game_entity->get_component< gv_com_animation >();
		gv_ani_set* pani_set = gvt_cast< gv_ani_set >(object);
		if (!pani)
		{
			pani = m_game_entity->add_component< gv_com_animation >();
		}
		pani->set_ani_set(pani_set);
		gv_ani_sequence* pseq = pani_set->get_sequence(0);
		if (pseq)
		{
			pani->play_animation(pseq->get_name_id(), 0.5, true, true);
		}
	}
	if (object->is_a(gv_ani_sequence::static_class()) &&
		get_editor_mode() == e_editor_mode_actor)
	{
		gv_com_animation* pani = m_game_entity->get_component< gv_com_animation >();
		gv_ani_set* pani_set = gvt_cast< gv_ani_set >(object->get_owner());
		if (!pani)
		{
			pani = m_game_entity->add_component< gv_com_animation >();
		}
		pani->set_ani_set(pani_set);
		gv_ani_sequence* pseq = (gv_ani_sequence*)object;
		pani->play_animation(pseq->get_name_id(), 0.5, true, true);
	}
	else if (object->is_a(gv_effect::static_class()) &&
			 get_editor_mode() == e_editor_mode_actor)
	{
		gv_com_graphic* pcom = m_game_entity->get_component< gv_com_static_mesh >();
		if (!pcom)
			pcom = m_game_entity->get_component< gv_com_skeletal_mesh >();
		gv_material* pmaterial = NULL;
		if (pcom)
			pmaterial = pcom->get_material();
		gv_effect* my_effect = gvt_cast< gv_effect >(object);
		my_effect = my_effect->get_renderable_effect();
		if (my_effect)
		{
			// m_default_material->m_effect=my_effect;
			if (m_default_material.ptr() == pmaterial)
			{
				pmaterial = get_sandbox()->create_object< gv_material >(pcom);
				pcom->set_material(pmaterial);
			}
			if (pmaterial)
				pmaterial->set_effect(my_effect);
		}
		else
		{
			MessageBoxA(
				NULL,
				"failed to set the effect , it's not renderable (no pass inside)",
				"error", MB_OK);
		}
	}
	else if (object->is_a(gv_material::static_class()) &&
			 get_editor_mode() == e_editor_mode_actor)
	{
		gv_com_static_mesh* pcom =
			m_game_entity->get_component< gv_com_static_mesh >();
		gv_com_skeletal_mesh* pcom_s =
			m_game_entity->get_component< gv_com_skeletal_mesh >();
		gv_material* pmaterial = gvt_cast< gv_material >(object);
		pmaterial = get_sandbox()->clone_object(pmaterial);
		pmaterial->set_owner(m_game_entity);
		if (pcom)
			pcom->set_material(pmaterial);
		if (pcom_s)
			pcom_s->set_material(pmaterial);
	}
	else if (object->is_a(gv_world::static_class()))
	{
		switch_world(gvt_cast< gv_world >(object));
	}
	else if (object->is_a(gv_entity::static_class()) &&
			 get_editor_mode() == e_editor_mode_map)
	{
		m_selected_entity = gvt_cast< gv_entity >(object);
		if (m_selected_entity->get_world() == m_current_world)
		{
			float r = m_selected_entity->get_world_bounding_sphere().get_radius() * 4;
			m_world_editor_entity->set_position(m_selected_entity->get_position() +
												gv_vector3(0, r, r));
			m_world_editor_entity->set_rotation(gv_euler(0, 0, -0.7f));
			m_world_editor_entity->update_matrix();
			set_selection_locked(lock);
		}
	}
};

bool gv_rts_editor_engine::switch_world(gv_world* my_world)
{
	this->m_current_world->set_hided(true);
	this->m_current_world = my_world;
	this->get_sandbox()->set_processor(my_world, gve_event_channel_world);
	my_world->set_hided(false);
	my_world->pause();
	gv_entity* entity;
	entity = m_world_editor_entity;
	{
		gv_sandbox* my_sandbox = get_sandbox();
		entity->set_position(gv_vector3(0, 2, 0));
		entity->set_rotation(gv_euler(0, 0, -0.7f));
		entity->set_ignore_line_check(true);
		m_current_world = my_world;
	}
	return true;
};

void gv_rts_editor_engine::single_click_obj(gv_object* object){

};

gv_bool gv_rts_editor_engine::exec(const gv_string_tmp& command,
								   gv_string_tmp& param1, gv_string_tmp& param2,
								   gv_string_tmp& param3)
{
	GVM_LOG(main, "[EDITOR][EXEC]: " << command << "  " << param1 << " " << param2
									 << gv_endl);
	gv_global::fm->reset_work_path();
	try
	{
		if (command == "update")
		{
			return get_sandbox()->assign_global_property_text(param1, param2);
		}
		else if (command == "call")
		{
			return get_sandbox()->exec_global_call(param1, param2) != 0;
		}
		else if (command == "delete")
		{
			gv_object* owner = get_sandbox()->find_object(param1);
			if (owner)
			{
				if (owner->is_a(gv_component::static_class()) && owner->get_owner() &&
					owner->get_owner()->is_a(gv_entity::static_class()))
				{
					gv_entity* pentity = gvt_cast< gv_entity >(owner->get_owner());
					gv_component* pcom = gvt_cast< gv_component >(owner);
					pentity->detach_component(pcom);
				}
				else
				{
					get_sandbox()->delete_object_tree(owner);
					;
				}
			}
		}
		else if (command == "new")
		{
			gv_class_info* pclass_info = get_sandbox()->find_class(gv_id(*param1));
			gv_object* powner = get_sandbox()->find_object(param3);
			gv_object* pnew_obj = get_sandbox()->create_object(
				pclass_info, gv_object_name(gv_id(*param2)), powner);
			gv_entity* pentity = gvt_cast< gv_entity >(powner);
			gv_component* pcom = gvt_cast< gv_component >(pnew_obj);
			if (pentity && pcom)
			{
				pentity->add_component(pcom);
				GVM_LOG(main, "attack " << pcom->get_name() << " to entity "
										<< pentity->get_name_id());
			}
		}
		else if (command == "move")
		{
			gv_module* mod = get_sandbox()->find_module(gv_id(*param2));
			gv_object* owner = get_sandbox()->find_object(param1);
			owner->set_owner(mod);
			double_click_obj(owner);
		}
		else if (command == "rename")
		{
			gv_object* owner = get_sandbox()->find_object(param1);
			owner->rename(param2);
			double_click_obj(owner);
		}
		else if (command == "clone")
		{
			gv_object* owner = get_sandbox()->find_object(param1);
			gv_entity* pentity = gvt_cast< gv_entity >(owner);
			gv_object* pobj;
			if (pentity)
				pobj = pentity->clone();
			else
				pobj = owner->get_sandbox()->clone(owner);
			pobj->set_owner(owner->get_owner());
			pobj->rename(param2);
			double_click_obj(owner);
		}
	}
	catch (const char* p)
	{
		GVM_LOG(main, "Error in exec " << command << "message is :" << p << "!!!"
									   << gv_endl);
	}
	catch (...)
	{
		GVM_LOG(main, "Error in exec " << command << "!!!!" << gv_endl);
	}
	return true;
}

void gv_rts_editor_engine::undo(){

};
void gv_rts_editor_engine::redo(){

};

gv_int gv_rts_editor_engine::get_nb_imp_exp()
{
	return m_imp_exp.size();
};

gv_importer_exporter* gv_rts_editor_engine::get_imp_exp(int idx)
{
	if (idx < m_imp_exp.size())
		return m_imp_exp[idx];
	return 0;
};
gv_com_camera* gv_rts_editor_engine::get_world_editor_camera()
{
	return m_world_editor_entity->get_component< gv_com_camera >();
};
void gv_rts_editor_engine::drop_object_to_entity(gv_object* object,
												 gv_entity* entity)
{

	if (object->is_a(gv_static_mesh::static_class()))
	{
		gv_com_static_mesh* pcom =
			get_sandbox()->create_object< gv_com_static_mesh >();
		pcom->set_resource(gvt_cast< gv_static_mesh >(object));
		pcom->set_renderer_id(gve_render_pass_opaque,
							  gv_id("gv_com_effect_renderer"));
		pcom->set_material(m_default_material);
		entity->add_component(pcom);
	}
	else if (object->is_a(gv_skeletal_mesh::static_class()))
	{
		gv_com_skeletal_mesh* pcom =
			get_sandbox()->create_object< gv_com_skeletal_mesh >();
		pcom->set_resource(gvt_cast< gv_skeletal_mesh >(object));
		pcom->set_renderer_id(gve_render_pass_opaque,
							  gv_id("gv_com_effect_renderer"));
		pcom->set_material(m_default_material);
		entity->add_component(pcom);
	}
	else if (object->is_a(gv_ani_set::static_class()))
	{
		gv_com_animation* pcom = get_sandbox()->create_object< gv_com_animation >();
		gv_ani_set* pani = gvt_cast< gv_ani_set >(object);
		pcom->set_ani_set(pani);
		pcom->play_animation(pani->get_sequence(0)->get_name_id(), 0, true);
		entity->add_component(pcom);
	}
	else if (object->is_a(gv_material::static_class()))
	{
		gv_com_static_mesh* pcom_static_mesh =
			entity->get_component< gv_com_static_mesh >();
		gv_material* pmat = gvt_cast< gv_material >(object);
		if (pcom_static_mesh)
		{
			pcom_static_mesh->set_material(pmat);
		}
		gv_com_skeletal_mesh* pcom_skeletal_mesh =
			entity->get_component< gv_com_skeletal_mesh >();
		if (pcom_skeletal_mesh)
		{
			pcom_skeletal_mesh->set_material(pmat);
		}
	}
	else if (object->is_a(gv_effect::static_class()))
	{
		gv_com_static_mesh* pcom_static_mesh =
			entity->get_component< gv_com_static_mesh >();
		gv_effect* peffect = gvt_cast< gv_effect >(object);
		if (pcom_static_mesh)
		{
			gv_material* pmat = pcom_static_mesh->get_material();
			if (!pmat)
			{
				pmat = get_sandbox()->create_object< gv_material >(pcom_static_mesh);
				pcom_static_mesh->set_material(pmat);
			}
			pmat->set_effect(peffect);
		}
		gv_com_skeletal_mesh* pcom_skeletal_mesh =
			entity->get_component< gv_com_skeletal_mesh >();
		if (pcom_skeletal_mesh)
		{
			gv_material* pmat = pcom_skeletal_mesh->get_material();
			if (!pmat)
			{
				pmat = get_sandbox()->create_object< gv_material >(pcom_skeletal_mesh);
				pcom_skeletal_mesh->set_material(pmat);
			}
			pmat->set_effect(peffect);
		}
	}
	else if (object->is_a(gv_class_info::static_class()))
	{
		gv_class_info* pcls = gvt_cast< gv_class_info >(object);
		if (pcls->is_derive_from(gv_component::static_class()))
		{
			gv_component* pcom =
				gvt_cast< gv_component >(get_sandbox()->create_object(pcls, entity));
			entity->add_component(pcom);
		}
	}
};
void gv_rts_editor_engine::drop_object_to_world_use_mouse_position(
	gv_object* object)
{

	gv_vector3 hit_point;
	gv_entity* entity = NULL;
	if (!this->test_mouse_point(hit_point, entity))
	{
		return;
	}
	if (entity)
	{
		bool is_terrain = entity->get_component< gv_com_terrain_roam >() != NULL;
		if (is_terrain)
			entity = NULL; // don't add component to a terrain.
	}
	if (object->is_a(gv_entity::static_class()))
	{
		gv_entity* cu_entity = gvt_cast< gv_entity >(object);
		entity = cu_entity->clone();
		entity->set_position(hit_point + gv_vector3(0, 1, 0));
		entity->update_matrix();
		m_current_world->add_entity(entity);
		return;
	}
	if (!entity)
	{
		gv_string_tmp name = get_new_object_name_dialog();
		entity =
			get_sandbox()->create_object< gv_entity >(gv_id(*name), m_current_world);
		entity->set_position(hit_point + gv_vector3(0, 1, 0));
		entity->update_matrix();
		m_current_world->add_entity(entity);
	}
	drop_object_to_entity(object, entity);
};

void gv_rts_editor_engine::drop_object_to_actor(gv_object* object)
{
	gv_entity* entity = this->get_game_entity();
	drop_object_to_entity(object, entity);
	return;
};

bool gv_rts_editor_engine::validate_selected_entity()
{
	if (!m_selected_entity || !m_selected_entity->get_world() ||
		m_selected_entity->get_world()->is_hided() ||
		m_selected_entity->is_deleted())
	{
		m_selected_entity = NULL;
		return false;
	}
	return true;
};

void gv_rts_editor_engine::drag_object_with_mouse(bool shift, bool ctrl,
												  bool alt, bool use_right)
{
	if (!validate_selected_entity())
		return;
	if (ctrl) // move
	{
		if (!use_right)
		{
			// move to the ground
			gv_vector3 hit_point;
			gv_entity* entity = NULL;
			m_selected_entity->set_ignore_line_check(true);
			if (this->test_mouse_point(hit_point, entity))
			{
				gv_float height =
					m_selected_entity->get_world_aabb().get_ydelta() / 2.f;
				m_selected_entity->set_position(hit_point + gv_vector3(0, height, 0));
				m_selected_entity->update_matrix();
			}
			m_selected_entity->set_ignore_line_check(false);
		}
		else
		{
			// update height
			gv_vector3 v = this->get_world_editor_camera()->get_world_dir();
			;
			gv_vector3 v2 = v.cross(gv_vector3(0, 1, 0));
			gv_vector3 normal = v2.cross((gv_vector3(0, 1, 0)));
			gv_plane plane(normal, m_selected_entity->get_position());
			;
			gv_vector2i cursor;
			gv_global::input->get_mouse_pos(cursor);
			v = get_world_editor_camera()->window_to_world(cursor);
			gv_vector3 o;
			o = get_world_editor_camera()->get_world_position();
			v -= o;
			v.normalize();
			gv_line line(o, v);
			gv_vector3 hit_point;
			gv_float line_param;
			if (gv_geom::intersect_ray_plane(line, plane, hit_point, line_param))
			{
				v = m_selected_entity->get_position();
				m_selected_entity->set_position(gv_vector3(v.x, hit_point.y, v.z));
				m_selected_entity->update_matrix();
			};
		}
	}
	else if (shift)
	{
		if (!use_right)
		{
			float dt = get_sandbox()->get_delta_time();
			gv_vector2i delta;
			get_sandbox()->get_input_manager()->get_mouse_pos_delta(delta);
			gv_euler rot = m_selected_entity->get_rotation();
			rot.yaw -= gvt_clamp(delta.x, -100, +100) * dt;
			rot.pitch -= gvt_clamp(delta.y, -100, +100) * dt;
			//{
			//	gv_float
			// step=get_sandbox()->get_input_manager()->get_mouse_wheel_delta()*(dt);
			//	rot.roll+=step*0.01f;
			//	get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
			//}
			m_selected_entity->set_rotation(rot);
			m_selected_entity->update_matrix();
		}
		else
		{
			float dt = get_sandbox()->get_delta_time();
			gv_vector2i delta;
			get_sandbox()->get_input_manager()->get_mouse_pos_delta(delta);
			gv_euler rot = m_selected_entity->get_rotation();
			rot.roll -= gvt_clamp(delta.y, -100, +100) * dt;
			m_selected_entity->set_rotation(rot);
			m_selected_entity->update_matrix();
		}
	}
	else if (alt)
	{
		if (!use_right)
		{
			float dt = get_sandbox()->get_delta_time();
			gv_vector2i delta;
			get_sandbox()->get_input_manager()->get_mouse_pos_delta(delta);
			gv_vector3 scale = m_selected_entity->get_scale();
			scale.x -= gvt_clamp(delta.x, -100, +100) * dt;
			scale.z -= gvt_clamp(delta.y, -100, +100) * dt;
			m_selected_entity->set_scale(scale);
			m_selected_entity->update_matrix();
		}
		else
		{
			float dt = get_sandbox()->get_delta_time();
			gv_vector2i delta;
			get_sandbox()->get_input_manager()->get_mouse_pos_delta(delta);
			gv_vector3 scale = m_selected_entity->get_scale();
			scale.y -= gvt_clamp(delta.y, -100, +100) * dt;
			m_selected_entity->set_scale(scale);
			m_selected_entity->update_matrix();
		}
	}
	m_selected_entity->notify_change_to_components();
};

bool gv_rts_editor_engine::test_mouse_point(gv_vector3& world_pos,
											gv_entity*& entity)
{
	gv_vector2i cursor;
	gv_global::input->get_mouse_pos(cursor);
	gv_vector3 v;
	v = get_world_editor_camera()->window_to_world(cursor);
	gv_vector3 o;
	o = get_world_editor_camera()->get_world_position();
	gv_line_segment line(o, o + (v - o) * 30000.f);
	entity = NULL;
	// gv_global::debug_draw->draw_line_3d(line.start_p,line.end_p,gv_color::RED_B(),
	// gv_color::RED_B());
	gvt_object_iterator< gv_com_terrain_roam > it(get_sandbox());
	while (!it.is_empty())
	{
		it->unselect_all_patch();
		++it;
	}
	bool ret = m_current_world->line_check(line, world_pos, entity);
	if (ret)
	{
		m_last_hit_position = world_pos;
		GVM_LOG(main, "mouse hit " << world_pos << "entity : "
								   << (entity ? *entity->get_name_id() : "none"))
	}
	return ret;
};

void gv_rts_editor_engine::start_drag(gv_object* obj)
{
	m_drag_start = true;
	m_drag_object = obj;
}

void gv_rts_editor_engine::end_drag()
{
	m_drag_start = false;
}

bool gv_rts_editor_engine::is_drag_move_started()
{
	return m_is_drag_move_started;
};

void gv_rts_editor_engine::start_drag_move()
{
	m_is_drag_move_started = true;
	if (m_current_world)
	{
		m_world_editor_entity->get_component(get_sandbox()->find_class(
												 gv_id("gv_com_cam_fps_fly")))
			->set_enabled(false);
	}
}

void gv_rts_editor_engine::end_drag_move()
{
	m_is_drag_move_started = false;
	if (m_current_world)
	{
		m_world_editor_entity->get_component(get_sandbox()->find_class(
												 gv_id("gv_com_cam_fps_fly")))
			->set_enabled(true);
	}
}
//============================================================================================
//								:
//============================================================================================
bool gv_rts_editor_engine::is_paint_terrain_start()
{
	return m_is_paint_terrain_start;
};

void gv_rts_editor_engine::start_paint_terrain()
{
	m_is_paint_terrain_start = true;
	if (m_current_world)
	{
		m_world_editor_entity->get_component(get_sandbox()->find_class(
												 gv_id("gv_com_cam_fps_fly")))
			->set_enabled(false);
	}
}

void gv_rts_editor_engine::end_paint_terrain()
{
	m_is_paint_terrain_start = false;
	if (m_current_world)
	{
		m_world_editor_entity->get_component(get_sandbox()->find_class(
												 gv_id("gv_com_cam_fps_fly")))
			->set_enabled(true);
	}
}

void gv_rts_editor_engine::paint_object_with_mouse(bool shift, bool ctrl,
												   bool alt, bool use_right)
{
	// if (!validate_selected_entity() ) return ;
	gv_vector3 hit_point;
	gv_entity* entity = NULL;
	if (this->test_mouse_point(hit_point, entity))
	{
		gv_com_terrain_roam* pterrain =
			entity ? entity->get_component< gv_com_terrain_roam >() : NULL;
		if (pterrain)
		{
			gv_vector3 v = pterrain->world_to_terrain_local(hit_point);
			gv_float ix = gvt_floor(v.x);
			gv_float iy = gvt_floor(v.y);
			ix -= m_terrain_update_pattern_image_size.get_x() / 2;
			iy -= m_terrain_update_pattern_image_size.get_y() / 2;
			gv_rect r(ix, iy, ix + m_terrain_update_pattern_image_size.get_x(),
					  iy + m_terrain_update_pattern_image_size.get_y());
			gv_int operation = 0;
			if (shift)
			{
				operation = 1;
			}
			switch (m_current_brush)
			{
			case e_brush_height_map:
				pterrain->update_heightmap(r, m_terrain_update_pattern_image, operation,
										   get_sandbox()->get_delta_time() *
											   m_terrain_brush_power);
				break;

			case e_brush_grass:
				pterrain->update_grass_layer(
					m_terrain_paint_layer, r, m_terrain_update_pattern_image, operation,
					get_sandbox()->get_delta_time() * m_terrain_brush_power);
				break;

			case e_brush_mesh:
				pterrain->update_mesh_layer(
					m_terrain_paint_layer, r, m_terrain_update_pattern_image, operation,
					get_sandbox()->get_delta_time() * m_terrain_brush_power);
				break;

			case e_brush_fur:
				pterrain->update_fur_layer(
					m_terrain_paint_layer, r, m_terrain_update_pattern_image, operation,
					get_sandbox()->get_delta_time() * m_terrain_brush_power);
				break;
			}
		}
	}
}
void gv_rts_editor_engine::c_terrain_set_brush(terrain_set_brush_param* input)
{
	m_terrain_brush_power = input->power;
	m_terrain_update_pattern_image_size.set(input->size, input->size);
	m_terrain_paint_layer = input->layer;
	float coeff = -0.35f / (input->size * input->size) * 25;
	gvt_get_gaussian_kernal(m_terrain_update_pattern_image,
							m_terrain_update_pattern_image_size.get_x(),
							m_terrain_update_pattern_image_size.get_y(), coeff);
	// m_terrain_update_pattern_image=0.5f;
}

gv_bool gv_rts_editor_engine::is_selection_locked()
{
	return m_is_selection_locked;
};

void gv_rts_editor_engine::set_selection_locked(gv_bool b)
{
	m_is_selection_locked = b;
	GVM_LOG(main, "select lock is " << m_is_selection_locked << gv_endl);
};

void gv_rts_editor_engine::create_map_preview()
{
	if (!m_current_world)
		return;
	if (m_current_world->get_module()->get_name_id() == gv_id_native)
		return;
	gv_global::fm->reset_work_path();
	gv_string_tmp cmd = "..\\client\\gv_rts_client.exe -offline -map -";
	cmd << m_current_world->get_module()->get_name_id();
	gv_global::fm->reset_work_path();
	gv_system_call(*cmd);
}

//============================================================================================
//								:
//============================================================================================
gv_rts_editor_engine_option::gv_rts_editor_engine_option()
{
}

GVM_IMP_CLASS(gv_rts_editor_engine, gv_game_engine)
GVM_VAR(gv_color, m_clear_color)
/*
GVM_VAR(gv_color	,		m_test_color_red)
GVM_VAR(gv_color	,		m_test_color_green)
GVM_VAR(gv_color	,		m_test_color_blue)
GVM_VAR(gvt_array<gv_int > ,	m_test_array)
GVM_STATIC_ARRAY(gv_int,16 ,	m_test_array_static)
GVM_VAR(gvt_array<gvt_array<gv_string> >	,m_test_array_2)
GVM_VAR(gv_text				,m_test_text)
GVM_VAR_ATTRIB_SET(file_name)
        GVM_VAR(gv_text				,m_test_file_name)
GVM_VAR_ATTRIB_UNSET(file_name)
GVM_VAR(gv_vector3			,m_test_v3)
GVM_VAR(gv_matrix44			,m_test_m )*/
GVM_VAR(gvt_ref_ptr< gv_material >, m_default_material)
GVM_VAR(gvt_ref_ptr< gv_static_mesh >, m_cube_mesh)
GVM_VAR(gvt_ref_ptr< gv_static_mesh >, m_sphere_mesh)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_object_builder > >, m_builders)
GVM_END_CLASS

GVM_IMP_STRUCT(terrain_set_brush_param)
GVM_VAR(gv_int, size)
GVM_VAR(gv_float, power)
GVM_VAR(gv_int, operation)
GVM_VAR(gv_int, layer)
GVM_VAR_ATTRIB_SET(file_name);
GVM_VAR(gv_text, pattern_image_file)
GVM_END_STRUCT

GVM_BGN_FUNC(gv_rts_editor_engine, switch_to_edit_map_mode)
get_editor()->set_editor_mode(e_editor_mode_map);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, switch_to_edit_terrain_mode)
get_editor()->set_editor_mode(e_editor_mode_terrain);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, switch_to_edit_actor_mode)
get_editor()->set_editor_mode(e_editor_mode_actor);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, terrain_set_brush)
c_terrain_set_brush(input);
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, terrain_switch_to_height_map_brush)
m_current_brush = e_brush_height_map;
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, terrain_switch_to_mesh_brush)
m_current_brush = e_brush_mesh;
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, terrain_switch_to_grass_brush)
m_current_brush = e_brush_grass;
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_rts_editor_engine, terrain_switch_to_fur_brush)
m_current_brush = e_brush_fur;
return 1;
GVM_END_FUNC
}

#include "gv_rts_editor_engine_structs.h"
namespace gv
{
GVM_IMP_STATIC_CLASS(gv_rts_editor_engine_option)
}