#pragma once

namespace gv
{
class gv_input_manager_factory_win32 : public gvt_factory< gvi_input_manager >
{
public:
	virtual gvi_input_manager* create();
	virtual void destroy(gvi_input_manager* p);
	static gv_input_manager_factory_win32* static_get();
};
};