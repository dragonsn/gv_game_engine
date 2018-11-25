#pragma once
namespace gv
{
//         /
// HHHH   |=====================================================================\
	//H@@@@###|-------------------------[]------------------------------------------->
// HHHH |=====================================================================/
//        \   

class gv_var_info : public gv_object
{
public:
	friend class gv_class_info;
	friend class gv_func_info;
	GVM_DCL_CLASS(gv_var_info, gv_object);
	gv_var_info();
	gv_var_info(gv_sandbox* sandbox, gv_class_info*, const gv_char* var,
				const gv_char* dcl, gv_int native_size,
				const gv_char* range_value, const gv_char* default_value,
				gv_int offset, gv_uint extra_flag);
	gv_var_info(gvi_type* type, gv_char* name);

public:
	//============================================================================================
	//								:
	//============================================================================================
	int get_size();
	int get_offset()
	{
		return this->m_offset;
	};
	int get_end_offset()
	{
		return get_offset() + get_size();
	}
	gvi_type* get_type()
	{
		return this->m_type;
	};
	void initialize();
	gv_int get_index_in_cls()
	{
		return this->m_index;
	}
	void set_default_add_quote(const char* default_string)
	{
		gv_string_tmp s;
		s << "\"" << default_string << "\"";
		set_default(*s);
	}
	void set_default(const char* default_string)
	{
		m_default = default_string;
		if (default_string)
			m_is_with_default = 1;
		else
			m_is_with_default = 0;
	}
	void set_range(const char* range_string)
	{
		m_range = range_string;
		if (range_string)
			m_is_ranged = 1;
		else
			m_is_ranged = 0;
	}
	bool clamp(gv_object* object);
	bool set_var_to_default(gv_byte* object);
	bool set_var_value(gv_byte* object, const gv_byte* value,
					   gv_int size_of_value);
	bool get_var_value(gv_byte* object, gv_byte* value, gv_int size_of_value);
	const gv_id& get_enum_id()
	{
		return m_enum_id;
	}
	void set_enum_id(const char* name)
	{
		m_enum_id = name;
	}
	bool is_enum()
	{
		return !m_enum_id.is_empty();
	}
	void set_tooltip(const gv_char* s)
	{
		m_tooltip = s;
	}
	const gv_text& get_tooltip()
	{
		return m_tooltip;
	}
	//============================================================================================
	//								:
	//============================================================================================
	bool operator==(const gv_var_info& var)
	{
		return this->m_name == var.m_name;
	}
	gv_uint is_transient() const
	{
		return m_is_transient;
	};
	gv_uint is_no_clone() const
	{
		return m_is_no_clone;
	};
	gv_uint is_no_auto_clear() const
	{
		return m_is_no_auto_clear;
	};
	gv_uint is_need_replicate() const
	{
		return m_is_need_replicate;
	};
	gv_uint is_primary_key() const
	{
		return m_is_primary_key;
	}
	// sorry for lazy me , don't want to spend time for many set and get function
	// . ...,don't touch the flags , if you don't know how to play.....
	union {
		//------------------------------------------------------
		struct
		{
			gv_uint m_is_local : 1;			 // internal use
			gv_uint m_is_class_member : 1;   // internal use
			gv_uint m_is_inited : 1;		 // internal use, for test whether the var info is
											 // intialized or not.
			gv_uint m_is_with_default : 1;   // there is a default value for this
											 // variable , but normally , you can set in
											 // constructor
			gv_uint m_is_ranged : 1;		 // there is max and min value for this variable
			gv_uint m_is_need_replicate : 1; // used for network
			gv_uint m_is_read_only : 1;		 // used for the editor,is read only or not
			gv_uint m_is_invisible : 1;		 // used for the editor,is this var visible in
											 // the editor panel
			gv_uint m_is_transient : 1;		 // don't save
			gv_uint m_is_no_clone : 1;		 // don't clone
			gv_uint m_is_no_import : 1;
			gv_uint m_is_no_auto_clear : 1;
			gv_uint m_is_file_name : 1;
			gv_uint m_is_hide_in_editor : 1;
			gv_uint m_is_primary_key : 1;
		};
		gv_uint m_var_flags;
	};

protected:
	gv_int m_offset;
	gvt_ref_ptr< gvi_type > m_type;
	gvt_ptr< gv_class_info > m_p_cls_loc;
	gvt_ptr< gv_func_info > m_p_func_loc;
	gv_int m_native_size;
	gv_int m_index;
	gv_id m_enum_id;
	gv_text m_default; // the default value
	gv_text m_range;   //
	gv_text m_dcl;
	gv_text m_tooltip;
};
}