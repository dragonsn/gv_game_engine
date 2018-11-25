namespace unit_test_vector3
{
GV_STATIC_ASSERT(sizeof(gv_vector3) == 12);
void main(gvt_array< gv_string >& args)
{
	gvf_stdout< gv_float > printer;
	gv_vector3 v;
	v = 1;
	gvt_for_each(v.v, printer);
	test_log() << gv_endl;
	v.normalize();
	gvt_for_each(v.v, printer);
	test_log() << gv_endl;

	gv_vector3 v1(v);
	gv_vector3 v2(gv_vector3::get_x_axis());
	v2 += v1;
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;

	v2.z = 10;
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;

	v2 *= v1;
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;

	v2 /= v1;
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;

	v2.normalize();
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;

	v2.negate();
	gvt_for_each(v2.v, printer);
	test_log() << gv_endl;
	gv_vector3 result(-0.155556f, -0.0569374f, -0.986185f);
	GV_ASSERT(v2.is_almost_equal(result));
	{
		gv_vector3 dirZ(-1, -1, -1);
		dirZ.normalize();
		gv_vector3 up = gv_vector3(0, 1, 0);
		gv_vector3 dirX = up.cross(dirZ);
		gv_vector3 dirY = dirZ.cross(dirX);
		dirX.normalize();
		dirY.normalize();
		test_log() << dirX << dirY << dirZ;
		int i = 0;
	}
}
}
