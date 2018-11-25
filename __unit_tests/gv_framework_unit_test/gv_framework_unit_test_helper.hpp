#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_test_classes.h"

GVM_DCL_DRIVER(dummy)
GVM_DCL_DRIVER(d3d9)
GVM_DCL_DRIVER(ogl)
GVM_DCL_DRIVER(impexp)
GVM_DCL_DRIVER(gameplay3d)
GVM_DCL_DRIVER(renderer_bgfx)
#if WITH_CEGUI
GVM_DCL_DRIVER(cegui)
#endif

namespace gv
{
gv_sandbox* m_sandbox = NULL;
gv_sandbox* get_sandbox()
{
	return m_sandbox;
}

gv_unit_test_context_guard::gv_unit_test_context_guard(bool with_render)
{
	gv_global::framework_config.data_path_root= RES_ROOT"/";
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	gv_register_test_classes();
	m_sandbox = gv_global::sandbox_mama->create_sandbox(
		gv_global::sandbox_mama->get_base_sandbox());
	m_sandbox->set_resource_root_path(RES_ROOT);
	if (with_render)
	{
		gv_driver_info::static_create_event_processors(m_sandbox);
		GVM_POST_EVENT(render_init, render, (pe->window_handle = NULL));
		GVM_POST_EVENT(render_enable_pass, render,
					   (pe->pass = gve_render_pass_opaque));
	}
}
gv_unit_test_context_guard::~gv_unit_test_context_guard()
{
	gv_unregister_test_classes();
	gv_global::sandbox_mama->delete_sandbox(m_sandbox);
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}

void gv_register_test_classes()
{
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_test_classes.h"
	sandbox->init_classes();
};
void gv_unregister_test_classes()
{
	gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_test_classes.h"
}
}
