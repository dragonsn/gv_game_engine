#pragma once
namespace gv
{
void gv_importer_exporter::set_import_config(
	const gv_string_tmp& config_string)
{
	if (this->get_import_config_class())
	{
		if (m_configure)
		{
			get_sandbox()->delete_object(m_configure);
			m_configure = NULL;
		}
		m_configure = get_sandbox()->create_object(get_import_config_class());
		m_configure->import_object_xml_in_memory(config_string);
	}
};
GVM_IMP_CLASS(gv_importer_exporter, gv_object)
GVM_END_CLASS
}
