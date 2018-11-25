namespace unit_test_importer_exporter_3ds
{
void main(gvt_array< gv_string >& args)
{
	{
		gv_unit_test_context_guard context;
		sub_test_timer timer("test_importer_3ds");
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("3ds_mesh_test"));

		{
			gv_model* pmesh = m_sandbox->create_object< gv_model >(my_mod);
			m_sandbox->import_external_format(pmesh, FILE_3DS_TEAPOT);
		}
		m_sandbox->export_module(my_mod->get_name_id());
	}
}
}
