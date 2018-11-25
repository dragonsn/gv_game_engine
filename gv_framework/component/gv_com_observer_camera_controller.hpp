#pragma once
namespace gv
{
class gv_com_observer_camera_controller : public gv_component
{
public:
	GVM_DCL_CLASS(gv_com_observer_camera_controller, gv_component)
	gv_com_observer_camera_controller()
	{
		m_is_drag = false;
		m_cu_mouse_pos.set(0, 0);
		m_last_mouse_pos.set(0, 0);
		link_class(gv_com_observer_camera_controller::static_class());
	};
	~gv_com_observer_camera_controller()
	{
	}
	bool is_tickable()
	{
		return true;
	}
	bool tick(gv_float dt)
	{
		const gv_entity* my_camera = this->get_entity();
		gv_com_camera* pcamera =
			(gv_com_camera*)my_camera->get_component< gv_com_camera >();
		gv_input_manager* input = get_sandbox()->get_input_manager();
		if (!pcamera)
			return true;
		gv_vector3 eye;
		gv_vector3 target, up;
		pcamera->get_look_at(eye, target, up);
		gv_vector3 eye_dir = eye - target;
		float dist = eye_dir.normalize();
		gv_matrix43 mat;
		gv_euler rot(0, 0, 0);
		if (input->is_key_down(e_key_shift))
		{
			dt *= 10;
		}
		// control direction
		bool down = input->is_key_down(e_key_lbutton);
		if (m_is_drag && !down)
		{
			m_is_drag = false;
		}
		m_is_drag = down;
		m_last_mouse_pos = m_cu_mouse_pos;
		input->get_mouse_pos(m_cu_mouse_pos);
		if (m_is_drag)
		{
			gv_vector2i delta = m_cu_mouse_pos - m_last_mouse_pos;
			rot.yaw -= gvt_clamp(delta.x, -100, +100) * dt;
			// rot.roll-=gvt_clamp(delta.y,-100,+100)*dt;
			eye_dir.y = gvt_clamp(eye_dir.y - gvt_clamp(delta.y, -100, +100) * dt,
								  -0.9f, 0.9f);
		}
		gv_math::convert(mat, rot);
		eye_dir = mat.mul_by(eye_dir);
		eye_dir.normalize();
		// control distance.
		static float key_step = 10.0f;
		gv_float step =
			get_sandbox()->get_input_manager()->get_mouse_wheel_delta() * (dt);
		if (input->is_key_down(e_key_w))
		{
			step += key_step;
		}
		if (input->is_key_down(e_key_s))
		{
			step -= key_step;
		}
		dist += step * 0.01f * dist;
		if (dist < 1.0f)
			dist = 1.0f;
		get_sandbox()->get_input_manager()->set_mouse_wheel_delta(0);
		pcamera->set_look_at(target + eye_dir * dist, target);
		pcamera->update_projection_view_matrix();
		return true;
	}

	bool m_is_drag;
	gv_vector2i m_cu_mouse_pos;
	gv_vector2i m_last_mouse_pos;
};
GVM_IMP_CLASS(gv_com_observer_camera_controller, gv_component)
GVM_END_CLASS
}