namespace unit_test_importer_exporter_fbx
{
void main(gvt_array< gv_string >& args)
{
	gv_unit_test_context_guard context;
	sub_test_timer timer("unit_test_importer_exporter_fbx");
	gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("fbx_mesh_test"));

	{
		gv_model* pmesh = m_sandbox->create_object< gv_model >(my_mod);
		gv_string_tmp file_name = FILE_FBX_HERON;
		if (args.size() >= 1)
			args[0] >> file_name;
		m_sandbox->import_external_format(pmesh, *file_name);
	}
	m_sandbox->export_module(my_mod->get_name_id());
}
}