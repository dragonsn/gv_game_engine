#pragma once
namespace gv
{
class gv_com_3d_lobby_controller : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_3d_lobby_controller, gv_component)
	gv_com_3d_lobby_controller()
	{
		m_is_drag = false;
		m_cu_mouse_pos.set(0, 0);
		m_last_mouse_pos.set(0, 0);
		link_class(gv_com_3d_lobby_controller::static_class());
	};
	~gv_com_3d_lobby_controller()
	{
	}
	bool is_tickable()
	{
		return true;
	}
	bool tick(gv_float dt)
	{
		return true;
	}

	bool sync_to_entity(gv_entity* entity)
	{
		gv_entity* my_camera = (gv_entity*)this->get_entity();
		float dt = this->get_sandbox()->get_delta_time();

		pos = my_camera->get_position();
		dir = -my_camera->get_tm().axis_z;
		dir.y = 0;
		rot = my_camera->get_rotation();
		if (get_sandbox()->get_input_manager()->is_key_down(e_key_shift))
		{
			dt *= 10;
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
				// rot.roll-=gvt_clamp(delta.y,-100,+100)*dt;
				gv_com_camera* com_camera = my_camera->get_component< gv_com_camera >();
				if (com_camera)
				{
					gv_vector3 eye, look_at, up;
					com_camera->get_look_at(eye, look_at, up);
					static float speed_factor = 1.0f;
					eye.y += gvt_clamp(delta.y, -100, +100) * dt * speed_factor;
					com_camera->set_look_at(eye, look_at, up);
					com_camera->update_projection_view_matrix();
				}
			}
		}
		{
			gv_float step =
				get_sandbox()->get_input_manager()->get_mouse_wheel_delta() * (dt);
			pos += dir * step;
			get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
		}

		// gv_com_camera * pcamera=entity->get_component<gv_com_camera>();
		entity->get_world()->move_entity(entity, pos, rot);
		pos = entity->get_position();
		rot = entity->get_rotation();
		// entity->set_rotation(rot);
		// entity->set_position(pos);
		// entity->update_matrix();
		return true;
	}

	bool m_is_drag;
	gv_vector2i m_cu_mouse_pos;
	gv_vector2i m_last_mouse_pos;

	gv_vector3 pos;
	gv_vector3 dir;
	gv_euler rot;
};
GVM_IMP_CLASS(gv_com_3d_lobby_controller, gv_component)
GVM_END_CLASS
}