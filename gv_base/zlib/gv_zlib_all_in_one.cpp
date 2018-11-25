#include "gv_base_internal.h"

#if GV_WITH_ZLIB

// based on zlib 1.2.8
#define NO_DUMMY_DECL
#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4200)
#pragma warning(disable : 4324)
#pragma warning(disable : 4065)
#pragma warning(disable : 4510)
#pragma warning(disable : 4610)
#pragma warning(disable : 4706)
#pragma warning(disable : 4800)
#endif

#include "zlib/trees.hpp"
#include "zlib/uncompr.hpp"
#include "zlib/zutil.hpp"
#include "zlib/adler32.hpp"
#include "zlib/compress.hpp"
#include "zlib/crc32.hpp"
#include "zlib/deflate.hpp"
#include "zlib/gzclose.hpp"
#include "zlib/gzlib.hpp"
#include "zlib/gzread.hpp"
#include "zlib/gzwrite.hpp"
#include "zlib/infback.hpp"
#include "zlib/inffast.hpp"
#include "zlib/inflate.hpp"
#include "zlib/inftrees.hpp"

#endif