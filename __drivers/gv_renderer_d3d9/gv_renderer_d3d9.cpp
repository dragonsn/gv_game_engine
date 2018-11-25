#include "gv_renderer_d3d_private.h"
#include "gv_framework_events.h"
#include "gv_render_target_mgr_d3d.h"
#include "gv_post_effect_mgr_d3d.h"
#include "gv_driver_d3d.hpp"
#include "gv_renderer_d3d9_precache.hpp"


#if GV_WITH_IMGUI
#pragma include_alias( "imgui.h", "bgfx/3rdparty/dear-imgui/imgui.h" )  
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#include "gv_renderer_d3d9.hpp"
