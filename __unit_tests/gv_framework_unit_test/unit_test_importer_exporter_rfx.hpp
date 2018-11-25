namespace unit_test_importer_exporter_rfx
{
void main(gvt_array< gv_string >& args)
{

	{

		gv_unit_test_context_guard context;
		sub_test_timer timer("unit_test_importer_exporter_rfx");
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("rfx_test"));
		{
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_mod);
			gv_string_tmp file_name = FILE_RFX_ILLUMINATION;
			if (args.size() >= 1)
				args[0] >> file_name;
			m_sandbox->import_external_format(effect, *file_name);
		}
		m_sandbox->export_module(my_mod->get_name_id());
	}
}
}
