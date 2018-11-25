#pragma once
namespace gv
{

#define GVM_EVENT_IMP_STRUCT
#define GVM_EVENT_IMP_ID
#define GV_EVENT_INC_FILE "gv_render_event.h"
#include "gv_event_maker.h"

#define GVM_EVENT_IMP_STRUCT
#define GVM_EVENT_IMP_ID
#define GV_EVENT_INC_FILE "gv_world_event.h"
#include "gv_event_maker.h"

#define GVM_EVENT_IMP_STRUCT
#define GVM_EVENT_IMP_ID
#define GV_EVENT_INC_FILE "gv_net_event.h"
#include "gv_event_maker.h"

#define GVM_EVENT_IMP_STRUCT
#define GVM_EVENT_IMP_ID
#define GV_EVENT_INC_FILE "gv_ui_event.h"
#include "gv_event_maker.h"
}