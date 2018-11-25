namespace unit_test_gv_id
{

void main(gvt_array< gv_string >& args)
{
	{

		gv_id id0, id1, id2;
		id0 = "abc";

		GV_TEST_PRINT_VAR(id0, test_log());
		GV_TEST_PRINT_VAR(id0.get_refcount(), test_log());
		{
			gv_id id3(id0);
			GV_TEST_PRINT_VAR(id3, test_log());
			GV_TEST_PRINT_VAR(id3.get_refcount(), test_log());
		}
		id1 = "abc";
		GV_TEST_PRINT_VAR(id1, test_log());
		GV_TEST_PRINT_VAR(id1.get_refcount(), test_log());
		id2 = "cba";
		GV_TEST_PRINT_VAR(id2, test_log());
		GV_TEST_PRINT_VAR(id2.get_refcount(), test_log());
		GV_ASSERT(id1 == id0);
		GV_ASSERT(!(id1 == id2));
	}
	{
		gv_string_tmp s("DFDF");
		;
		gv_string_tmp s2;
		s >> s2;
		gv_id id;
		s >> id;
	}
	gv_id::static_purge();
}
}
