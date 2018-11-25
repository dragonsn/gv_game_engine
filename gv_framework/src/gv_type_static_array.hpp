gv_type_static_array::gv_type_static_array()
{
	m_array_size = 0;
}

bool gv_type_static_array::is_the_same_type(gvi_type* p)
{
	if (!p)
		return false;
	if (p->get_type() == gve_data_type_static_array)
	{
		return this->get_element_type()->is_the_same_type(
			((gv_type_static_array*)p)->get_element_type());
	}
	return false;
};

bool gv_type_static_array::is_equal(gv_byte* psrc, gv_byte* ptgt)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = psrc;
	gv_byte* pt = ptgt;
	for (int i = 0; i < this->m_array_size; i++)
	{
		if (!this->get_element_type()->is_equal(ps, pt))
			return false;
		ps += esize;
		pt += esize;
	}
	return true;
};

gv_int gv_type_static_array::compare(gv_byte* psrc, gv_byte* ptgt)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = psrc;
	gv_byte* pt = ptgt;
	for (int i = 0; i < this->m_array_size; i++)
	{
		gv_int ret;
		ret = this->get_element_type()->compare(ps, pt);
		if (ret)
			return ret;
		ps += esize;
		pt += esize;
	}
	return 0;
};

void gv_type_static_array::construct(gv_byte* pdata)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = pdata;
	for (int i = 0; i < this->m_array_size; i++)
	{
		this->get_element_type()->construct(ps);
		ps += esize;
	}
};

void gv_type_static_array::destroy(gv_byte* pdata)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = pdata;
	for (int i = 0; i < this->m_array_size; i++)
	{
		this->get_element_type()->destroy(ps);
		ps += esize;
	}
};

void gv_type_static_array::read_data(gv_byte* pdata, gvi_stream* ps)
{
	int esize = this->get_element_type()->get_size();
	gv_int size_write;
	(*ps) >> size_write;
	gv_byte* p = pdata;
	gv_int size_read = gvt_min(this->m_array_size, size_write);

	for (int i = 0; i < size_read; i++)
	{
		this->get_element_type()->read_data(p, ps);
		p += esize;
	}
	if (size_write > this->m_array_size)
	{
		GVM_DEBUG_LOG(
			warning,
			"[warning] static array read size mismatch, seek to skip unread data");
		ps->seek_from_current(esize * (size_write - m_array_size));
	}
};

void gv_type_static_array::write_data(gv_byte* pdata, gvi_stream* ps)
{
	int esize = this->get_element_type()->get_size();
	(*ps) << this->m_array_size;
	gv_byte* p = pdata;
	for (int i = 0; i < m_array_size; i++)
	{
		this->get_element_type()->write_data(p, ps);
		p += esize;
	}
};

void gv_type_static_array::copy_to(const gv_byte* psrc, gv_byte* ptgt)
{
	int esize = this->get_element_type()->get_size();
	for (int i = 0; i < this->m_array_size; i++)
	{
		this->get_element_type()->copy_to(psrc, ptgt);
		psrc += esize;
		ptgt += esize;
	}
}

void gv_type_static_array::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
										 int indent)
{
	gvt_xml_write_element_open(text, "static_array", false);
	gvt_xml_write_attribute(text, "array_size", m_array_size);
	gvt_xml_write_element_open(text);
	gv_byte* parray_data = pdata;
	for (int i = 0; i < m_array_size; i++)
	{
		m_element_type->export_to_xml(parray_data, text, indent);
		text << gv_endl;
		parray_data += get_element_size();
	}
	gvt_xml_write_element_close(text, "static_array");
};

bool gv_type_static_array::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
										   gv_sandbox* sandbox,
										   gv_module* module)
{

	ps->read_element_open("static_array", false);
	gv_int size_write;
	ps->read_attribute("array_size", size_write);
	ps->read_element_open();

	gv_int size_read = gvt_min(this->m_array_size, size_write);
	gv_byte* parray_data = pdata;
	for (int i = 0; i < size_read; i++)
	{
		m_element_type->import_from_xml(parray_data, ps, sandbox, module);
		parray_data += get_element_size();
	}
	if (size_write > m_array_size)
	{
		GVM_DEBUG_LOG(
			main,
			"[warning] static array read size mismatch, seek to skip unread data");
		ps->step_out_element();
	}
	else
		ps->read_element_close("static_array");
	return true;
};

//
gv_string_tmp gv_type_static_array::get_type_string()
{
	gv_string_tmp s = "gvt_array_static{ ";
	s << m_element_type->get_type_string() << "," << m_array_size << " }";
	return s;
};

gv_byte* gv_type_static_array::get_element(gv_byte* parray, int index)
{
	GV_ASSERT(index < this->m_array_size);
	return parray + get_element_size() * index;
}

int gv_type_static_array::get_alignment()
{
	return m_element_type->get_alignment();
};

bool gv_type_static_array::is_less(gv_byte* psrc, gv_byte* ptgt)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = psrc;
	gv_byte* pt = ptgt;
	for (int i = 0; i < this->m_array_size; i++)
	{
		if (this->get_element_type()->is_less(ps, pt))
			return false;
		else if (!this->get_element_type()->is_equal(ps, pt))
			return false;
		ps += esize;
		pt += esize;
	}
	return true;
};

gve_data_type gv_type_static_array::get_type()
{
	return gve_data_type_static_array;
}

gv_int gv_type_static_array::get_size()
{
	return get_element_type()->get_size() * this->m_array_size;
}

bool gv_type_static_array::find(gv_byte* pdata, gv_byte* pelement, int& index)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = pdata;
	for (int i = 0; i < this->m_array_size; i++)
	{
		if (this->get_element_type()->is_equal(ps, pelement))
		{
			index = i;
			return true;
		}
		ps += esize;
	}
	return false;
};

void gv_type_static_array::sort(gv_byte* parray, bool ascend)
{
	gv_sort_with_type_info(get_element_type(), parray, m_array_size, ascend);
};

void gv_type_static_array::visit_ptr(gv_byte* pdata,
									 gvi_object_ptr_visitor& v)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = pdata;
	for (int i = 0; i < this->m_array_size; i++)
	{
		this->get_element_type()->visit_ptr(ps, v);
		ps += esize;
	}
};