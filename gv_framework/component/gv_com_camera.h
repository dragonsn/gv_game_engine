#pragma once
namespace gv
{
class gv_camer_info
{
public:
	gv_float m_fov;
	gv_vector3 m_dir;
	gv_vector3 m_pos;
	gv_frustum m_world_frustum;
	gv_matrix44 m_view;
	gv_matrix44 m_view_project;
};

class gv_com_camera : public gv_component
{
	friend class gv_renderer_d3d9; //...
	friend class gv_renderer;

public:
	GVM_DCL_CLASS(gv_com_camera, gv_component)
	gv_com_camera();
	~gv_com_camera()
	{
	}
	//====================================================================================
	inline gv_float get_fov()
	{
		return m_fov;
	}
	void set_look_at(const gv_vector3& eye, const gv_vector3& lookat,
					 const gv_vector3 up = gv_vector3::get_y_axis());
	void get_look_at(gv_vector3& eye, gv_vector3& lookat, gv_vector3& up);
	gv_float get_eye_distance();
	void set_eye_distance(gv_float dist);
	void set_fov(gv_float fov, gv_float ratio, gv_float z_near, gv_float z_far);
	void set_fov(gv_float fov);
	void set_current_camera();
	void update_projection_view_matrix();
	void update_world_matrix();

	virtual bool sync_to_entity(gv_entity*);
	virtual void update_camera(gv_float dt); // called from renderer.
	const gv_frustum& get_local_frustum();
	const gv_frustum& get_world_frustum();
	gv_vector3 get_world_position();
	gv_vector3 get_world_dir();
	gv_matrix44 get_world_rotation();

	inline gv_float get_far_clip()
	{
		return m_far_z;
	}
	inline gv_float get_near_clip()
	{
		return m_near_z;
	}

	gv_vector3 window_to_world(const gv_vector2i& win_pos);
	gv_vector2i world_to_window(const gv_vector3& world_pos);
	inline gv_float get_ratio()
	{
		return m_ratio;
	}
	inline void set_ratio(float f)
	{
		m_ratio = f;
	}

protected:
	// attribute..
	gv_float m_fov;
	gv_float m_near_z, m_far_z;
	gv_vector3 m_eye_offset;
	gv_vector3 m_lookat_offset;
	gv_vector3 m_up_vector;
	gv_bool m_use_smooth_follow;
	gv_float m_ratio;

	// base on attribute.
	gv_frustum m_frustum;
	gv_matrix44 m_projection_matrix;
	gv_matrix44 m_entity_view_matrix;

	// world space
	gv_matrix44 m_world_view_matrix;
	gv_matrix44 m_world_entity_matrix;
	gv_matrix44 m_entity_world_matrix;
	gv_frustum m_world_frustum;
	gv_matrix44 m_world_view_project_matrix;
	gv_matrix44 m_view_world_matrix;
};
}