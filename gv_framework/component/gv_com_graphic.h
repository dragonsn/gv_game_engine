#pragma once
namespace gv
{
class gv_material;
class gv_component_renderer;
//============================================================================================
//								:
//============================================================================================
class gv_com_graphic : public gv_component
{
	friend class gv_component_renderer;

public:
	GVM_DCL_CLASS(gv_com_graphic, gv_component)
	gv_com_graphic();
	~gv_com_graphic();
	virtual bool is_renderable()
	{
		return true;
	}
	virtual gv_box get_local_bbox();
	virtual gv_sphere get_local_bsphere();
	virtual bool set_resource(gv_resource* resource)
	{
		this->m_main_resource = resource;
		return true;
	}
	virtual gv_resource* get_resource()
	{
		return m_main_resource;
	}
	template < class T >
	inline T* get_resource()
	{
		return gvt_cast< T >(m_main_resource);
	}
	virtual gv_component_renderer* get_renderer(gv_int pass)
	{
		return m_renderers[pass];
	}
	virtual void set_renderer(gv_int pass, gv_component_renderer* renderer)
	{
		m_renderers[pass] = renderer;
	}
	virtual const gv_id& get_renderer_id(gv_int pass)
	{
		return m_com_render_id[pass];
	}
	virtual void set_renderer_id(gv_int pass, const gv_id& id)
	{
		m_com_render_id[pass] = id;
	}
	virtual gv_component* clone(gv_sandbox* sandbox);
	virtual class gv_static_mesh* get_static_mesh()
	{
		return NULL;
	};
	virtual gv_material* get_material();
	virtual void create_sub_material_from_resource();
	virtual gv_material* get_sub_material(int idx);
	virtual void set_material(gv_material* material);
	virtual void on_visible(class gv_com_camera*, gv_ulong visible_test_tag){};
	virtual void switch_wireframe(){};
	inline gv_bool is_always_visible()
	{
		return m_always_visible;
	}
	inline void set_always_visible(gv_bool b)
	{
		m_always_visible = b;
	}
	inline bool is_visible_last_frame() const
	{
		return m_is_visible;
	}
	inline void set_visible(gv_bool b)
	{
		m_is_visible = b;
	}
	void hide_segment(gv_int seg);
	void unhide_segment(gv_int seg);
	bool is_segment_hided(gv_int seg);
	void unhide_all_segment();
	void hide_all_segment();
	inline gv_bool has_visible_callback()
	{
		return m_has_visible_callback;
	}
	inline gv_bool test_visible_test_tag(gv_ulong test_tag)
	{
		return m_visible_tag_enabled && (m_visible_test_tag == test_tag);
	}
	inline void set_visible_test_tag(gv_ulong test_tag)
	{
		m_visible_test_tag = test_tag;
	}
	inline void enable_visible_test_tag(gv_bool enable = true)
	{
		m_visible_tag_enabled = enable;
	}
	inline gv_bool is_visible_test_tag_enabled()
	{
		return m_visible_tag_enabled;
	}
	inline gv_bool is_component_tm_enabled()
	{
		return m_is_component_tm_enabled;
	}
	inline void set_component_tm_enabled(gv_bool enable = true)
	{
		m_is_component_tm_enabled = enable;
	}
	inline gv_obj_node& get_visible_node()
	{
		return m_visible_node;
	}
	inline gv_bool is_background()
	{
		return m_is_background;
	}
	inline void set_background(gv_bool b)
	{
		m_is_background = b;
	};
	inline gv_bool is_particle()
	{
		return m_is_particle;
	}
	inline void set_is_particle(gv_bool b)
	{
		m_is_particle = b;
	};
	inline const gv_matrix44& get_component_tm()
	{
		return m_component_matrix;
	}
	inline void set_component_tm(const gv_matrix44& m)
	{
		m_component_matrix = m;
	}
	inline gv_bool is_post_effect() const
	{
		return m_is_post_effect;
	}
	inline gv_uint get_render_priority() const
	{
		return m_render_priority;
	}
	inline void set_render_priority(gv_uint o)
	{
		m_render_priority = o;
	}
	inline gv_bool is_in_z_pass()
	{
		return m_is_in_z_pass;
	}
	inline void set_is_in_z_pass(gv_bool b)
	{
		m_is_in_z_pass = b;
	}

public:
	GVM_DCL_FUNC(scp_switch_wireframe);

protected:
	gvt_array_static< gv_id, gve_render_pass_max > m_com_render_id;
	gvt_array_static< gvt_ptr< gv_component_renderer >, gve_render_pass_max >
		m_renderers;
	gvt_ref_ptr< gv_resource > m_main_resource;
	gvt_ref_ptr< gv_material > m_material;
	gv_bool m_has_visible_callback;
	gv_bool m_always_visible;
	gv_uint m_render_priority;
	gv_bool m_is_post_effect;
	gv_bool m_is_visible;
	gv_bool m_is_background;
	gv_bool m_is_component_tm_enabled;
	gv_bool m_is_particle;
	gv_obj_node m_visible_node;
	gv_ulong m_visible_test_tag;
	gv_bool m_visible_tag_enabled;
	gv_bool m_is_in_z_pass;
	gv_matrix44 m_component_matrix;
	union {
		gv_uint m_segment_hide_mask;
		struct
		{
			gv_uint m_hide_seg_00 : 1;
			gv_uint m_hide_seg_01 : 1;
			gv_uint m_hide_seg_02 : 1;
			gv_uint m_hide_seg_03 : 1;
			gv_uint m_hide_seg_04 : 1;
			gv_uint m_hide_seg_05 : 1;
			gv_uint m_hide_seg_06 : 1;
			gv_uint m_hide_seg_07 : 1;
			gv_uint m_hide_seg_08 : 1;
			gv_uint m_hide_seg_09 : 1;
			gv_uint m_hide_seg_10 : 1;
			gv_uint m_hide_seg_11 : 1;
			gv_uint m_hide_seg_12 : 1;
			gv_uint m_hide_seg_13 : 1;
			gv_uint m_hide_seg_14 : 1;
			gv_uint m_hide_seg_15 : 1;
			gv_uint m_hide_seg_16 : 1;
			gv_uint m_hide_seg_17 : 1;
			gv_uint m_hide_seg_18 : 1;
			gv_uint m_hide_seg_19 : 1;
			gv_uint m_hide_seg_20 : 1;
			gv_uint m_hide_seg_21 : 1;
			gv_uint m_hide_seg_22 : 1;
			gv_uint m_hide_seg_23 : 1;
			gv_uint m_hide_seg_24 : 1;
			gv_uint m_hide_seg_25 : 1;
			gv_uint m_hide_seg_26 : 1;
			gv_uint m_hide_seg_27 : 1;
			gv_uint m_hide_seg_28 : 1;
			gv_uint m_hide_seg_29 : 1;
			gv_uint m_hide_seg_30 : 1;
			gv_uint m_hide_seg_31 : 1;
		};
	};
};
}