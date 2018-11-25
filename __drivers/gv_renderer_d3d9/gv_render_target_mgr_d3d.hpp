namespace gv
{

struct gv_renderable_tex_d3d_info
{
	gv_renderable_tex_d3d_info()
	{
	}
	gv_renderable_tex_d3d_info(const gv_renderable_tex_d3d_info& info)
	{
		(*this) = info;
	}
	~gv_renderable_tex_d3d_info()
	{
	}
	gv_renderable_tex_d3d_info&
	operator=(const gv_renderable_tex_d3d_info& info)
	{
		texture = info.texture;
		format = info.format;
		clear = info.clear;
		clear_color = info.clear_color;
		size = info.size;
		id = info.id;
		return *this;
	}
	bool operator==(const gv_renderable_tex_d3d_info& info)
	{
		if (clear && info.clear)
		{
			return /*size==info.size && format==info.format &&*/ id == info.id;
		}
		return false;
	}
	gvt_ref_ptr< gv_texture > texture;
	D3DFORMAT format;
	gv_bool clear;
	gv_color clear_color;
	gv_vector2i size;
	gv_id id;
};

class gv_render_target_mgr_d3d_imp : public gv_refable
{
public:
	gv_render_target_mgr_d3d_imp()
	{
		m_is_locked = false;
	};
	~gv_render_target_mgr_d3d_imp()
	{
	}
	void set_viewport(IDirect3DSurface9* surf)
	{
		D3DSURFACE_DESC desc;
		surf->GetDesc(&desc);
		D3DVIEWPORT9 port;
		port.X = 0;
		port.Y = 0;
		port.Width = (DWORD)desc.Width;
		port.Height = (DWORD)desc.Height;
		port.MinZ = 0;
		port.MaxZ = 1;
		get_device_d3d9()->SetViewport(&port);
	}
	gv_texture* create_texture(const gv_id& id, gv_uint usage, gv_vector2i size,
							   gv_uint format, gv_bool clear)
	{
		gv_renderable_tex_d3d_info info;
		info.clear = clear;
		info.format = (D3DFORMAT)format;
		info.size = size;
		info.id = id;
		gv_int idx = 0;
		if (m_render_targets.find(info, idx))
		{
			return m_render_targets[idx].texture;
		}
		gv_texture_d3d* ptex = get_renderer_d3d9()
								   ->get_sandbox()
								   ->create_nameless_object< gv_texture_d3d >();
		IDirect3DTexture9* ptex9;
		GVM_VERIFY_D3D(get_device_d3d9()->CreateTexture(
			size.get_x(), size.get_y(), 1, usage, (D3DFORMAT)format,
			D3DPOOL_DEFAULT, &ptex9, 0));
		ptex->set_texture_d3d(ptex9);
		gv_texture* pt = get_renderer_d3d9()
							 ->get_sandbox()
							 ->create_nameless_object< gv_texture >();
		pt->set_hardware_cache(ptex);
		info.texture = pt;
		m_render_targets.push_back(info);
		GVM_DEBUG_LOG(render, "create render target: " << id << " size :" << size
													   << " format " << format
													   << gv_endl);
		return pt;
	}
	gvt_array< gv_renderable_tex_d3d_info > m_render_targets;
	IDirect3DSurface9* m_default_color_buffer;
	IDirect3DSurface9* m_default_depth_buffer;
	IDirect3DSurface9* m_current_color_buffer;
	IDirect3DSurface9* m_current_depth_buffer;
	gv_vector2i m_default_color_buffer_size;
	gv_vector2i m_default_depth_buffer_size;
	gv_bool m_is_locked;
};
//============================================================================================
//								:
//============================================================================================
gv_render_target_mgr_d3d::gv_render_target_mgr_d3d()
{
	m_impl = new gv_render_target_mgr_d3d_imp;
};

gv_render_target_mgr_d3d::~gv_render_target_mgr_d3d(){

};

void gv_render_target_mgr_d3d::on_init()
{
	on_device_reset();
};

void gv_render_target_mgr_d3d::on_destroy(){

};
void gv_render_target_mgr_d3d::on_device_lost(){

};
void gv_render_target_mgr_d3d::on_device_reset()
{
	get_device_d3d9()->GetRenderTarget(0, &m_impl->m_default_color_buffer);
	get_device_d3d9()->GetDepthStencilSurface(&m_impl->m_default_depth_buffer);
	m_impl->m_current_color_buffer = m_impl->m_default_color_buffer;
	m_impl->m_current_depth_buffer = m_impl->m_default_depth_buffer;
	D3DSURFACE_DESC desc;
	m_impl->m_default_color_buffer->GetDesc(&desc);
	m_impl->m_default_color_buffer_size.set(desc.Width, desc.Height);
	m_impl->m_default_depth_buffer->GetDesc(&desc);
	m_impl->m_default_depth_buffer_size.set(desc.Width, desc.Height);
};

gv_vector2i gv_render_target_mgr_d3d::get_default_color_buffer_size()
{
	return m_impl->m_default_color_buffer_size;
};
gv_vector2i gv_render_target_mgr_d3d::get_default_depth_buffer_size()
{
	return m_impl->m_default_depth_buffer_size;
};

gv_texture* gv_render_target_mgr_d3d::create_color_texture(
	const gv_id& id, gv_vector2i size, gve_pixel_format format, gv_bool clear)
{
	return m_impl->create_texture(id, D3DUSAGE_RENDERTARGET, size,
								  gv_to_d3d_format(format), clear);
};

gv_texture* gv_render_target_mgr_d3d::create_depth_texture(
	const gv_id& id, gv_vector2i size, gve_pixel_format format, gv_bool clear)
{
	return m_impl->create_texture(id, D3DUSAGE_DEPTHSTENCIL, size,
								  gv_to_d3d_format(format), clear);
};

void gv_render_target_mgr_d3d::begin_render_target(gv_texture* color_texture,
												   gv_texture* depth_texture,
												   bool clear,
												   gv_color clear_color)
{
	if (m_impl->m_is_locked)
		return;
//  [9/1/2011 Administrator]
#pragma GV_REMINDER( \
	"when use PIX must return in this function , ... must be  PIX bug!!!")
	// return;
	GV_PROFILE_EVENT_PIX(set_render_target, 0)
	IDirect3DSurface9* color_surface = m_impl->m_default_color_buffer;
	IDirect3DSurface9* depth_surface = NULL;
	m_impl->m_current_depth_buffer;
	;
	for (int stage = 0; stage < DX9_MAX_TEXTURE; ++stage)
	{
		get_device_d3d9()->SetTexture(stage, NULL);
	}
	if (color_texture)
		color_surface =
			color_texture->get_hardware_cache< gv_texture_d3d >()->get_surface_d3d();
	if (depth_texture)
		depth_surface =
			depth_texture->get_hardware_cache< gv_texture_d3d >()->get_surface_d3d();
	if (color_surface == m_impl->m_current_color_buffer &&
		depth_surface == m_impl->m_current_depth_buffer)
	{
		return;
	}
	get_device_d3d9()->SetRenderTarget(0, color_surface);
	if (depth_surface)
	{
		get_device_d3d9()->SetDepthStencilSurface(depth_surface);
	}
	else
	{
		get_device_d3d9()->SetDepthStencilSurface(NULL);
	}
	if (clear)
	{
		gv_uint clear_flag = D3DCLEAR_TARGET;
		if (depth_texture)
			clear_flag |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
		get_device_d3d9()->Clear(0, NULL, clear_flag, clear_color.BGRA().fixed32,
								 1.0f, 0);
	}
	m_impl->set_viewport(color_surface);
	m_impl->m_current_color_buffer = color_surface;
	m_impl->m_current_depth_buffer = depth_surface;
};

void gv_render_target_mgr_d3d::set_default_render_target()
{
	if (m_impl->m_is_locked)
		return;
	if (m_impl->m_current_color_buffer == m_impl->m_default_color_buffer &&
		m_impl->m_current_depth_buffer == m_impl->m_default_depth_buffer)
	{
		return;
	}
	{
		GV_PROFILE_EVENT_PIX(set_default_render_target, 0)
		get_device_d3d9()->SetRenderTarget(0, m_impl->m_default_color_buffer);
		get_device_d3d9()->SetDepthStencilSurface(m_impl->m_default_depth_buffer);
		m_impl->set_viewport(m_impl->m_default_color_buffer);
		m_impl->m_current_color_buffer = m_impl->m_default_color_buffer;
		m_impl->m_current_depth_buffer = m_impl->m_default_depth_buffer;
	}
};

void gv_render_target_mgr_d3d::end_render_target(){

};

void gv_render_target_mgr_d3d::resolve_backbuffer(gv_texture* tex)
{

	IDirect3DSurface9 *pBackBuffer, *pDestBuffer;

	get_device_d3d9()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	pDestBuffer = tex->get_hardware_cache< gv_texture_d3d >()->get_surface_d3d();

	RECT rect;
	D3DSURFACE_DESC desc;
	pDestBuffer->GetDesc(&desc);

	rect.left = rect.top = 0;
	rect.right = desc.Width;
	rect.bottom = desc.Height;
	GVM_VERIFY_D3D(get_device_d3d9()->StretchRect(pBackBuffer, &rect, pDestBuffer,
												  NULL, D3DTEXF_NONE));
	// pBackBuffer->Release();
	// pDestBuffer->Release();
};

void gv_render_target_mgr_d3d::precache(gv_effect_renderable_texture* texture)
{
	if (texture->m_texture)
		return;
	// if (texture->m_name =="g_color_buffer" ) return;
	// if (texture->m_name =="g_depth_buffer" ) return;
	// if (texture->m_name =="g_shadow_map"  ) return;

	gv_vector2i size = texture->m_size;
	if (texture->m_use_window_size)
	{
		size = get_renderer_d3d9()->get_screen_size();
		size.x = (int)(size.x * texture->m_width_ratio);
		size.y = (int)(size.y * texture->m_height_ratio);
		if (size.get_x() * size.get_y() < 1)
		{
			size = get_renderer_d3d9()->get_screen_size();
		}
	}
	texture->m_texture = m_impl->create_texture(
		texture->m_name, D3DUSAGE_RENDERTARGET, size, texture->m_format, true);
};

void gv_render_target_mgr_d3d::lock_render_target(gv_bool lock)
{
	m_impl->m_is_locked = lock;
}
}