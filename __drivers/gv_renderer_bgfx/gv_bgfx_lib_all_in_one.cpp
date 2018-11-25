#include "stdafx.h"

#if GV_WITH_BGFX

#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include "bx/src/amalgamated.cpp"
#include "bimg/src/image.cpp"
#include "bimg/src/image_decode.cpp"
#include "bimg/src/image_gnf.cpp"
#include "bgfx/3rdparty/ib-compress/indexbuffercompression.cpp"
#include "bgfx/3rdparty/ib-compress/indexbufferdecompression.cpp"

#include "bgfx/3rdparty/dear-imgui/imgui.cpp"
#include "bgfx/3rdparty/dear-imgui/imgui_draw.cpp"

#endif
