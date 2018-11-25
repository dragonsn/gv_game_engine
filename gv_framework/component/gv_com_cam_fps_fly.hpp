#pragma once
namespace gv
{
class gv_com_cam_fps_fly : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_cam_fps_fly, gv_component)
	gv_com_cam_fps_fly()
	{
		m_is_drag = false;
		m_cu_mouse_pos.set(0, 0);
		m_last_mouse_pos.set(0, 0);
		link_class(gv_com_cam_fps_fly::static_class());
	};
	~gv_com_cam_fps_fly()
	{
	}
	bool is_tickable()
	{
		return true;
	}
	bool tick(gv_float dt)
	{
		const gv_entity* my_camera = this->get_entity();
		pos = my_camera->get_position();
		dir = -my_camera->get_tm().axis_z;
		rot = my_camera->get_rotation();
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_shift))
		{
			dt *= 10;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_up))
		{
			pos.y += 10.f * dt;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_down))
		{
			pos.y -= 10.f * dt;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_w))
		{
			gv_float step = 10 * (dt);
			pos += dir * step;
		}

		if (get_sandbox()->get_input_manager()->is_key_down(e_key_s))
		{
			gv_float step = 10 * (dt);
			pos -= dir * step;
		}

		gv_matrix43 mat;
		gv_math::convert(mat, this->rot);
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_left))
		{
			pos += mat.axis_x * dt;
		}
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_right))
		{
			pos -= mat.axis_x * dt;
		}
		{
			bool down =
				get_sandbox()->get_input_manager()->is_key_down(e_key_lbutton);
			if (m_is_drag && !down)
			{
				m_is_drag = false;
			}
			if (down && !m_is_drag)
			{
			}
			m_is_drag = down;
			m_last_mouse_pos = m_cu_mouse_pos;
			get_sandbox()->get_input_manager()->get_mouse_pos(m_cu_mouse_pos);
			if (m_is_drag)
			{
				gv_vector2i delta = m_cu_mouse_pos - m_last_mouse_pos;
				rot.yaw -= gvt_clamp(delta.x, -100, +100) * dt;
				rot.roll -= gvt_clamp(delta.y, -100, +100) * dt;
			}
		}
		{
			gv_float step =
				get_sandbox()->get_input_manager()->get_mouse_wheel_delta() * (dt);
			pos += dir * step;
			get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
		}

		return true;
	}

	bool sync_to_entity(gv_entity* entity)
	{
		// gv_com_camera * pcamera=entity->get_component<gv_com_camera>();
		entity->set_rotation(rot);
		entity->set_position(pos);
		entity->update_matrix();
		return true;
	}

	bool m_is_drag;
	gv_vector2i m_cu_mouse_pos;
	gv_vector2i m_last_mouse_pos;

	gv_vector3 pos;
	gv_vector3 dir;
	gv_euler rot;
};
GVM_IMP_CLASS(gv_com_cam_fps_fly, gv_component)
GVM_END_CLASS
}