#include "gv_framework_private.h"
#include "gv_object_struct.h"
#include "gv_type.h"
#pragma GV_REMINDER( \
	"[MEMO]remember to restore the gt & lt operator after load from XML before use, and backup before save to make XML tools happy ")
namespace gv
{
// typedef int type; //private type ,o
// GVM_NEW_TYPE(type)
static gvi_type* current_type = NULL;
static bool s_use_ascend = true;

bool gv_can_directly_copy(gve_data_type type)
{
	return type < gve_data_type_string;
};

int gv_compare_with_type(const void* p1, const void* p2)
{
	if (s_use_ascend)
		return current_type->compare((gv_byte*)p1, (gv_byte*)p2);
	else
		return -current_type->compare((gv_byte*)p1, (gv_byte*)p2);
}

void gv_sort_with_type_info(gvi_type* type, gv_byte* first, gv_int size,
							bool use_ascend)
{
	if (size <= 1)
		return; // no need
	static gv_mutex mutex;
	gv_thread_lock lock(
		mutex); // for the global current_type, need better solution
	current_type = type;
	s_use_ascend = use_ascend;
	gv_qsort(first, size, type->get_size(), gv_compare_with_type);
};

#include "gv_type_array.hpp"
#include "gv_type_static_array.hpp"
#include "gv_type_text.hpp"
}