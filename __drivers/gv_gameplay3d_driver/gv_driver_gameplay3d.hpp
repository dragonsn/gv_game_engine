#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_gameplay3d_classes.h"
namespace gv
{

class gv_driver_gameplay3d : public gv_driver
{
public:
	gv_driver_gameplay3d(){

	};

	~gv_driver_gameplay3d(){

	};

	void init(gv_sandbox* sandbox)
	{
		gv_driver::init(sandbox);
		gv_native_module_guard g(sandbox, "gv_gameplay3d_module");
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_gameplay3d_classes.h"
		gv_global::debug_draw.set(new gv_debug_renderer_gameplay3d);
		gv_global::stats.register_category(gv_id_gp3d, gv_color::PINK(), true);
	}

	void uninit(gv_sandbox* sandbox)
	{
		gv_global::debug_draw.destroy();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_gameplay3d_classes.h"
		gv_driver::uninit(sandbox);
	}

	bool get_processor_descs(gvt_array< gv_event_processor_desc >& descs)
	{
		gv_event_processor_desc desc;
		desc.m_is_autonomous = false; //! gv_global::framework_config.is_editor;
		desc.m_is_synchronization = true;
		desc.m_channel_id = "channel_render";
		desc.m_processor_cls = "gv_renderer_gameplay3d";
		descs.push_back(desc);
		return true;
	};
};

GVM_IMP_DRIVER(gameplay3d);
}
