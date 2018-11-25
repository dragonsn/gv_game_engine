#include "gv_framework_private.h"
#include "gv_importer_exporter.h"
#include "gv_input_manager.h"
#include "gv_driver.h"
#include "../task/gv_task_manager.h"

#include "../src/gv_sandbox.hpp"
#include "../src/gv_sandbox_events.hpp"
#include "../src/gv_sandbox_classes.hpp"
#include "../src/gv_sandbox_module.hpp"
#include "../src/gv_sandbox_property.hpp"
#include "../src/gv_object_iterator.hpp"
#include "../src/gv_event_processor.hpp"
#include "../src/gv_sandbox_manager.hpp"
#include "../task/gv_task_manager.hpp"

//  [9/25/2010 Administrator]#pragma  GV_REMINDER("[TASK][P0] create event
//  channel !!!")
#pragma GV_REMINDER( \
	"[MEMO]remember to check GVM_PENDING and GVM_UNDER_CONSTRUCT to finish code")

namespace gv
{
namespace gv_global
{
gvt_global< gv_sandbox_manager > sandbox_mama;
}
}