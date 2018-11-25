#include "gv_renderer_d3d_private.h"
#include "gv_debug_renderer_d3d.h"
#include "gv_renderer_d3d9.h"
#include "gv_render_device_d3d9.h"

#if GV_WITH_IMGUI

#pragma include_alias( "imgui.h", "bgfx/3rdparty/dear-imgui/imgui.h" )  
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_win32.cpp"
#include "imgui_impl_dx9.cpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace gv
{
static bool g_init_from_existed_window = false;
class gv_debug_renderer_d3d_imp
{
	friend class gv_debug_renderer_d3d;

protected:
	struct vertex
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;		// The vertex color
	};

	struct vertex_3d
	{
		FLOAT x, y, z; // The untransformed position for the vertex
		DWORD color;   // The vertex color
	};

	struct vertex_tex
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;		// The vertex color
		FLOAT u, v;
	};
	static const gv_int gvc_string_info_length = 512;
	struct string_info
	{
		char string[512];
		gv_color color;
		gv_vector2i pos;
		string_info()
		{
		}
		string_info(const string_info& info)
		{
			(*this) = info;
		}
		string_info& operator=(const string_info& s)
		{
			strncpy(string, s.string, gvc_string_info_length - 1);
			pos = s.pos;
			color = s.color;
			return *this;
		}
	};

	struct debug_texture_state
	{
		debug_texture_state()
		{
			GVM_ZERO_ME;
		}
		debug_texture_state(const debug_texture_state& s)
		{
			*this = s;
		}
		bool operator==(const debug_texture_state& s)
		{
			return texture == s.texture && use_alpha_blending == s.use_alpha_blending;
		}
		debug_texture_state& operator=(const debug_texture_state& s)
		{
			texture = s.texture;
			use_alpha_blending = s.use_alpha_blending;
			return *this;
		}

		gvt_ref_ptr< gv_texture > texture;
		gv_bool use_alpha_blending;

		void set()
		{
			if (texture && texture->get_hardware_cache< gv_texture_d3d >())
			{
				texture->get_hardware_cache< gv_texture_d3d >()->set(0);
			}
			else
			{
				if (get_renderer_d3d9())
					get_renderer_d3d9()
						->get_default_texture()
						->get_hardware_cache< gv_texture_d3d >()
						->set(0);
			}
			get_device_d3d9()->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,
											  D3DMCS_COLOR1);
			get_device_d3d9()->SetRenderState(D3DRS_AMBIENT,
											  D3DCOLOR_XRGB(255, 255, 255));
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP,
													D3DTOP_SELECTARG2);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1,
													D3DTA_DIFFUSE);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG2,
													D3DTA_TEXTURE);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAOP,
													D3DTOP_SELECTARG1);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAARG1,
													D3DTA_TEXTURE);
			get_device_d3d9()->SetTextureStageState(1, D3DTSS_COLOROP,
													D3DTOP_DISABLE);
		}
	};

#undef D3DFVF_CUSTOMVERTEX
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
protected:
	static const int c_nb_vb_buffer = 2;

	gv_vector2i m_window_size;
	gv_vector2i m_view_pos;
	gv_vector2i m_view_size;
	HWND m_hwnd;
	gv_double m_last_tick_time;
	gv_double m_delta_time;
	LPDIRECT3D9 m_pD3D;				// Used to create the D3DDevice
	LPDIRECT3DDEVICE9 m_pd3dDevice; // Our rendering device
	ID3DXFont* m_pFont;
	//==================================
	int m_max_tri_vertex;
	int m_max_line_vertex;
	int m_current_vb;
	int last_vb()
	{
		return !m_current_vb;
	}

	gvt_array< string_info > m_strings_to_render[2];
	gvt_array< string_info >& cu_strings_to_render()
	{
		return m_strings_to_render[m_current_vb];
	}
	gvt_array< string_info >& last_strings_to_render()
	{
		return m_strings_to_render[!m_current_vb];
	}

	gvt_array< vertex > m_vb_triangle[2];
	gvt_array< vertex >& cu_vb_triangle()
	{
		return m_vb_triangle[m_current_vb];
	}
	gvt_array< vertex >& last_vb_triangle()
	{
		return m_vb_triangle[!m_current_vb];
	}

	gvt_array< vertex > m_vb_line[2];
	gvt_array< vertex >& cu_vb_line()
	{
		return m_vb_line[m_current_vb];
	}
	gvt_array< vertex >& last_vb_line()
	{
		return m_vb_line[!m_current_vb];
	}

	gvt_array< vertex_3d > m_vb_line_3d[2];
	gvt_array< vertex_3d >& cu_vb_line_3d()
	{
		return m_vb_line_3d[m_current_vb];
	}
	gvt_array< vertex_3d >& last_vb_line_3d()
	{
		return m_vb_line_3d[!m_current_vb];
	}

	gvt_array< vertex_3d > m_vb_triangle_3d[2];
	gvt_array< vertex_3d >& cu_vb_triangle_3d()
	{
		return m_vb_triangle_3d[m_current_vb];
	}
	gvt_array< vertex_3d >& last_vb_triangle_3d()
	{
		return m_vb_triangle_3d[!m_current_vb];
	}

	LPDIRECT3DVERTEXBUFFER9 m_p_vbs_tri_d3d[c_nb_vb_buffer];
	LPDIRECT3DVERTEXBUFFER9 m_p_vbs_line_d3d[c_nb_vb_buffer];
	LPDIRECT3DVERTEXBUFFER9 m_p_vbs_line_3d_d3d[c_nb_vb_buffer];

	typedef gvt_dynamic_vb_d3d_mt< vertex_tex,
								   D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE >
		vb_tex;
	typedef gvt_dynamic_renderer_d3d< debug_texture_state, vb_tex,
									  gv_draw_policy_tri_list_d3d >
		debug_tex_renderer;
	debug_tex_renderer tex_renderer;

	//=======================================

public:
	gv_debug_renderer_d3d_imp()
		: m_window_size(0, 0), m_view_pos(0, 0), m_view_size(0, 0), m_hwnd(0),
		  m_last_tick_time(0), m_delta_time(0), m_pD3D(0), m_pd3dDevice(0),
		  m_pFont(0), m_max_tri_vertex(0), m_max_line_vertex(0), m_current_vb(0)
	{
		for (int buf = 0; buf < c_nb_vb_buffer; buf++)
		{
			this->m_p_vbs_tri_d3d[buf] = NULL;
			this->m_p_vbs_line_d3d[buf] = NULL;
			this->m_p_vbs_line_3d_d3d[buf] = NULL;
			this->m_p_vbs_tri_d3d[buf] = NULL;
			this->m_p_vbs_line_d3d[buf] = NULL;
			this->m_p_vbs_line_3d_d3d[buf] = NULL;
		}
	}
	~gv_debug_renderer_d3d_imp()
	{
		uninit_d3d();
	}
	void uninit_d3d()
	{
		// We might be re-creating the font and device, so release them if so.
		if (m_pFont != NULL)
		{
			m_pFont->Release();
			m_pFont = NULL;
		}
		for (int buf = 0; buf < c_nb_vb_buffer; buf++)
		{
			if (this->m_p_vbs_tri_d3d[buf])
			{
				(this->m_p_vbs_tri_d3d[buf])->Release();
				this->m_p_vbs_tri_d3d[buf] = NULL;
			}
			if (this->m_p_vbs_line_d3d[buf])
			{
				(this->m_p_vbs_line_d3d[buf])->Release();
				this->m_p_vbs_line_d3d[buf] = NULL;
			}
			if (this->m_p_vbs_line_3d_d3d[buf])
			{
				(this->m_p_vbs_line_3d_d3d[buf])->Release();
				this->m_p_vbs_line_3d_d3d[buf] = NULL;
			}
		}
		tex_renderer.on_device_lost();
		// get_device_d3d9()->UnInitialize();
		m_pd3dDevice = NULL;
		m_pD3D = NULL;
	}
	void init_vb(const int max_triangle, const int max_line)
	{
		GV_ASSERT(m_pd3dDevice);
		m_max_tri_vertex = max_triangle * 3;
		m_max_line_vertex = max_line * 2;
		m_current_vb = 0;
		m_vb_triangle[0].reserve(m_max_tri_vertex);
		m_vb_line[0].reserve(m_max_line_vertex);
		m_vb_line_3d[0].reserve(m_max_line_vertex);

		m_vb_triangle[1].reserve(m_max_tri_vertex);
		m_vb_line[1].reserve(m_max_line_vertex);
		m_vb_line_3d[1].reserve(m_max_line_vertex);
		for (int buf = 0; buf < c_nb_vb_buffer; buf++)
		{
			get_device_d3d9()->CreateVertexBuffer(
				(m_max_tri_vertex + 1) * sizeof(vertex), D3DUSAGE_WRITEONLY,
				D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &this->m_p_vbs_tri_d3d[buf],
				NULL);
			get_device_d3d9()->CreateVertexBuffer(
				4 * (m_max_line_vertex + 1) * sizeof(vertex), D3DUSAGE_WRITEONLY,
				D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &this->m_p_vbs_line_d3d[buf],
				NULL);

			get_device_d3d9()->CreateVertexBuffer(
				4 * (m_max_line_vertex + 1) * sizeof(vertex_3d), D3DUSAGE_WRITEONLY,
				D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT,
				&this->m_p_vbs_line_3d_d3d[buf], NULL);
		}
		tex_renderer.init_vb(m_max_tri_vertex);
	}
	void fill_vb()
	{
		GV_PROFILE_EVENT(fill_vb, 0);
		int current = m_current_vb;

		VOID* pVertices;

		if (last_vb_triangle().size())
		{
			GVM_VERIFY_D3D(m_p_vbs_tri_d3d[current]->Lock(
				0, last_vb_triangle().size() * sizeof(vertex), (void**)&pVertices,
				0));
			memcpy(pVertices, last_vb_triangle().begin(),
				   last_vb_triangle().size() * sizeof(vertex));
			m_p_vbs_tri_d3d[current]->Unlock();
		}

		if (last_vb_line().size())
		{
			GVM_VERIFY_D3D(this->m_p_vbs_line_d3d[current]->Lock(
				0, last_vb_line().size() * sizeof(vertex), (void**)&pVertices, 0));
			memcpy(pVertices, last_vb_line().begin(),
				   last_vb_line().size() * sizeof(vertex));
			m_p_vbs_line_d3d[current]->Unlock();
		}

		if (last_vb_line_3d().size())
		{
			GVM_VERIFY_D3D(this->m_p_vbs_line_3d_d3d[current]->Lock(
				0, last_vb_line_3d().size() * sizeof(vertex_3d), (void**)&pVertices,
				0));
			memcpy(pVertices, last_vb_line_3d().begin(),
				   last_vb_line_3d().size() * sizeof(vertex_3d));
			m_p_vbs_line_3d_d3d[current]->Unlock();
		}
	}

	static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam,
								  LPARAM lParam)
	{
		GV_PROFILE_EVENT(MsgProc, 0);
#if GV_WITH_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
#endif
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SETFOCUS:
			break;

		case WM_KILLFOCUS:
			break;

		case WM_SIZE:
			break;

		case WM_RBUTTONUP:
		{
			gv_global::input->set_key_down(e_key_rbutton, false);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			gv_global::input->set_key_down(e_key_rbutton, true);
			break;
		}

		case WM_LBUTTONUP:
		{
			gv_global::input->set_key_down(e_key_lbutton, false);
			gv_global::debug_draw->release_mouse();
			break;
		}

		case WM_LBUTTONDOWN:
		{
			gv_global::input->set_key_down(e_key_lbutton, true);
			gv_global::debug_draw->capture_mouse();
		}

		case WM_MOUSEMOVE:
		{
			gv_vector2i pos;
			pos.x = LOWORD(lParam); // X position of cursor
			pos.y = HIWORD(lParam); // Y position of cursor
			gv_global::input->set_mouse_pos(pos);
			break;
		}

		case WM_KEYDOWN:
		{
			int key = (int)wParam;
			gv_global::input->set_key_down(gv_global::input->map_key(key), true);
		}
		break;

		case WM_KEYUP:
		{
			int key = (int)wParam;
			gv_global::input->set_key_down(gv_global::input->map_key(key), false);
		}

		break;

		case WM_MOUSEWHEEL:
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			gv_global::input->set_mouse_wheel_delta(zDelta);
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	HRESULT init_d3d(HWND hWnd)
	{
		GV_PROFILE_EVENT(init_d3d, 0);
		if (NULL == m_pD3D)
		{
			// Create the D3D object.
			if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
				return E_FAIL;
		}

		// We might be re-creating the font and device, so release them if so.
		if (m_pFont != NULL)
		{
			m_pFont->Release();
			m_pFont = NULL;
		}
		if (m_pd3dDevice != NULL)
		{
			get_device_d3d9()->Release();
			m_pd3dDevice = NULL;
		}

		// Set up the structure used to create the D3DDevice
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));

		//d3dpp.BackBufferWidth = m_view_size.x;
		//d3dpp.BackBufferHeight = m_view_size.y;
		

		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		switch (gv_global::rnd_opt.m_msaa_times)
		{
		case 2:
			d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			break;
		case 4:
			d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
			break;
		case 8:
			d3dpp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
			break;
		default:
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		}
		d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		d3dpp.PresentationInterval =
			D3DPRESENT_INTERVAL_IMMEDIATE; // D3DPRESENT_INTERVAL_ONE;
		// d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; //VSYNC
		// d3dpp.hDeviceWindow = GetShellWindow();

		/*(
    D3DPRESENT_PARAMETERS pp;
    pp.BackBufferWidth = 320;
    pp.BackBufferHeight = 240;
    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.BackBufferCount = 1;
    pp.MultiSampleType = D3DMULTISAMPLE_NONE;
    pp.MultiSampleQuality = 0;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = GetShellWindow();
    pp.Windowed = true;
    pp.Flags = 0;
    pp.FullScreen_RefreshRateInHz = 0;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    pp.EnableAutoDepthStencil = false;
    */

		// Create the D3DDevice
		/*
    DX9_CALL_ERR(m_Direct3D->CreateDevice(remappedAdapter, devType,
    windowHandle,	D3DCREATE_FPU_PRESERVE |
    D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_DevicePresentParams, &dev),
            "Could	not	create D3D Device");
            */
		DWORD BehaviorFlags = D3DCREATE_FPU_PRESERVE |
							  D3DCREATE_HARDWARE_VERTEXPROCESSING |
							  D3DCREATE_MULTITHREADED;
#if DEBUG_VS
		BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
#endif

		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
										BehaviorFlags, &d3dpp, &m_pd3dDevice)))
		{
			return E_FAIL;
		}
		get_device_d3d9()->Initialize(m_pd3dDevice, m_pD3D);

		D3DVIEWPORT9 port;
		port.X = 0;
		port.Y = 0;
		port.Width = (DWORD)m_view_size.x;
		port.Height = (DWORD)m_view_size.y;
		port.MinZ = 0;
		port.MaxZ = 1;
		GVM_VERIFY_D3D(get_device_d3d9()->SetViewport(&port));

		if (FAILED(D3DXCreateFontA(m_pd3dDevice, 15, 0, FW_NORMAL, 1, FALSE,
								   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
								   (LPCSTR) "Arial", &m_pFont)))
		{
			return E_FAIL;
		}

#if GV_WITH_IMGUI
		// Setup Dear ImGui binding
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX9_Init(m_pd3dDevice);
		// Setup style
		ImGui::StyleColorsDark();
		ImGui_ImplDX9_CreateDeviceObjects();
		//imguiCreate();
		//ImGui::StyleColorsClassic();
#endif

		return S_OK;
	}

	void do_render()
	{
		if (gv_global::rnd_opt.m_no_debug_draw)
			return;
		GV_PROFILE_EVENT_PIX(debug_draw_do_render, 0);
		this->fill_vb();
		get_device_d3d9()->SetVertexShader(0);
		get_device_d3d9()->SetPixelShader(0);
		get_device_d3d9()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		// g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		// g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, g_nTris/3 );
		if (last_vb_triangle().size())
		{
			GV_PROFILE_EVENT_PIX(debug_draw_triangle, 0);
			static int batch_size = 20000; // number of triangles to send at once
			int tris_left = last_vb_triangle().size() / 3;
			get_device_d3d9()->SetStreamSource(0, m_p_vbs_tri_d3d[m_current_vb], 0,
											   sizeof(vertex));
			get_device_d3d9()->SetFVF(D3DFVF_CUSTOMVERTEX);
			int first_prim = 0;
			while (tris_left > 0)
			{
				int chunk;
				if (tris_left <= batch_size)
					chunk = tris_left;
				else
					chunk = batch_size;
				// Render the triangle list vertex buffer contents
				get_device_d3d9()->DrawPrimitive(D3DPT_TRIANGLELIST, first_prim * 3,
												 chunk);
				first_prim += chunk;
				tris_left -= chunk;
			}
		}
		// And the lines
		if (last_vb_line().size())
		{
			GV_PROFILE_EVENT_PIX(debug_draw_line, 0);
			get_device_d3d9()->SetStreamSource(0, m_p_vbs_line_d3d[m_current_vb], 0,
											   sizeof(vertex));
			get_device_d3d9()->SetFVF(D3DFVF_CUSTOMVERTEX);
			get_device_d3d9()->DrawPrimitive(D3DPT_LINELIST, 0,
											 last_vb_line().size() / 2);
		}

		if (last_vb_line_3d().size())
		{
			GV_PROFILE_EVENT_PIX(debug_draw_line_3d, 0);
			get_device_d3d9()->SetTransform(
				D3DTS_WORLD, (D3DMATRIX*)&gv_matrix44::get_identity());
			get_device_d3d9()->SetTransform(
				D3DTS_VIEW,
				(D3DMATRIX*)&gv_renderer_d3d9::static_get()->get_view_matrix());
			get_device_d3d9()->SetTransform(
				D3DTS_PROJECTION, (D3DMATRIX*)&gv_renderer_d3d9::static_get()
									  ->get_projection_matrix());
			get_device_d3d9()->SetStreamSource(0, m_p_vbs_line_3d_d3d[m_current_vb],
											   0, sizeof(vertex_3d));
			get_device_d3d9()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			// why vertex color doesn't work !!
			// http://www.gamedev.net/community/forums/topic.asp?topic_id=112326&whichpage=1&#602470
			get_device_d3d9()->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,
											  D3DMCS_COLOR1);
			get_device_d3d9()->SetRenderState(D3DRS_AMBIENT,
											  D3DCOLOR_XRGB(255, 255, 255));
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLOROP,
													D3DTOP_SELECTARG1);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_COLORARG1,
													D3DTA_DIFFUSE);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAOP,
													D3DTOP_SELECTARG1);
			get_device_d3d9()->SetTextureStageState(0, D3DTSS_ALPHAARG1,
													D3DTA_DIFFUSE);
			get_device_d3d9()->DrawPrimitive(D3DPT_LINELIST, 0,
											 last_vb_line_3d().size() / 2);
		}

		{
			// test
			GV_PROFILE_EVENT_PIX(debug_draw_textured_vb, 0);
			tex_renderer.draw();
		}

		if (last_strings_to_render().size())
		{
			GV_PROFILE_EVENT_PIX(debug_draw_string, 0);
			for (int i = 0; i < last_strings_to_render().size(); i++)
			{
				string_info& s = last_strings_to_render()[i];
				RECT rc;
				rc.left = s.pos.x;
				rc.top = s.pos.y;
				rc.right = 0;
				rc.bottom = 0;
				m_pFont->DrawTextA(NULL, s.string, -1, &rc, DT_NOCLIP,
								   s.color.BGRA().fixed32);
			}
		}
	}
	void render(gv_unit_test_with_renderer*& test)
	{
		GV_PROFILE_EVENT(debug_draw_render, 0);
		// Clear the backbuffer to a neutral color
		// g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
		// D3DCOLOR_XRGB(180,180,200), 1.0f, 0 );
		// g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0),
		// 1.0f, 0 );
		get_device_d3d9()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER |
											  D3DCLEAR_STENCIL,
								 D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

		// Begin the scene
		if (SUCCEEDED(get_device_d3d9()->BeginScene()))
		{
			test->render();

			this->do_render();
			// RECT rc; rc.left=100; rc.top=100;rc.right=0; rc.bottom=0;
			// m_pFont->DrawText(NULL, "shennan is superman", -1, &rc, DT_NOCLIP,
			// gv_color::BLACK().fixed32);
			get_device_d3d9()->EndScene();
		}

		// Present the backbuffer contents to the display
		{
			GV_PROFILE_EVENT(debug_draw_do_flip_buffer, 0);
			get_device_d3d9()->Present(NULL, NULL, NULL, NULL);
		}
		if (test->is_finished() && test->next())
		{
			// wait stop render
			GV_PROFILE_EVENT(debug_stop_render, 0);
			get_device_d3d9()->Present(NULL, NULL, NULL, NULL);
			get_device_d3d9()->Clear(0, NULL, D3DCLEAR_TARGET,
									 D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
			test->destroy();
			test = test->next();
			test->initialize();
		}
	}
	void draw_string(const char* string, const gv_vector2i& screen_pos,
					 const gv_color& color)
	{
		string_info s;
		s.color = color;
		s.pos = screen_pos;
		strncpy(s.string, string, gvc_string_info_length - 1);
		cu_strings_to_render().add(s);
	};

	void create_window(const gv::gv_string& name, const gv::gv_string& title,
					   const gv::gv_vector2i& pos, const gv::gv_vector2i& size,
					   bool init_device, HWND hWnd = NULL)
	{
		// Register the window class
		GV_PROFILE_EVENT(debug_draw_create_window, 0);
		if (!hWnd)
		{
			WNDCLASSEXA wc = {sizeof(WNDCLASSEX),
							  CS_CLASSDC,
							  gv_debug_renderer_d3d_imp::MsgProc,
							  0L,
							  0L,
							  GetModuleHandle(NULL),
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  *name,
							  NULL};
			RegisterClassExA(&wc);
			// Create the application's window
			hWnd = CreateWindowA(*name,
								 "g\0\0", // need to be wide character..
								 WS_OVERLAPPEDWINDOW, pos.x, pos.y, size.x, size.y,
								 GetDesktopWindow(), NULL, wc.hInstance, NULL);
			m_view_pos.set(0, 0);
			m_view_size = size;
			m_window_size = size;
		}
		else
		{
			RECT window_rect;
			GetClientRect(hWnd, &window_rect);
			m_view_pos.x = window_rect.left;
			m_view_pos.y = window_rect.top;
			m_view_size.x = window_rect.right - window_rect.left;
			m_view_size.y = window_rect.bottom - window_rect.top;
			m_window_size = m_view_size;
		}
		m_hwnd = hWnd;
		init_d3d(hWnd);
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);
		if (gv_renderer_d3d9::static_get())
		{
			gv_renderer_d3d9::static_get()->m_window_handle = hWnd;
		}
	}
	void start_test(gv_unit_test_with_renderer* test)
	{
		GV_PROFILE_EVENT(debug_draw_start_test, 0);
		if (!test)
			return;
		// Enter the message loop
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		test->initialize();
		m_last_tick_time = gv_global::time->get_sec_from_start();
		m_delta_time = 0;

		while (msg.message != WM_QUIT && !test->is_finished())
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				gv_double cu = gv_global::time->get_sec_from_start();
				m_delta_time = gvt_clamp(cu - m_last_tick_time, 0.01, 0.1);
				m_last_tick_time = cu;
				test->update((gv_float)m_delta_time);
				// std::cout<<"gogogogo!!"<<std::endl;
				render(test);
				do_synchronization();
			}
		}
		test->destroy();
	}

	bool do_render_3d_scene(float dt, gv_renderer* p)
	{
		GV_PROFILE_EVENT_PIX(do_render_3d_scene, 0);
		if (!g_init_from_existed_window)
		{
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					if (this->m_pd3dDevice)
					{
						p->do_render(dt);
						return true;
					}
					return true;
				}
			}
		}
		else
		{
			if (this->m_pd3dDevice)
			{
				p->do_render(dt);
				return true;
			}
		}
		return false;
	};

	void draw_triangle(const gv_vector3& v0, const gv_vector3& v1,
					   const gv_vector3& v2, const gv_color& color0,
					   const gv_color& color1, const gv_color& color2)
	{
		if (cu_vb_triangle().size() + 3 >= m_max_tri_vertex)
			return;
		vertex v;
		v.rhw = 1.0f;
		v.x = v0.x + 0.5f;
		v.y = v0.y + 0.5f;
		;
		v.z = v0.z;
		v.color = color0.BGRA().fixed32;
		this->cu_vb_triangle().add(v);
		v.x = v1.x + 0.5f;
		v.y = v1.y + 0.5f;
		;
		v.z = v1.z;
		v.color = color1.BGRA().fixed32;
		this->cu_vb_triangle().add(v);
		v.x = v2.x + 0.5f;
		v.y = v2.y + 0.5f;
		;
		v.z = v2.z;
		v.color = color2.BGRA().fixed32;
		this->cu_vb_triangle().add(v);
	};

	void draw_line(const gv_vector3& v0, const gv_vector3& v1,
				   const gv_color& color0, const gv_color& color1)
	{
		if (cu_vb_line().size() + 2 >= m_max_line_vertex)
			return;
		vertex v;
		v.rhw = 1.0f;
		v.x = v0.x;
		v.y = v0.y;
		;
		v.z = v0.z;
		v.color = color0.BGRA().fixed32;
		this->cu_vb_line().add(v);
		v.x = v1.x;
		v.y = v1.y;
		;
		v.z = v1.z;
		v.color = color1.BGRA().fixed32;
		this->cu_vb_line().add(v);
	};

	void draw_line_3d(const gv_vector3& v0, const gv_vector3& v1,
					  const gv_color& color0, const gv_color& color1)
	{
		if (cu_vb_line_3d().size() + 2 >= m_max_line_vertex)
			return;
		vertex_3d v;
		v.x = v0.x;
		v.y = v0.y;
		;
		v.z = v0.z;
		v.color = color0.BGRA().fixed32;
		this->cu_vb_line_3d().add(v);
		v.x = v1.x;
		v.y = v1.y;
		;
		v.z = v1.z;
		v.color = color1.BGRA().fixed32;
		this->cu_vb_line_3d().add(v);
	};

	void set_debug_texture(class gv_texture* p, bool use_alpha_blending,
						   bool for_3d)
	{
		if (!for_3d)
		{
			debug_texture_state state;
			state.texture = p;
			state.use_alpha_blending = use_alpha_blending;
			tex_renderer.begin_new_state(state);
		}
	};
	void draw_tex_triangle(const gv_vector3 v[3], const gv_vector2 uv[3],
						   const gv_color color[3])
	{
		vertex_tex vtx;
		for (int i = 0; i < 3; i++)
		{
			vtx.rhw = 1.f;
			vtx.x = v[i].get_x();
			vtx.y = v[i].get_y();
			vtx.z = v[i].get_z();
			vtx.u = uv[i].get_x();
			vtx.v = uv[i].get_y();
			vtx.color = color[i].BGRA().fixed32;
			tex_renderer.add_vertex(vtx);
		}
	};
	void draw_tex_triangle_3d(const gv_vector3 v[3], const gv_vector2 uv[3],
							  const gv_color color[3]){

	};

	gv_vector2i get_window_size()
	{
		return m_window_size;
	}
	void capture_mouse()
	{
		SetCapture(m_hwnd);
	};
	void release_mouse()
	{
		ReleaseCapture();
	}

	void do_synchronization()
	{
		m_current_vb = !m_current_vb;
		cu_vb_triangle().clear_and_reserve();
		cu_vb_line().clear_and_reserve();
		cu_strings_to_render().clear_and_reserve();
		cu_vb_line_3d().clear_and_reserve();
		tex_renderer.swap_buffer();
		if (get_renderer_d3d9())
			set_debug_texture(get_renderer_d3d9()->get_default_texture(), false,
							  false);
	}
};
gv_debug_renderer_d3d* s_gv_debug_renderer_d3d = NULL;
gv_debug_renderer_d3d::gv_debug_renderer_d3d()
{
	this->m_impl = new gv_debug_renderer_d3d_imp;
	GV_ASSERT(!s_gv_debug_renderer_d3d);
	s_gv_debug_renderer_d3d = this;
}

gv_debug_renderer_d3d::~gv_debug_renderer_d3d()
{
	s_gv_debug_renderer_d3d = NULL;
	delete this->m_impl;
}
void gv_debug_renderer_d3d::init_vb(const int max_triangle /* =100000 */,
									const int max_line /* =100000 */)
{
	this->m_impl->init_vb(max_triangle, max_line);
}

void gv_debug_renderer_d3d::render()
{
	this->m_impl->do_render();
}

void gv_debug_renderer_d3d::create_window(const gv::gv_string& name,
										  const gv::gv_string& title,
										  const gv::gv_vector2i& pos,
										  const gv::gv_vector2i& size,
										  bool init_device)
{
	this->m_impl->create_window(name, title, pos, size, init_device);
}
void gv_debug_renderer_d3d::init_from_existed_window(gv_int_ptr window)
{
	g_init_from_existed_window = true;
	this->m_impl->create_window("", "", gv_vector2i(),
								gv_global::framework_config.window_size, true,
								(HWND)window);
}

void gv_debug_renderer_d3d::draw_triangle(
	const gv_vector3& v0, const gv_vector3& v1, const gv_vector3& v2,
	const gv_color& color0, const gv_color& color1, const gv_color& color2)
{
	this->m_impl->draw_triangle(v0, v1, v2, color0, color1, color2);
};
void gv_debug_renderer_d3d::draw_line(const gv_vector3& v0,
									  const gv_vector3& v1,
									  const gv_color& color0,
									  const gv_color& color1)
{
	this->m_impl->draw_line(v0, v1, color0, color1);
}

void gv_debug_renderer_d3d::draw_line_3d(const gv_vector3& v0,
										 const gv_vector3& v1,
										 const gv_color& color0,
										 const gv_color& color1)
{
	this->m_impl->draw_line_3d(v0, v1, color0, color1);
}

void gv_debug_renderer_d3d::draw_string(const char* string,
										const gv_vector2i& screen_pos,
										const gv_color& color)
{
	this->m_impl->draw_string(string, screen_pos, color);
};

void gv_debug_renderer_d3d::set_debug_texture(class gv_texture* p,
											  bool use_alpha_blending,
											  bool for_3d)
{
	m_impl->set_debug_texture(p, use_alpha_blending, for_3d);
};
void gv_debug_renderer_d3d::draw_tex_triangle(const gv_vector3 v[3],
											  const gv_vector2 uv[3],
											  const gv_color color[3])
{
	m_impl->draw_tex_triangle(v, uv, color);
};
void gv_debug_renderer_d3d::draw_tex_triangle_3d(const gv_vector3 v[3],
												 const gv_vector2 uv[3],
												 const gv_color color[3])
{
	m_impl->draw_tex_triangle_3d(v, uv, color);
};
void gv_debug_renderer_d3d::start_test(gv_unit_test_with_renderer* test)
{
	this->m_impl->start_test(test);
}
gv_vector2i gv_debug_renderer_d3d::get_window_size()
{
	return this->m_impl->get_window_size();
};
void gv_debug_renderer_d3d::capture_mouse()
{
	this->m_impl->capture_mouse();
};
void gv_debug_renderer_d3d::release_mouse()
{
	this->m_impl->release_mouse();
};

bool gv_debug_renderer_d3d::init_d3d(gv_int_ptr handle)
{
	this->m_impl->init_d3d((HWND)handle);
	return true;
};

bool gv_debug_renderer_d3d::do_render_3d_scene(gv_float dt, gv_renderer* p)
{
	return this->m_impl->do_render_3d_scene(dt, p);
}
gv_debug_renderer_d3d* gv_debug_renderer_d3d::static_get()
{
	return s_gv_debug_renderer_d3d;
};
bool gv_debug_renderer_d3d::do_render_debug()
{
	this->m_impl->do_render();
	return true;
};
void gv_debug_renderer_d3d::uninit_d3d()
{
	this->m_impl->uninit_d3d();
	return;
}

void gv_debug_renderer_d3d::do_synchronization()
{
	this->m_impl->do_synchronization();
}
}

LRESULT WindowProc_Hook(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return gv::gv_debug_renderer_d3d_imp::MsgProc(wnd, message, wParam, lParam);
}