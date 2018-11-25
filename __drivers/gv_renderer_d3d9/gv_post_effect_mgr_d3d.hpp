namespace gv
{

//=========================================================================================
typedef gvt_dlist< gvt_ref_ptr< gv_com_graphic > > post_effect_list;

class gv_post_effect_mgr_d3d_imp : public gv_refable
{
	friend class gv_post_effect_mgr_d3d;

public:
	gv_post_effect_mgr_d3d_imp()
	{
	}
	post_effect_list effects;
	gvt_ref_ptr< gv_component_renderer > renderer;
};
//===========================================================================================
gv_post_effect_mgr_d3d::gv_post_effect_mgr_d3d()
{
	m_impl = new gv_post_effect_mgr_d3d_imp;
};
gv_post_effect_mgr_d3d::~gv_post_effect_mgr_d3d(){

};
//===========================================================================================
void gv_post_effect_mgr_d3d::on_init()
{
	m_impl->renderer =
		get_renderer_d3d9()->get_com_renderer(gv_id("gv_com_effect_renderer"));
};

void gv_post_effect_mgr_d3d::on_destroy(){

};
void gv_post_effect_mgr_d3d::on_device_lost(){

};
void gv_post_effect_mgr_d3d::on_device_reset(){

};
void gv_post_effect_mgr_d3d::render(gv_float dt, gv_recti* rect)
{
	GV_PROFILE_EVENT_PIX(render_post_effect, 0)

	post_effect_list::iterator it = m_impl->effects.begin();
	// always assume shadow and other static parameter to be set here
	// get_renderer_d3d9()->set_static_shader_param_for_post_process();
	gv_uint last_render_priority = gvc_render_priority_post_process;
	while (it != m_impl->effects.end())
	{
		{
		GV_PROFILE_EVENT_PIX(render_one_post_effect, 0)
		gv_com_graphic* ptr = (*it);
		bool is_visible = false;
		if (ptr->is_deleted())
		{
			it++;
			continue;
		}
		if (ptr->get_entity()->is_hided())
		{
			it++;
			continue;
		}
		const gv_world* pworld = ptr->get_entity()->get_world();
		if (!pworld)
		{
			it++;
			continue;
		}
		if (pworld && pworld->is_hided())
		{
			it++;
			continue;
		}
		get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		get_device_d3d9()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		get_device_d3d9()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_impl->renderer->render_component(ptr);
		it++;
		}
		if (it != m_impl->effects.end())
		{ 
			// 
			GV_PROFILE_EVENT_PIX(render_objects_between_post_effect, 0)
			//====================================================================================
			get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			get_device_d3d9()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			get_device_d3d9()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			get_device_d3d9()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			{
				// render priority group
				gv_com_graphic* ptr = (*it);
				get_renderer_d3d9()->get_batch_renderer()->render_priority_group(
					last_render_priority,
					ptr->get_render_priority() + gvc_render_priority_post_process);
				last_render_priority =
					ptr->get_render_priority() + gvc_render_priority_post_process;
			}
			get_renderer_d3d9()->get_render_target_mgr()->resolve_backbuffer(
				get_renderer_d3d9()->get_color_buffer_tex());
		}
	}
	get_device_d3d9()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	get_device_d3d9()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	get_device_d3d9()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	get_renderer_d3d9()->get_batch_renderer()->render_priority_group(
		last_render_priority, gvc_render_priority_last);
};
void gv_post_effect_mgr_d3d::add_post_effect(gv_com_graphic* effect)
{
	post_effect_list::iterator it = m_impl->effects.begin();
	while (it != m_impl->effects.end())
	{
		gv_com_graphic* pcom = (*it);
		if (pcom->get_render_priority() > effect->get_render_priority())
		{
			m_impl->effects.insert(it, effect);
			return;
		}
		it++;
	}
	m_impl->effects.push_back(effect);
	return;
};
void gv_post_effect_mgr_d3d::remove_post_effect(gv_com_graphic* effect)
{
	m_impl->effects.erase(effect);
	return;
};

void gv_post_effect_mgr_d3d::enable_post_effect(const gv_id& effect_name,
												gv_bool e){

};
void gv_post_effect_mgr_d3d::set_post_effect_param(const gv_id& effect_name,
												   const gv_id& id,
												   const gv_vector4& param){

};

//===========================================================================================
}