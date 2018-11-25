#include "gv_base/math/gv_vector_dynamic.h"
namespace unit_test_dynamic_vector
{
void main(gvt_array< gv_string >& args)
{
	gvt_vector_dynamic< gv_float > a, b, c;
	a.set_vector_size(10);
	a = 12;
	a[0] = 3;
	b = a;
	b += b;
	b += 12;
	a = a * b;
	a -= 13;
	a.clamp(0, 10);
	a.build_arithmetic_progression(0, 1, 10);
	b -= 6;
	a -= b;
	a.abs();
	a += 0.5;
	//a.floor();
}
}
