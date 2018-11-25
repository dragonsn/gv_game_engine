#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DCL
#include "gv_impexp_classes.h"

namespace gv
{

class gv_driver_impexp : public gv_driver
{
public:
	gv_driver_impexp(){

	};

	~gv_driver_impexp(){

	};

	void init(gv_sandbox* sandbox)
	{
		gv_driver::init(sandbox);
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_IMP
#include "gv_impexp_classes.h"
	}

	void uninit(gv_sandbox* sandbox)
	{
#define GVM_REGISTER_CLASS GVM_REGISTER_CLASS_DEL
#include "gv_impexp_classes.h"
		gv_driver::uninit(sandbox);
	}

	bool get_processor_descs(gvt_array< gv_event_processor_desc >& descs)
	{
		return false;
	};
};

GVM_IMP_DRIVER(impexp);
}
