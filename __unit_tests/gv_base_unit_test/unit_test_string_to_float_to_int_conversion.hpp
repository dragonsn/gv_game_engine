namespace unit_test_string_to_float_to_int_conversion
{
using namespace std;
void main(gvt_array< gv_string >& args)
{
	test_log() << " test accuracy!!" << gv_endl;
	double d = 123.456789012345, d2 = 0;
	float f = 123.456789012345f, f2 = 0;
	int ivalue = -1;
	gv_uint uvalue = -1;
	gv_ulong uvalue64 = -1;
	gv_long ivalue64 = -1;

	GV_TEST_PRINT_VAR(d, test_log());
	GV_TEST_PRINT_VAR(f, test_log());
	GV_TEST_PRINT_VAR(ivalue, test_log());
	GV_TEST_PRINT_VAR(uvalue, test_log());
	GV_TEST_PRINT_VAR(ivalue64, test_log());
	GV_TEST_PRINT_VAR(uvalue64, test_log());

	char buffer[1024];
	{
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%f", d), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%f", f), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%d", ivalue), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%u", uvalue), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%lld", ivalue64), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC(sprintf(buffer, "%llu", uvalue64), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
	}

	{
		GV_TEST_PRINT_FUNC(gv_double_to_string(d, buffer), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC_AND_RESULT(gv_string_to_double(buffer), test_log());

		GV_TEST_PRINT_FUNC(gv_int_to_string(ivalue, buffer), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC_AND_RESULT(gv_string_to_int(buffer), test_log());

		GV_TEST_PRINT_FUNC(gv_uint_to_string(uvalue, buffer), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC_AND_RESULT(gv_string_to_uint(buffer), test_log());

		GV_TEST_PRINT_FUNC(gv_long_to_string(ivalue64, buffer), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC_AND_RESULT(gv_string_to_long(buffer), test_log());

		GV_TEST_PRINT_FUNC(gv_ulong_to_string(uvalue64, buffer), test_log());
		GV_TEST_PRINT_VAR(buffer, test_log());
		GV_TEST_PRINT_FUNC_AND_RESULT(gv_string_to_ulong(buffer), test_log());
	}
	double d_tmp = 0;

	{

		sub_test_timer timer("sprintf_scanf");
		for (int i = 0; i < 10000; i++)
		{

			int ivalue_tmp;
			gv_uint uvalue_tmp;
			gv_ulong uvalue64_tmp;
			gv_long ivalue64_tmp;
			gv_float f_tmp;
			gv_double d_tmp;

			ivalue = gvt_rand< gv_int >(ivalue, -1000, 1000);
			uvalue = gvt_rand< gv_uint >(uvalue, 0, 1000);
			uvalue64 = gvt_rand< gv_ulong >(uvalue64, 0, 1000);
			ivalue64 = gvt_rand< gv_long >(ivalue64, -1000, 1000);
			f = gvt_rand< gv_float >(f, -1000, 1000);
			d = gvt_rand< gv_double >(d, -1000, 1000);

			sprintf(buffer, "%lf", d);
			sscanf(buffer, "%lf", &d_tmp);

			sprintf(buffer, "%f", f);
			sscanf(buffer, "%f", &f_tmp);

			sprintf(buffer, "%d", ivalue);
			sscanf(buffer, "%d", &ivalue_tmp);

			sprintf(buffer, "%u", uvalue);
			sscanf(buffer, "%u", &uvalue_tmp);

			sprintf(buffer, "%lld", ivalue64);
			sscanf(buffer, "%lld", &ivalue64_tmp);

			sprintf(buffer, "%llu", uvalue64);
			sscanf(buffer, "%llu", &uvalue64_tmp);

			GV_ASSERT(gvt_is_almost_equal(d_tmp, d));
			GV_ASSERT(gvt_is_almost_equal(f_tmp, f));
			GV_ASSERT(ivalue == ivalue_tmp);
			GV_ASSERT(uvalue_tmp == uvalue);
			GV_ASSERT(ivalue64 == ivalue64_tmp);
			GV_ASSERT(uvalue64 == uvalue64);
		}
	}

	{
		sub_test_timer timer("int_to_string__string_to_float");
		for (int i = 0; i < 10000; i++)
		{
			int ivalue_tmp;
			gv_uint uvalue_tmp;
			gv_ulong uvalue64_tmp;
			gv_long ivalue64_tmp;
			gv_float f_tmp;
			gv_double d_tmp;

			ivalue = gvt_rand< gv_int >(ivalue, -1000, 1000);
			uvalue = gvt_rand< gv_uint >(uvalue, 0, 1000);
			uvalue64 = gvt_rand< gv_ulong >(uvalue64, 0, 1000);
			ivalue64 = gvt_rand< gv_long >(ivalue64, -1000, 1000);
			f = gvt_rand< gv_float >(f, -1000, 1000);
			d = gvt_rand< gv_double >(d, -1000, 1000);

			gv_float_to_string(f, buffer);
			f_tmp = gv_string_to_float(buffer);

			gv_double_to_string(d, buffer);
			d_tmp = gv_string_to_double(buffer);

			gv_int_to_string(ivalue, buffer);
			ivalue_tmp = gv_string_to_int(buffer);

			gv_uint_to_string(uvalue, buffer);
			uvalue_tmp = gv_string_to_uint(buffer);

			gv_long_to_string(ivalue64, buffer);
			ivalue64_tmp = gv_string_to_long(buffer);

			gv_ulong_to_string(uvalue64, buffer);
			uvalue64_tmp = gv_string_to_ulong(buffer);

			GV_ASSERT(gvt_is_almost_equal(d_tmp, d));
			GV_ASSERT(gvt_is_almost_equal(f_tmp, f));
			GV_ASSERT(ivalue == ivalue_tmp);
			GV_ASSERT(uvalue_tmp == uvalue);
			GV_ASSERT(ivalue64 == ivalue64_tmp);
			GV_ASSERT(uvalue64 == uvalue64);
		}
	}

} //main
}
