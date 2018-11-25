

namespace unit_test_database_stored_proc
{

/*
	gv_database_stream
	s(50,
	"{ "
	" my_proc1(:f1<int,in>) "
	"   :#1<int,out> "
	"   :#2<char[31],out> "
	"}",
	db, // otl_connect object
	otl_implicit_select // implicit result set / select flag
	);
	*/

struct the_test
{
	gv_database_connection db; // connect object

	void stored_proc(void)
	// invoking stored procedure
	{
		gv_database_stream o(1, // buffer size should be equal to 1 in case of stored procedure call
							 "{call my_proc("
							 " :A<int,    in>, "
							 " :C<char[31],in> "
							 ")}",
							 // stored procedure call
							 db, // connect object
							 gvc_implicit_select);

		o.set_commit(0); // set stream auto-commit off since
		// the stream does not generate transaction

		o << 1 << "Test String1"; // assigning :1 = 1, :3 = "Test String1"

		int a;
		char b[31];
		o >> a >> b;
		test_log() << "A=" << a << ", B=" << b << gv_endl;
	}

	int main()
	{
		try
		{
			db.rlogon("UID=gv_boost_test;PWD=1234;DSN=test_mysql"); // connect to data source MY_DB
			db.direct_exec("drop  procedure if exists my_proc;");
			db.direct_exec(
				"CREATE  PROCEDURE my_proc \r\n "
				"  (in	   A  int, \r\n"
				"   in     C  varchar(20)  \r\n"
				") "
				"BEGIN \r\n"
				"   set A = A+1; \r\n"
				"   set @B = C; \r\n"
				"   select A, C;"
				"END ;\r\n"); // create stored procedure

			stored_proc(); // invoking stored procedure
		}
		catch (gv_database_exception& p)
		{								 // intercept OTL exceptions
			cerr << p.what() << gv_endl; // print out error message
		}
		db.logoff(); // disconnect from the data source
		return 0;
	}
};

void main(gvt_array< gv_string >& args)
{
	the_test test;
	test.main();
}
}

/*
[intrinsic column flags]
- PK: primary key (column is part of a pk)
- NN: not null (column is nullable)
- UQ: unique (column is part of a unique key)
- AI: auto increment (the column is auto incremented when rows are inserted)

[additional data type flags, depend on used data type]
- BIN: binary (if dt is a blob or similar, this indicates that is binary data, rather than text)
- UN: unsigned (for integer types, see docs: "10.2. Numeric Types")
- ZF: zero fill (rather a display related flag, see docs: "10.2. Numeric Types")

*/
namespace unit_test_database_stored_proc2
{

struct the_test
{
	gv_database_connection db; // connect object

	void stored_proc(void)
	// invoking stored procedure
	{
		gv_database_stream cur(1, // buffer size should be equal to 1 in case of stored procedure call
							   "{call addchrinfo(:uid<int,in>,:rid<short,in>,:n<char[20],in>,:rc<short,in>,:sx<short,in>,:expv<int,in>,:mv<int,in>)}",
							   // stored procedure call
							   db, // connect object
							   gvc_implicit_select);

		//gv_database_stream cur( 50, DBPROC_CHRADDINFO, *pconn, otl_implicit_select);
		cur.set_commit(0);
		int uid = 1;
		short rid = 0;
		short sx = 0;
		short rc = 1;
		short sv = 1;
		cur << uid << rid << "wang9" << rc << sv << 1000 << 1000;
		int ret_;
		if (!cur.eof())
		{
			cur >> ret_;
			if (ret_ == -1)
				ret_ = 4;
			else if (ret_ == -2)
				ret_ = 2;
			else if (ret_ == -3)
				ret_ = 3;
			else
			{
				int chrid_;
				char char_name[1024];
				int exp_, level_, moneys_;

				cur >> chrid_ >> char_name >> rc >> sv >> exp_ >> level_ >> moneys_;
			}
		}
		else
			ret_ = 3;

		cur.close();
	}

	int main()
	{
		try
		{
			db.rlogon("UID=gv_boost_test;PWD=1234;DSN=test_mysql"); // connect to data source MY_DB
			stored_proc();											// invoking stored procedure
		}
		catch (gv_database_exception& p)
		{								 // intercept OTL exceptions
			cerr << p.what() << gv_endl; // print out error message
			GVM_DEBUG_OUT(p.what());
		}
		db.logoff(); // disconnect from the data source
		return 0;
	}
};

void main(gvt_array< gv_string >& args)
{
	the_test test;
	test.main();
}
}

namespace unit_test_database_stored_proc_3
{

struct the_test
{
	gv_database_connection db; // connect object

	void insert()
	// insert rows into table
	{
		gv_database_stream o(1, // buffer size
							 "insert into test_tab values(:f1<float>,:f2<char[31]>)",
							 // SQL statement
							 db // connect object
							 );
		char tmp[32];

		for (int i = 1; i <= 100; ++i)
		{
			sprintf(tmp, "Name%d", i);
			o << (float)i << tmp;
		}
	}

	void select(void)
	// fetching result set returned by a stored procedure
	{
		gv_database_stream i(50, // buffer size
							 "{call my_proc(:f1<int,in>)}",
							 // implicit SELECT statement
							 db,				 // connect object
							 gvc_implicit_select // implicit SELECT statement
							 );
		// create select stream

		int f1;
		char f2[31];

		i << 8; // assigning :f1 = 8
		// SELECT automatically executes when all input variables are
		// assigned. First portion of output rows is fetched to the buffer

		while (!i.eof())
		{ // while not end-of-data
			i >> f1 >> f2;
			test_log() << "f1=" << f1 << ", f2=" << f2 << gv_endl;
		}

		i << 4; // assigning :f1 = 4
		// SELECT automatically re-executes when all input variables are
		// assigned. First portion of out rows is fetched to the buffer

		while (!i.eof())
		{ // while not end-of-data
			i >> f1 >> f2;
			test_log() << "f1=" << f1 << ", f2=" << f2 << gv_endl;
		}
	}

	int main()
	{
		try
		{
			db.rlogon("uid=scott;pwd=tiger;dsn=mssql"); // connect to ODBC
			db.direct_exec(
				"drop table test_tab",
				0  // disable OTL exceptions
				); // drop table

			db.direct_exec(
				"create table test_tab(f1 int, f2 varchar(30))"); // create table

			db.direct_exec(
				"DROP PROCEDURE my_proc",
				0  // ignore any errors
				); // drop stored procedure

			db.direct_exec(
				"CREATE PROCEDURE my_proc "
				"  @F1 int "
				"AS "
				"SELECT * FROM test_tab "
				"WHERE f1>=@F1 AND f1<=@F1*2 "); // create stored procedure

			insert(); // insert records into table
			select(); // select records from table
		}
		catch (gv_database_exception& p)
		{								 // intercept OTL exceptions
			cerr << p.what() << gv_endl; // print out error message
			GVM_DEBUG_OUT(p.what());
		}
		db.logoff(); // disconnect from the data source
		return 0;
	}
};

void main(gvt_array< gv_string >& args)
{
	the_test test;
	test.main();
}
}