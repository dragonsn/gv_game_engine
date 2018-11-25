#define GVM_DCL_STRUCT(cls)                                      \
	static gv_class_info* static_class();                        \
	friend void _gv_initialize_class_##cls(class gv_sandbox* sandbox); \
	typedef cls this_class;

#define GVM_DCL_BASE_CLASS(cls)                                  \
private:                                                         \
	cls(const cls& a);                                           \
	cls& operator=(const cls& a);                                \
                                                                 \
public:                                                          \
	static gv_class_info* static_class();                        \
	friend void _gv_initialize_class_##cls(gv_sandbox* sandbox); \
	typedef cls this_class;

#define GVM_DCL_CLASS(cls, supercls) \
	GVM_DCL_BASE_CLASS(cls);         \
	typedef supercls super;

#define GVM_DCL_CLASS_WITH_CONSTRUCT(cls, supercls) \
	GVM_DCL_CLASS(cls, supercls)                    \
	cls()                                           \
	{                                               \
		link_class(cls::static_class());            \
	}

#define GVM_DECLARE_EVENT_ID(event, id) \
	const static gv_ushort gvc_object_event_id_##event = id;

#define GVM_IMP_CLASS(cls, supercls)                                \
	gv_class_info* _##cls##_s_ptr;                                  \
	gv_class_info* cls::static_class()                              \
	{                                                               \
		return _##cls##_s_ptr;                                      \
	}                                                               \
	static void _##cls##_constructor(gv_byte* pb)                   \
	{                                                               \
		gvt_construct((cls*)pb);                                    \
	};                                                              \
	gv_func_init_helper* _##cls##_func_list = NULL;                 \
	void _gv_initialize_class_##cls(gv_sandbox* sandbox)            \
	{                                                               \
		typedef cls _THIS_CLS_TYPE;                                 \
		if (_##cls##_s_ptr)                                         \
			return;                                                 \
		GV_ASSERT(sandbox);                                         \
		gv_class_info* pcls = sandbox->register_class(gv_id(#cls)); \
		pcls->set_native_constructor(_##cls##_constructor);         \
		pcls->set_super_name(gv_id(#supercls));                     \
		pcls->set_size(sizeof(cls));                                \
		_##cls##_s_ptr = pcls;                                      \
		gv_func_init_helper* pfunc_info = _##cls##_func_list;       \
		gv_var_info var_template;                                   \
		var_template.m_is_class_member = 1;                         \
		gv_string_tmp s_temp;                                       \
		gv_var_info* var = NULL;

#define GVM_END_CLASS                              \
	pcls->set_owner(sandbox->get_native_module()); \
	pcls->set_function_list(pfunc_info);           \
	}

#define GVM_VAR(type, name)                                                      \
	var = new gv_var_info(sandbox, pcls, #name, #type, sizeof(type), NULL, NULL, \
						  GV_STRUCT_OFFSET(_THIS_CLS_TYPE, name),                \
						  var_template.m_var_flags);

#define GVM_VAR_ENUM(enum, name) \
	GVM_VAR(gv_int, name);       \
	var->set_enum_id(#enum);

#define GVM_STATIC_ARRAY(type, size, name)                                      \
	var = new gv_var_info(sandbox, pcls, #name, "gvt_array_static<" #type       \
												"," GV_REMINDER_STR2(size) ">", \
						  sizeof(type) * size, NULL, NULL,                      \
						  GV_STRUCT_OFFSET(_THIS_CLS_TYPE, name),               \
						  var_template.m_var_flags);

#define GVM_VAR_ATTRIB_SET(name) var_template.m_is_##name = 1;

#define GVM_VAR_ATTRIB_UNSET(name) var_template.m_is_##name = 0;

#define GVM_VAR_DEFAULT(val) var->set_default(#val);

#define GVM_VAR_RANGE(val) var->set_range(#val);

#define GVM_VAR_TOOLTIP(val) var->set_tooltip(#val);

#define GVM_VAR_SET(name, val) var->set_##name(#val);

#define GVM_DCL_FUNC(func) gv_int func(class func##_param* param);

#define GVM_BGN_FUNC_BASE(cls, fn, flag)                             \
	static gv_func_init_helper _##cls##_##fn(                        \
		#fn, (reinterpret_cast< gv_callback_func >(&cls::fn)), flag, \
		_##cls##_func_list, #fn "_param");                           \
	gv_int cls::fn(fn##_param* input)                                \
	{

#define GVM_BGN_FUNC(cls, fn) GVM_BGN_FUNC_BASE(cls, fn, 0)

#define GVM_PARAM(type, name) type& name = input->name;

#define GVM_END_FUNC }

#define GVM_IMP_STRUCT_ASSIGN(cls)                            \
	cls(const cls& s)                                         \
	{                                                         \
		(*this) = s;                                          \
	}                                                         \
	gv_sandbox* native_sandbox()                              \
	{                                                         \
		return gv_global::sandbox_mama->get_base_sandbox();   \
	}                                                         \
	cls& operator=(const cls& s)                              \
	{                                                         \
		gv_sandbox* psb = native_sandbox();                   \
		GV_ASSERT(psb);                                       \
		gv_class_info* pclass = psb->find_class(gv_id(#cls)); \
		GV_ASSERT(pclass);                                    \
		pclass->copy_to((gv_byte*) & s, (gv_byte*)this);      \
		return *this;                                         \
	}

#define GVM_IMP_STRUCT_BAS(cls, supercls, is_copyable)              \
	gv_class_info* _##cls##_s_ptr;                                  \
	static void _##cls##_constructor(gv_byte* pb)                   \
	{                                                               \
		gvt_construct((cls*)pb);                                    \
	};                                                              \
	void _gv_initialize_class_##cls(gv_sandbox* sandbox)            \
	{                                                               \
		typedef cls _THIS_CLS_TYPE;                                 \
		if (_##cls##_s_ptr)                                         \
			return;                                                 \
		GV_ASSERT(sandbox);                                         \
		gv_class_info* pcls = sandbox->register_class(gv_id(#cls)); \
		pcls->set_native_constructor(_##cls##_constructor);         \
		pcls->set_super_name(gv_id(#supercls));                     \
		pcls->set_is_struct();                                      \
		pcls->set_is_copyable(is_copyable);                         \
		pcls->set_size(sizeof(cls));                                \
		_##cls##_s_ptr = pcls;                                      \
		gv_var_info var_template;                                   \
		var_template.m_is_class_member = 1;                         \
		gv_var_info* var = NULL;

#define GVM_WITH_STATIC_CLS static class gv_class_info* static_class();
#define GVM_IMP_STATIC_CLASS(cls)      \
	gv_class_info* cls::static_class() \
	{                                  \
		return _##cls##_s_ptr;         \
	}
#define GVM_IMP_STRUCT(stst) GVM_IMP_STRUCT_BAS(stst, null, false)
#define GVM_IMP_COPYABLE_STRUCT(stst) GVM_IMP_STRUCT_BAS(stst, null, true)
#define GVM_END_STRUCT                             \
	pcls->set_owner(sandbox->get_native_module()); \
	}
#define GVM_IMP_STRUCT_WITH_SUPER(stst, supercls) \
	GVM_IMP_STRUCT_BAS(stst, supercls, false)

#define GVM_IMP_EVENT(stst) GVM_IMP_STRUCT_BAS(stst, gv_object_event, false)
#define GVM_END_EVENT GVM_END_STRUCT

#define GVM_HOOK_EVENT(event, func, channel)                                            \
	{                                                                                   \
		if (get_sandbox()->is_channel_open(gve_event_channel_##channel))                \
		{                                                                               \
			gv_object_event_handler_c< this_class >* prec;                              \
			prec = new gv_object_event_handler_c< this_class >(                         \
				this, reinterpret_cast<                                                 \
						  gv_object_event_handler_c< this_class >::gv_event_callback >( \
						  &this_class::func));                                          \
			this->get_sandbox()->register_event(gv_object_event_id_##event, prec,       \
												gve_event_channel_##channel);           \
		}                                                                               \
	}

#define GVM_HOOK_EVENT_2(event, channel) \
	GVM_HOOK_EVENT(event, on_event_##event, channel)

#define GVM_UNHOOK_EVENT(event, channel)                                        \
	if (get_sandbox()->is_channel_open(gve_event_channel_##channel))            \
		this->get_sandbox()->unregister_event(gv_object_event_id_##event, this, \
											  gve_event_channel_##channel);

#define GVM_POST_EVENT_TO_SANDBOX(sandbox, event, channel, init_statement) \
	{                                                                      \
		if (sandbox->is_channel_open(gve_event_channel_##channel))         \
		{                                                                  \
			gv_object_event_##event* pe =                                  \
				sandbox->create_object_event< gv_object_event_##event >(); \
			init_statement;                                                \
			sandbox->post_event(pe, gve_event_channel_##channel);          \
		}                                                                  \
	}

#define GVM_POST_EVENT(event, channel, init_statement) \
	GVM_POST_EVENT_TO_SANDBOX(get_sandbox(), event, channel, init_statement)

#define GVM_PROCESS_EVENT(event, channel, init_statement)                  \
	{                                                                      \
		if (get_sandbox()->is_channel_open(gve_event_channel_##channel))   \
		{                                                                  \
			gv_object_event_##event* pe = new gv_object_event_##event;     \
			init_statement;                                                \
			get_sandbox()->process_event(pe, gve_event_channel_##channel); \
		}                                                                  \
	}

#define GVM_POST_JOB_BASE(the_sandbox, job_type, channel, init_statement)        \
	{                                                                            \
		if (the_sandbox->is_channel_open(gve_event_channel_##channel))           \
		{                                                                        \
			gv_job_base* pe = gvt_create_##job_type(boost::bind init_statement); \
			the_sandbox->post_##job_type(pe, gve_event_channel_##channel);       \
		}                                                                        \
	}

#define GVM_POST_JOB(channel, init_statement) \
	GVM_POST_JOB_BASE(get_sandbox(), job, channel, init_statement)
// JOB_NEED_PROCESSOR_AS_PARAM_1
#define GVM_POST_JOB1(channel, init_statement) \
	GVM_POST_JOB_BASE(get_sandbox(), job1, channel, init_statement)
#define GVM_POST_JOB_S(channel, init_statement) \
	GVM_POST_JOB_BASE(get_super_sandbox(), job, channel, init_statement)
// JOB_NEED_PROCESSOR_AS_PARAM_1
#define GVM_POST_JOB1_S(channel, init_statement) \
	GVM_POST_JOB_BASE(get_super_sandbox(), job1, channel, init_statement)

#define GVM_POST_JOB_WITH_ACK(channel, init_statement, ack_channel, ack_init) \
	{                                                                         \
		if (get_sandbox()->is_channel_open(gve_event_channel_##channel))      \
		{                                                                     \
			gv_job_base* pe = gvt_create_job_with_ack(                        \
				boost::bind init_statement, gve_event_channel_##ack_channel,  \
				boost::bind ack_init, get_sandbox());                         \
			get_sandbox()->post_job(pe, gve_event_channel_##channel);         \
		}                                                                     \
	}

#define GVM_SET_CLASS(cls) link_class(cls::static_class());
#define GVM_REGISTER_CLASS_DCL 1
#define GVM_REGISTER_CLASS_IMP 2
#define GVM_REGISTER_CLASS_DEL 3
