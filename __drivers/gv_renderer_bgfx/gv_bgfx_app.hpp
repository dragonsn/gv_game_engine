
//#include <bx/uint32_t.h>
//this is a fork from bgfx sample ; 
#define BGFX_MAX_GPU_SKIN_BONE_NUMBER 64
namespace bgfx_debug_cube
{
	struct PosColorVertex
	{
		float m_x;
		float m_y;
		float m_z;
		uint32_t m_abgr;

		static void init()
		{
			ms_decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
				.end();
		};

		static bgfx::VertexDecl ms_decl;
	};

	bgfx::VertexDecl PosColorVertex::ms_decl;

	static PosColorVertex s_cubeVertices[] =
	{
		{-1.0f,  1.0f,  1.0f, 0xff000000 },
		{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
		{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
		{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
		{-1.0f,  1.0f, -1.0f, 0xffff0000 },
		{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
		{-1.0f, -1.0f, -1.0f, 0xffffff00 },
		{ 1.0f, -1.0f, -1.0f, 0xffffffff },
	};

	static const uint16_t s_cubeTriList[] =
	{
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	};


	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
	bgfx::ProgramHandle m_program;

	void init()
	{
		// Create vertex stream declaration.
		PosColorVertex::init();

		// Create static vertex buffer.
		m_vbh = bgfx::createVertexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
			, PosColorVertex::ms_decl
		);
		m_ibh = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
		);
		// Create program from shaders.
		m_program = loadProgram("vs_default", "fs_default");
	}

	void destroy()
	{
	}


	void paint()
	{

		bgfx::IndexBufferHandle ibh = m_ibh;
		uint64_t state = 0
			| BGFX_STATE_WRITE_R 
			| BGFX_STATE_WRITE_G 
			| BGFX_STATE_WRITE_B 
			| BGFX_STATE_WRITE_A 
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			;

		// Submit 11x11 cubes.
		for (uint32_t yy = 0; yy < 11; ++yy)
		{
			for (uint32_t xx = 0; xx < 11; ++xx)
			{
				float mtx[16];
				bx::mtxRotateXY(mtx,  xx * 0.21f,  yy * 0.37f);
				mtx[12] = -15.0f + float(xx)*3.0f;
				mtx[13] = -15.0f + float(yy)*3.0f;
				mtx[14] = 0.0f;

				// Set model matrix for rendering.
				bgfx::setTransform(mtx);

				// Set vertex and index buffer.
				bgfx::setVertexBuffer(0, m_vbh);
				bgfx::setIndexBuffer(ibh);

				// Set render states.
				bgfx::setState(state);

				// Submit primitive for rendering to view 0.
				bgfx::submit(0, m_program);
			}
		}
	}

}

gv_bgfx_app::gv_bgfx_app(const char* _name, const char* _description) : entry::AppI(_name, _description)
	
{
}

void gv_bgfx_app::init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) 
{
	Args args(_argc, _argv);

	m_width = _width;
	m_height = _height;
	m_debug = BGFX_DEBUG_TEXT;
	m_reset = BGFX_RESET_VSYNC;

	bgfx::Init init;
	init.type = args.m_type;
	init.vendorId = args.m_pciId;
	init.resolution.width = m_width;
	init.resolution.height = m_height;
	init.resolution.reset = m_reset;
	bgfx::init(init);

	// Enable debug text.
	bgfx::setDebug(m_debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x303030ff
		, 1.0f
		, 0
	);
	imguiCreate();
	m_inited = 1;;
	entry::setCurrentDir(gv_global::framework_config.data_path_root+"/bgfx/");
	bgfx_debug_cube::init(); 
}

 int gv_bgfx_app::shutdown() 
{
	 m_inited = 0;
	 imguiDestroy();
	// Shutdown bgfx.
	bgfx::shutdown();
	return 0;
}


 void gv_bgfx_app::pre_tick()
 {
	 m_start_tick_event.wait();
 };

 void  gv_bgfx_app::set_camera()
 {
	 float at[3] = { 0.0f, 0.0f,   0.0f };
	 float eye[3] = { 0.0f, 0.0f, -35.0f };

	 // Set view and projection matrix for view 0.
	 {
		 float view[16];
		 bx::mtxLookAt(view, eye, at);

		 float proj[16];
		 bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 1000.0f, bgfx::getCaps()->homogeneousDepth);
		 bgfx::setViewTransform(0, view, proj);

		 // Set view 0 default viewport.
		 bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
	 }

	 // This dummy draw call is here to make sure that view 0 is cleared
	 // if no other draw calls are submitted to view 0.
	 bgfx::touch(0);
 }
 void gv_bgfx_app::tick()
 {
	 imguiBeginFrame(m_mouseState.m_mx
		 , m_mouseState.m_my
		 , (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
		 | (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
		 | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
		 , m_mouseState.m_mz
		 , uint16_t(m_width)
		 , uint16_t(m_height)
	 );

	 showExampleDialog(this);

	 imguiEndFrame();

	 // Set view 0 default viewport.
	 bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

	 // This dummy draw call is here to make sure that view 0 is cleared
	 // if no other draw calls are submitted to view 0.
	 bgfx::touch(0);

	 // Use debug font to print information about this example.
	 bgfx::dbgTextClear();
	 bgfx::dbgTextImage(
		 bx::max<uint16_t>(uint16_t(m_width / 2 / 8), 20) - 20
		 , bx::max<uint16_t>(uint16_t(m_height / 2 / 16), 6) - 6
		 , 40
		 , 12
		 , s_logo
		 , 160
	 );
	 bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

	 bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
	 bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

	 const bgfx::Stats* stats = bgfx::getStats();
	 bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
		 , stats->width
		 , stats->height
		 , stats->textWidth
		 , stats->textHeight
	 );

	 set_camera();
	 for (int i = 0; i < this->m_renderables.size(); i++)
	 {
		 this->render_renderable(m_renderables[i]); 
	 }
	 bgfx_debug_cube::paint();
	 bgfx::frame();
 };

 void gv_bgfx_app::post_tick()
 {
	 m_end_tick_event.set();
 };

bool gv_bgfx_app::update() 
{
	
	bool ret = false; 
	pre_tick(); 
	if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState))
	{
		
		//notify gv signal end 
		// Advance to next frame. Rendering thread will be kicked to
		// process submitted rendering primitives.
		tick(); 
		ret=true;
	}
	post_tick();
	return ret;
}

gv::gv_int	gv_bgfx_app::on_event(gv::gv_object_event* pevent)
{
	switch (pevent->m_id)
	{
		case gv_object_event_id_render_init:
		{
			gv_object_event_render_init* pe =gvt_cast<gv_object_event_render_init>(pevent);
			GV_ASSERT(pe);
			//m_window_handle = (void*)pe->window_handle;
		}
		break;
		case gv_object_event_id_render_uninit:
		{
			//this->destroy_gameplay3d();
		}
		break;
		case gv_object_event_id_add_component:
		{
			gv_object_event_add_component* pe =gvt_cast<gv_object_event_add_component>(pevent);
			GV_ASSERT(pe);
			this->add_renderable(gvt_cast<gv_component>(pe->component));
		}
		break;
		case gv_object_event_id_remove_component:
		{
			gv_object_event_remove_component* pe =gvt_cast<gv_object_event_remove_component>(pevent);
			GV_ASSERT(pe);
			this->remove_renderable(pe->component);
		}
		break;

		case gv_object_event_id_render_enable_pass:
		{
			gv_object_event_render_enable_pass* pe =gvt_cast<gv_object_event_render_enable_pass>(pevent);
			GV_ASSERT(pe);
			// todo this->enable_render_pass( pe->pass);
		}
		break;

		case gv_object_event_id_render_disable_pass:
		{
			gv_object_event_render_disable_pass* pe =
				gvt_cast<gv_object_event_render_disable_pass>(pevent);
			GV_ASSERT(pe);
			// todo this->enable_render_pass( pe->pass,false);
		}
		break;

		case gv_object_event_id_render_set_camera:
		{
			gv_object_event_render_set_camera* pe =
				gvt_cast<gv_object_event_render_set_camera>(pevent);
			GV_ASSERT(pe);
			this->m_main_camera = gvt_cast<gv_com_camera>(pe->camera);
			GV_ASSERT(this->m_main_camera);
		}
		break;

		case gv_object_event_id_render_set_ui_manager:
		{
			gv_object_event_render_set_ui_manager* pe =
				gvt_cast<gv_object_event_render_set_ui_manager>(pevent);
			GV_ASSERT(pe);
			// todo this->m_ui_mgr=pe->ui_mgr;
		}
		break;

		case gv_object_event_id_render_reload_shader_cache:
		{
			//this->m_hook->reloadShaderCache();
		}
		break;
	}
	return 1;
};
void gv_bgfx_app::add_renderable(gv_component* in_com){
	//don't cache if not add to any world
	if (!in_com || !in_com->get_entity()->get_world() || m_renderables.find(in_com)) {
		return; 
	}
	m_renderables.add(in_com);
	gv_com_graphic* com = gvt_cast<gv_com_graphic>(in_com);
	if (!com) {
		return;
	}
	//if error in material ,failed.
	//if (!precache(com->get_material())) return; 
	//if (precache(gvt_cast<gv_com_terrain_roam>(in_com))) return; 
	gv_renderer_bgfx::static_get()->precache_material(com->get_material());
	if (precache(gvt_cast<gv_com_static_mesh>(in_com))) return;
	if (precache(gvt_cast<gv_com_skeletal_mesh>(in_com))) return;
}
void  gv_bgfx_app::remove_renderable(gv_component* pcomponent){
	this->m_renderables.remove(pcomponent); 
}

bool gv_bgfx_app::render_renderable(gv_component * pcomponent)
{
	gv_com_static_mesh * pmesh = gvt_cast<gv_com_static_mesh>(pcomponent);

	if (pmesh)
	{
		{//update transform
			//SN_TEMP TEST;
			float mtx[16];
			bx::mtxRotateXY(mtx, (float)gv_global::time->get_sec_from_start() , 0);
			mtx[12] = 0;
			mtx[13] = 0;
			mtx[14] = 150.0f;
			// Set model matrix for rendering.
			bgfx::setTransform(mtx);
		}

		gv_material * mat=pmesh->get_material(); 
		gv_static_mesh* mesh_gv = pmesh->get_resource<gv_static_mesh>();
		if (!mesh_gv) return false;
		mesh_gv->get_vb()->get_hardware_cache<gv_vertex_buffer_bgfx> ()->set();
		mesh_gv->get_ib()->get_hardware_cache<gv_index_buffer_bgfx>()->set();
		//for bgfx ,not support mesh segment,create submeshes!! .....
		//if (pmesh->get_material() )
		{
			set_material(pmesh, mat); 
		//	bgfx::submit(0, mat->get_effect()->get_hardware_cache<gv_effect_bgfx>()->m_bgfx_program);
			bgfx::submit(0,  bgfx_debug_cube:: m_program);
		}
	}
	return true;
};

bool  gv_bgfx_app::precache(gv_com_static_mesh* mesh) {
	gv_renderer_bgfx::static_get()->precache_static_mesh(mesh->m_static_mesh);
	return true;
};
bool  gv_bgfx_app::precache(gv_com_skeletal_mesh* mesh) {
	gv_renderer_bgfx::static_get()->precache_skeletal_mesh(mesh->get_skeletal_mesh());
	return true;
};

bool gv_bgfx_app::set_material(gv_com_graphic* com, gv_material * mat){
	if (mat)
	{
		uint64_t state = 0
			| BGFX_STATE_WRITE_R 
			| BGFX_STATE_WRITE_G 
			| BGFX_STATE_WRITE_B 
			| BGFX_STATE_WRITE_A 
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			//| BGFX_STATE_PT_TRISTRIP
			;
		//set shader parameter here ..
		
	}
	return true;
};

//dummy function , should never be called from entry 
int32_t _main_(int32_t _argc, char** _argv)
{
	GV_ASSERT(0); 
	return 1; 
};

ENTRY_IMPLEMENT_MAIN(gv_bgfx_app, "gv_bgfx", "Initialization and debug text.");