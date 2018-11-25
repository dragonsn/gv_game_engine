#include "gv_framework/world/gv_world_2d.h"

namespace unit_test_fixed_point
{
void main(gvt_array< gv_string >& args)
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{
		sub_test_timer timer("test_simple_entity_get_com");
		gv_math_fixed h;
		h.rebuild_sin_table();
		gvt_random_range< gv_float > r(-1000, 1000);
		for (int i = 0; i < 10000; i++)
		{
			gv_float a = r();
			gv_int fixed_a = h.rad_to_deg(a);
			{ //sine
				gv_float s0 = sinf(a);
				gv_float s1 = h.to_float(h.sini(fixed_a));
				GV_ASSERT(gvt_abs(s0 - s1) < 0.01f);
			}
			{ //cose
				gv_float s0 = cosf(a);
				gv_float s1 = h.to_float(h.cosi(fixed_a));
				GV_ASSERT(gvt_abs(s0 - s1) < 0.01f);
			}
			{ //tan
				gv_float s0 = tanf(a);
				gv_float s1 = h.to_float(h.tani(fixed_a));
				//GV_ASSERT(gvt_abs(s0*s1)>=0);
				//GV_ASSERT(gvt_abs(s0-s1)< gvt_max(gvt_abs(s0*s0)*0.05 ,0.03) );
			}
			{ //atan
				gv_float b = gvt_abs(a / 300.f);
				gv_float s0 = atanf(b);
				gv_float s1 = h.deg_to_rad(h.atani(h.to_fixed(b)));
				GV_ASSERT(gvt_abs(s0 - s1) < 0.01f);
			}
			{ //atan2
				float c = r();
				gv_float s0 = atan2f(c, a);
				gv_float s1 = h.deg_to_rad(h.dir_to_angle(gv_vector2i(h.to_fixed(a), h.to_fixed(c))));
				GV_ASSERT(gvt_abs(s0 - s1) < 0.01f);
			}
		}
	}

	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
};
}
