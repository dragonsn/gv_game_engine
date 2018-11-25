namespace unit_test_network_database
{
void main(gvt_array< gv_string >& args)
{
	using namespace std;
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_database_manager* my_database = m_sandbox->create_object< gv_database_manager >();
		gv_id table("m_test_table");
		my_database->connect("UID=gv_base_test;PWD=1234;DSN=test_mysql");
		{
			sub_test_timer timer("read_whole_table");
			my_database->read_whole_table(table);
		}
		gv_unit_test_table t;
		t.id = 2;
		gv_int error_code;
		{
			sub_test_timer timer("update_table_with_pk");
			my_database->find_in_table_with_pk(table, t);
			t.last_login_date = gv_global::time->local_time_stamp_unix();
			t.weight = 5;
			t.age += 1;
			my_database->update_table_with_pk(table, t);
		}
		{
			sub_test_timer timer("find_in_table");
			gvt_array< gv_unit_test_table > result_table;
			my_database->find_in_table(table, result_table, "id >2 and id < 100", error_code);
		}
		{
			sub_test_timer timer("insert");
			srand((int)gv_global::time->get_performance_frequency());
			for (int i = 0; i < 100; i++)
			{
				t.id = rand() % 100000;
				t.join_date = gv_global::time->local_time_stamp_unix();
				t.name = "tester";
				t.name << (t.id + 1);
				my_database->insert_to_table(table, t);
			}
		}
	}
	gv_global::sandbox_mama->delete_sandbox(m_sandbox);
	gv_id::static_purge();
	m_sandbox = NULL;
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}