#include "gv_base.h"
#include "gvt_array.h"
#include "gvt_slist.h"
#include "gv_string.h"
#include "gv_id.h"
#include "gv_thread.h"
#include "gv_profiler.h"
#include "gv_stream.h"
#include "gv_file_manager.h"
#include "gv_stream_detail.h"
#include "gv_stream_cached.h"
#include "boost/filesystem.hpp"
#if defined(__ANDROID__)
#include "android/gv_stream_android_asset.hpp"
#endif

#pragma GV_REMINDER( \
	"[NOTES]the << and >> operator will update  the stream object, so never use const modifier on the operator declaration !!!")

namespace gv
{

namespace gv_global
{
gvt_global< gv_file_manager > fm;
}

namespace gv_file_manager_detail
{
template < class T >
gvi_stream* open_successed_stream(const char* name)
{
	gvi_stream* presult = new T;
	if (presult->open(name))
	{
		return presult;
	}
	delete presult;
	return NULL;
}
};

gvi_stream* gv_file_manager::open_binary_file_for_read(const gv_string& name)
{
	using namespace gv_file_manager_detail;
#if defined(__ANDROID__) && GV_WITH_OS_API
	gvi_stream* ps = open_successed_stream< gv_stream_android_asset >(*name);
	if (ps)
		return ps;
#endif
	return open_successed_stream< gv_stream_binary_read >(*name);
};

gvi_stream* gv_file_manager::cache_file_for_read(const gv_string& file_name)
{
	gv_stream_cached* ps = new gv_stream_cached;
	ps->open(*file_name, std::fstream::in);
	return ps;
};

gvi_stream* gv_file_manager::cache_file_for_write(const gv_string& file_name)
{
	gv_stream_cached* ps = new gv_stream_cached;
	ps->open(*file_name, std::fstream::out);
	return ps;
};

gvi_stream* gv_file_manager::open_binary_file_for_write(const gv_string& name)
{
	using namespace gv_file_manager_detail;
	return open_successed_stream< gv_stream_binary_write >(*name);
};

gvi_stream*
gv_file_manager::open_binary_file_for_append(const gv_string& name)
{
	using namespace gv_file_manager_detail;
	return open_successed_stream< gv_stream_binary_append >(*name);
};
gvi_stream* gv_file_manager::open_text_file_for_read(const gv_string& name)
{
	using namespace gv_file_manager_detail;
	return open_successed_stream< gv_stream_text_read >(*name);
};
gvi_stream* gv_file_manager::open_text_file_for_write(const gv_string& name)
{
	using namespace gv_file_manager_detail;
#if defined(__ANDROID__) && GV_WITH_OS_API
	gvi_stream* ps = open_successed_stream< gv_stream_android_asset >(*name);
	if (ps)
		return ps;
#endif
	return open_successed_stream< gv_stream_text_write >(*name);
};
gvi_stream* gv_file_manager::open_text_file_for_append(const gv_string& name)
{
	using namespace gv_file_manager_detail;
	return open_successed_stream< gv_stream_text_append >(*name);
};
gvi_stream* gv_file_manager::open_binary_size_counter()
{
	return new gv_stream_binary_size_count;
};
gv_string gv_file_manager::get_file_string(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return gv_string(p.filename().string().c_str());
};
gv_string gv_file_manager::get_directory_string(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.string().c_str();
};
gv_string gv_file_manager::get_root_name(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.root_name().string().c_str();
};
gv_string gv_file_manager::get_root_directory(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.root_directory().string().c_str();
};
gv_string gv_file_manager::get_relative_path(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.relative_path().string().c_str();
};
gv_string gv_file_manager::get_parent_path(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.parent_path().string().c_str();
};
gv_string gv_file_manager::get_filename(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.filename().string().c_str();
};
gv_string gv_file_manager::get_stem(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.stem().string().c_str();
};
gv_string gv_file_manager::get_main_name(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.stem().string().c_str();
};
gv_string gv_file_manager::get_extension(const gv_string& name)
{
	using namespace boost::filesystem;
	path p(*name);
	return p.extension().string().c_str();
};
void gv_file_manager::add_search_path(const gv_string& s)
{
	this->m_paths.push_back(s);
};

const gv_string& gv_file_manager::get_work_path()
{
	return this->m_work_path;
};

const gv_string& gv_file_manager::get_exe_path()
{
	return this->m_exe_path;
};
void gv_file_manager::reset_work_path()
{
	this->set_work_path(m_exe_path);
};
bool gv_file_manager::find_file(const gv_string& name, gv_string& output,
								const gv_string& in_folder,
								bool include_subfolder)
{
	using namespace boost::filesystem;
	gv_string folder = in_folder;
#if defined(__ANDROID__) && GV_WITH_OS_API
	if (folder.is_empty_string() || folder == "./")
	{
		GVM_DEBUG_OUT("find in    in :" << gv_global::config.path_for_write
										<< "  instead of an empty path ");
		folder = gv_global::config.path_for_write;
	}
#endif
	GVM_DEBUG_OUT("start_find_file  " << name << "  in :" << folder);
	if (folder == "*")
	{
		for (int i = 0; i < this->m_paths.size(); i++)
		{
			if (this->find_file(name, output, this->m_paths[i], include_subfolder))
				return true;
		}
		return false;
	}
	path dir_path(*folder);
	if (!exists(dir_path))
		return false;
	directory_iterator end_itr; // default construction yields past-the-end
	for (directory_iterator itr(dir_path); itr != end_itr; ++itr)
	{
		if (is_directory(itr->status()) && include_subfolder)
		{
			if (find_file(name, output, itr->path().string().c_str(),
						  include_subfolder))
				return true;
		}
		else if (itr->path().filename() == *name)
		{
			output = itr->path().string().c_str();
			return true;
		}
	}
	return false;
};

bool gv_file_manager::is_file_exsist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	return false;
};

// http://stackoverflow.com/questions/1257721/can-i-use-a-mask-to-iterate-files-in-a-directory-with-boost
#pragma GV_REMINDER( \
	"[*file system]: this version will not work exactly as the wildcard one!!!")
// but I don't want to include regex here,
gv_int gv_file_manager::find_file_with_pattern_with_boost(
	const gv_string& pattern, gvt_array< gv_string >& output,
	const gv_string& folder, bool include_subfolder)
{
	using namespace boost::filesystem;
	const std::string target_path(*folder);
	boost::filesystem::recursive_directory_iterator
		end_itr; // Default ctor yields past-the-end
	gv_string_tmp s;
	s = *pattern;
	s.replace_all("*", ""); // replace wildcard , make it a substring.

	for (boost::filesystem::recursive_directory_iterator i(target_path); i != end_itr;
		 ++i)
	{
		// Skip if not a file
		if (!boost::filesystem::is_regular_file(i->status()))
			continue;

		gv_string_tmp fname;
		fname = i->path().string().c_str();
		if (fname.find_string(s))
		{
			fname.replace_all("\\", "/");
			output.add_unique(*fname);
		}
	}
	return output.size();
};
}