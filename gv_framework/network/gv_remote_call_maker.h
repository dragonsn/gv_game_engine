#ifndef GV_REMOTE_CALL_INC_FILE
#error
#endif

#undef GVM_REMOTE_CALL_IMP_RECV_IMP
#ifdef GVM_REMOTE_CALL_IMP_RECV
#define GVM_REMOTE_CALL_IMP_RECV_IMP
#endif

#define GVM_DCL_REMOTE_CALL GVM_DCL_REMOTE_CALL_2
#define GV_REMOTE_CALL_IS_ACK false
#define GVM_REMOTE_CALL_PARAM GVM_REMOTE_CALL_VAR
#define GVM_REMOTE_CALL_RETURN(...)
#define GVM_REMOTE_CALL_IMP_CHECK_ID
#include "gv_remote_call_maker_helper.h"
#undef GVM_DCL_REMOTE_CALL
#undef GV_REMOTE_CALL_IS_ACK
#undef GVM_REMOTE_CALL_PARAM
#undef GVM_REMOTE_CALL_RETURN

//============================================================================================
//								:
//============================================================================================
#undef GVM_REMOTE_CALL_IMP_RECV_IMP
#ifdef GVM_REMOTE_CALL_IMP_RECV_ACK
#define GVM_REMOTE_CALL_IMP_RECV_IMP
#endif

#define GVM_DCL_REMOTE_CALL(name, id) GVM_DCL_REMOTE_CALL_2(name##_ack, id)
#define GV_REMOTE_CALL_IS_ACK true
#define GVM_REMOTE_CALL_RETURN GVM_REMOTE_CALL_VAR
#define GVM_REMOTE_CALL_PARAM(...)
#undef GVM_REMOTE_CALL_IMP_CHECK_ID
#include "gv_remote_call_maker_helper.h"
#undef GVM_DCL_REMOTE_CALL
#undef GV_REMOTE_CALL_IS_ACK
#undef GVM_REMOTE_CALL_PARAM
#undef GVM_REMOTE_CALL_RETURN

#undef GV_REMOTE_CALL_IS_ACK
#undef GVM_REMOTE_CALL_IMP_STRUCT
#undef GVM_REMOTE_CALL_DCL_STRUCT
#undef GVM_REMOTE_CALL_IMP_ID
#undef GVM_REMOTE_CALL_IMP_CHECK_ID
#undef GVM_REMOTE_CALL_IMP_RECV
#undef GVM_REMOTE_CALL_IMP_RECV_ACK

//============================================================================================
#undef GV_REMOTE_CALL_INC_FILE
