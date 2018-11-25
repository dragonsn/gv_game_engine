#include "gv_base.h"
#include "gvt_array.h"
#include "gvt_list.h"
#include "gv_string.h"
#include "gv_file_manager.h"
#include "gv_time.h"
#include "boost/filesystem.hpp"
namespace gv
{
gv_file_manager::gv_file_manager()
{
}

gv_file_manager::~gv_file_manager()
{
}

gv_int gv_file_manager::find_file_with_pattern(const gv_string& pattern,
											   gvt_array< gv_string >& output,
											   const gv_string& folder,
											   bool include_subfolder)
{

#if defined(__ANDROID__)
	extern gv_int find_file_with_pattern_for_android(
		const gv_string& pattern, gvt_array< gv_string >& files,
		const gv_string& folder, bool include_subfolder);
	return find_file_with_pattern_for_android(pattern, output, folder,
											  include_subfolder);
#else
	return find_file_with_pattern_with_boost(pattern, output, folder,
											 include_subfolder);
#endif
}

void gv_file_manager::set_work_path(const gv_string& s)
{
	this->m_work_path = s;
};

gv_int_ptr gv_file_manager::load_lib(const gv_string& s)
{
	return 0;
};

bool gv_file_manager::free_lib(gv_int_ptr handle)
{
	return true;
};

bool gv_file_manager::delete_file(const gv_string& s)
{
	boost::filesystem::remove(*s);
	return true;
}
}
