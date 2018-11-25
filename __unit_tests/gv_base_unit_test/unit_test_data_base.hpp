#include "gv_base/database/gv_database.h"

namespace unit_test_data_base
{

struct the_test
{
	gv_database_connection db; // connect object
	void insert()
	// insert rows into table
	{
		sub_test_timer timer("insert");
		gv_database_stream o(1, // buffer size should be == 1 always on INSERT
							 "insert into test_tab values "
							 " (:f1<int>,:f2<char[31]>), "
							 " (:f12<int>,:f22<char[31]>), "
							 " (:f13<int>,:f23<char[31]>), "
							 " (:f14<int>,:f24<char[31]>), "
							 " (:f15<int>,:f25<char[31]>) ",
							 // INSERT statement. Multiple sets of values can be used
							 // to work around the lack of the bulk interface
							 db // connect object
							 );

		// If the number of rows to be inserted is not known in advance,
		// another stream with the same INSERT can be opened
		gv_database_stream o2(1, // buffer size should be == 1 always on INSERT
							  "insert into test_tab values "
							  " (:f1<int>,:f2<char[31]>)",
							  db // connect object
							  );

		char tmp[32];
		int i;
		for (i = 1; i <= 100; ++i)
		{
			sprintf(tmp, "Name%d", i);
			o << i << tmp;
		}
		for (i = 101; i <= 103; ++i)
		{
			sprintf(tmp, "Name%d", i);
			o2 << i << tmp;
		}
	}
	void insert_11()
	// insert rows into table
	{
		sub_test_timer timer("insert");

		// If the number of rows to be inserted is not known in advance,
		// another stream with the same INSERT can be opened
		//insert into `tablename1`(`t1`,`t2`,`t3name`) values ( NULL,NULL,NULL)
		gv_database_stream o2(1, // buffer size should be == 1 always on INSERT
							  "insert into test_tab "
							  " values "
							  //" (?,?)",
							  " (:f1<int>,:f2<char[128]>)",
							  //" (:f1,:f2)",
							  //"insert into test_tab(f1,f2) values  (1, 'ttmmd')",
							  //"insert into test_tab values "
							  db // connect object
							  );

		char tmp[32];
		for (int i = 1; i <= 103; ++i)
		{
			sprintf(tmp, "Name%d", i);
			o2 << i << tmp;
		}
	}

	void update(const int af1)
	// insert rows into table
	{
		sub_test_timer timer("update");
		gv_database_stream o(1, // buffer size should be == 1 always on UPDATE
							 "UPDATE test_tab "
							 "   SET f2=:f2<char[31]> "
							 " WHERE f1=:f1<int>",
							 //"   SET f2=:f2 "
							 //" WHERE f1=:f1 ",
							 // UPDATE statement
							 db // connect object
							 );
		o << "Name changed" << af1;
		o << "" << af1 + 1; // set f2 to NULL
	}

	void select(const int af1)
	{
		sub_test_timer timer("select");
		gv_database_stream i(50, // buffer size may be > 1
							 "select * from test_tab "
							 "where f1>=8 ",
							 //"where f1>=:f11<int> "
							 //"  and f1<=:f12<int>*2",
							 // SELECT statement
							 db // connect object
							 );
		// create select stream

		int f1;
		char f2[31];

		//i<<af1<<af1; // :f11 = af1, :f12 = af1
		while (!i.eof())
		{ // while not end-of-data
			i >> f1;
			test_log() << "f1=" << f1 << ", f2=";
			i >> f2;
#if GV_DEBUG_VERSION
			if (i.is_null())
				test_log() << "NULL";
			else
				test_log() << f2;
			test_log() << gv_endl;
#endif
		}
	}

	int main()
	{
		try
		{

			sub_test_timer timer("try");
			db.rlogon("UID=gv_boost_test;PWD=1234;DSN=test_mysql"); // connect to ODBC
			//  db.rlogon("scott/tiger@mysql35"); // connect to ODBC, alternative format
			// of connect string
			db.direct_exec(
				"drop table test_tab",
				0  // disable OTL exceptions
				); // drop table

			db.direct_exec(
				"create table test_tab(f1 int, f2 varchar(30))"); // create table

			//insert(); // insert records into the table
			insert_11(); //slow one?
			update(10);  // update records in the table
			select(8);   // select records from the table
		}

		catch (gv_database_exception& p)
		{								 // intercept OTL exceptions
			cerr << p.what() << gv_endl; // print out error message
		}

		db.logoff(); // disconnect from ODBC

		return 0;
	}
};

void main(gvt_array< gv_string >& args)
{
	the_test test;
	test.main();
}
}
