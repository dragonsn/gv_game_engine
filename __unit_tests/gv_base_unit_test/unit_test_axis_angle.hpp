namespace unit_test_axis_angle
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("unit_test_axis_angle0");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random(-6.f, 6.f);
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1, mat2;
			{
				gv_float angle = random();
				mat1 = gv_matrix44::get_rotation_by_z(angle);
				gv_vector4 v(gv_vector3::get_z_axis(), angle);
				gv_math::convert(mat2, v);
				//GVM_CONSOLE_OUT(mat1<<gv_endl);
				//GVM_CONSOLE_OUT(mat2<<gv_endl);
				//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
				GV_ASSERT(mat1 == mat2);
				gv_vector4 v2;
				gv_math::convert(v2, mat2);
				gv_math::convert(mat1, v2);
//GVM_CONSOLE_OUT(mat1<<gv_endl);
//GVM_CONSOLE_OUT(mat2<<gv_endl);
//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
#pragma GV_REMINDER("[bug]why android will assert here?")
				GV_ASSERT(mat1 == mat2);
			}
			{
				gvt_matrix44< gv_double > mat1, mat2, mat3;
				gv_double angle = (gv_double)random();
				mat1 = gvt_matrix44< gv_double >::get_rotation_by_y(angle);
				gvt_vector4< gv_double > v(gvt_vector3< gv_double >::get_y_axis(), angle);
				gvt_math< gv_double >::convert(mat2, v);
				//GVM_CONSOLE_OUT(mat1<<gv_endl);
				//GVM_CONSOLE_OUT(mat2<<gv_endl);
				//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
				GV_ASSERT(mat1 == mat2);
				gvt_vector4< gv_double > v2;
				gvt_math< gv_double >::convert(v2, mat2);
				gvt_math< gv_double >::convert(mat1, v2);
				//GVM_CONSOLE_OUT(mat1<<gv_endl);
				//GVM_CONSOLE_OUT(mat2<<gv_endl);
				//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
				GV_ASSERT(mat1 == mat2);
			}
			{
				gv_float angle = random();
				mat1 = gv_matrix44::get_rotation_by_x(angle);
				gv_vector4 v(gv_vector3::get_x_axis(), angle);
				gv_math::convert(mat2, v);
				//GVM_CONSOLE_OUT(mat1<<gv_endl);
				//GVM_CONSOLE_OUT(mat2<<gv_endl);
				//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
				GV_ASSERT(mat1 == mat2);
				gv_vector4 v2;
				gv_math::convert(v2, mat2);
				gv_math::convert(mat1, v2);
				//GVM_CONSOLE_OUT(mat1<<gv_endl);
				//GVM_CONSOLE_OUT(mat2<<gv_endl);
				//GVM_CONSOLE_OUT("-------------------------"<<gv_endl);
				GV_ASSERT(mat1 == mat2);
			}
		}
	}
}
}
