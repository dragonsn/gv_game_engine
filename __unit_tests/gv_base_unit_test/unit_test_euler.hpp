namespace unit_test_euler
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("unit_test_euler0");
		const static int nb_test = 1000;
		for (int i = 0; i < nb_test; i++)
		{
			gvt_matrix44< gv_double > mat1, mat2, mat3;
			gvt_euler< gv_double > e, e2;
			e.v3.v.randomize(-6.f, 6.f);
			//e.pitch=0;
			//e.roll=0;
			mat1 = gvt_matrix44< gv_double >::get_rotation_by_y(e.yaw);
			gvt_math< gv_double >::convert(mat3, gvt_vector4< gv_double >(mat1.axis_z, e.pitch));
			mat1 *= mat3;
			gvt_math< gv_double >::convert(mat3, gvt_vector4< gv_double >(mat1.axis_x, e.roll));
			mat1 *= mat3;
			//!!!COMMON mistake!! the axis aready rotated!!
			//mat1*=gvt_matrix44<gv_double>::get_rotation_by_z( e.pitch);
			//mat1*=gvt_matrix44<gv_double>::get_rotation_by_x( e.roll);
			gvt_math< gv_double >::convert(mat2, e);
			GV_ASSERT(mat1 == mat2);
			gvt_math< gv_double >::convert(e2, mat2);
			gvt_math< gv_double >::convert(mat3, e2);
			gv_double d = (mat2.matrix44 - mat3.matrix44).abs_sum();
			GV_ASSERT(d < gvt_value< gv_double >().epsilon() * 3000.f);
		}
	}
} //main
}
