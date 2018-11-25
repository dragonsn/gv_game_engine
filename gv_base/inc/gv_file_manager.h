#pragma once

#include "gv_stream.h"
namespace gv
{
class gv_file_manager
// basic implementation.
// may have bigfile , async implementation later .
{

public:
	gv_file_manager();
	virtual ~gv_file_manager();

	virtual gvi_stream* open_binary_file_for_read(const gv_string& file_name);
	virtual gvi_stream* cache_file_for_read(const gv_string& file_name);
	virtual gvi_stream* open_binary_file_for_write(const gv_string& file_name);
	virtual gvi_stream* cache_file_for_write(const gv_string& file_name);
	virtual gvi_stream* open_binary_file_for_append(const gv_string& file_name);

	virtual gvi_stream* open_text_file_for_read(const gv_string& file_name);
	virtual gvi_stream* open_text_file_for_write(const gv_string& file_name);
	virtual gvi_stream* open_text_file_for_append(const gv_string& file_name);
	virtual gvi_stream* open_binary_size_counter();

	virtual gv_int find_file_with_pattern(const gv_string& pattern,
										  gvt_array< gv_string >& output,
										  const gv_string& folder = "./",
										  bool include_subfolder = true);
	virtual gv_int find_file_with_pattern_with_boost(
		const gv_string& pattern, gvt_array< gv_string >& output,
		const gv_string& folder, bool include_subfolder = true);
	virtual bool find_file(const gv_string& name, gv_string& output,
						   const gv_string& folder = "./",
						   bool include_subfolder = true);
	virtual void add_search_path(const gv_string&);
	virtual bool delete_file(const gv_string&);

	virtual const gv_string& get_work_path();
	virtual const gv_string& get_exe_path();
	virtual void reset_work_path();
	virtual void set_work_path(const gv_string&);
	virtual gv_int_ptr load_lib(const gv_string&);
	virtual bool free_lib(gv_int_ptr handle);

	static gv_string get_file_string(const gv_string& path_name);
	static gv_string get_directory_string(const gv_string& path_name);
	static gv_string get_root_name(const gv_string& path_name);
	static gv_string get_root_directory(const gv_string& path_name);
	static gv_string get_relative_path(const gv_string& path_name);
	static gv_string get_parent_path(const gv_string& path_name);
	static gv_string get_filename(const gv_string& path_name);
	static gv_string get_stem(const gv_string& path_name);
	static gv_string get_main_name(const gv_string& path_name);
	static gv_string get_extension(const gv_string& path_name);
	static bool is_file_exsist(const char* path_name);

private:
	gvt_array< gv_string > m_paths;
	gv_string m_work_path;
	gv_string m_exe_path;
	gvp_multi_thread::mutex_holder m_fm_mutex;

protected:
};

namespace gv_global
{
extern gvt_global< gv_file_manager > fm;
}

template < class type_of_array >
bool gv_load_file_to_string(const gv_string_tmp& file_name,
							type_of_array& text)
{
	gv_file_manager* pfm = gv_global::fm.get();
	gvi_stream* ps = pfm->open_binary_file_for_read(*file_name);
	if (!ps)
		return false;
	text.resize(ps->size() + 1);
	ps->read(text.begin(), text.size());
	text.add_end();
	delete ps;
	return true;
};

template < class type_of_array >
bool gv_load_file_to_array(const gv_string_tmp& file_name,
						   type_of_array& array)
{
	gv_file_manager* pfm = gv_global::fm.get();
	gvi_stream* ps = pfm->open_binary_file_for_read(*file_name);
	if (!ps)
		return false;
	gv_int el_size = sizeof(typename type_of_array::type_of_data);
	GV_ASSERT(ps->size() % el_size == 0);
	array.resize(ps->size() / el_size);
	ps->read(array.begin(), ps->size());
	delete ps;
	return true;
};

template < class type_of_char >
bool gv_save_string_to_file(const gv_string_tmp& file_name,
							const type_of_char* text, int text_size)
{
	gv_file_manager* pfm = gv_global::fm.get();
	gvi_stream* ps = pfm->open_binary_file_for_write(*file_name);
	if (!ps)
		return false;
	ps->write(text, text_size);
	delete ps;
	return true;
};

template < class type_of_array >
bool gv_save_array_to_file(const gv_string_tmp& file_name,
						   type_of_array& array)
{

	gv_file_manager* pfm = gv_global::fm.get();
	gvi_stream* ps = pfm->open_binary_file_for_write(*file_name);
	if (!ps)
		return false;
	gv_int el_size = sizeof(typename type_of_array::type_of_data);
	ps->write(array.begin(), array.size() * el_size);
	delete ps;
	return true;
};

template < class type_of_string >
bool gv_save_string_to_file(const gv_string_tmp& file_name,
							const type_of_string& text)
{
	return gv_save_string_to_file(file_name, text.begin(), text.size() - 1);
};

} // gv