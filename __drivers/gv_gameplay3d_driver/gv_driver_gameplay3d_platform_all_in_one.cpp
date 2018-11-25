#include "stdafx.h"
using namespace gv;
#define GLEW_NO_GLU 1
#define GLEW_STATIC 1

#ifdef WIN32
#include "gl/glew.hpp"
#include "gv_base/openal/gv_audio_all_in_one.hpp"
#endif

#include "gameplay3d/src/Platform.hpp"
#include "gameplay3d/src/PlatformAndroid.hpp"
#include "gameplay3d/src/PlatformWindows.hpp"
