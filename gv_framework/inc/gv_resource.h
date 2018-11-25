#pragma once
namespace gv
{
// async load and unload support ,  streaming data can help streaming level
// seamless.
// resource is normally has a external reference , i.g the texture, and the mesh
// file,

class gv_renderer;
class gv_resource_cache;
class gv_resource : public gv_object
{
	friend class gv_resource_cache;

public:
	GVM_DCL_CLASS(gv_resource, gv_object);
	typedef gv_int (gv_object::*gv_load_complete_callback)();

	gv_resource();
	~gv_resource();
	virtual gv_ulong last_visit_time_in_ms(); // when sort for resource cache.
	virtual void update_visit_time();
	virtual void load_data();
	virtual void unload_data();
	virtual void start_async_loading();
	virtual void is_loading_complete();
	virtual void wait_loading_complete();
	virtual void set_loading_complete_callback(gv_load_complete_callback,
											   gv_object*);

	virtual void get_file_name(gv_string_tmp& s);
	virtual void set_file_name(const gv_string_tmp& s);
	virtual const gv_text& get_file_name() const;
	virtual gv_string_tmp get_physical_path()
	{
		gv_string_tmp s;
		get_file_name(s);
		return this->get_sandbox()->get_physical_resource_path(s);
	};
	// debug
	virtual void query_reference(gvt_array< gv_string > result);
	//
	inline void set_hardware_cache(gv_resource_cache* cache)
	{
		m_hardware_cache = cache;
	}

	inline gv_resource_cache* get_hardware_cache()
	{
		return m_hardware_cache;
	}
	template < class type_of_cache >
	inline type_of_cache* get_hardware_cache()
	{
		return gvt_cast< type_of_cache >(get_hardware_cache());
	}
	virtual bool precache(gv_renderer*)
	{
		return false;
	};
	virtual void uncache()
	{
		m_hardware_cache = NULL;
	};
	virtual bool is_precached()
	{
		return get_hardware_cache() != NULL;
	};

public:
	// script here
	GVM_DCL_FUNC(scp_recache);
	GVM_DCL_FUNC(scp_reimport);

protected:
	virtual void set_loading_complete();
	gv_text m_file_name;
	gvt_ref_ptr< gv_resource_cache > m_hardware_cache;
};

//==========================================================================
class gv_resource_cache : public gv_object
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_resource_cache, gv_object);
	~gv_resource_cache(){};
	virtual		bool		set_origin	( gv_resource * )	{return false;};
	virtual		bool		precache	(gv_uint flag)		{return false;};
	virtual		bool		uncache		()					{return false;};
	virtual		bool		recache		()					{return false;};
};

}