namespace unit_test_matrix44
{
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("matrix_44_determinant");
		const static int nb_test = 1000;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1.matrix44.randomize();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen = mat1.matrix44;
			gv_float d1 = mat1.determinant();
			gv_float d2 = mat_gen.determinant();
			GV_ASSERT(gvt_value< gv_float >::is_almost_equal(d1, d2));
		}
	} //test 1

	{
		sub_test_timer timer("matrix_44_inverse_no_scale");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());

			gvt_matrix_generic< gv_float, 4, 4 > mat_gen = mat1.matrix44;
			gv_matrix44 mat_r = mat1.get_inverse_noscale_3dtransform();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen_r = mat_gen.inverse();
			//mat_r.axis_x.x+=0.001f;
			GV_ASSERT(mat_r.matrix44 == mat_gen_r);
		}
	}

	{
		sub_test_timer timer("matrix_44_inverse_with_scale");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());
			gv_vector3 v;
			v.v.randomize(0.2f, 2.0f);
			mat1.scale_by(v);

			gvt_matrix_generic< gv_float, 4, 4 > mat_gen = mat1.matrix44;
			gv_matrix44 mat_r = mat1.get_inverse_with_scale_3dtransform();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen_r = mat_gen.inverse();
			GV_ASSERT(mat_r.matrix44 == (mat_gen_r));
		}
	}

	{
		sub_test_timer timer("matrix_mul");
		const static int nb_test = 1000;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1, mat2, mat3;
			mat1.matrix44.randomize();
			mat2.matrix44.randomize();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen1 = mat1.matrix44;
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen2 = mat2.matrix44;
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen3;

			mat3 = mat1 * mat2;
			mat_gen3 = mat_gen1 * mat_gen2;
			GV_ASSERT(mat3.matrix44 == mat_gen3);
		}
	}

	{
		sub_test_timer timer("matrix_mul_by");
		const static int nb_test = 1000;
		gvt_random_range< gv_float > random;
		for (int i = 0; i < nb_test; i++)
		{
			gv_matrix44 mat1;
			mat1 = gv_matrix44::get_rotation_by_z(random());
			mat1 *= gv_matrix44::get_rotation_by_y(random());
			mat1 *= gv_matrix44::get_rotation_by_x(random());
			gv_vector3 v, v1, vr;
			v.v.randomize(0.2f, 2.0f);
			mat1.scale_by(v);
			v1.v.randomize();
			gvt_matrix_generic< gv_float, 4, 4 > mat_gen = mat1.matrix44;
			gvt_vector_generic< gv_float, 3 > vec_gen = mat_gen.mul_by(v1.v);
			vr = mat1.mul_by(v1);
			GV_ASSERT(vr.v == vec_gen);
			vr = v1 * mat1;
			GV_ASSERT(vr.v == vec_gen);
		}
	}

} //main

} //
