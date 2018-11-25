
namespace unit_test_matrix_generic
{

class visitor
{
public:
	void operator()(const gv_float& s)
	{
		test_log() << s << "  ";
	}
} ccc;

void main(gvt_array< gv_string >& args)
{
	gv_float a[] = {1, 4, 2, 3};
	gvt_matrix_generic< gv_float, 2, 2 > m(a);
	gvt_for_each(m, ccc);
	test_log() << gv_endl;

	gv_float d = m.determinant();
	GV_ASSERT(d == -5.f);

	gvt_matrix_generic< gv_float, 2, 2 > m2;
	m2.identity();
	gvt_for_each(m2, ccc);
	test_log() << gv_endl;

	m *= m2;
	gvt_for_each(m, ccc);
	test_log() << gv_endl;

	gvt_matrix_generic< gv_float, 2, 2 > m3;

	m3 = m.inverse();
	gvt_for_each(m3, ccc);
	test_log() << gv_endl;

	m2 = m * m3;
	gvt_for_each(m2, ccc);
	test_log() << gv_endl;

	m *= -0.2f;
	gvt_for_each(m, ccc);
	test_log() << gv_endl;

	d = m.determinant();

	{
		sub_test_timer timer("matrix_gen_inverse");
		const static int nb_test = 1000;
		for (int i = 0; i < nb_test; i++)
		{
			gvt_matrix_generic< gv_double, 4, 4 > mat1;
			gvt_matrix_generic< gv_double, 4, 4 > mat2;
			gvt_matrix_generic< gv_double, 4, 4 > mat3;
			mat1.randomize(0.2, 2.);
			mat2 = mat1.inverse();
			mat3 = mat1 * mat2;
			GV_ASSERT((mat3 == gvt_matrix_generic< gv_double, 4, 4 >::get_identity()));
		}
	}
	//GV_ASSERT(d==-5.f);
}
}
