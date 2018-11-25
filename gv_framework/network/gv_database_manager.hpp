#include "gv_framework_private.h"
#include "gv_framework.h"
#include "gv_base\database\gv_database.h"
#include "gv_database_manager.h"
namespace gv
{
class gv_database_manager_impl : public gv_refable
{
public:
	gv_database_manager_impl()
	{
		connected = false;
	}
	~gv_database_manager_impl()
	{
	}
	gv_database_connection db; // connect object
	gv_string connection;
	gv_bool connected;
};

const char* gv_get_db_type_bind_string(gve_data_type type)
{
	switch (type)
	{
	case gve_data_type_short:
	case gve_data_type_ushort:
	case gve_data_type_byte:
	{
		return "short";
	}
	case gve_data_type_text:
	{
		return "raw_long";
	}
	case gve_data_type_string:
	{
		return "char[128]";
	}
	case gve_data_type_time_stamp:
	{
		return "timestamp";
	}
	case gve_data_type_float:
	{
		return "float";
	}
	case gve_data_type_int:
	{
		return "int";
	}
	case gve_data_type_uint:
	{
		return "unsigned";
	}
	}
	GV_ASSERT(0);
	return "unknown";
};

//============================================================================================
//								:
//============================================================================================
gv_database_manager::gv_database_manager()
{
	GVM_SET_CLASS(gv_database_manager);
	m_dbm_impl = new gv_database_manager_impl;
	m_dbm_impl->connected = false;
	this->set_fixed_delta_time(true, 0.01f);
};

gv_database_manager::~gv_database_manager()
{
	if (m_dbm_impl->connected)
	{
		m_dbm_impl->db.logoff();
	}
};

gv_database_manager_impl* gv_database_manager::get_impl()
{
	return m_dbm_impl;
};

bool gv_database_manager::connect(const char* connect_string)
{
	try
	{
		if (m_dbm_impl->connected)
			m_dbm_impl->db.logoff();
		m_dbm_impl->db.rlogon(connect_string);
		m_dbm_impl->db.set_max_long_size(128 * 1024);
		m_dbm_impl->connected = true;
		m_dbm_impl->connection = connect_string;
		GVM_LOG(database, this->get_name_id() << "connected to " << connect_string
											  << " success !!");
	}
	catch (gv_database_exception& p)
	{
		GVM_LOG(database, this->get_name_id() << "connected to " << connect_string
											  << " failed !!");
		GVM_WARNING(p.what() << gv_endl);
	}
	return m_dbm_impl->connected;
}

bool gv_database_manager::is_connected()
{
	return m_dbm_impl->connected;
};

const char* gv_database_manager::get_connection_string()
{
	return *m_dbm_impl->connection;
};

const gv_id& gv_database_manager::get_database_id()
{
	return gv_id_database;
};

gv_class_info*
gv_database_manager::get_table_struct_info(const gv_id& table_id)
{
	gv_type_array* parray_type;
	gv_class_info* object_type;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, object_type);
	return the_array ? object_type : NULL;
};

gv_string_tmp gv_database_manager::get_db_type_string(gv_var_info* pvar)
{
	gv_string_tmp s;
	s = gv_get_db_type_bind_string(pvar->get_type()->get_type());
	return s;
};

bool gv_database_manager::create_table(const gv_id& table_id, bool drop)
{
	GV_PROFILE_EVENT(gv_database_manager__create_table, 0);
	gv_bool ret = false;
	gv_class_info* table_info = get_table_struct_info(table_id);
	if (!table_info)
		return false;
	if (drop)
	{
		gv_string_tmp s;
		s << "drop table " << table_id;
		m_dbm_impl->db.direct_exec(*s, 0);
	}
	// create table `test`.`TableName1`( `t1` int NOT NULL , `t2` float NOT NULL ,
	// `t3name` varchar(32) NOT NULL , PRIMARY KEY (`t1`, `t3name`))
	//"create table test_tab(f1 int, f2 varchar(30)) type=innoDB"
	try
	{
		gv_string_tmp s;
		s << "create table " << table_id << "(";
		gvt_array_cached< gv_var_info* > pk_cache;
		for (int i = 0; i < table_info->get_nb_var(); i++)
		{
			gv_var_info* pvar = table_info->get_nth_var(i);
			s << pvar->get_name_id() << " " << get_db_type_string(pvar);
			if (i != table_info->get_nb_var() - 1)
				s << " ,";
			if (pvar->is_primary_key())
			{
				pk_cache.push_back(pvar);
			}
		}
		if (pk_cache.size())
		{
			s << ", PRIMARY KEY (";
			for (int i = 0; i < pk_cache.size(); i++)
			{
				s << pk_cache[i]->get_name_id();
				if (i != pk_cache.size() - 1)
					s << " ,";
			}
			s << " )";
		}
		s << ")";
		s << " type=innoDB";
		m_dbm_impl->db.direct_exec(*s);
		GVM_DEBUG_LOG(database, s << gv_endl);
		ret = true;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
	}
	return ret;
};

bool gv_database_manager::read_whole_table(const gv_id& table_id)
{
	GV_PROFILE_EVENT(gv_database_manager__read_whole_table, 0);
	gv_bool ret = false;
	gv_type_array* parray_type;
	gv_class_info* object_type;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, object_type);
	if (!the_array)
		return false;
	try
	{
		gv_string_tmp s;
		s << "select * from " << table_id;
		gv_database_stream i(50, // buffer size may be > 1
							 *s, m_dbm_impl->db);
		// create select stream
		parray_type->resize_array(the_array, 0);
		gv_byte* element = object_type->create_instance();
		while (!i.eof())
		{ // while not end-of-data
			read_from_db(&i, object_type, element);
			parray_type->push_back(the_array, element);
		}
		object_type->delete_instance(element);
		ret = true;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
	}
	return ret;
};

bool gv_database_manager::write_whole_table_to_db(const gv_id& table_id)
{
	GV_PROFILE_EVENT(gv_database_manager__write_whole_table_to_db, 0);
	gv_bool ret = false;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, table_info);
	if (!the_array)
		return false;
	try
	{
		// delete the table
		gv_string_tmp s;
		s << "delete from" << table_id;
		m_dbm_impl->db.direct_exec(*s, 0);
		int array_size = parray_type->get_array_size(the_array);
		for (int i = 0; i < array_size; i++)
		{ // while not end-of-data
			// insert into `tablename1`(`t1`,`t2`,`t3name`) values ( '1','3','asg')
			gv_byte* element = parray_type->get_element(the_array, i);
			s = "insert into ";
			s << table_id;
			s << "( ";
			for (int i = 0; i < (int)table_info->get_nb_var(); i++)
			{
				gv_var_info* pvar = table_info->get_nth_var(i);
				s << pvar->get_name_id();
				if (i != table_info->get_nb_var() - 1)
					s << " ,";
			}
			parray_type->push_back(the_array, element);
			s << ") values ( ";
			for (int i = 0; i < table_info->get_nb_var(); i++)
			{
				gv_var_info* pvar = table_info->get_nth_var(i);
				save_a_var_to_string(s, pvar, element);
				if (i != table_info->get_nb_var() - 1)
					s << " ,";
			}
			s << ")";
			s.replace_char('\"', '\'');
			m_dbm_impl->db.direct_exec(*s);
		}
		ret = true;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
	}
	return ret;
};

void gv_database_manager::find_in_table_with_pk(const gv_id& table_id,
												gv_byte* row,
												gv_int& error_code)
{
	// select * from `test`.`tablename1`   where `t1`='1'
	GV_PROFILE_EVENT(gv_database_manager__find_in_table_with_pk, 0);
	error_code = e_database_error_not_found;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, table_info);
	if (!the_array)
		return;
	try
	{
		gv_string_tmp s;
		s << "select * from " << table_id << "   where ";
		gvt_array_cached< gv_var_info* > pk_cache;
		for (int i = 0; i < table_info->get_nb_var(); i++)
		{
			gv_var_info* pvar = table_info->get_nth_var(i);
			if (pvar->is_primary_key())
			{
				pk_cache.push_back(pvar);
			}
		}
		if (pk_cache.size())
		{
			for (int i = 0; i < pk_cache.size(); i++)
			{
				gv_var_info* pk = pk_cache[i];
				s << pk->get_name_id() << " = ";
				save_a_var_to_string(s, pk, row);
				if (i != pk_cache.size() - 1)
					s << " and ";
			}
			s.replace_char('\"', '\'');
			gv_database_stream i(50, // buffer size may be > 1
								 *s, m_dbm_impl->db);
			if (!i.eof())
			{
				read_from_db(&i, table_info, row);
				error_code = e_database_no_error;
			}
		}
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
		error_code = p.error_code();
	}
};

void gv_database_manager::find_in_memory_with_pk(const gv_id& table_id,
												 gv_byte* row,
												 gv_int& error_code)
{
	GV_PROFILE_EVENT(gv_database_manager__find_in_memory_with_pk, 0);
	error_code = e_database_error_not_found;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, table_info);
	if (!the_array)
		return;
	gv_var_info* pk = table_info->get_primary_key();
	if (pk)
	{
		gv_byte* array_data = parray_type->get_array_data(the_array);
		for (int i = 0; i < parray_type->get_array_size(the_array); i++)
		{
			if (pk->get_type()->is_equal(row + pk->get_offset(),
										 array_data + pk->get_offset()))
			{
				table_info->copy_to(array_data, row);
				error_code = e_database_no_error;
			}
			array_data += parray_type->get_element_size();
		}
	}
};

void gv_database_manager::find_first_in_table(const gv_id& table_id,
											  gv_byte* row,
											  gv_db_query_string condition,
											  gv_int& error_code)
{

	GV_PROFILE_EVENT(gv_database_manager__find_in_table, 0);
	error_code = e_database_error_not_found;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* a = find_table_in_memory(table_id, parray_type, table_info);
	if (!a)
		return;
	try
	{
		gv_string_tmp s;
		s << "select * from " << table_id << "   where " << condition;
		gv_database_stream i(50, *s, m_dbm_impl->db);
		gv_byte* element = row;
		if (!i.eof())
		{
			read_from_db(&i, table_info, element);
			error_code = e_database_no_error;
		}
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
		error_code = p.error_code();
	}
}

void gv_database_manager::find_in_table(const gv_id& table_id,
										gv_byte* the_array,
										gv_db_query_string condition,
										gv_int& error_code)
{
	GV_PROFILE_EVENT(gv_database_manager__find_in_table, 0);
	error_code = e_database_error_not_found;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* a = find_table_in_memory(table_id, parray_type, table_info);
	if (!a)
		return;
	try
	{
		gv_string_tmp s;
		s << "select * from " << table_id << "   where " << condition;
		gv_database_stream i(50, *s, m_dbm_impl->db);
		parray_type->resize_array(the_array, 0);
		gv_byte* element = table_info->create_instance();
		while (!i.eof())
		{
			read_from_db(&i, table_info, element);
			parray_type->push_back(the_array, element);
		}
		table_info->delete_instance(element);
		error_code = e_database_no_error;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
		error_code = p.error_code();
	}
};

void gv_database_manager::find_in_table_with_pk(
	const gv_id& table_id, gv_byte* row, gv_db_field_mask& read_field_mask,
	gv_int& error_code){
	// select t2,t3name from `test`.`tablename1`   where `t1`='1'
};

void gv_database_manager::delete_row_with_pk(const gv_id& table_id,
											 gv_byte* row, gv_int& error_code){

};

void gv_database_manager::update_table_with_pk(const gv_id& table_id,
											   gv_byte* row,
											   gv_int& error_code)
{
	GV_PROFILE_EVENT(gv_database_manager__update_table_with_pk, 0);
	// update `tablename1` set `t1`='1',`t2`='3',`t3name`='asg' where `t1`='1'
	//"UPDATE test_tab   SET f2=:f2<char[31]> WHERE f1=:f1<int>"
	gv_type_array* parray_type;
	gv_class_info* table_info;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, table_info);
	if (!the_array)
		return;
	try
	{
		gv_string_tmp s;
		s << "update " << table_id << "   set ";
		gvt_array_cached< gv_var_info* > pk_cache;
		for (int i = 0; i < (int)table_info->get_nb_var(); i++)
		{
			gv_var_info* pvar = table_info->get_nth_var(i);
			if (pvar->is_primary_key())
			{
				pk_cache.push_back(pvar);
				continue;
			}
			{
				save_a_var_data_bind(s, pvar);
				if (i != table_info->get_nb_var() - 1)
					s << " , ";
			}
		}
		if (pk_cache.size())
		{
			s << " where ";
			for (int i = 0; i < pk_cache.size(); i++)
			{
				gv_var_info* pk = pk_cache[i];
				s << pk->get_name_id() << " = ";
				this->save_a_var_to_string(s, pk, row);
				if (i != pk_cache.size() - 1)
					s << " and ";
			}
			s.replace_char('\"', '\'');
			;
			gv_database_stream i(1, *s, m_dbm_impl->db);
			gv_byte* element = row;
			write_row_to_db(&i, table_info, row, true);
			error_code = e_database_no_error;
		}
		else
			error_code = e_database_error_not_found;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
		error_code = p.error_code();
	}
};

void gv_database_manager::insert_to_table(const gv_id& table_id, gv_byte* row,
										  gv_int& error_code)
{
	// insert into `tablename1`(`t1`,`t2`,`t3name`) values ( '1','3','asg')
	GV_PROFILE_EVENT(gv_database_manager__insert_to_table, 0);
	error_code = e_database_error_not_found;
	gv_type_array* parray_type;
	gv_class_info* table_info;
	;
	gv_byte* the_array = find_table_in_memory(table_id, parray_type, table_info);
	if (!the_array)
		return;
	try
	{
		gv_string_tmp s;
		s << "insert into  " << table_id << "   ( ";
		for (int i = 0; i < table_info->get_nb_var(); i++)
		{
			gv_var_info* pvar = table_info->get_nth_var(i);
			if (pvar->is_primary_key())
				continue; // skip auto increased
			s << pvar->get_name_id();
			if (i != table_info->get_nb_var() - 1)
				s << " , ";
		}
		s << ") values (";
		for (int i = 0; i < table_info->get_nb_var(); i++)
		{
			gv_var_info* pvar = table_info->get_nth_var(i);
			if (pvar->is_primary_key())
				continue; // skip auto increased
			save_a_var_to_string(s, pvar, row);
			if (i != table_info->get_nb_var() - 1)
				s << " , ";
		}
		s << ")";
		m_dbm_impl->db.direct_exec(*s);
		error_code = e_database_no_error;
	}
	catch (gv_database_exception& p)
	{
		GVM_WARNING(p.what() << gv_endl);
		error_code = p.error_code();
	}
};

gv_byte*
gv_database_manager::find_table_in_memory(const gv_id& table_id,
										  gv_type_array*& parray_type,
										  gv_class_info*& type_element)
{
	gv_var_info* pvar = this->get_class()->get_var(table_id);
	if (!pvar)
		return NULL;
	parray_type = gvt_cast< gv_type_array >(pvar->get_type());
	if (!parray_type)
		return NULL;
	gv_type_object* pobject_type =
		gvt_cast< gv_type_object >(parray_type->get_element_type());
	if (!pobject_type)
		return NULL;
	type_element = pobject_type->get_class();
	gv_byte* the_array = (gv_byte*)this + pvar->get_offset();
	return the_array;
};

void gv_database_manager::read_from_db(gvi_stream* ps, gv_class_info* pcls,
									   gv_byte* object)
{
	for (int i = 0; i < pcls->get_nb_var(); i++)
	{
		gv_var_info* pvar = pcls->get_nth_var(i);
		pvar->get_type()->read_data(object + pvar->get_offset(), ps);
	}
};

void gv_database_manager::write_row_to_db(gvi_stream* ps, gv_class_info* pcls,
										  gv_byte* object, bool ignore_pk)
{

	for (int i = 0; i < pcls->get_nb_var(); i++)
	{
		gv_var_info* pvar = pcls->get_nth_var(i);
		if (pvar->is_primary_key() && ignore_pk)
			continue;
		pvar->get_type()->write_data(object + pvar->get_offset(), ps);
	}
};

void gv_database_manager::begin_transaction()
{
	get_impl()->db.begin_transaction();
};
void gv_database_manager::end_transaction()
{
	get_impl()->db.end_transaction();
};
void gv_database_manager::rollback_transaction()
{
	get_impl()->db.rollback();
};

void gv_database_manager::save_a_var_to_string(gv_string_tmp& s,
											   gv_var_info* pvar,
											   gv_byte* row)
{
	if (pvar->get_type()->get_type() != gve_data_type_text)
	{
		pvar->get_type()->export_to_xml(row + pvar->get_offset(), s);
	}
	else
	{
		s << "\"" << *((gv_text*)(row + pvar->get_offset())) << "\"";
	}
};

void gv_database_manager::save_a_var_data_bind(gv_string_tmp& s,
											   gv_var_info* pvar)
{
	// f2=:f2<char[31]>
	s << pvar->get_name_id() << "=:" << pvar->get_name_id() << "<"
	  << get_db_type_string(pvar) << "> ";
};

//============================================================================================
//								:
//============================================================================================
GVM_IMP_STRUCT(gv_unit_test_table)
GVM_VAR_ATTRIB_SET(primary_key)
GVM_VAR(gv_int, id)
GVM_VAR_ATTRIB_UNSET(primary_key)
GVM_VAR(gv_string, name)
GVM_VAR(gv_time_stamp, join_date)
GVM_VAR(gv_float, weight)
GVM_VAR(gv_short, age)
GVM_VAR(gv_short, iq)
GVM_VAR(gv_time_stamp, last_login_date)
GVM_VAR(gv_text, image)
GVM_END_STRUCT

GVM_IMP_STATIC_CLASS(gv_unit_test_table)

GVM_IMP_CLASS(gv_database_manager, gv_event_processor)
GVM_VAR(gvt_array< gv_unit_test_table >, m_test_table)
GVM_END_CLASS
}