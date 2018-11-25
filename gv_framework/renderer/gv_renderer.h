#pragma once
namespace gv
{

//============================================================================================
//								:
//============================================================================================
class gv_texture;
class gv_com_camera;
class gv_renderer : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gv_renderer, gv_event_processor);
	gv_renderer();
	virtual ~gv_renderer();
	virtual bool do_render(gv_float dt)
	{
		return true;
	};
	virtual void do_render_pass(gv_float dt, gv_int pass){};
	virtual void begin_render_pass(gv_int pass)
	{
		set_cu_render_pass(pass);
	}
	virtual gv_texture* get_default_texture()
	{
		return NULL;
	}

	virtual gv_texture* get_tex_base()
	{
		return NULL;
	}
	virtual gv_texture* get_tex_normal()
	{
		return NULL;
	}
	virtual gv_texture* get_tex_env()
	{
		return NULL;
	}

	virtual void set_tex_base(gv_texture* p)
	{
	}
	virtual void set_tex_normal(gv_texture* p)
	{
	}
	virtual void set_tex_env(gv_texture* p)
	{
	}

	virtual bool precache_resource(class gv_resource* resource)
	{
		return false;
	}
	virtual bool precache_static_mesh(class gv_static_mesh*)
	{
		return false;
	}
	virtual bool precache_skeletal_mesh(class gv_skeletal_mesh*)
	{
		return false;
	}
	virtual bool precache_texture(class gv_texture*)
	{
		return false;
	}
	virtual bool precache_index_buffer(class gv_index_buffer*)
	{
		return false;
	}
	virtual bool precache_vertex_buffer(class gv_vertex_buffer*)
	{
		return false;
	}
	virtual bool precache_effect(class gv_effect*)
	{
		return false;
	}
	virtual bool precache_shader(class gv_shader*)
	{
		return false;
	}
	virtual bool precache_material(class gv_material*)
	{
		return false;
	}

	virtual bool update_matrix_from_camera(gv_com_camera* camera);

	inline void set_cu_render_pass(gv_int pass)
	{
		m_current_render_pass = pass;
	}
	inline gve_render_pass get_cu_render_pass()
	{
		return m_current_render_pass_e;
	};
	inline bool is_render_pass_enable(gv_int pass)
	{
		return m_render_pass_flag[pass];
	};
	inline void enable_render_pass(gv_int pass, bool enable = true)
	{
		m_render_pass_flag[pass] = enable;
		if (enable)
			m_render_order.add_unique(pass);
		else
			m_render_order.erase_item(pass);
	};
	virtual void disable_all_render_pass(gv_int pass, bool enable = true)
	{
		gvt_zero(m_render_pass_flag);
	}

	inline gv_matrix44& get_view_matrix()
	{
		return this->m_view_matrix;
	};

	inline gv_matrix44& get_projection_matrix()
	{
		return this->m_projection_matrix;
	};

	inline gv_matrix44& get_world_matrix()
	{
		return this->m_world_matrix;
	};

	inline gv_matrix44& get_world_view_project_matrix()
	{
		return this->m_world_view_project_matrix;
	};

	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_view_projection()
	{
		return get_view_matrix() * get_projection_matrix();
	}

	inline gv_matrix44 get_view_projection_inverse()
	{
		return get_view_projection().get_inverse();
	}

	inline gv_matrix44 get_view_projection_inverse_transpose()
	{
		return get_view_projection_inverse().get_transpose();
	}

	inline gv_matrix44 get_view_projection_transpose()
	{
		return get_view_projection().get_transpose();
	}

	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_view()
	{
		return get_view_matrix();
	}

	inline gv_matrix44 get_view_inverse()
	{
		return get_view().get_inverse();
	}

	inline gv_matrix44 get_view_inverse_transpose()
	{
		return get_view_inverse().get_transpose();
	}

	inline gv_matrix44 get_view_transpose()
	{
		return get_view().get_transpose();
	}
	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_world()
	{
		return get_world_matrix();
	}

	inline gv_matrix44 get_world_inverse()
	{
		return get_world().get_inverse();
	}

	inline gv_matrix44 get_world_inverse_transpose()
	{
		return get_world_inverse().get_transpose();
	}

	inline gv_matrix44 get_world_transpose()
	{
		return get_world().get_transpose();
	}
	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_projection()
	{
		return get_projection_matrix();
	}

	inline gv_matrix44 get_projection_inverse()
	{
		return get_projection().get_inverse();
	}

	inline gv_matrix44 get_projection_inverse_transpose()
	{
		return get_projection_inverse().get_transpose();
	}

	inline gv_matrix44 get_projection_transpose()
	{
		return get_projection().get_transpose();
	}

	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_world_view()
	{
		return get_world_matrix() * get_view_matrix();
	}

	inline gv_matrix44 get_world_view_inverse()
	{
		return get_world_view().get_inverse();
	}

	inline gv_matrix44 get_world_view_inverse_transpose()
	{
		return get_world_view_inverse().get_transpose();
	}

	inline gv_matrix44 get_world_view_transpose()
	{
		return get_world_view().get_transpose();
	}

	//============================================================================================
	//								:
	//============================================================================================
	inline gv_matrix44 get_world_view_projection()
	{
		return get_world_view() * get_projection();
	}

	inline gv_matrix44 get_world_view_projection_inverse()
	{
		return get_world_view_projection().get_inverse();
	}

	inline gv_matrix44 get_world_view_projection_inverse_transpose()
	{
		return get_world_view_projection_inverse().get_transpose();
	}

	inline gv_matrix44 get_world_view_projection_transpose()
	{
		return get_world_view_projection().get_transpose();
	}

protected:
	union {
		gv_int m_current_render_pass;
		gve_render_pass m_current_render_pass_e;
	};
	bool m_render_pass_flag[gve_render_pass_max];
	gvt_array< gv_int > m_render_order;
	gv_matrix44 m_projection_matrix;
	gv_matrix44 m_inv_view_matrix;
	gv_matrix44 m_view_matrix;
	gv_matrix44 m_world_matrix;
	gv_matrix44 m_world_view_project_matrix;
};
}