namespace unit_test_geom
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("unit_test_box");
		gvt_box< gv_float > box1;
		box1.add(gv_vector3(0, 0, 0));
		box1.add(gv_vector3(2, 2, 2));
		gvt_box< gv_float > box2;
		box2.add(gv_vector3(1, 1, 1));
		box2.add(gv_vector3(2, 2, 2));
		gvt_box< gv_float > box3;
		box3 = box1.clip(box2);
		GV_ASSERT(box3 == box2);
	}

	{
		sub_test_timer timer("matrix_44_inverse_with_scale");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random(-1000, 1000);
		gvt_random_range< gv_float > random_s(0, 1000);
		for (int i = 0; i < nb_test; i++)
		{
			gv_box a, b;
			a.min_p = gv_vector3(random(), random(), random());
			a.max_p = a.min_p + gv_vector3(random_s(), random_s(), random_s());

			b.min_p = gv_vector3(random(), random(), random());
			b.max_p = b.min_p + gv_vector3(random_s(), random_s(), random_s());

			gv_bool r1 = a.is_overlap(b);
			gv_bool r2 = a.minkowski_difference(b).is_point_inside(gv_vector3(0));
			GV_ASSERT(r1 == r2);
		}
	}
	{
		sub_test_timer timer("unit_test_plane");
		gv_plane plane;
		gv_vector3 nml(0.7f, 0.7f, 0.2f);
		nml.normalize();
		plane.set(nml, 1.f);
		gv_vector3 v = plane.project(gv_vector3::get_zero_vector());
		gv_float f = plane.test_point(v);
		GV_ASSERT(gvt_value< gv_float >::is_almost_zero(f));
	}

	{
		sub_test_timer timer("gv_frustum");
		gvt_frustum< gv_float > f1;
		f1.set_fov(90.f, 1.f, 1.f, 100.f);
		gvt_frustum< gv_float > f2;
		f2.set_frustum(-1, 1.f, -1.f, 1.f, 1.f, 100.f);
		GV_ASSERT(f1 == f2);
	}
}
};
