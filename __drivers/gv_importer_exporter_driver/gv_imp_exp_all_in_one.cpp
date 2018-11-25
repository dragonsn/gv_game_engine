#include "stdafx.h"

#include "gv_driver_impexp.hpp"

#if defined(WIN32)

#if !GV_64
#pragma comment(lib, "/lib/win32/jpeg.lib")
#pragma comment(lib, "/lib/win32/libpsd.lib")
//#pragma comment( lib,"zlib.lib")
#if !GVM_NOT_SUPPORT_XIMAGE
#pragma comment(lib, "/lib/win32/cximage.lib")
#endif
//#pragma comment( lib,"png.lib")
#pragma comment(lib, "/lib/win32/tiff.lib")
#pragma comment(lib, "/lib/win32/mng.lib")
#pragma comment(lib, "/lib/win32/jasper.lib")
#pragma comment(lib, "/lib/win32/jbig.lib")
#pragma comment(lib, "/lib/win32/libdcr.lib")
#else
#pragma comment(lib, "/lib/x64/jpeg.lib")
#pragma comment(lib, "/lib/x64/libpsd.lib")
//#pragma comment( lib,"zlib.lib")
#if !GVM_NOT_SUPPORT_XIMAGE
#pragma comment(lib, "/lib/x64/cximage.lib")
#endif
//#pragma comment( lib,"png.lib")
#pragma comment(lib, "/lib/x64/tiff.lib")
#pragma comment(lib, "/lib/x64/mng.lib")
#pragma comment(lib, "/lib/x64/jasper.lib")
#pragma comment(lib, "/lib/x64/jbig.lib")
#pragma comment(lib, "/lib/x64/libdcr.lib")
#endif
#include "gv_impexp_cximage.hpp"
#endif

#include "gv_impexp_3ds.hpp"
#include "gv_impexp_sn.hpp"
#include "gv_impexp_obj.hpp"
#include "gv_impexp_rfx.hpp"
