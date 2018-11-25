
#define GVM_DCL_EVENT(name)                               \
	class gv_object_event_##name : public gv_object_event \
	{                                                     \
	public:                                               \
		gv_object_event_##name()                          \
		{                                                 \
			init();                                       \
			this->m_id = gv_object_event_id_##name;       \
		}                                                 \
		static gv_object_event_id static_event_id()       \
		{                                                 \
			return gv_object_event_id_##name;             \
		}                                                 \
		const char* get_id_string()                       \
		{                                                 \
			return "gv_object_event_id_" #name;           \
		}

#define GVM_END_DCL_EVENT \
	}                     \
	;
