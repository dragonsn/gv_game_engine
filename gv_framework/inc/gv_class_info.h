#pragma once
namespace gv
{
class gv_func_info;
class gv_var_info;
static const gv_int gvc_invalid_hieararchy_distance = 65536;
typedef void (*gv_callback_construct)(gv_byte*);

//         /
// HHHH   |=====================================================================\
	//H@@@@###|-------------------------[]------------------------------------------->
// HHHH |=====================================================================/
//        \   

class gv_any
{
	friend class gv_class_info;
	friend class gv_sandbox;

public:
	gv_any();
	gv_any(gv_class_info* pcls);
	~gv_any();
	gv_class_info* get_class();
	void set_class(gv_class_info*);
	bool is_equal(const gv_any&);
	bool is_less(const gv_any&);
	gv_int compare(const gv_any&);
	bool is_same_type(const gv_any&);
	gv_byte* get_data();
	void clear();

protected:
	gv_byte* m_data;
	gv_class_info* m_class;
};
#define GVM_ANY_TO(a, cls) \
	((cls*)(a.get_class()->get_name_id() == #cls ? a.get_data() : NULL))
//         /
// HHHH   |=====================================================================\
	//H@@@@###|-------------------------[]------------------------------------------->
// HHHH |=====================================================================/
//        \  

class gv_class_info : public gv_object
{
	friend class gv_sandbox;

public:
	GVM_DCL_CLASS(gv_class_info, gv_object);
	gv_class_info();
	~gv_class_info();
	///
public:
	void clear_var_func_list();
	void set_size(gv_uint size)
	{
		m_size = size;
	}
	void set_function_list(struct gv_func_init_helper* first);
	//============================================================================================
	//								type method
	//============================================================================================
	gv_int get_size()
	{
		return m_size;
	}
	gv_int get_aligned_size()
	{
		return gvt_align(m_size, m_aligment);
	}
	gv_int get_alignment()
	{
		return m_aligment;
	}
	gv_string_tmp get_type_string();
	bool is_equal(gv_byte* psrc, gv_byte* ptgt);
	bool is_less(gv_byte* psrc, gv_byte* ptgt);
	gv_int compare(gv_byte* p1, gv_byte* p2);
	void construct(gv_byte* pdata);
	void destroy(gv_byte* pdata);
	void read_data(gv_byte* pdata, gvi_stream*);
	void write_data(gv_byte* pdata, gvi_stream*);
	void copy_to(const gv_byte* psrc, gv_byte* ptgt);
	void export_to_xml(gv_byte* pdata, gv_string_tmp& text, gv_int indent = 0);
	bool import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
						 bool with_header = true, gv_sandbox* sandbox = NULL,
						 gv_module* module = NULL);
	void export_to_txt(gv_byte* pdata, gv_string_tmp& text);
	bool import_from_txt(gv_byte* pdata, gv_xml_parser* ps,
						 gv_sandbox* sandbox = NULL, gv_module* module = NULL);
	void copy_to(const gv_object* psrc, gv_object* ptgt)
	{
		return copy_to((gv_byte*)psrc, (gv_byte*)ptgt);
	};
	bool import_attribute(gv_byte* pdata, gv_xml_parser* ps, gv_int indent = 0);
	void export_attribute(gv_byte* pdata, gv_string_tmp& text, gv_int indent = 0);
	//============================================================================================
	//								:factory method
	//============================================================================================
	virtual gv_byte* create_instance();
	virtual void delete_instance(gv_byte* pb);
	virtual gv_byte* create_array(gv_int array_size);
	virtual void delete_array(gv_byte* pb, gv_int array_size);
	virtual bool is_complex_type()
	{
		return true;
	};
	gv_object* create_object()
	{
		GV_ASSERT(!this->is_struct());
		gv_object* pobj = (gv_object*)create_instance();
		pobj->m_class = this;
		return pobj;
	};
	void delete_object(gv_object*);
	gv_callback_construct get_native_constructor()
	{
		return m_constructor;
	}
	void set_native_constructor(gv_callback_construct c)
	{
		m_is_native = 1;
		m_constructor = c;
	}
	//============================================================================================
	//								:function related
	//method
	//============================================================================================
	void initialize();
	gv_uint get_nb_func();
	gv_func_info* get_func(gv_uint index);
	gv_func_info* get_func(const gv_id& name);
	gv_int get_func_idx(const gv_id& name);
	bool add_func(gv_func_info* pinfo);
	gv_func_info* get_func_compiler(const gv_id& funcName);
	//============================================================================================
	//								:derivation
	//============================================================================================
	gv_class_info* get_super() const;
	void relink_super();
	void set_super(gv_class_info* pcls);
	bool is_derive_from(const gv_id& name) const;
	bool is_derive_from(gv_class_info*) const;
	void set_super_name(const gv_id& name)
	{
		this->m_super_name = name;
	}
	gv_int get_hierarchy_distance_to(gv_class_info*);
	//============================================================================================
	//								:variable related
	//method
	//============================================================================================
	void visit_ptr(gv_byte*, gvi_object_ptr_visitor& v);
	gv_var_info* get_var(const gv_id& varName);
	gv_var_info* get_var(const char* varName);
	gv_var_info* get_var(gv_int index);
	bool add_var(gv_var_info* pinfo);
	bool load_default(gv_byte* pdata, gv_int size_of_data);
	template < class T >
	inline bool load_default(T* pstruct)
	{
		return this->load_default((gv_byte*)pstruct, (gv_int)sizeof(T));
	}
	bool load_default(gv_object* pobj);
	bool clamp_var(gv_object* pobj); // clamp var if out of range!!
	bool query_property(gv_object* pobj, gv_lexer& s, gv_byte*& pdata,
						gvi_type*& type, gv_string_tmp& tail, gv_id& enum_id);
	gv_var_info* get_primary_key()
	{
		return m_primary_key;
	}

	//============================================================================================
	//								:
	//============================================================================================
	gv_class_info* fetch_completed_class();
	gv_class_info* get_persistent_class();
	gv_int get_nb_var_for_save()
	{
		return m_nb_var_to_serialize;
	};
	gv_int get_nb_var()
	{
		return m_p_var_tbl.size();
	};
	void set_nb_var(gv_int s)
	{
		return m_p_var_tbl.resize(s);
	};
	gv_var_info* get_nth_var(gv_int idx)
	{
		return m_p_var_tbl[idx];
	}

	//============================================================================================
	//								:
	//============================================================================================
	bool is_struct() const
	{
		return m_is_struct;
	};
	void set_is_struct()
	{
		m_is_struct = 1;
	}
	void set_is_copyable(bool b)
	{
		m_is_copyable = b ? 1 : 0;
	}
	bool is_copyable()
	{
		return m_is_copyable;
	}

	//============================================================================================
	//										:member
	//var
	//============================================================================================
	union {
		gv_uint m_class_flags;
		struct
		{
			gv_uint m_is_inited : 1;
			gv_uint m_is_not_completed : 1;
			gv_uint m_is_replaceable : 1;
			gv_uint m_is_final : 1;
			gv_uint m_is_struct : 1;
			gv_uint m_is_event : 1;
			gv_uint m_is_copyable : 1;
		};
	};

protected:
	gvt_ptr< gv_class_info > m_super;
	gvt_ptr< gv_var_info > m_primary_key;
	gv_uint m_size;
	gv_uint m_aligment;
	gvt_array< gvt_ref_ptr< gv_func_info > > m_p_vtl_tbl;
	gvt_array< gvt_ref_ptr< gv_var_info > > m_p_var_tbl;
	gv_id m_super_name;
	gvt_array< gv_id > m_string_tbl; // the resource of the class
	gv_callback_construct m_constructor;
	gvt_ptr< gv_class_info > m_replaced_class;
	gv_uint m_nb_var_to_serialize;

	enum gve_class_flag
	{
		gve_class_flag_native = 1,
		gve_class_flag_is_inited = 2,
		gve_class_flag_is_not_completed = 4,
		gve_class_flag_is_replaceable = 8,
		gve_class_flag_is_final = 16,
		gve_class_flag_is_struct = 32,
		gve_class_flag_is_event = 64,
	};
};

template < class T >
inline bool gvt_load_struct_from_xml_text(T& t, gv_string_tmp& text)
{
	gv_class_info* pcls = T::static_class();
	GV_ASSERT(pcls);
	gv_xml_parser parser;
	parser.load_string(*text);
	return pcls->import_from_xml(gvt_byte_ptr(t), &parser);
}

template < class T >
inline bool gvt_load_option_file(T& option, const char* file_name)
{
	gv_string_tmp text;
	bool find_option = gv_load_file_to_string(file_name, text);
	if (!find_option)
		return gvt_save_option_file(option,file_name);
	return gvt_load_struct_from_xml_text(option, text);
}

template < class T >
inline bool gvt_save_struct_to_xml_string(T& t, gv_string_tmp& text)
{
	gv_class_info* pcls = T::static_class();
	GV_ASSERT(pcls);
	{
		pcls->export_to_xml(gvt_byte_ptr(t), text);
	}
	return true;
}

template < class T >
inline bool gvt_save_option_file(T& option, const char* file_name)
{
	gv_string_tmp text;
	if (gvt_save_struct_to_xml_string(option, text))
		return gv_save_string_to_file(file_name, text);
	else
		return false;
}

template < class T >
inline bool gvt_load_save_data(T& option, const char* file_name)
{
	gv_string_tmp full_path = *gv_global::config.path_for_save_data;
	full_path += file_name;
	return gvt_load_option_file(option, *full_path);
}

template < class T >
inline bool gvt_save_save_data(T& option, const char* file_name)
{
	gv_string_tmp full_path = *gv_global::config.path_for_save_data;
	full_path += file_name;
	return gvt_save_option_file(option, *full_path);
}
}