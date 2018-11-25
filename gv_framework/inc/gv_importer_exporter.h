#pragma once
namespace gv
{
class gv_importer_exporter : public gv_object
{
public:
	GVM_DCL_CLASS(gv_importer_exporter, gv_object)
	gv_importer_exporter(){};
	~gv_importer_exporter(){};

public:
	virtual void get_import_extension(gvt_array< gv_id >& result){};
	virtual gv_class_info* get_import_target_class()
	{
		return NULL;
	}
	virtual gv_class_info* get_import_config_class()
	{
		return NULL;
	}
	virtual void set_import_config(const gv_string_tmp& config_string);
	virtual bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		return false;
	};

	virtual void get_export_extension(gvt_array< gv_id >& result){};
	virtual gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	virtual bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		return false;
	};

protected:
	gvt_ref_ptr< gv_object > m_configure;
};
};