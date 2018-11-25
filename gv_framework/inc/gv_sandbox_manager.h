#pragma once
namespace gv
{
//=================================================================================>
// this is a singleton to manager all the sandboxes , create , and destroy
// sandboxs
// this is the universe!
//=================================================================================>
class gv_sandbox_manager_data;
class gv_sandbox_manager
{
public:
	gv_sandbox_manager();
	~gv_sandbox_manager();
	bool init(gv_ushort nb_sand_box = 2, gv_uint max_object_per_sand_box = 655360,
			  bool enable_name_hash = true, bool is_editor = false);
	void destroy();
	gv_sandbox* get_sand_box(const gv_sandbox_handle& hd);
	gv_sandbox* get_base_sandbox();
	gv_sandbox* create_sandbox(gv_sandbox* outer = NULL);
	bool delete_sandbox(gv_sandbox* p);
	bool is_valid(const gv_sandbox_handle& hd);
	bool is_valid(const gv_sandbox* sandbox);
	bool run();
	gv_string get_xml_module_file_path(const gv_id& name);
	gv_string get_bin_module_file_path(const gv_id& name);
	bool add_new_xml_module(const gv_id& name, gv_sandbox* outer,
							bool force_create = true);
	bool add_new_bin_module(const gv_id& name, gv_sandbox* outer,
							bool force_create = true);
	void register_new_xml_module_path(const gv_id& name, const gv_string_tmp& s);
	;
	void register_new_bin_module_path(const gv_id& name, const gv_string_tmp& s);
	;
	gv_int query_registered_modules(gvt_array< gv_id >& modules,
									gvt_array< gv_string >& path, bool is_binary);

protected:
	bool init_data_dir();
	bool init_native_classes();
	bool init_network();

	gvt_array< gvt_ref_ptr< gv_sandbox > > m_sandboxs;
	gv_uint m_cu_uuid;
	gv_ushort m_max_sandbox;
	gv_ushort m_sandbox_nb;
	gv_uint m_max_object_per_sand_box;
	bool m_enable_name_hash;
	bool m_is_inited;
	gv_mutex m_mutex;
	gv_sandbox_manager_data* m_pimpl;
};

namespace gv_global
{
extern gvt_global< gv_sandbox_manager > sandbox_mama;
}
}