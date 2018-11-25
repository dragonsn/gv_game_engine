#include "stdafx.h"
#include "gv_framework/inc/gv_framework.h"
#include "gv_framework/inc/gv_framework_events.h"
#include "gv_gp.h"

//=======================================================
#include "gv_gp_helper.hpp"
#include "gv_driver_gameplay3d.hpp"
#include "gv_debug_renderer_gameplay3d.hpp"
#include "gv_hook_gameplay3d.hpp"
#include "gv_renderer_gameplay3d.hpp"

#if defined(WIN32)

#if GP_WITH_BULLET
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "LinearMath.lib")
#endif

//#pragma comment( lib,"freetype245.lib")
//#pragma comment( lib,"glew32.lib")
//#pragma comment( lib,"libogg.lib")
//#pragma comment( lib,"libpng14.lib")
//#pragma comment( lib,"libvorbis.lib")
//#pragma comment( lib,"libvorbisfile.lib")

#if GP_WITH_LUA
#pragma comment(lib, "lua.lib")
#endif
//#pragma comment( lib,"OpenAL32.lib")
//#pragma comment( lib,"zlib.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")
#endif