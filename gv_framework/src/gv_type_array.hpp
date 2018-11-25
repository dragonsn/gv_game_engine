gv_type_array::gv_type_array(){};

int gv_type_array::get_size()
{
	return sizeof(type_of_byte_array);
};

int gv_type_array::get_alignment()
{
	return gvt_get_alignment< type_of_byte_array >();
};
gve_data_type gv_type_array::get_type()
{
	return gve_data_type_array;
};
gv_string_tmp gv_type_array::get_type_string()
{
	gv_string_tmp s = "gvt_array{ ";
	s += m_element_type->get_type_string();
	s += " }";
	return s;
};

bool gv_type_array::is_the_same_type(gvi_type* type)
{
	if (!type)
		return false;
	if (type->get_type() == gve_data_type_array)
	{
		gv_type_array* p = (gv_type_array*)type;
		return get_element_type()->is_the_same_type(p->get_element_type());
	}
	return false;
};
bool gv_type_array::is_equal(gv_byte* psrc, gv_byte* ptgt)
{
	int i_s = get_array_size(psrc);
	int i_t = get_array_size(ptgt);
	if (i_s != i_t)
		return false;
	int step = get_element_type()->get_size();
	ptgt = get_array_data(ptgt);
	psrc = get_array_data(psrc);
	for (int i = 0; i < i_s; i++)
	{
		if (!get_element_type()->is_equal(psrc, ptgt))
			return false;
		psrc += step;
		ptgt += step;
	}
	return true;
};
bool gv_type_array::is_less(gv_byte* psrc, gv_byte* ptgt)
{
	int i_s = get_array_size(psrc);
	int i_t = get_array_size(ptgt);
	int nb = gvt_min(i_s, i_t);
	int step = get_element_type()->get_size();
	ptgt = get_array_data(ptgt);
	psrc = get_array_data(psrc);
	for (int i = 0; i < nb; i++)
	{
		if (get_element_type()->is_less(psrc, ptgt))
			return true;
		else if (!get_element_type()->is_equal(psrc, ptgt))
			return false;
		psrc += step;
		ptgt += step;
	}
	if (i_s < i_t)
		return true;
	return false;
};

gv_int gv_type_array::compare(gv_byte* p1, gv_byte* p2)
{
	if (is_equal(p1, p2))
		return 0;
	if (is_less(p1, p2))
		return -1;
	return 1;
};

void gv_type_array::construct(gv_byte* pdata)
{
	gvt_construct< type_of_byte_array >((type_of_byte_array*)pdata);
};

void gv_type_array::destroy(gv_byte* pdata)
{
	gv_byte* parray_data = get_array_data(pdata);
	for (int i = 0; i < get_array_size(pdata); i++)
	{
		m_element_type->destroy(parray_data);
		parray_data += get_element_size();
	}
	type_of_byte_array& array = get_byte_array(pdata);
	int esize = this->get_element_type()->get_size();
	array.array_size *= esize;
	array.array_max *= esize;
	gvt_destroy(&array);
};
void gv_type_array::read_data(gv_byte* pdata, gvi_stream* ps)
{
	gv_int array_size;
	(*ps) >> array_size;
	gv_module* mod = (gv_module*)ps->get_user_data();
	this->resize_array(pdata, array_size);
	gv_byte* parray_data = get_array_data(pdata);
	if (m_element_type->is_copyable() &&
		mod->get_module_version() >=
			gve_module_version::gve_version_opt_array_sl)
	{
		gv_int size;
		(*ps) >> size;
		gv_int cu_size = get_element_size() * array_size;
		GV_ASSERT(size == cu_size && " array load size mismatch, this may be "
									 "because change of a native type or platform "
									 "compatibity ");
		ps->read(parray_data, cu_size);
	}
	else
	{
		for (int i = 0; i < get_array_size(pdata); i++)
		{
			m_element_type->read_data(parray_data, ps);
			parray_data += get_element_size();
		}
	}
};

void gv_type_array::write_data(gv_byte* pdata, gvi_stream* ps)
{
	gv_int array_size = get_array_size(pdata);
	(*ps) << array_size;
	gv_byte* parray_data = get_array_data(pdata);
	gv_module* mod = (gv_module*)ps->get_user_data();
	if (m_element_type->is_copyable() &&
		mod->get_module_version() >=
			gve_module_version::gve_version_opt_array_sl)
	{
		gv_int size = get_element_size() * array_size;
		(*ps) << size;
		ps->write(parray_data, size);
	}
	else
	{
		for (int i = 0; i < get_array_size(pdata); i++)
		{
			m_element_type->write_data(parray_data, ps);
			parray_data += get_element_size();
		}
	}
};
void gv_type_array::copy_to(const gv_byte* psrc, gv_byte* ptgt)
{
	int i_s = get_array_size((gv_byte*)psrc);
	resize_array(ptgt, i_s);
	int step = get_element_type()->get_size();
	ptgt = get_array_data(ptgt);
	psrc = get_array_data((gv_byte*)psrc);
	for (int i = 0; i < i_s; i++)
	{
		get_element_type()->copy_to(psrc, ptgt);
		psrc += step;
		ptgt += step;
	}
	return;
};

void gv_type_array::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
								  int indent)
{
	gvt_xml_write_element_open(text, "array", false);
	gvt_xml_write_attribute(text, "array_size", get_array_size(pdata));
	gvt_xml_write_element_open(text);
	gv_byte* parray_data = get_array_data(pdata);
	for (int i = 0; i < get_array_size(pdata); i++)
	{
		m_element_type->export_to_xml(parray_data, text, indent);
		text << gv_endl;
		parray_data += get_element_size();
	}
	gvt_xml_write_element_close(text, "array");
};

bool gv_type_array::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
									gv_sandbox* sandbox, gv_module* module)
{
	gv_int array_size;
	if (ps->look_ahead() != '<')
	{
		// only set the size!!
		ps->read_attribute_value(array_size);
		resize_array(pdata, array_size);
		return true;
	}
	ps->read_element_open("array", false);
	ps->read_attribute("array_size", array_size);
	ps->read_element_open();
	resize_array(pdata, array_size);
	gv_byte* parray_data = get_array_data(pdata);
	for (int i = 0; i < get_array_size(pdata); i++)
	{
		m_element_type->import_from_xml(parray_data, ps, sandbox, module);
		parray_data += get_element_size();
	}
	ps->read_element_close("array");
	return true;
};

gv_int gv_type_array::get_array_size(gv_byte* parray)
{
	type_of_byte_array& a = get_byte_array(parray);
	return a.size();
};

gvi_type* gv_type_array::get_element_type()
{
	return m_element_type;
};

gv_byte* gv_type_array::get_element(gv_byte* parray, int index)
{
	GV_ASSERT(index < this->get_array_size(parray));
	return parray + get_element_size() * index;
}

void gv_type_array::insert_element(gv_byte* parray, gv_byte* pelement,
								   int index)
{
	type_of_byte_array& array = get_byte_array(parray);
	int esize = this->get_element_type()->get_size();
	array.array_size *= esize;
	array.array_max *= esize;
	array.insert(pelement, index * esize, esize);
	this->get_element_type()->construct(array.begin() + esize * index);
	this->get_element_type()->copy_to(pelement, array.begin() + esize * index);
	array.array_size /= esize;
	array.array_max /= esize;
};
bool gv_type_array::find(gv_byte* pdata, gv_byte* pelement, int& index)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = this->get_array_data(pdata);
	for (int i = 0; i < this->get_array_size(pdata); i++)
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
void gv_type_array::resize_array(gv_byte* parray, int size)
{
	int old_size = this->get_array_size(parray);
	if (old_size == size)
		return;
	else if (old_size < size)
	{
		type_of_byte_array& array = get_byte_array(parray);
		int esize = this->get_element_type()->get_size();
		array.array_size *= esize;
		array.array_max *= esize;
		array.resize(size * esize);
		for (int i = old_size; i < size; i++)
		{
			this->get_element_type()->construct(array.begin() + i * esize);
		}
		array.array_size /= esize;
		array.array_max /= esize;
	}
	else
	{
		type_of_byte_array& array = get_byte_array(parray);
		int esize = this->get_element_type()->get_size();
		array.array_size *= esize;
		array.array_max *= esize;
		for (int i = size; i < old_size; i++)
		{
			this->get_element_type()->destroy(array.begin() + i * esize);
		}
		array.resize(size * esize);
		array.array_size /= esize;
		array.array_max /= esize;
	}
};
void gv_type_array::erase(gv_byte* parray, int i)
{
	type_of_byte_array& array = get_byte_array(parray);
	int esize = this->get_element_type()->get_size();
	array.array_size *= esize;
	array.array_max *= esize;
	this->get_element_type()->destroy(array.begin() + esize * i);
	array.erase(i * esize, esize);
	array.array_size /= esize;
	array.array_max /= esize;
};
gv_byte* gv_type_array::get_array_data(gv_byte* parray)
{
	return get_byte_array(parray).begin();
};

void gv_type_array::push_back(gv_byte* parray, gv_byte* pelement)
{
	insert_element(parray, pelement, get_array_size(parray));
};

void gv_type_array::pop_back(gv_byte* parray)
{
	if (get_array_size(parray))
		erase(parray, get_array_size(parray) - 1);
};

void gv_type_array::sort(gv_byte* parray, bool ascend)
{
	gv_sort_with_type_info(get_element_type(), get_array_data(parray),
						   get_array_size(parray), ascend);
};

void gv_type_array::visit_ptr(gv_byte* pdata, gvi_object_ptr_visitor& v)
{
	int esize = this->get_element_type()->get_size();
	gv_byte* ps = this->get_array_data(pdata);
	for (int i = 0; i < this->get_array_size(pdata); i++)
	{
		this->get_element_type()->visit_ptr(ps, v);
		ps += esize;
	}
};