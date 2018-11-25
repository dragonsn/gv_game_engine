namespace gv
{

gv_object::gv_object()
{
	link_class(gv_object::static_class());
	this->m_flags = 0;
}

gv_object::~gv_object()
{
}

bool gv_object::static_is_valid(const gv_object* p)
{
	if (gv_global::sandbox_mama->is_valid(p->get_sandbox()))
	{
		return p->get_sandbox()->is_valid(p);
	}
	return false;
};

void gv_object::link_class(gv_class_info* cls)
{
	// GV_ASSERT(cls);
	// for before gv_class_info is initilized , the input cls can be zero
	m_class = cls;
};

bool gv_object::is_a(const gv_id& class_name) const
{
	if (this->get_class()->is_derive_from(class_name))
	{
		return true;
	}
	else if (this->get_class()->get_name() == class_name)
	{
		return true;
	}
	else
		return false;
}

bool gv_object::write(gvi_stream* ps)
{
	this->m_class->write_data((gv_byte*)this, ps);
	return true;
}

bool gv_object::read(gvi_stream* ps)
{
	this->m_class->read_data((gv_byte*)this, ps);
	return true;
}

void gv_object::push_my_location(gv_object_location& loc) const
{
	if (this->m_owner && !this->m_owner->is_deleted())
		this->m_owner->push_my_location(loc);
	loc.m_object_location.push_back(this->m_name);
}

gv_object_location gv_object::get_location() const
{
	gv_object_location loc;
	push_my_location(loc);
	return loc;
}

bool gv_object::is_in_location(const gv_object_location& loc) const
{
	const gv_object* obj = this;
	for (int i = loc.m_object_location.size() - 1; i >= 0; i--)
	{
		if (!obj)
			return false;
		if (obj->m_name != loc.m_object_location[i])
			return false;
		obj = obj->m_owner;
	}
	return true;
};

gv_string_tmp gv_object::get_location_string() const
{
	gv_object_location loc = this->get_location();
	gv_string_tmp s;
	s << loc;
	return s;
}

void gv_object::rename(const gv_id& obj_name)
{
	rename(gv_object_name(obj_name));
	// if (this->get_sandbox())
	// this->get_sandbox()->rename_object(this,obj_name);
};

bool gv_object::export_object_xml(gv_string_tmp& s)
{
	this->get_class()->get_persistent_class()->export_to_xml((gv_byte*)this, s);
	return true;
}

bool gv_object::import_object_xml(gv_xml_parser* ps, gv_sandbox* sandbox,
								  gv_module* module)
{
	this->get_class()->get_persistent_class()->import_from_xml(
		(gv_byte*)this, ps, true, sandbox, module);
	return true;
}
bool gv_object::import_object_xml_in_memory(const gv_string_tmp& s)
{
	return gv_import_from_xml_in_memory(get_class(), (gv_byte*)this, s);
};
bool gv_object::import_object_txt(gv_xml_parser* ps, gv_sandbox* sandbox,
								  gv_module* module)
{
	this->get_class()->get_persistent_class()->import_from_txt(
		(gv_byte*)this, ps, sandbox, module);
	return true;
};
bool gv_object::export_object_txt(gv_string_tmp& s)
{
	this->get_class()->get_persistent_class()->export_to_txt((gv_byte*)this, s);
	return true;
};
gv_module* gv_object::get_module()
{
	gv_object* pout = this->m_owner;
	gv_object* pre = this;
	while (pout)
	{
		pre = pout;
		pout = pout->m_owner;
	}
	return gvt_cast< gv_module >(pre);
}

bool gv_object::is_a(gv_class_info* cls) const
{
	if (this->m_class && this->m_class->is_derive_from(cls))
		return true;
	else
		return false;
}

void gv_object::rename(const gv_object_name& name, gv_bool update_hash)
{
	if (this->get_sandbox() && update_hash)
		this->get_sandbox()->rename_object(this, name);
	else
		m_name = name;
};

void gv_object::rename(const char* obj_name)
{
	this->rename(gv_id(obj_name));
};

bool gv_object::is_owned_by(gv_object* pobject) const
{
	if (this->m_owner == pobject)
		return true;
	else if (this->m_owner)
		return this->m_owner->is_owned_by(pobject);
	return false;
}

bool gv_object::load_default()
{
	return this->get_class()->load_default(this);
};

bool gv_object::clamp_var()
{
	return this->get_class()->clamp_var(this);
}; // clamp var if out of range!!

gv_int gv_object::inc_ref()
{
	gv_int ret = gv_refable::inc_ref();
	//	GV_ASSERT(!is_deleted());
	return ret;
};

gv_int gv_object::dec_ref()
{
	gv_int ret = gv_refable::dec_ref();
	if (ret == 1)
	{
		if (!get_sandbox() || !get_sandbox()->is_destructing())
		{
			gv_bool is_root = this->get_owner() &&
							  this->get_owner()->is_a(gv_module::static_class());
			if (!is_root)
				this->m_is_deleted = 1;
		}
	}
	return ret;
};

gv_object_handle::gv_object_handle()
{
	invalidate();
}
gv_object_handle::gv_object_handle(gv_object* obj)
{
	if (obj && !obj->is_deleted() && obj->get_sandbox())
	{
		m_index = obj->get_runtime_index();
		m_name = obj->get_name();
		m_object = obj;
		m_sandbox = obj->get_sandbox()->get_handle();
	}
	else
	{
		invalidate();
	}
}
gv_object_handle::gv_object_handle(const gv_object_handle& hd)
{
	(*this) = (hd);
}
bool gv_object_handle::is_any()
{
	return (m_index == gvc_invalid_object_index) || (m_name.is_any());
}
void gv_object_handle::set_any()
{
	m_index = gvc_invalid_object_index;
	m_name.set_any();
}
bool gv_object_handle::operator==(const gv_object_handle& loc) const
{
	if (m_name != loc.m_name)
		return false;
	if (m_index != loc.m_index)
		return false;
	if (m_sandbox != loc.m_sandbox)
		return false;
	if (m_object != loc.m_object)
		return false;
	return true;
}
bool gv_object_handle::operator!=(const gv_object_handle& loc) const
{
	return !(*this == loc);
}
bool gv_object_handle::operator<(const gv_object_handle& loc) const
{
	if (m_name < loc.m_name)
		return true;
	if (m_index < loc.m_index)
		return true;
	return false;
}
gv_object_handle& gv_object_handle::operator=(const gv_object_handle& hd)
{
	m_name = hd.m_name;
	m_index = hd.m_index;
	m_sandbox = hd.m_sandbox;
	m_object = hd.m_object;
	return *this;
}
void gv_object_handle::invalidate()
{
	m_name = gv_id_none;
	m_index = gvc_invalid_object_index;
	m_object = NULL;
	m_sandbox.init(NULL);
}
bool gv_object_handle::is_valid()
{
	if (m_sandbox.get_sandbox())
		return m_sandbox.get_sandbox()->is_valid(*this);
	return false;
}
gv_object* gv_object_handle::get_object()
{
	if (is_valid())
		return m_object;
	return NULL;
}

gv_enum_info::gv_enum_info(){};
void gv_enum_info::add_pair(gv_int enum_val, const gv_id& name)
{
	m_dict.add_pair(name, enum_val);
};
gv_int gv_enum_info::get_int(const gv_id& name) const
{
	gv_int ret = 0;
	if (m_dict.find(name, ret))
		return ret;
	else
		return -1;
}
gv_id gv_enum_info::get_id(gv_int i) const
{
	gv_id id;
	m_dict.find(i, id);
	return id;
};
const gv_id& gv_enum_info::get_name() const
{
	return m_name;
};
void gv_enum_info::set_name(const gv_id& name)
{
	m_name = name;
}

void gv_enum_info::query_all(gvt_array< gv_id >& a1, gvt_array< gv_int >& a2)
{
	m_dict.query_all(a1, a2);
};

void gv_object_location::from_sting(const gv_string_tmp& s)
{
	if (s.size() && s[0] == '(')
	{
		gv_xml_parser parser;
		parser.goto_string(*s);
		parser >> *this;
	}
};

GVM_IMP_CLASS(gv_object, null)
GVM_VAR_ATTRIB_SET(no_clone)
GVM_VAR(gv_object_name, m_name)
GVM_VAR(gvt_ptr< gv_class_info >, m_class);
GVM_VAR(gvt_ptr< gv_object >, m_owner);
// GVM_VAR(gvt_ptr <gv_sandbox  >	,	m_sandbox);	//sandbox is not a
// gv_object...
GVM_VAR(gv_uint, m_flags);
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gv_uint, m_runtime_index);
GVM_VAR_TOOLTIP("object index in sandbox, don't change it manully");
GVM_VAR_ATTRIB_SET(no_auto_clear)
GVM_VAR(gvt_ptr< gv_object >, m_next);
GVM_VAR_TOOLTIP("next in the object hash table");
GVM_VAR(gv_int, m_ref_count);
GVM_END_CLASS

#include "gv_object_script.hpp"
}