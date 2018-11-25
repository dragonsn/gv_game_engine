#pragma once
namespace gv
{
typedef gv_uint (gv_object::*gv_callback_func)(void*);
class gv_var_info;
class gv_class_info;
//=============================================================================>
//
//
//
//=============================================================================>
struct gv_func_init_helper
{
	gv_func_init_helper(const char* fn, gv_callback_func pfunc, gv_uint flags,
						gv_func_init_helper*& head, const char* param_struct)
	{
		m_fn = fn;
		m_pfunc = pfunc;
		m_flags = flags;
		m_next = head;
		head = this;
		m_param_struct = param_struct;
	}
	const char* m_fn;
	gv_callback_func m_pfunc;
	gv_uint m_flags;
	gv_func_init_helper* m_next;
	const char* m_param_struct;
};
//=============================================================================>
enum gve_func_flag
{
	gve_func_flag_is_inited = 1,
	gve_func_flag_is_event_handler = 2,
};

class gv_func_info : public gv_object
{
	friend class gv_class_info;

public:
	GVM_DCL_CLASS(gv_func_info, gv_object);
	gv_func_info();
	gv_func_info(char* fn);
	gv_func_info(gv_sandbox* sandbox, gv_class_info* cls,
				 gv_func_init_helper* init_info);
	~gv_func_info();

	gv_uint get_func_index()
	{
		return m_func_index;
	};
	bool is_equal(const gv_func_info*) const;
	gv_callback_func get_callback()
	{
		return m_callback;
	}
	void initialize();
	gv_class_info* get_param_struct();

	// int				get_param_size			();
	// void				add_param				(class gv_var_info *)
	// ;
	// void				add_local				(class gv_var_info *)
	// ;
	// gv_var_info*		get_local_var			(const gv_id &
	// name);
	// gv_var_info*		get_param_var			(const gv_id & name
	// );
	// int				refresh_stack_size		();
	// int				get_local_size			();
	// gv_byte	*			get_script_code			()
	// {return m_script.begin();}
protected:
	/*gvt_array			<gvt_ref_ptr <gvi_type>	>	m_parameters;//
  gvt_array			<gvt_ref_ptr <gvi_type>	>	m_locals
  ;//
  gvt_ref_ptr			<gvi_type			>
  m_return_type;//
  gvt_array			<gv_byte			>
  m_script;
  gvt_array			<gvt_ref_ptr<gv_var_info>	>
  m_param_vars;//only usable for the gvc compiler
  gvt_array			<gvt_ref_ptr<gv_var_info>	>
  m_local_vars;//only usable for the gvc compiler
  gv_uint				m_local_size;
  gv_uint				m_param_size;*/

	gvt_ptr< gv_class_info > m_cls;
	gvt_ptr< gv_class_info > m_param_struct;
	union {
		gv_callback_func m_callback; //
		void* m_node;
	};
	union {
		gv_uint m_func_flags;
		struct
		{
			gv_uint m_is_inited : 1;
			gv_uint m_is_event_handler : 1;

			// gv_uint	m_is_compiled	:1;
			// gv_uint	m_is_compiling	:1;
			// gv_uint	m_is_final		:1;
			// gv_uint	m_is_post_loaded:1;
			// gv_uint	m_is_latent		:1;
			// gv_uint	m_is_need_reset	:1;
		};
	};

	gv_uint m_func_index;
};

} // gv