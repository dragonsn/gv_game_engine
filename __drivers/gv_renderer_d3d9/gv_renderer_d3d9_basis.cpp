#include "gv_renderer_d3d_private.h"
#include "gv_resource_d3d_cache.hpp"
#include "gv_render_target_mgr_d3d.h"
#include "gv_render_target_mgr_d3d.hpp"
#include "gv_post_effect_mgr_d3d.h"
#include "gv_post_effect_mgr_d3d.hpp"

/*
D3DDECLUSAGE_POSITION = 0,
D3DDECLUSAGE_BLENDWEIGHT,   // 1
D3DDECLUSAGE_BLENDINDICES,  // 2
D3DDECLUSAGE_NORMAL,        // 3
D3DDECLUSAGE_PSIZE,         // 4
D3DDECLUSAGE_TEXCOORD,      // 5
D3DDECLUSAGE_TANGENT,       // 6
D3DDECLUSAGE_BINORMAL,      // 7
D3DDECLUSAGE_TESSFACTOR,    // 8
D3DDECLUSAGE_POSITIONT,     // 9
D3DDECLUSAGE_COLOR,         // 10
D3DDECLUSAGE_FOG,           // 11
D3DDECLUSAGE_DEPTH,         // 12
D3DDECLUSAGE_SAMPLE,        // 13
//

typedef enum _D3DDECLTYPE
{
D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
D3DDECLTYPE_FLOAT4    =  3,  // 4D float
D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1.
range
// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0.,
1.)
D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to
255.0
D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized
(v[0]/32767.0,v[1]/32767.0,0,1)
D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized
(v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized
(v[0]/65535.0,v[1]/65535.0,0,1)
D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized
(v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value,
value, value, 1)
D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and
expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to
(value, value, 0, 1)
D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
} D3DDECLTYPE;

*/
//  [8/2/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][1]
//  |------>[rendering][d3d] rendering setting ini")
//  [8/17/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][2]
//  |------>[rendering][d3d] render target manager (mostly for post effects)  (
//  resolve to render target, create renderable texture ,set render target ,
//  draw fullscreen quad .  ) , ")
//  [8/17/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][2]
//  |------>[rendering][d3d] post effect manager ")
//  [8/17/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][5]
//  |------>[rendering][d3d][post effect]  bloom ")
#pragma GV_REMINDER( \
	"[PLAN][P1][5] |------>[rendering][d3d][post effect]  god ray ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P1][2]
//  |------>[rendering][d3d] dynamic buffer manager")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P1][5]
//  |------>[rendering][particle] particle component ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P1][5]
//  |------>[rendering][particle] particle manager ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P1][5]
//  |------>[rendering][particle] particle pipeline (import , export, editor) ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P1][5]
//  |------>[rendering][particle] [d3d] particle renderer")
//  [8/17/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][5]
//  |------>[rendering][particle] particle effects  (trail , hit effect , impact
//  effect )")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][5]
//  |------>[rendering][d3d][CSM shadow] ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][5]
//  |------>[rendering][d3d][depth only pass] ")
//  [9/6/2011 Administrator]#pragma		GV_REMINDER("[PLAN][P0][5]
//  |------>[rendering][d3d][post effect with depth texture]  deferred lighting
//  effect  ")
#pragma GV_REMINDER( \
	"[PLAN][P1][2] |------>[rendering][d3d] advanced water  effect")
#pragma GV_REMINDER( \
	"[PLAN][P1][2] |------>[rendering][d3d] advanced foliage effect")
#pragma GV_REMINDER( \
	"[PITFALL]     |------>[rendering] all the param not set by app ,has to be declared with const static")
#pragma GV_REMINDER( \
	"[PITFALL]     |------>[rendering] the view (lookat ) and perspective matrix is left handed for some reason!! don't use it to get direction of camera")
#pragma GV_REMINDER( \
	"[PITFALL]     |------>[rendering] use linear value in vertex shader to pass to pixel shader ,or the interpolator will be silly")
#pragma GV_REMINDER( \
	"[TASK]        |------>[rendering] need to solve the shader  instance issue.")
#pragma GV_REMINDER( \
	"[PLAN]        |------>[rendering] need to handle device lost")
