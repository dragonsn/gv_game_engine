#pragma once
namespace gv
{
	struct gv_framework_config
	{

		GVM_DCL_STRUCT(gv_framework_config);
		gv_framework_config();
		gv_framework_config& operator=(const gv_framework_config& c)
		{
			module_path_xml = c.module_path_xml;
			module_path_gvb = c.module_path_gvb;
			physics_asset_root_path = c.physics_asset_root_path;
			texture_cache_path = c.texture_cache_path;
			data_path_root = c.data_path_root;
			driver_list = c.driver_list;
			window_size = c.window_size;
			return *this;
		}
		void clear();
		static gv_string_tmp get_binary_module_path();
		static gv_string_tmp get_xml_module_path();
		static gv_string_tmp get_texture_cache_path();

		gv_int_ptr window_handle;
		gv_vector2i window_size;
		gv_text data_path_root;
		gv_uint nb_debug_line;
		gv_uint nb_debug_tri;
		gv_uint max_sandbox_nb;
		gv_uint max_object_per_sandbox;
		gv_text physics_asset_root_path;

		gvt_array< gv_string > driver_list;
		gv_bool is_editor;
	protected:
		gv_text module_path_xml;
		gv_text module_path_gvb;
		gv_text texture_cache_path;
	};

	namespace gv_global
	{
		extern gv_framework_config framework_config;
	};
}