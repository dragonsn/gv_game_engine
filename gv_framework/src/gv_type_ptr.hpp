namespace gv
{

bool gv_type_ptr::is_complex_type()
{
	if (is_struct_ptr())
		return true;
	return false;
	;
};

bool gv_type_ptr::is_object_ptr()
{
	gv_type_object* pcls = (gv_type_object*)this->m_pointed_type.ptr();
	return (this->m_pointed_type->get_type() == gve_data_type_object &&
			!pcls->m_class->m_is_struct);
};

bool gv_type_ptr::is_struct_ptr()
{
	gv_type_object* pcls = (gv_type_object*)this->m_pointed_type.ptr();
	return (this->m_pointed_type->get_type() == gve_data_type_object &&
			pcls->m_class->m_is_struct);
};

void gv_type_ptr::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
								int indent)
{
	if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* pi = (gvt_ptr< gv_object >*)pdata;
		if (*pi)
		{
			text << "\"" << (*pi)->get_location() << "\"";
		}
		else
			text << "\"" << gv_object_location() << "\"";
	}
	else if (this->get_pointed_type()->get_type() == gve_data_type_type)
	{
		gvt_ptr< gvi_type >* pi = (gvt_ptr< gvi_type >*)pdata;
		if (*pi)
			text << "\"" << (*pi)->get_type_string() << "\"";
	}
	else
	{
		// a point to structure.
		gvt_ptr< gv_byte >* pi = (gvt_ptr< gv_byte >*)pdata;
		if (*pi)
			this->get_pointed_type()->export_to_xml(*(gv_byte**)pdata, text,
													indent + 1);
	}
};
bool gv_type_ptr::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								  gv_sandbox* sandbox, gv_module* module)
{
	if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* pi = (gvt_ptr< gv_object >*)pdata;
		gv_object_location loc;
		ps->read_attribute_value(loc);
		if (!loc.size())
			(*pi) = NULL;
		else
		{
			if (sandbox)
			{
				gv_object* pobj = sandbox->find_object(loc);
				if (pobj)
					(*pi) = pobj;
				else
					module->register_import_ptr(loc, *pi);
			}
		}
	}
	else if (this->get_pointed_type()->get_type() == gve_data_type_type)
	{
		gvt_ptr< gvi_type >* pi = (gvt_ptr< gvi_type >*)pdata;
		gv_string_tmp s;
		ps->read_attribute_value(s);
		gv_xml_restore_gt_lt(s);
		*pi = sandbox->create_type(*s);
	}
	else
	{
		// a point to structure.
		gvt_ptr< gv_byte >* pi = (gvt_ptr< gv_byte >*)pdata;
		if (ps->is_element_will_close())
		{
			*pi = NULL;
		}
		else
		{
			gv_byte* pstruct = this->get_pointed_type()->create_instance();
			this->get_pointed_type()->import_from_xml(pstruct, ps, sandbox, module);
			*pi = pstruct;
		}
	}
	return true;
};

void gv_type_ptr::read_data(gv_byte* pdata, gvi_stream* ps)
{
	gv_module* mod = (gv_module*)ps->get_user_data();
	gvi_stream& str = *ps;
	gv_int index;
	str >> index;
	if (!index)
	{
		get(pdata) = NULL;
		return;
	}
	if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* pi = (gvt_ptr< gv_object >*)pdata;
		gv_object* pobject = mod->index_to_object(index);
		if (pobject)
			*pi = pobject;
		else
			mod->register_import_ptr(mod->index_to_object_location(index), *pi);
	}
	else if (this->get_pointed_type()->get_type() == gve_data_type_type)
	{
		gvt_ptr< gvi_type >* pi = (gvt_ptr< gvi_type >*)pdata;
		(*pi) = mod->index_to_type(index);
	}
	else
	{
		gv_byte* pnew = this->get_pointed_type()->create_instance();
		this->get_pointed_type()->read_data(pnew, ps);
		get(pdata).void_ptr() = pnew;
	}
}
void gv_type_ptr::write_data(gv_byte* pdata, gvi_stream* ps)
{
	gv_module* mod = (gv_module*)ps->get_user_data();
	gvi_stream& str = *ps;
	gv_int index;
	gv_string s;
	if (!get(pdata))
	{
		index = 0;
		str << index;
		return;
	}
	else if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* pi = (gvt_ptr< gv_object >*)pdata;
		str.visit_ptr_to_object(pi->void_ptr());
		index = mod->object_to_index(pi->ptr());
		str << index;
	}
	else if (this->get_pointed_type()->get_type() == gve_data_type_type)
	{
		gvt_ptr< gvi_type >* pi = (gvt_ptr< gvi_type >*)pdata;
		str.visit_ptr_to_type(pi->void_ptr());
		index = mod->type_to_index(pi->ptr());
		str << index;
	}
	else
	{
		gvt_ptr< gv_byte >* pi = (gvt_ptr< gv_byte >*)pdata;
		str.visit_ptr_to_struct(pi->void_ptr());
		index = (gv_int)(gv_int_ptr)pi->ptr();
		str << index;
		this->get_pointed_type()->write_data((gv_byte*)get(pdata).ptr(), ps);
	}
}

gv_string_tmp gv_type_ptr::get_type_string()
{
	gv_string_tmp s;
	s << "gvt_ptr { ";
	s << this->get_pointed_type()->get_type_string();
	s << " } ";
	return s;
}
gvi_type* gv_type_ptr::get_pointed_type()
{
	return this->m_pointed_type;
}
bool gv_type_ptr::is_the_same_type(gvi_type* p)
{

	if (p && p->get_type() == gve_data_type_ptr)
	{
		gv_type_ptr* pp = (gv_type_ptr*)p;
		if (!this->m_pointed_type || !pp->m_pointed_type)
			return true;
		if (this->m_pointed_type->get_type() == gve_data_type_object &&
			pp->m_pointed_type->get_type() == gve_data_type_object)
			return true;
		if (this->m_pointed_type->is_the_same_type(pp->m_pointed_type))
			return true;
	}
	return false;
}

void gv_type_ptr::visit_ptr(gv_byte* pdata, gvi_object_ptr_visitor& v)
{
	if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* pi = (gvt_ptr< gv_object >*)pdata;
		gvt_ref_ptr< gv_object > ref_ptr;
		v(*pi, ref_ptr);
	}
}

void gv_type_ptr::copy_to(const gv_byte* psrc, gv_byte* ptgt)
{
	if (this->is_object_ptr())
	{
		gvt_ptr< gv_object >* s = (gvt_ptr< gv_object >*)psrc;
		gvt_ptr< gv_object >* t = (gvt_ptr< gv_object >*)ptgt;
		(*t) = (*s);
		return;
	}
	else if (this->get_pointed_type()->get_type() == gve_data_type_type)
	{
		gvt_ptr< gvi_type >* s = (gvt_ptr< gvi_type >*)psrc;
		gvt_ptr< gvi_type >* t = (gvt_ptr< gvi_type >*)ptgt;
		(*t) = (*s);
		return;
	}
	else
	{
		// a point to structure.
		gvt_ptr< gv_byte >* s = (gvt_ptr< gv_byte >*)psrc;
		gvt_ptr< gv_byte >* t = (gvt_ptr< gv_byte >*)ptgt;
		if (*t)
		{
			this->get_pointed_type()->delete_instance(*t);
			*t = NULL;
		}
		if (*s)
		{
			gv_byte* pstruct = this->get_pointed_type()->create_instance();
			this->get_pointed_type()->copy_to((*s).ptr(), pstruct);
			(*t) = pstruct;
		}
		else
		{
			*t = NULL;
		};
	}
}
}