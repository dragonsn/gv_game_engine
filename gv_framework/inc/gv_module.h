#pragma once
namespace gv
{
//============================================================================================
//								:
//============================================================================================
class gv_object_location_info
{
public:
	gv_object_location_info()
	{
		m_refill_ptr_list.p = NULL;
		m_refill_ref_ptr_list.p = NULL;
		m_offset_in_file = 0;
	}
	struct pointer_list
	{
		union {
			pointer_list* pnext;
			void* p;
		};
	};
	gvt_array< gv_object_name > m_location;
	gvt_ptr< gv_object > m_resolved_address;
	gv_int m_offset_in_file;
	pointer_list m_refill_ptr_list;
	pointer_list m_refill_ref_ptr_list;
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gv_object_location_info& info)
{
	s << info.m_location;
	s << info.m_offset_in_file;
	return s;
}

template < class type_of_stream >
inline type_of_stream& operator>>(type_of_stream& s,
								  gv_object_location_info& info)
{
	s >> info.m_location;
	s >> info.m_offset_in_file;
	return s;
}

class gv_module_type_info
{
public:
	gv_text m_type_string;
	gv_int m_index;
	gvt_ptr< gvi_type > m_type;
};

template < class type_of_stream >
inline type_of_stream& operator<<(type_of_stream& s,
								  const gv_module_type_info& info)
{
	s << info.m_type_string;
	return s;
}

template < class type_of_stream >
inline type_of_stream& operator>>(type_of_stream& s,
								  gv_module_type_info& info)
{
	s >> info.m_type_string;
	return s;
}

//============================================================================================
//								:
//============================================================================================
class gv_world;
class gv_module_imp;
class gv_module : public gv_object
{
	friend class gv_sandbox;
	friend class gv_module_imp;

public:
	GVM_DCL_CLASS(gv_module, gv_object);
	gv_module();
	~gv_module();

	gv_int register_import_ref_ptr(const gv_object_location& loc,
								   gvt_ref_ptr< gv_object >& reference);
	gv_int register_import_ref_ptr(gv_int index,
								   gvt_ref_ptr< gv_object >& reference);
	gv_int register_import_ptr(const gv_object_location& loc,
							   gvt_ptr< gv_object >& reference);

	gv_int register_export_ref_ptr(const gv_object_location& loc,
								   gvt_ref_ptr< gv_object >& reference);
	gv_int register_export_ptr(const gv_object_location& loc,
							   gvt_ptr< gv_object >& reference);

	void refresh_import_reference();
	void refresh_export_reference();
	void clear_import_export_reference();

	gv_int object_to_index(gv_object*, bool add_entry = false);
	gv_object* index_to_object(gv_int index);
	gv_int type_to_index(gvi_type* type, bool add_entry = false);
	gvi_type* index_to_type(gv_int index);
	gv_object_location index_to_object_location(gv_int index);
	void update_offset(gv_object* object, gv_int offset);
	gvi_stream* get_size_counter();
	virtual bool read(gvi_stream*, gvt_array< gv_object* >& objects);
	virtual bool write(gvi_stream*);
	//============================================================================================
	//								:
	//============================================================================================
	gv_world* get_world()
	{
		return m_world;
	};
	void set_world(gv_world* world)
	{
		GV_ASSERT(!m_world);
		m_world = world;
	}
	gv_int get_module_version()
	{
		return m_module_version;
	}

protected:
	bool do_write(gvi_stream* ps, bool create_imp_exp = false);
	void prepare_imp_exp();
	gv_module_imp* get_imp();
	void rebuild_imp_exp_map();
	//============================================================================================
	//								:data here
	//============================================================================================
	gv_bool m_load_completed;
	gv_int m_module_version;
	gvt_array< gv_object_location_info > m_export_table;
	gvt_array< gv_object_location_info > m_import_table;
	gvt_array< gv_module_type_info > m_type_table; // types used by this module
	gv_id m_tag;								   // used to set save directory .
	gvt_ptr< gv_module_imp > m_pimpl;
	gvt_ptr< gv_world > m_world;
};
}