// this is a node that can be continued from last run.
#define GVM_BT_START                                                             \
	set_stack_depth(0);                                                          \
	if (get_bt_state() == gve_bt_not_started || get_bt_state() == gve_bt_zombie) \
		return false;                                                            \
	switch (current_state())                                                     \
	{                                                                            \
	case 0:                                                                      \
	{                                                                            \
	}

#define GVM_BT_SUBTREE                                                           \
	if (get_bt_state() == gve_bt_not_started || get_bt_state() == gve_bt_zombie) \
		return false;                                                            \
	step_in_state();                                                             \
	switch (current_state())                                                     \
	{                                                                            \
	case 0:                                                                      \
	{                                                                            \
	}

#define GVM_BT_NODE_SQ_CALL(state, call) \
	case state:                          \
	{                                    \
		if (current_state() != state)    \
			current_state() = (state);   \
		debug_state(#state);             \
		call;                            \
		step_in_state();                 \
		bool need_break = true;          \
		switch (current_state())         \
		{                                \
		case 0:                          \
		{                                \
		}

#define GVM_BT_NODE_SQ(state) GVM_BT_NODE_SQ_CALL(state, nop())

#define GVM_BT_NODE_SELECT_CALL(state, call) \
	case state:                              \
	{                                        \
		if (current_state() != state)        \
			current_state() = (state);       \
		debug_state(#state);                 \
		call;                                \
		step_in_state();                     \
		bool need_break = true;              \
		switch (current_state())             \
		{                                    \
		case 0:                              \
		{                                    \
		}

#define GVM_BT_NODE_SELECT(state) GVM_BT_NODE_SELECT_CALL(state, nop())

//! RISK!
#define GVM_BT_LEAF_LATENT(state, call)               \
	case state:                                       \
	{                                                 \
		if (current_state() != state)                 \
			current_state() = (state);                \
		debug_state(#state);                          \
		bool need_break = false;                      \
		if (get_bt_state() == gve_bt_wait_for_signal) \
		{                                             \
			if (is_signaled())                        \
			{                                         \
				set_bt_state(gve_bt_runing);          \
				signal_reset();                       \
			}                                         \
			else                                      \
				break;                                \
		}                                             \
		else                                          \
		{                                             \
			set_bt_state(gve_bt_wait_for_signal);     \
			call;                                     \
			need_break = true;                        \
			break;                                    \
		}                                             \
		if (need_break)                               \
			break;                                    \
	}
//leaf
#define GVM_BT_LEAF(state, call)       \
	case state:                        \
	{                                  \
		if (current_state() != state)  \
			current_state() = (state); \
		debug_state(#state);           \
		bool need_break = call;        \
		if (need_break)                \
			break;                     \
	}

#define GVM_BT_LEAF_TEST(state, condition, call) \
	case state:                                  \
	{                                            \
		if (current_state() != state)            \
			current_state() = (state);           \
		debug_state(#state);                     \
		if (condition)                           \
		{                                        \
			bool need_break = call;              \
			if (need_break)                      \
				break;                           \
		}                                        \
	}

#define GVM_BT_END_NODE      \
	default:                 \
		need_break = false;  \
		current_state() = 0; \
		}                    \
		step_out_state();    \
		if (need_break)      \
			break;           \
		}

#define GVM_BT_END_LEAF

#define GVM_BT_END                   \
	default:                         \
		set_bt_state(gve_bt_zombie); \
		}                            \
		;                            \
		set_stack_depth(0);

#define GVM_BT_END_SUBTREE \
	default:               \
		GV_ASSERT(0);      \
		break;             \
		}                  \
		;                  \
		step_out_state();



//the macro about is deprecated because it's difficult to have correct indent with the coding standard , use new set of macros instead.
#include "gv_bt_macro2.h"