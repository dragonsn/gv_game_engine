#include "gv_base.h"
#include "gvt_array.h"
#include "gvt_list.h"
#include "gv_string.h"
#include "gv_file_manager.h"
#include "gv_time.h"
#include "gv_stream_detail.h"
#include <windows.h>
#include "boost/filesystem.hpp"
namespace gv
{
gv_file_manager::gv_file_manager()
{

	using namespace boost::filesystem;
	using namespace std;
	char filename[256];
	HMODULE hInstance = 0;
	hInstance = GetModuleHandle(NULL);
	GetModuleFileNameA(hInstance, filename, 256);
	this->m_exe_path = this->get_parent_path(filename);
	this->m_exe_path += "/";
	gvt_array< gv_string > a;
	a.add("");
	this->m_paths.add(this->m_exe_path);
	reset_work_path();

#if !GV_WITH_OS_API
	if (!gv_global::config.no_reset_path)
		SetCurrentDirectoryA(*this->m_exe_path);
#endif
	this->m_work_path = this->m_exe_path;
}

gv_file_manager::~gv_file_manager()
{
}

gv_int gv_file_manager::find_file_with_pattern(const gv_string& pattern,
											   gvt_array< gv_string >& output,
											   const gv_string& folder,
											   bool include_subfolder)
{
	gv_string fullname = folder;
	fullname += pattern;
	WIN32_FIND_DATAA FindData;
	HANDLE hFindFile = FindFirstFileA(*fullname, &FindData);
	if (hFindFile &&
		(hFindFile != INVALID_HANDLE_VALUE /*(HANDLE)0xffffffff */))
	{
		if (folder != "")
		{
			fullname = folder;
			fullname += FindData.cFileName;
		}
		else
		{
			// not in any folder , it's absolute full path.
			fullname = pattern;
		}
		fullname.replace_all("\\", "/");
		output.add(fullname);

		while (1)
		{
			if (!FindNextFileA(hFindFile, &FindData))
			{
				break;
			}
			fullname = folder;
			fullname += FindData.cFileName;
			fullname.replace_all("\\", "/");
			output.add(fullname);
		}
	}

	if (!include_subfolder)
		return output.size();
	fullname = folder;
	fullname += "*";
	hFindFile = FindFirstFileA(*fullname, &FindData);
	if (!hFindFile || (hFindFile == INVALID_HANDLE_VALUE))
		return output.size();
	if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
		FindData.cFileName[0] != '.')
	{
		fullname = folder;

		fullname += FindData.cFileName;
		fullname += "/";
		this->find_file_with_pattern(pattern, output, fullname);
	}

	while (1)
	{
		if (!FindNextFileA(hFindFile, &FindData))
		{
			break;
		}
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
			FindData.cFileName[0] != '.')
		{
			fullname = folder;
			fullname += FindData.cFileName;
			fullname += "/";
			this->find_file_with_pattern(pattern, output, fullname);
		}
	}
	return output.size();
	;
};

void gv_file_manager::set_work_path(const gv_string& s)
{
	this->m_work_path = s;
	SetCurrentDirectoryA(*s);
};

gv_int_ptr gv_file_manager::load_lib(const gv_string& s)
{
	return (gv_int_ptr)LoadLibraryA(*s);
};

bool gv_file_manager::free_lib(gv_int_ptr handle)
{
	return FreeModule((HMODULE)handle) == TRUE;
};

bool gv_file_manager::delete_file(const gv_string& s)
{
	return ::DeleteFileA(*s) == TRUE;
}
}
