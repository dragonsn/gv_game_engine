#pragma once
class gv_resource_manager : public gv_object
{
public:
	GVM_DCL_CLASS(gv_resource_manager, gv_object);
	gv_resource_manager();
	~gv_resource_manager();
	gv_resource* get_resource(const char* logic_file_name);
	template < class T >
	inline T* get_resource(const char* logic_file_name)
	{
		return gvt_cast< T >(get_resource(logic_file_name));
	}

protected:
	class gv_resource_manager_imp* m_imp;
};
