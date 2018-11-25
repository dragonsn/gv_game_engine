#pragma once
namespace gv
{
//============================================================================================
//								:
//============================================================================================
class gv_database_manager_impl;
static const gv_int gvc_max_db_table_colums = 32;
typedef gvt_bool_array< 64 > gv_db_field_mask;
typedef gv_string128 gv_db_query_string;
//============================================================================================

enum gve_database_error_code
{
	e_database_no_error = 0,
	e_database_error_not_found = 1,
	e_database_error_wrong_format = 2,
	e_database_error_key_not_unique = 3,
	e_database_error_invalid_id_for_user_name = 4,
	e_database_error_invalid_id = 5,
	e_database_error_code_incompatible_data = 32000,
	e_database_error_code_no_input_variables_defined = 32004,
	e_database_error_code_not_all_input_initialized = 32003,
	e_database_error_code_row_must_be_flushing_output_stream = 32001,
	e_database_error_code_input_string_too_large = 32005,
	e_database_error_code_input_long_string_too_large = 32006,
	e_database_error_code_table_size_too_large = 32007,
	e_database_error_code_7 = 32008,
	e_database_error_code_8 = 32009,
	e_database_error_code_9 = 32010,
	e_database_error_code_10 = 32011,
	e_database_error_code_11 = 32012,
	e_database_error_code_12 = 32013,
	e_database_error_code_13 = 32014,
	e_database_error_code_14 = 32015,
	e_database_error_code_15 = 32016,
	e_database_error_code_16 = 32017,
	e_database_error_code_17 = 32018,
	e_database_error_code_18 = 32019,
	e_database_error_code_19 = 32020,
	e_database_error_code_20 = 32021,
	e_database_error_code_21 = 32022,
	e_database_error_code_22 = 32023,
	e_database_error_code_23 = 32024,
	e_database_error_code_24 = 32025,
	e_database_error_code_25 = 32026,
	e_database_error_code_26 = 32027,
	e_database_error_code_27 = 32028,
	e_database_error_code_28 = 32029,
	e_database_error_code_29 = 32030,
	e_database_error_code_30 = 32031,
	e_database_error_code_31 = 32032,
	e_database_error_code_32 = 32033,
	e_database_error_code_33 = 32034,
	e_database_error_code_34 = 32035,
	e_database_error_code_35 = 32036,
};

class gv_unit_test_table
{
public:
	GVM_WITH_STATIC_CLS;
	gv_unit_test_table(){};
	~gv_unit_test_table(){};
	gv_string name;
	gv_int id;
	gv_time_stamp join_date;
	gv_time_stamp last_login_date;
	gv_float weight;
	gv_short age;
	gv_short iq;
	gv_text image;

	gv_atomic_count done;
	gv_int error_code;
};

// http://otl.sourceforge.net/otl3_bind_variables.htm
const char* gv_get_db_type_bind_string(gve_data_type type);

class gv_database_manager : public gv_event_processor
{
public:
	GVM_DCL_CLASS(gv_database_manager, gv_event_processor);
	gv_database_manager();
	~gv_database_manager();

public:
	virtual bool connect(const char* connect_string);
	virtual bool is_connected();
	virtual const char* get_connection_string();
	virtual const gv_id& get_database_id();
	virtual gv_class_info* get_table_struct_info(const gv_id& table_id);

	virtual bool create_table(const gv_id& table_id, bool drop = true);
	virtual bool read_whole_table(const gv_id& table_id);
	virtual bool write_whole_table_to_db(const gv_id& table_id);

	template < class T >
	inline bool find_in_table_with_pk(const gv_id& table_id, T& row)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set by app
		if (!row.done.get())
		{
			find_in_table_with_pk(table_id, (gv_byte*)&row, row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

	template < class T >
	inline bool find_in_table(const gv_id& table_id, gvt_array< T >& result,
							  gv_db_query_string condition, gv_int& error_code,
							  gv_atomic_count* signal = NULL)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		if (!signal || !signal->get())
		{
			find_in_table(table_id, (gv_byte*)&result, condition, error_code);
		}
		if (signal)
		{
			signal->set(1);
		}
		return error_code == e_database_no_error;
	};

	template < class T >
	inline bool find_first_in_table(const gv_id& table_id, T& row,
									gv_db_query_string condition)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set by app
		if (!row.done.get())
		{
			find_first_in_table(table_id, (gv_byte*)&row, condition, row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

	template < class T >
	inline void find_in_table_with_pk(const gv_id& table_id, T& row,
									  gv_db_field_mask& read_field_mask)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set  by app
		if (!row.done.get())
		{
			find_in_table_with_pk(table_id, (gv_byte*)&row, read_field_mask,
								  row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

	template < class T >
	inline bool update_table_with_pk(const gv_id& table_id, T& row)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set by app
		if (!row.done.get())
		{
			update_table_with_pk(table_id, (gv_byte*)&row, row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

	template < class T >
	inline bool delete_row_with_pk(const gv_id& table_id, T& row)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set by app
		if (!row.done.get())
		{
			delete_row_with_pk(table_id, (gv_byte*)&row, row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

	template < class T >
	inline bool insert_to_table(const gv_id& table_id, T& row)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		// row.done.set(0); set by app
		if (!row.done.get())
		{
			insert_to_table(table_id, (gv_byte*)&row, row.error_code);
		}
		row.done.set(1);
		return row.error_code == e_database_no_error;
	}

	template < class T >
	inline bool find_in_memory_with_pk(const gv_id& table_id, T& row)
	{
		GV_ASSERT(get_table_struct_info(table_id) == T::static_class());
		find_in_memory_with_pk(table_id, (gv_byte*)&row, row.error_code);
		row.done.set(1);
		return row.error_code == e_database_no_error;
	};

protected:
	virtual void find_in_table(const gv_id& table_id, gv_byte* result,
							   gv_db_query_string condition, gv_int& error_code);
	virtual void find_first_in_table(const gv_id& table_id, gv_byte* row,
									 gv_db_query_string condition,
									 gv_int& error_code);
	virtual void find_in_table_with_pk(const gv_id& table_id, gv_byte* row,
									   gv_int& error_code);
	virtual void find_in_memory_with_pk(const gv_id& table_id, gv_byte* row,
										gv_int& error_code);
	virtual void find_in_table_with_pk(const gv_id& table_id, gv_byte* row,
									   gv_db_field_mask& read_field_mask,
									   gv_int& error_code);
	virtual void update_table_with_pk(const gv_id& table_id, gv_byte* row,
									  gv_int& error_code);
	virtual void delete_row_with_pk(const gv_id& table_id, gv_byte* row,
									gv_int& error_code);
	virtual void insert_to_table(const gv_id& table_id, gv_byte* row,
								 gv_int& error_code);
	virtual void read_from_db(gvi_stream* ps, gv_class_info* pcls,
							  gv_byte* object);
	virtual void write_row_to_db(gvi_stream* ps, gv_class_info* pcls,
								 gv_byte* object, bool ignore_pk = true);
	virtual void begin_transaction();
	virtual void end_transaction();
	virtual void rollback_transaction();

	gv_string_tmp get_db_type_string(gv_var_info* pvar);
	gv_byte* find_table_in_memory(const gv_id& table_id,
								  gv_type_array*& array_type,
								  gv_class_info*& type_element);
	void save_a_var_to_string(gv_string_tmp& s, gv_var_info* pvar, gv_byte* row);
	void save_a_var_data_bind(gv_string_tmp& s, gv_var_info* pvar);

protected:
	gv_database_manager_impl* get_impl();
	gvt_ref_ptr< gv_database_manager_impl > m_dbm_impl;
	gvt_array< gv_unit_test_table > m_test_table;
};
}