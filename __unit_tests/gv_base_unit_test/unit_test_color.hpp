namespace unit_test_color
{
void main(gvt_array< gv_string >& args)
{
	{
		GV_STATIC_ASSERT(sizeof(gv_color) == 4);
		GV_STATIC_ASSERT(sizeof(gv_colorf) == 16);

		gv_color c;
		gv_colorf cf;

		c = c.BLACK();
		cf = c;
		test_log() << "BLACK=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;

		c = c.WHITE();
		cf = c;
		test_log() << "WHITE=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;

		c = c.GREEN_B();
		cf = c;
		test_log() << "GREEN_B=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;

		c = c.GREEN_D();
		cf = c;
		test_log() << "GREEN_D=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;

		c = c.BLUE_B();
		cf = c;
		test_log() << "BLUE_B=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;

		c = c.RED_B();
		cf = c;
		test_log() << "RED_B=";
		gvt_for_each(cf.v.v4(), gvf_stdout< gv_float >());
		test_log() << gv_endl;
	}
}
}
