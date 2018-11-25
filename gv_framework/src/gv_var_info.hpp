namespace gv
{
gv_var_info::gv_var_info()
{
	this->m_var_flags = 0;
	link_class(gv_var_info::static_class());
	m_native_size = 0;
};

gv_var_info::gv_var_info(gv_sandbox* sandbox, gv_class_info* cls,
						 const gv_char* var, const gv_char* dcl,
						 gv_int native_size, const gv_char* range_value,
						 const gv_char* default_value, gv_int offset,
						 gv_uint extra_flag)
{
	// native varible declartion;
	link_class(gv_var_info::static_class());
	// this->rename(var);
	this->m_name = gv_object_name(var);
	this->m_owner = cls;
	sandbox->add_object(this);
	this->m_var_flags = 0;
	this->m_owner = cls;
	this->m_range = range_value;
	this->m_default = default_value;
	this->m_offset = offset;
	this->m_dcl = dcl;
	if (offset == -1)
		this->m_is_native = 1;
	this->m_native_size = native_size;
	this->m_var_flags |= extra_flag;
	cls->add_var(this);
};

gv_var_info::gv_var_info(gvi_type* type, char* name)
{
	this->m_type = type;
	this->m_range = NULL;
	this->m_default = NULL;
	this->m_offset = -1;
	this->m_dcl = "";
	this->m_is_native = 0;
	this->rename(gv_id(name));
	m_native_size = 0;
	link_class(gv_var_info::static_class());
}

void gv_var_info::initialize()
{
	if (m_is_inited)
		return;
	link_class(gv_var_info::static_class());
	if (!this->m_type)
	{
		this->m_type = get_sandbox()->create_type(this->m_dcl);
		// this->m_dcl="";
	}
	GV_ASSERT(m_type);
	GV_ASSERT(m_native_size == this->m_type->get_size());
	m_is_inited = true;
}

int gv_var_info::get_size()
{
	GV_ASSERT(this->m_type);
	return this->m_type->get_size();
}

bool gv_var_info::clamp(gv_object* object)
{
	if (!this->m_is_ranged)
		return false;
	gv_byte* pmember = (gv_byte*)object;
	pmember += this->m_offset;
	gv_byte max_value[1024];
	gv_byte min_value[1024];
	GV_ASSERT(this->get_type()->get_size() < 1024);
	this->get_type()->construct(max_value);
	this->get_type()->construct(min_value);
	gv_xml_parser x;
	x.goto_string(this->m_range);
	this->get_type()->import_from_xml(min_value, &x);
	if (this->get_type()->is_less(pmember, min_value))
	{
		get_type()->copy_to(min_value, pmember);
		return true;
	}
	this->get_type()->import_from_xml(max_value, &x);
	if (this->get_type()->is_less(max_value, pmember))
	{
		get_type()->copy_to(max_value, pmember);
		return true;
	}
	return false;
};
bool gv_var_info::set_var_to_default(gv_byte* object)
{
	if (!this->m_is_with_default)
		return false;
	gv_byte* pmember = object + this->m_offset;
	gv_byte default_value[1024];
	GV_ASSERT(this->get_type()->get_size() < 1024);
	this->get_type()->construct(default_value);
	gv_xml_parser x;
	x.goto_string(*this->m_default);
	get_type()->import_from_xml(default_value, &x);
	get_type()->copy_to(default_value, pmember);
	return true;
};

bool gv_var_info::set_var_value(gv_byte* object, const gv_byte* value,
								gv_int size_of_value)
{
	GV_ASSERT(this->get_size() == size_of_value);
	gv_byte* pmember = object + this->m_offset;
	get_type()->copy_to(value, pmember);
	return true;
};

bool gv_var_info::get_var_value(gv_byte* object, gv_byte* value,
								gv_int size_of_value)
{
	GV_ASSERT(this->get_size() == size_of_value);
	gv_byte* pmember = object + this->m_offset;
	get_type()->copy_to(pmember, value);
	return true;
};
}