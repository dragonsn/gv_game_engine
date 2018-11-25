#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_renderer_d3d9_classes.h"
namespace gv
{

class gv_driver_d3d9 : public gv_driver
{
public:
	gv_driver_d3d9(){

	};

	~gv_driver_d3d9(){

	};

	void init(gv_sandbox* sandbox)
	{
		gv_driver::init(sandbox);
		gv_native_module_guard g(sandbox, "gv_renderer_d3d9_module");
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_renderer_d3d9_classes.h"
		gv_global::debug_draw.set(new gv_debug_renderer_d3d);
	}

	void uninit(gv_sandbox* sandbox)
	{
		gv_global::debug_draw.destroy();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_renderer_d3d9_classes.h"
		gv_driver::uninit(sandbox);
	}

	bool get_processor_descs(gvt_array< gv_event_processor_desc >& descs)
	{
		gv_event_processor_desc desc;
		desc.m_is_autonomous = !gv_global::framework_config.is_editor;
		desc.m_is_synchronization = true;
		desc.m_channel_id = "channel_render";
		desc.m_processor_cls = "gv_renderer_d3d9";
		descs.push_back(desc);
		return true;
	};
};

GVM_IMP_DRIVER(d3d9);
}
