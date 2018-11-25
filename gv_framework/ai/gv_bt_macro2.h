// this is a node that can be continued from last run.
//GSM is the prefix of prefix 
#pragma  once
namespace gv
{
	class  gv_scoped_stack_pointer
	{
	public:
		gv_scoped_stack_pointer(gv_state_machine * sm)
		{
			sm->step_in_state();
			m_sm = sm;
		}

		~gv_scoped_stack_pointer()
		{
			m_sm->step_out_state();
		}
	protected:
		gv_state_machine * m_sm;
	};
}


#define	 GVM_HSM \
	set_stack_depth(0);                                                          \
	if (get_bt_state() == gve_bt_not_started || get_bt_state() == gve_bt_zombie) \
		return false;                                                            \
	bool hit_any_state=false;\
	{switch (current_state())                                                     



#define GVM_SUB_HSM                      \
	{   gv_scoped_stack_pointer pt(this);  \
        switch (current_state())                                                     

#define GVM_STATE(state)  case state:   \
				{   \
				if (current_state() != state)  \
					current_state() = (state);  \
				hit_any_state=true;debug_state( #state );  \
				gv_scoped_stack_pointer pt(this);   \
				bool need_break = true;  \
				bool hit_check_point=false;\
				switch (current_state()) 

//SM will resume at closet checkpoint of last break , and if condition succeed , pause execution and return,  resume at this check point in the next update.
#define GVM_CHECK_POINT(state, condition)  \
							case state:  \
							{\
								if (current_state() != state) \
									current_state() = (state); \
								hit_check_point=true;\
	                            debug_state( #state );  \
								if (condition) \
									break; \
							}




#define GVM_END_STATE     	if (!hit_check_point) {GV_ASSERT(0&&"not any checkpoint in this state , always need at least a '0' zero checkpoint in state!");}	if (need_break)\
		break;\
		}\



#define GVM_END_HSM   if (!hit_any_state) {GV_ASSERT(0&&"not any state in this SM , always need at least a '0' zero state!");}\
					set_stack_depth(0);}


#define GVM_END_SUB_HSM    }
		                 
