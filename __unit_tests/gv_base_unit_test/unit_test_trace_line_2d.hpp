namespace unit_test_trace_line_2d
{
bool print(int x, int y)
{
	test_log() << "==>(" << x << "," << y << ")";
	return true;
}
void main(gvt_array< gv_string >& args)
{
	{
		sub_test_timer timer("trace_line_2d");
		gvt_random_range< float > rand(0, 10);
		for (int i = 0; i < 100; i++)
		{
			gv_vector2 v0;
			gv_vector2 v1;
			v0.v.randomize();
			v1.v.randomize();
			v1 -= 0.5f;
			v1 *= rand();
			test_log() << gv_endl;
			gvt_trace_line2d(v0, v1, boost::bind(&print, _1, _2));
		}
		{
			gv_vector2 v0(42.812500f, 19.500000f);
			gv_vector2 v1(44.000000f, 18.000000f);
			test_log() << gv_endl;
			gvt_trace_line2d(v0, v1, boost::bind(&print, _1, _2));
		}
	}
}
}
