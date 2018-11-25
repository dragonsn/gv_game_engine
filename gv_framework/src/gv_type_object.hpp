namespace gv
{
gv_type_object::gv_type_object(){};

int gv_type_object::get_size()
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->get_size();
}

int gv_type_object::get_alignment()
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->get_alignment();
};

gvi_type::type gv_type_object::get_type()
{
	m_class = m_class->fetch_completed_class();
	return gve_data_type_object;
};

bool gv_type_object::is_the_same_type(gvi_type* p)
{
	m_class = m_class->fetch_completed_class();
	if (p && p->get_type() == gve_data_type_object)
	{
		return this->m_class == ((gv_type_object*)p)->m_class;
	}
	return false;
};

bool gv_type_object::is_equal(gv_byte* psrc, gv_byte* ptgt)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->is_equal(psrc, ptgt);
};

int gv_type_object::compare(gv_byte* psrc, gv_byte* ptgt)
{
	// under_construct;
	if (this->is_equal(psrc, ptgt))
		return 0;
	else
		return 1;
	// return -2;
}

void gv_type_object::construct(gv_byte* pdata)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->construct(pdata);
};

void gv_type_object::destroy(gv_byte* pdata)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->destroy(pdata);
};

void gv_type_object::read_data(gv_byte* pdata, gvi_stream* ps)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->read_data(pdata, ps);
}

void gv_type_object::write_data(gv_byte* pdata, gvi_stream* ps)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->write_data(pdata, ps);
}

void gv_type_object::copy_to(const gv_byte* psrc, gv_byte* ptgt)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->copy_to(psrc, ptgt);
}

void gv_type_object::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
								   int indent)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->get_persistent_class()->export_to_xml(pdata, text,
																indent);
}

bool gv_type_object::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
									 gv_sandbox* sandbox, gv_module* module)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->import_from_xml(pdata, ps, true, sandbox, module);
}
gv_string_tmp gv_type_object::get_type_string()
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->get_type_string();
}

bool gv_type_object::is_less(gv_byte* psrc, gv_byte* ptgt)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->is_less(psrc, ptgt);
};

void gv_type_object::visit_ptr(gv_byte* pb, gvi_object_ptr_visitor& v)
{
	m_class = m_class->fetch_completed_class();
	return this->m_class->visit_ptr(pb, v);
}

bool gv_type_object::is_copyable()
{
	bool b = m_class->is_copyable();
	if (b)
	{
		b = b;
	};
	return b;
};
}