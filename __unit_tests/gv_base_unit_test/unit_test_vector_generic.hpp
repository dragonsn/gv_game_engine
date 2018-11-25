namespace unit_test_vector_generic
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
	{
		sub_test_timer timer("unit_test_vector_generic");

		gvt_vector_generic< gv_float, 12 > v12;
		GV_ASSERT(sizeof(v12) == 12 * sizeof(gv_float));
		v12 = 0;
		v12 += 1.0;
		std::for_each(v12.begin(), v12.end(), ccc);
		GV_ASSERT(v12[0] == 1.0f);
		GV_ASSERT(v12[11] == 1.0f);
		test_log() << gv_endl;

		gvt_vector_generic< gv_float, 12 > v12_1;
		v12_1 = 2.0;
		v12 += v12_1;
		std::for_each(v12.begin(), v12.end(), ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == 3.0f);
		GV_ASSERT(v12[11] == 3.0f);

		v12 = v12_1;
		std::for_each(v12.begin(), v12.end(), ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == 2.0f);
		GV_ASSERT(v12[11] == 2.0f);

		gvt_for_each_unary_op(v12, v12, gvf_negate< gv_float >());
		gvt_for_each(v12, ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == -2.0f);
		GV_ASSERT(v12[11] == -2.0f);

		v12.mark_index();
		gvt_for_each(v12, ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == 1.0f);
		GV_ASSERT(v12[11] == 12.0f);

		gvt_for_each_binary_op(v12, v12_1, v12, gvf_add< gv_float >());
		gvt_for_each(v12, ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == 3.0f);
		GV_ASSERT(v12[11] == 14.0f);

		std::sort(v12.begin(), v12.end(), std::greater< gv_float >());
		gvt_for_each(v12, ccc);
		test_log() << gv_endl;
		GV_ASSERT(v12[0] == 14.0f);
		GV_ASSERT(v12[11] == 3.0f);
		{
			gv_float a[] = {0, 1, 2, 3};
			gvt_vector_generic< gv_float, 4 > v4(a);
			gvt_for_each(v4, ccc);
			test_log() << gv_endl;
		}
	}
}
}
