#pragma once
namespace gv
{
struct gv_rts_editor_engine_option
{
	GVM_WITH_STATIC_CLS;
	gv_rts_editor_engine_option();
};
//============================================================================================
//								:
//============================================================================================
enum gve_editor_mode
{
	e_editor_mode_actor,
	e_editor_mode_map,
	e_editor_mode_game,
	e_editor_mode_material,
	e_editor_mode_terrain,
};

enum gve_terrain_brush_type
{
	e_brush_height_map,
	e_brush_grass,
	e_brush_mesh,
	e_brush_fur,
};

class gv_rts_editor_engine_impl;
class gv_rts_editor_engine : public gv_game_engine
{
public:
	GVM_DCL_CLASS(gv_rts_editor_engine, gv_game_engine);
	gv_rts_editor_engine();
	~gv_rts_editor_engine();
	virtual void init();
	static void static_init(int nb_arg, char* args[], char* extra_arg = NULL);
	static void static_destroy();
	virtual bool tick();
	virtual gv_material* get_default_material();
	virtual void double_click_obj(gv_object* object, gv_bool lock = false);
	virtual void single_click_obj(gv_object* object);
	virtual gv_bool exec(const gv_string_tmp& command, gv_string_tmp& param1,
						 gv_string_tmp& param2,
						 gv_string_tmp& param3 = *((gv_string_tmp*)NULL));
	virtual void undo();
	virtual void redo();
	virtual bool load_default_option(gv_engine_option& option);
	gv_int get_nb_imp_exp();
	class gv_importer_exporter* get_imp_exp(int idx);
	void set_editor_mode(gve_editor_mode mode)
	{
		m_editor_mode = mode;
	};
	gve_editor_mode get_editor_mode()
	{
		return m_editor_mode;
	};
	void drop_object_to_entity(gv_object* object, gv_entity* pentity);
	void drop_object_to_world_use_mouse_position(gv_object* object);
	void drop_object_to_actor(gv_object* object);
	bool test_mouse_point(gv_vector3& world_pos, gv_entity*& entity);
	void start_drag(gv_object*);
	void end_drag();
	bool switch_world(gv_world*);
	void drag_object_with_mouse(bool shift, bool ctrl, bool alt, bool use_right);
	bool validate_selected_entity();
	bool is_drag_move_started();
	void start_drag_move();
	void end_drag_move();
	gv_entity* get_selected_entity()
	{
		if (!validate_selected_entity())
			return NULL;
		return m_selected_entity;
	}
	void create_map_preview();
	gv_bool is_selection_locked();
	void set_selection_locked(gv_bool);
	gv_world* get_world_for_save();

protected:
	gv_rts_editor_engine_impl* get_editor_impl()
	{
		return m_impl;
	}
	gv_com_camera* get_world_editor_camera();
	gvt_ref_ptr< gv_rts_editor_engine_impl > m_impl;
	gv_color m_clear_color;
	// test
	/*
  gv_color			m_test_color_red;
  gv_color			m_test_color_green;
  gv_color			m_test_color_blue;
  gvt_array<gv_int >					m_test_array;
  gvt_array_static<gv_int,16 >		m_test_array_static;
  gvt_array<gvt_array<gv_string> >	m_test_array_2;
  gv_text				m_test_text;
  gv_text				m_test_file_name;
  gv_vector3			m_test_v3;
  gv_matrix44			m_test_m;*/

	//==========================================================
	gvt_ref_ptr< gv_static_mesh > m_cube_mesh;
	gvt_ref_ptr< gv_static_mesh > m_sphere_mesh;
	gvt_array< gvt_ptr< gv_importer_exporter > > m_imp_exp;
	gve_editor_mode m_editor_mode;
	gvt_ref_ptr< gv_world > m_current_world;
	gvt_array< gvt_ref_ptr< gv_object_builder > > m_builders;
	gv_bool m_drag_start;
	gvt_ptr< gv_object > m_drag_object;
	gvt_ref_ptr< gv_entity > m_selected_entity;
	gvt_ref_ptr< gv_entity > m_world_editor_entity;
	gvt_ref_ptr< gv_entity > m_achitype_editor_entity;
	gv_vector3 m_last_hit_position;
	gv_bool m_is_drag_move_started;
	gv_bool m_is_selection_locked;
	gv_int m_terrain_paint_layer;
	//======================================================
public:
	// terrain
	//
	bool is_paint_terrain_start();
	void start_paint_terrain();
	void end_paint_terrain();
	void paint_object_with_mouse(bool shift, bool ctrl, bool alt, bool use_right);
	void c_terrain_set_brush(class terrain_set_brush_param* input);

	gvt_array< gv_float > m_terrain_update_pattern_image;
	gv_vector2i m_terrain_update_pattern_image_size;
	gv_int m_terrain_brush_operation;
	gv_float m_terrain_brush_power;
	gv_bool m_is_paint_terrain_start;
	gve_terrain_brush_type m_current_brush;

public:
	GVM_DCL_FUNC(switch_to_edit_map_mode);
	GVM_DCL_FUNC(switch_to_edit_terrain_mode);
	GVM_DCL_FUNC(switch_to_edit_actor_mode);
	GVM_DCL_FUNC(terrain_set_brush);
	GVM_DCL_FUNC(terrain_switch_to_height_map_brush);
	GVM_DCL_FUNC(terrain_switch_to_mesh_brush);
	GVM_DCL_FUNC(terrain_switch_to_grass_brush);
	GVM_DCL_FUNC(terrain_switch_to_fur_brush);
};

class terrain_set_brush_param
{
public:
	terrain_set_brush_param()
	{
		size = 5;
		power = 300.f;
		operation = 1;
		layer = 0;
	};
	gv_text pattern_image_file;
	gv_int size;
	gv_float power;
	gv_int operation; // 0 add , 1  sub ,
	gv_int layer;
};

gv_rts_editor_engine* get_editor();
}