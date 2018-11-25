namespace unit_test_quaternion
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("unit_test_quaternion0");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random(-6.f, 6.f);
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());
			// row or colum major !!!make sure
			gv_quat q;
			gv_math::convert(q, mat1);
			gv_vector3 v, v1, v2;
			v.v.randomize(0.2f, 2.0f);
			v1 = mat1.rotate(v);
			v2 = q.rotate(v);
			GV_ASSERT(v1.is_almost_equal(v2));
		}

		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());
			// row or colum major !!!make sure
			gv_quat q;
			gv_matrix44 mat2;
			gv_math::convert(q, mat1);
			gv_math::convert(mat2, q);
			GV_ASSERT(mat1.matrix44 == mat2.matrix44);
		}

		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());
			// row or colum major !!!make sure
			gv_quat q;
			gv_matrix44 mat2;
			gv_math::convert(q, mat1);
			gv_quat q2(q);
			q2.inverse();
			gv_math::convert(mat2, q2);
			mat1 = mat1.get_inverse();
			GV_ASSERT(mat1 == mat2);
		}

		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1, mat2, mat3, mat4;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());

			mat2 = gv_matrix44::get_rotation_by_z(random());
			mat2 *= gv_matrix44::get_rotation_by_y(random());
			mat2 *= gv_matrix44::get_rotation_by_x(random());

			mat3 = mat1 * mat2;

			gv_quat q1, q2, q3;

			gv_math::convert(q1, mat1);
			gv_math::convert(q2, mat2);
			q3 = q1 * q2;
			gv_math::convert(mat4, q3);
			GV_ASSERT(mat3 == mat4);
		}
	}
}
}
