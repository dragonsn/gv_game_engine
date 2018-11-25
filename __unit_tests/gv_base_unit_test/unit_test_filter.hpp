namespace unit_test_filter
{
void main(gvt_array< gv_string >& args)
{
	gvt_filtered_taylor< gv_float > a;
	a.append(1);
	a.append(2);
	test_log() << a.calculate_filtered() << gv_endl;
	a.append(4);
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	a.append(-1);
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	a.append(-2);
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	a.append(7);
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	a.append(11);
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
	test_log() << a.calculate_filtered() << gv_endl;
}
}
