//---------------------------------------------------------------------------->
#if defined(GVM_MAKE_LOG_DCL_LOG)
#define GVM_MAKE_LOG(name) gv_log* m_##name##_log;
#elif defined(GVM_MAKE_LOG_DCL_LOG_FUNC)
#define GVM_MAKE_LOG(name)             \
	gv_log& name##_log()               \
	{                                  \
		return *m_##name##_log;        \
	};                                 \
	void set_##name##_log(gv_log* log) \
	{                                  \
		if (m_##name##_log)            \
			m_##name##_log->dec_ref(); \
		m_##name##_log = log;          \
	}
#elif defined(GVM_MAKE_LOG_INIT_LOG)
#define GVM_MAKE_LOG(name)                                                 \
	if (!gv_global::config.no_log_##name && !gv_global::config.no_log_all) \
		m_##name##_log = create_log("gv_" #name "_log_");                  \
	else                                                                   \
		m_##name##_log = NULL;
#elif defined(GVM_MAKE_LOG_DESTROY_LOG)
#define GVM_MAKE_LOG(name) \
	if (m_##name##_log)    \
		m_##name##_log->dec_ref();
#endif
//---------------------------------------------------------------------------->

GVM_MAKE_LOG(net)
GVM_MAKE_LOG(main)
GVM_MAKE_LOG(render)
GVM_MAKE_LOG(ai)
GVM_MAKE_LOG(script)
GVM_MAKE_LOG(physics)
GVM_MAKE_LOG(database)
GVM_MAKE_LOG(debug)
GVM_MAKE_LOG(object)
GVM_MAKE_LOG(warning)
GVM_MAKE_LOG(error)
GVM_MAKE_LOG(event)

//---------------------------------------------------------------------------->
#undef GVM_MAKE_LOG
#undef GVM_MAKE_LOG_DCL_LOG
#undef GVM_MAKE_LOG_DCL_LOG_FUNC
#undef GVM_MAKE_LOG_INIT_LOG
#undef GVM_MAKE_LOG_DESTROY_LOG

//---------------------------------------------------------------------------->