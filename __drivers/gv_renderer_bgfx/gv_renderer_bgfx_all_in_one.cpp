#include "stdafx.h"

using namespace gv;

#if GV_WITH_BGFX
#include "gv_renderer_bgfx.h"
#include "common.h"
#include "bgfx_utils.h"
#include "gv_bgfx_app.h"
#include "logo.h"
#include "imgui/imgui.h"
#include "gv_bgfx_cache.h"


#include "gv_bgfx_app.hpp"
#include "gv_bgfx_cache.hpp"
#include "gv_renderer_bgfx.hpp"
#include "gv_bgfx_windows.hpp"
#include "bgfx/examples/common/entry/entry.cpp"
#include "bgfx/examples/common/entry/input.cpp"
#include "bgfx/examples/common/entry/cmd.cpp"
#include "bgfx/examples/common/imgui/imgui.cpp"
#include "bgfx/examples/common/bgfx_utils.cpp"
#include "bgfx/examples/common/example-glue.cpp"

#endif

//==register classes =======================================================>
namespace gv
{
class gv_driver_renderer_bgfx : public gv_driver
{
public:
	gv_driver_renderer_bgfx(){};
	~gv_driver_renderer_bgfx(){};
	void init(gv_sandbox* sandbox)
	{
		gv_driver::init(sandbox);
		gv_native_module_guard g(sandbox, "renderer_bgfx");
#undef GVM_REGISTER_CLASS
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "my_classes.h"
	}
	void uninit(gv_sandbox* sandbox)
	{
#undef GVM_REGISTER_CLASS
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "my_classes.h"
		gv_driver::uninit(sandbox);
	}
	bool get_processor_descs(gvt_array< gv_event_processor_desc >& descs)
	{
#if GV_WITH_BGFX
		gv_event_processor_desc desc;
		desc.m_is_autonomous = true;
		desc.m_is_synchronization = true;
		desc.m_channel_id = "channel_render";
		desc.m_processor_cls = "gv_renderer_bgfx";
		descs.push_back(desc);
#endif
		return true;
	};
};
GVM_IMP_DRIVER(renderer_bgfx);
}
