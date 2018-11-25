#pragma once
namespace gv
{
//=================================================================================>
//	the runtime context of the game engine ,
//	one process can hold many sandbox to support multiple instance game
//	it's a object container , message handler container, system resource
//container
//  and is running independently and parallel with other sand box.
//	sandbox 0  is a special base sandbox,
//		it's a base sandbox initialize at the beginning and destroyed when
//application quit
//		it's a read only sandbox, all the objects in sand box 0 should be
//static
//	sandbox 1   used in client and in most tools.
//	sandbox 1...sandbox max_sand_box used in server cluster.
//=================================================================================>
enum gve_module_version
{
	gve_version_opt_array_sl = 0x00010000
};

static const gv_uint gvc_cu_module_version = 0x00010000;
static const gv_uint gvc_cu_xml_module_version = 0x00010000;
static const char* gvc_gv_binary_identity = "gv_bin";
static const char* gvc_gv_xml_identity = "gv_xml";
static const char* gvc_gv_xml_ext = "xml";
static const char* gvc_gv_bin_ext = "gvb";
static const gv_int gvc_max_object_hash = 4096;
static const gv_int gvc_max_object_event_channel = 1024;

extern bool gv_import_from_xml(class gv_class_info* pcls, gv_byte* pb,
							   const gv_string_tmp& file_name);
extern bool gv_export_to_xml(class gv_class_info* pcls, gv_byte* pb,
							 const gv_string_tmp& file_name);
extern bool gv_import_from_xml_in_memory(class gv_class_info* pcls, gv_byte* pb,
										 const gv_string_tmp& xml_content);

class gv_object;
class gv_any;
class gv_class_info;
class gv_sandbox_data;
class gv_module;
class gv_event_processor;
class gv_object_iterator;
class gv_job_base;

class gv_sandbox : public gv_refable
{
	friend class gv_sandbox_manager;
	friend class gv_var_info;
	friend class gv_func_info;
	friend class gv_object_iterator;
	friend class gv_modele;

protected:
	gv_sandbox();
	~gv_sandbox();

public:
	//------------------------------------------------------------->
	static const gv_ushort invalid_index = gvc_max_sand_box_nb;
	//------------------------------------------------------------->
	bool tick();
	bool pre_tick();
	bool post_tick();
	bool do_tick();
	// this is the real delta time between each tick, don't take the fixed delta
	// time & pause into consideration.
	gv_float get_delta_time();
	gv_double get_time_in_seconds();
	gv_ulong get_perf_count();
	gv_int get_tick_count()
	{
		return m_tick_count;
	}

	gv_ushort get_index();			// the index of the sandbox,
	gv_uint get_runtime_uuid();		// the unique id of the sandbox , it 's possible
									// the old sandbox has the same id
	gv_ushort get_owner_index();	//
	gv_sandbox_handle get_handle(); //
	gv_sandbox* get_outer();		// get the parent sandbox
	void set_outer(gv_sandbox*);
	//---object  manager-------------------------------------------->
	gv_object_iterator begin();
	gv_object_iterator end();
	gv_uint get_nb_object();
	gv_object* get_object(const gv_object_handle& handle);
	bool is_valid(const gv_object_handle& handle);
	bool is_valid(const gv_object* obj) const;
	void add_object(gv_object* obj);
	gv_object*
	find_object(const gv_object_location& location,
				bool find_in_outer =
					true); // find the object in this sandbox and in base sand box
	inline gv_object* find_object(const gv_string_tmp& s)
	{
		gv_object_location loc;
		// gv_xml_parser parser; 			parser.goto_string(*s);
		// parser>>loc;
		loc.from_sting(s);
		return find_object(loc);
	}
	template < class T >
	inline T* find_object(const gv_string_tmp& s, bool do_assert = true)
	{
		T* ret = gvt_cast< T >(find_object(s));
		if (do_assert && !ret)
		{
			GV_ASSERT(0 && "failed to find object ");
		}
		return ret;
	}
	template < class T >
	inline T* find_object(const gv_object_location& s, bool do_assert = true)
	{
		T* ret = gvt_cast< T >(find_object(s));
		if (do_assert && !ret)
		{
			GV_ASSERT(0 && "failed to find object ");
		}
		return ret;
	}
	gv_object*
	load_object(const gv_object_location& location); // find & load the object
	gv_object* create_object(gv_class_info* class_info,
							 const gv_object_name& name, gv_object* outer);
	gv_object* create_object(gv_class_info* class_info, gv_object* outer = NULL);

	gv_object* clone(gv_object* pobj);
	int delete_object(gv_object* pobj, bool force_clear = false);
	int delete_object_tree(gv_object* top, bool clear_reference = true);
	gv_mutex get_obj_mutex();
	void rename_object(gv_object* obj, const gv_object_name new_name,
					   bool need_unique = false);
	template < class T >
	inline T* create_object(const gv_object_name& name, gv_object* pout = NULL)
	{
		GV_ASSERT(T::static_class()->m_is_native)
		return gvt_cast< T >(create_object(T::static_class(), name, pout));
	}
	template < class T >
	inline T* create_object(gv_object* pout = NULL)
	{
		GV_ASSERT(T::static_class());
		GV_ASSERT(T::static_class()->m_is_native)
		return gvt_cast< T >(create_object(
			T::static_class(),
			this->get_unique_object_name(T::static_class()->get_name_id()), pout));
	}
	template < class T >
	inline T* clone_object(T* in)
	{
		return gvt_cast< T >(clone(in));
	}
	template < class T >
	inline gv_bool import_object(const char* file_name, T& object)
	{

		return gv_import_from_xml(T::static_class(), gvt_byte_ptr(object),
								  file_name);
	}

	template < class T >
	inline gv_bool export_object(const char* file_name, T& object)
	{
		return gv_export_to_xml(T::static_class(), gvt_byte_ptr(object), file_name);
	}

	gv_object_name get_unique_object_name(const gv_id& s);
	// nameless object is a temp runtime objects, not able to save & locate,
	gv_object* create_nameless_object(gv_class_info* class_info);
	template < class T >
	inline T* create_nameless_object()
	{
		GV_ASSERT(T::static_class()->m_is_native)
		return gvt_cast< T >(create_nameless_object(T::static_class()));
	}
	inline gv_object* create_nameless_object(const gv_id cls_name)
	{
		gv_class_info* pclass = find_class(cls_name);
		if (!pclass)
			return NULL;
		return create_nameless_object(pclass);
	};
	template < class T >
	inline T* create_object_event()
	{
		return new T;
	}
	class gv_importer_exporter* get_impexp(const gv_string_tmp& file_name);
	bool import_external_format(gv_object* pobj, const gv_string_tmp& file_name,
								const gv_string_tmp& configuration = "");
	gv_object* import_external_format(const gv_string_tmp& file_name,
									  const gv_string_tmp& configuration = "");
	bool force_reimport(const gv_object_location& location);
	bool export_external_format(gv_object* pobj, const gv_string_tmp& file_name);
	bool query_objects_owned_by(gv_object* owner, gvt_array< gv_object* >& result);
	//---class  manager-------------------------------------------->
	gv_uint get_nb_class();
	gv_class_info* find_class(const gv_id& name);
	void replace_class(const gv_id& name, gv_class_info* new_info);
	gv_class_info* register_class(const gv_id& name);
	void unregister_class(const gv_id& name);
	void init_classes();
	void add_class(gv_class_info* pcls);
	void remove_class(const gv_id& name);
	void query_classes_derived_from(const gv_id& super_name,
									gvt_array< gv_class_info* >& result);
	void query_classes_with_prefix(const char* prefix,
								   gvt_array< gv_class_info* >& result);
	void init_persistent_type();
	gvi_type* create_type(const char* string);
	gvi_type* create_type(class gv_xml_parser* ps);
	gvi_type* create_type(gve_data_type type);
	bool import_xml_dom_attrib(gv_xml_parser* parser, gv_any& data,
							   gv_bool* pclosed = NULL);
	gv_class_info* import_xml_element_simple(gv_xml_parser* parser,
											 gv_byte*& data, gv_bool& closed);
	void export_xml_element_simple(gv_string_tmp& text, gv_class_info* pcls,
								   gv_byte* data);
	//----
	gv_enum_info* register_enum(const gv_id& name);
	gv_enum_info* find_enum(const gv_id& name);
	//---object event------------------------------------------------>
	bool register_processor(gv_class_info* cls,
							gv_int channel = gve_event_channel_default);
	bool register_processor(const gv_id& cls_id,
							gv_int channel = gve_event_channel_default);
	bool register_processor_group(const gv_id& cls_id, gv_int start_channel,
								  gv_int channel_number, bool synchronized,
								  bool autonomous);
	void set_processor(gv_event_processor* p,
					   gv_int channel = gve_event_channel_default);
	bool unregister_processor(gv_int channel = gve_event_channel_default);
	bool is_valid_event(gv_object_event* event);
	gv_event_processor* get_event_processor(gv_int channel);

	template<class typeof_processor> 
	typeof_processor * get_event_processor()
	{
		return gvt_cast<typeof_processor>(get_event_processor(typeof_processor::static_channel()));
	}
	template<class typeof_processor>
	typeof_processor * register_processor()
	{
		 register_processor(typeof_processor::static_class(),typeof_processor::static_channel());
		 return get_event_processor<typeof_processor>();
	}

	bool is_channel_open(gv_int channel)
	{
		return get_event_processor(channel) != NULL;
	}
	void stop_event_processor();
	bool share_event_processors(gv_sandbox* b);

	bool
	process_event(gv_object_event* event,
				  gv_int channel = gve_event_channel_default); // not thread safe
	bool process_named_event(
		gv_object_event* event,
		gv_int channel = gve_event_channel_default); // not thread safe
	bool post_event(gv_object_event* event,
					gv_int channel = gve_event_channel_default);
	bool post_job(gv_job_base* job, gv_int channel = gve_event_channel_default);
	bool post_job1(gv_job_base* job, gv_int channel = gve_event_channel_default);

	void register_event(gv_object_event_id id,
						gvi_object_event_handler_info* handler,
						gv_int channel = gve_event_channel_default);
	void unregister_event(gv_object_event_id id, gv_object* object,
						  gv_int channel = gve_event_channel_default);

	void register_named_event(gv_id id, gvi_object_event_handler_info* handler,
							  gv_int channel = gve_event_channel_default);
	void unregister_named_event(gv_id id, gv_object* object,
								gv_int channel = gve_event_channel_default);
	//--object property manipulate event----------------------------------->
	bool find_global_property(const gv_string_tmp& location, gv_byte*& pdata,
							  class gvi_type*& type, gv_object*& outer_object,
							  gv_string_tmp& tail_tag, gv_id& enum_id);
	bool assign_global_property_binary(const gv_string_tmp& location,
									   gv_byte* new_value);
	bool assign_global_property_text(const gv_string_tmp& location,
									 const gv_string_tmp& new_value);
	int compare_global_property_binary(const gv_string_tmp& location,
									   gv_byte* new_value);
	int compare_global_property_text(const gv_string_tmp& location,
									 const gv_string_tmp& new_value);
	int apply_ini_string(const gv_string& text);
	int apply_ini_file(const char* filename);
	int is_array_element(const gv_string_tmp& location, gv_string& array_loc,
						 int& array_idx, int& array_size, int& array_el_size,
						 gv_byte*& parray_start, gvi_type*& parray_type);
	int redirect(gv_object* pobj_old, gv_object* pnew);
	int redirect(gvt_array< gv_object* >& pobj_old,
				 gvt_array< gv_object* >& pobj_new);
	//--object function ---------------------------------------------------->
	gv_int exec_global_call(const gv_string_tmp& function_location,
							gv_string_tmp& param);
	gv_int exec_global_call(gv_object* object, gv_func_info* func,
							gv_byte* param);
	//---module managerment ------------------------------------------------>
	gv_module* find_module(const gv_id& mod_name);
	gv_module* get_native_module();
	void set_native_module(gv_module*);
	gv_module* add_native_module(const gv_id& mod_name);
	gv_int get_nb_native_modules();
	gv_module* get_nth_native_module(int);
	// binary
	gv_module* try_load_module(const gv_id& mod_name);
	gv_module* load_module(const gv_id& mod_name,
						   const gv_string_tmp& path_name = "",
						   bool force_reload = false);
	bool unload_module(const gv_id& mod_name);
	bool query_module_object(const gv_id& mod_name,
							 gvt_array< gv_object* >& result,
							 bool ignore_module = false);
	bool save_module(const gv_id& mod_name, const gv_string_tmp& path_name = "");
	// XML
	gv_module* import_module(const gv_id& mod_name, gv_xml_parser& text,
							 bool force_reload = false);
	bool export_module(const gv_id& mod_name, gv_string_tmp& text);
	gv_module* import_module(const gv_id& id);
	bool export_module(const gv_id& id);

	void prepare_clear_reference_to_module(gv_module* pmod);

	void set_resource_root_path(const gv_string_tmp& name);
	const gv_string& get_resource_root_path();
	gv_string_tmp get_physical_resource_path(const gv_string_tmp& name);
	gv_string_tmp get_logical_resource_path(const gv_string_tmp& name);
	gv_string_tmp
	absolute_path_to_logical_resource_path(const gv_string_tmp& name);
	gv_string_tmp
	logical_resource_path_to_absolute_path(const gv_string_tmp& name);

	gv_file_manager* get_file_manager();
	class gv_input_manager* get_input_manager();
	class gvi_debug_renderer* get_debug_renderer();
	class gv_game_engine* get_game_engine();
	void set_game_engine(gv_game_engine* eng)
	{
		m_engine = eng;
	};
	gv_uint get_unique_postfix()
	{
		return (gv_uint)m_max_name_id.get();
	}
	bool is_destructing();
	bool collect_garbage();

protected:
	bool init(gv_uint max_object_nb, bool enable_object_name_hash = false);
	void destroy();
	gv_object* get_object(gv_int index);
	void add_object_to_hash(gv_object* obj);
	void remove_object_from_hash(gv_object* obj);
	gv_int hash_object(gv_object* pobj);
	gv_int hash_object(const gv_object_name&);
	bool register_reference(gvt_ptr< gv_object >* ptr, const gv_string& location);
	bool refill_reference();

protected:
	gv_sandbox_data* get_impl()
	{
		return m_pimpl;
	};
	gv_sandbox_data* m_pimpl;
	gvt_ptr< gv_game_engine > m_engine;
	gv_ushort m_index;
	gv_atomic_count m_max_name_id;
	gv_uint m_uuid;
	gvt_ptr< gv_sandbox > m_outer;
	bool m_use_name_hash;
	gv_mutex m_sandbox_mutex;

	gv_float m_delta_time;
	gv_double m_last_update_time_stamp;
	gv_double m_start_time_stamp;
	gv_string m_resource_root;
	gv_int m_tick_count;
};

class gv_object_iterator
{
	friend class gv_sandbox;

public:
	gv_object_iterator(gv_sandbox*);
	gv_object_iterator();
	bool is_empty()
	{
		return m_p_object == NULL;
	};
	bool operator==(const gv_object_iterator& o)
	{
		return o.m_p_object == m_p_object;
	}
	gv_object_iterator& operator++();
	operator gv_object*() const
	{
		return m_p_object;
	}
	gv_object* operator->() const
	{
		return m_p_object;
	}

protected:
	gvt_ptr< gv_sandbox > m_sandbox;
	gvt_ptr< gv_object > m_p_object;
	gv_int m_i_hash_value;
};

class gv_native_module_guard
{
public:
	gv_native_module_guard(gv_sandbox* sandbox, const char* name);
	~gv_native_module_guard();

protected:
	gv_sandbox* m_sandbox;
	gv_module* m_module;
};

} // gv