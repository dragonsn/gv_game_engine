namespace gv
{
GVM_IMP_CLASS(gv_var_info, gv_object)
GVM_VAR(gv_int, m_offset)
GVM_VAR(gvt_ref_ptr< gvi_type >, m_type)
GVM_VAR(gvt_ptr< gv_class_info >, m_p_cls_loc)
GVM_VAR(gvt_ptr< gv_func_info >, m_p_func_loc)
GVM_VAR(gv_int, m_native_size)
GVM_VAR(gv_int, m_index)
GVM_VAR(gv_text, m_default)
GVM_VAR(gv_text, m_range)
GVM_VAR(gv_text, m_dcl)
GVM_END_CLASS

GVM_IMP_CLASS(gv_func_info, gv_object)
// GVM_VAR(gvt_array			<gvt_ref_ptr <gvi_type>	>		,
// m_parameters)
// GVM_VAR(gvt_array			<gvt_ref_ptr <gvi_type>	>		,
// m_locals	)
// GVM_VAR(gvt_ref_ptr		<gvi_type			>			,
// m_return_type)
// GVM_VAR(gvt_array			<gv_byte			>			,
// m_script)
// GVM_VAR(gvt_array			<gvt_ref_ptr<gv_var_info >	>	,
// m_param_vars)
// GVM_VAR(gvt_array			<gvt_ref_ptr<gv_var_info >	>	,
// m_local_vars)
// GVM_VAR(gv_uint			,m_local_size)
// GVM_VAR(gv_uint			,m_param_size)
GVM_VAR(gvt_ptr< gv_class_info >, m_cls)
GVM_VAR(gvt_ptr< gv_class_info >, m_param_struct)
GVM_VAR(gv_uint, m_func_flags)
GVM_VAR(gv_uint, m_func_index)
GVM_END_CLASS

GVM_IMP_CLASS(gv_class_info, gv_object)
GVM_VAR(gvt_ptr< gv_class_info >, m_super)
GVM_VAR(gvt_ptr< gv_var_info >, m_primary_key)
GVM_VAR(gv_uint, m_size)
GVM_VAR(gv_uint, m_aligment)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_func_info > >, m_p_vtl_tbl)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_var_info > >, m_p_var_tbl)
GVM_VAR(gv_id, m_super_name)
GVM_VAR(gvt_array< gv_id >, m_string_tbl)
GVM_VAR(gvt_ptr< gv_class_info >, m_replaced_class)
GVM_VAR(gv_uint, m_class_flags)
GVM_END_CLASS
}

namespace gv
{

gv_class_info::gv_class_info()
{
	this->m_size = 0;
	this->m_super = 0;
	this->m_class_flags = 0;
	this->m_constructor = 0;
	this->m_aligment = 4;
	this->m_super_name = gv_id_null;
	m_nb_var_to_serialize = 0;
	link_class(gv_class_info::static_class());
}

//=============================================================================================
gv_class_info::~gv_class_info()
{
}

void gv_class_info::clear_var_func_list()
{
	this->m_p_var_tbl.clear();
	this->m_p_vtl_tbl.clear();
};
//=============================================================================================
gv_func_info* gv_class_info::get_func(gv_uint index)
{
	if (!this->m_is_inited)
		this->initialize();
	return this->m_p_vtl_tbl[index];
}

//=============================================================================================
gv_class_info* gv_class_info::get_super() const
{
	return this->m_super;
};
//=============================================================================================
bool fn_is_equal(const gvt_ref_ptr< gv_func_info >& pf1,
				 const gvt_ref_ptr< gv_func_info >& pf2)
{
	return pf1->is_equal(pf2);
}
//=============================================================================================
void gv_class_info::initialize()
{
	if (this->m_is_inited)
		return;
	link_class(gv_class_info::static_class());
	GV_ASSERT(!this->m_is_not_completed);
	gvt_array< gvt_ref_ptr< gv_func_info > > v_tbl;
	if (this->m_super_name != gv_id_null)
		this->m_super = get_sandbox()->find_class(this->m_super_name);
	if (this->m_super)
	{
		if (this->m_super->m_name != this->m_super_name)
		{
			// must be replaced!
			gv_class_info* ps = this->m_super;
			while (ps)
			{
				if (ps->m_name == this->m_super_name)
					break;
				ps = ps->m_replaced_class;
			}
			GV_ASSERT(ps);
			this->m_super = ps;
		}
		this->m_super->initialize();
		v_tbl = this->m_super->m_p_vtl_tbl;
	}
	GV_ASSERT(this->m_super_name == gv_id_null || this->m_super);
	gv_class_info* psuper = this->m_super;
	gv_class_info* replace_class = NULL;

	while (psuper)
	{
		if (psuper->m_is_replaceable)
			replace_class = psuper;
		psuper = psuper->m_super;
	}
	if (replace_class)
	{
		gv_id name_backup = this->m_name.get_id();
		this->get_sandbox()->replace_class(replace_class->m_name.get_id(), this);
		m_replaced_class = replace_class;
	}
	//
	for (gv_int i = 0; i < this->m_p_vtl_tbl.size(); i++)
	{
		gv_int id = m_p_vtl_tbl[i]->m_func_index =
			v_tbl.add_unique(m_p_vtl_tbl[i], fn_is_equal, 1);
		v_tbl[id] = m_p_vtl_tbl[i];
	}
	this->m_nb_var_to_serialize = 0;
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		this->m_p_var_tbl[i]->initialize();
		if (!this->m_p_var_tbl[i]->m_is_transient)
			m_nb_var_to_serialize++;
	}
	this->m_p_vtl_tbl = v_tbl;
	this->m_is_inited = true;
	for (gv_int i = 0; i < this->m_p_vtl_tbl.size(); i++)
	{
		this->m_p_vtl_tbl[i]->initialize();
	}
	if (!this->m_is_struct)
	{
		// GVM_DEBUG_LOG (main,"CLASS: "<<(this->m_name)<<"\t\t initialized with "
		// <<this->m_p_vtl_tbl.size()<<" member functions size is
		// "<<this->m_size<<gv_endl);
	}
	else
	{
		// GVM_DEBUG_LOG (main,"STRUCT: "<<(this->m_name)<<"\t\t initialized with "
		// <<this->m_p_vtl_tbl.size()<<" member functions size is
		// "<<this->m_size<<gv_endl);
	}
}

//=============================================================================================
gv_int gv_class_info::get_func_idx(const gv_id& name)
{
	gv_int ret = -1;
	if (this->m_is_inited)
	{
		for (gv_int i = 0; i < this->m_p_vtl_tbl.size(); i++)
		{
			if (m_p_vtl_tbl[i]->get_name() == name)
			{
				ret = i;
				break;
			}
		}
	}
	else
	{
		gvt_array< gv_id > names;
		gvt_array< gv_class_info* > cls;
		gv_class_info* pc = this;
		while (pc)
		{
			cls.push_front(pc);
			pc = pc->m_super;
		}
		for (gv_int i = 0; i < cls.size(); i++)
		{
			for (gv_int j = 0; j < cls[i]->m_p_vtl_tbl.size(); j++)
			{
				names.add_unique(cls[i]->m_p_vtl_tbl[j]->m_name.get_id());
			}
		}
		ret = names.find(name);
	}
	return ret;
}

//=============================================================================================
gv_func_info* gv_class_info::get_func(const gv_id& name)
{
	gv_func_info* pinfo = NULL;
	for (gv_int i = 0; i < this->m_p_vtl_tbl.size(); i++)
	{
		if (name == m_p_vtl_tbl[i]->get_name().get_id())
		{
			pinfo = this->m_p_vtl_tbl[i];
			break;
		}
	}
	return pinfo;
}
//=============================================================================================
gv_uint gv_class_info::get_nb_func()
{
	return m_p_vtl_tbl.size();
};
//=============================================================================================
bool gv_class_info::is_derive_from(const gv_id& name) const
{
	bool ret = false;
	gv_class_info* pc = this->m_super;
	if (this->m_name == name)
	{
		ret = true;
		return ret;
	}
	GV_ASSERT(this->m_is_inited);
	while (pc)
	{
		if (pc->get_name() == name)
		{
			ret = true;
			return ret;
		};
		pc = pc->m_super;
	};
	return ret;
}

//=============================================================================================
bool gv_class_info::is_derive_from(gv_class_info* pinfo) const
{
	// GV_ASSERT(gv_object::static_is_valid(this));
	// GV_ASSERT(gv_object::static_is_valid(pinfo));
	bool ret = false;
	gv_class_info* pc = this->m_super;
	if (pinfo == this)
	{
		ret = true;
		return ret;
	}
	GV_ASSERT(this->m_is_inited);
	while (pc)
	{
		if (pc == pinfo)
		{
			ret = true;
			return ret;
		};
		pc = pc->m_super;
	};
	return ret;
};

//=============================================================================================
gv_byte* gv_class_info::create_instance()
{
	GV_PROFILE_EVENT(gv_class_info_create_instance, 0)
	gv_byte* pb = NULL;
	pb = new gv_byte[this->m_size];
	memset(pb, 0, this->m_size);
	this->construct(pb);
	load_default(pb, this->m_size);
	return pb;
};

//=============================================================================================
void gv_class_info::delete_instance(gv_byte* pb)
{
	this->destroy(pb);
	delete[] pb;
}
//=============================================================================================
gv_byte* gv_class_info::create_array(gv_int array_size)
{
	GV_PROFILE_EVENT(gv_class_info_create_array, 0)
	gv_byte* pb = NULL;
	pb = new gv_byte[this->get_aligned_size() * array_size];
	for (int i = 0; i < array_size; i++)
	{
		this->construct(pb + i * get_aligned_size());
	}
	return pb;
};
//=============================================================================================
void gv_class_info::delete_array(gv_byte* pb, gv_int array_size)
{
	for (int i = 0; i < array_size; i++)
	{
		this->destroy(pb + i * get_aligned_size());
	}
	delete[] pb;
};
//=============================================================================================

bool gv_class_info::add_func(gv_func_info* pinfo)
{
	this->m_p_vtl_tbl.add_unique(pinfo, fn_is_equal, 1);
	return 1;
}

//=============================================================================================
gv_var_info* gv_class_info::get_var(const gv_id& var_name)
{

	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		if (this->m_p_var_tbl[i]->get_name() == var_name)
			return this->m_p_var_tbl[i];
	}
	if (this->m_super)
		return this->m_super->get_var(var_name);
	return NULL;
}
//=============================================================================================
gv_var_info* gv_class_info::get_var(const char* var_name)
{

	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		if (this->m_p_var_tbl[i]->get_name() == gv_object_name(var_name))
			return this->m_p_var_tbl[i];
	}
	if (this->m_super)
		return this->m_super->get_var(var_name);
	return NULL;
}
//=============================================================================================
gv_var_info* gv_class_info::get_var(gv_int index)
{
	if (index < 0 || index >= this->m_p_var_tbl.size())
		return NULL;
	return this->m_p_var_tbl[index];
};
//=============================================================================================
bool gv_class_info::add_var(gv_var_info* pinfo)
{
	if (pinfo->m_offset == -1)
	{
		GV_ASSERT(!this->is_native());
		pinfo->initialize();
		if (!this->m_p_var_tbl.size())
		{
			pinfo->m_offset = this->m_super
								  ? gvt_align(this->m_super->get_size(),
											  pinfo->get_type()->get_alignment())
								  : 0;
		}
		else
		{
			gv_var_info* pre = *this->m_p_var_tbl.last();
			pinfo->m_offset = gvt_align(pre->m_offset + pre->get_size(),
										pinfo->get_type()->get_alignment());
		}
		this->m_size = gvt_align(pinfo->m_offset + pinfo->get_size(), 4);
		this->m_aligment =
			gvt_max(this->m_aligment, (gv_uint)pinfo->get_type()->get_alignment());
	}
	pinfo->m_index = m_p_var_tbl.size();
	this->m_p_var_tbl.add(pinfo);
	pinfo->set_owner(this);
	if (pinfo->is_primary_key())
	{
		this->m_primary_key = pinfo;
	}
	return 1;
}

//=============================================================================================
bool gv_class_info::is_equal(gv_byte* psrc, gv_byte* ptgt)
{
	if (!this->m_is_struct)
	{
		gv_object* p1 = (gv_object*)psrc;
		gv_object* p2 = (gv_object*)ptgt;
		if (p1->get_class() != p2->get_class())
			return false;
	};
	bool ret = false;
	if (this->get_super())
		ret = this->get_super()->is_equal(psrc, ptgt);
	if (!ret)
		return false;
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->is_transient())
			continue;
		if (pv->is_no_clone())
			continue;
		if (!(pv->get_type()->is_equal(psrc + pv->get_offset(),
									   ptgt + pv->get_offset())))
			return false;
	}
	return true;
};
bool gv_class_info::is_less(gv_byte* psrc, gv_byte* ptgt)
{
	bool ret = false;
	if (this->get_super())
	{
		ret = this->get_super()->is_less(psrc, ptgt);
		if (ret)
			return true;
		ret = this->get_super()->is_equal(psrc, ptgt);
		if (!ret)
			return false;
	}
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->is_transient())
			continue;
		if (pv->is_no_clone())
			continue;
		if (pv->get_type()->is_less(psrc + pv->get_offset(),
									ptgt + pv->get_offset()))
			return true;
		if (!pv->get_type()->is_equal(psrc + pv->get_offset(),
									  ptgt + pv->get_offset()))
			return false;
	}
	return false;
};
gv_int gv_class_info::compare(gv_byte* p1, gv_byte* p2)
{
	if (is_less(p1, p2))
		return -1;
	if (is_equal(p1, p2))
		return 0;
	return 1;
};

//=============================================================================================
void gv_class_info::construct(gv_byte* pdata)
{
	if (this->m_constructor)
	{
		this->m_constructor(pdata);
		return;
	} // native class
	GV_ASSERT(!(this->m_is_native) || this->m_is_struct);
	if (this->m_super)
		m_super->construct(pdata);
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		this->m_p_var_tbl[i]->get_type()->construct(
			pdata + this->m_p_var_tbl[i]->get_offset());
	}
};
//=============================================================================================
void gv_class_info::destroy(gv_byte* pdata)
{
	if (!this->m_is_struct)
	{
		if (this->m_is_native)
		{
			gvt_destroy((gv_object*)pdata);
		}
		else
		{
			for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
			{
				if (this->m_p_var_tbl[i])
					this->m_p_var_tbl[i]->get_type()->destroy(
						pdata + this->m_p_var_tbl[i]->get_offset());
			}
			if (this->m_super)
				this->m_super->destroy(pdata);
		}
	}
	else
	{
		for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
		{
			this->m_p_var_tbl[i]->get_type()->destroy(
				pdata + this->m_p_var_tbl[i]->get_offset());
		}
	}
};

//=============================================================================================
void gv_class_info::read_data(gv_byte* pdata, gvi_stream* ps)
{
	GV_PROFILE_EVENT(gv_class_info__read_dat, 0)
	// GV_PROFILE_EVENT_DYNAMIC(*m_name.get_id().string(),0);
	gv_object* p = (gv_object*)pdata;
	gv_module* pmod = (gv_module*)ps->get_user_data();
	if (!this->is_struct())
	{
		gv_object* p = (gv_object*)pdata;
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	gvi_stream& s = *ps;
	gv_object* pobject = (gv_object*)pdata;
	GV_ASSERT(pmod);
	if (this->get_super())
	{
		this->get_super()->read_data(pdata, ps);
	}
	gv_int nb_var;
	s >> nb_var;
	for (gv_int i = 0; i < nb_var; i++)
	{
		gv_int index;
		gv_int var_size;
		s >> index;
		s >> var_size;
		gv_var_info* pv = gvt_cast< gv_var_info >(pmod->index_to_object(index));
		if (!pv || pv->is_transient())
		{
			GVM_WARNING("[warning] failed to find the var with index "
						<< index << " in  class " << this->get_name_id()
						<< "var location is " << pmod->index_to_object_location(index)
						<< ", seek to skip " << var_size << gv_endl);
			s.seek_from_current(var_size);
			continue;
		}
		pv->get_type()->read_data(pdata + pv->get_offset(), ps);
	}
};

//=============================================================================================
void gv_class_info::write_data(gv_byte* pdata, gvi_stream* ps)
{
	gv_object* p = (gv_object*)pdata;
	gv_module* mod = (gv_module*)ps->get_user_data();
	if (!this->is_struct())
	{
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	if (this->get_super())
	{
		this->get_super()->write_data(pdata, ps);
	}
	gvi_stream& s = *ps;
	gvi_stream& byte_counter = *mod->get_size_counter();
	bool is_counting_byte = (ps == &byte_counter);

	gv_int nb_var = this->get_nb_var_for_save();
	s << nb_var;
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->is_transient())
			continue;
		byte_counter.visit_ptr_to_object(*((void**)&this->m_p_var_tbl[i]));
		s << mod->object_to_index(pv);
		gv_int size_of_var = 0;
		if (!is_counting_byte)
		{
			size_of_var = byte_counter.tell();
			pv->get_type()->write_data(pdata + pv->get_offset(), &byte_counter);
			size_of_var = byte_counter.tell() - size_of_var;
		};
		s << size_of_var;
		gv_int real_write_size = ps->tell();
		pv->get_type()->write_data(pdata + pv->get_offset(), ps);
		real_write_size = ps->tell() - real_write_size;
		if (!is_counting_byte)
		{
			GV_ASSERT(real_write_size == size_of_var);
		}
	}
};

//=============================================================================================
void gv_class_info::copy_to(const gv_byte* psrc, gv_byte* ptgt)
{
	if (this->m_is_not_completed)
		return;
	if (!this->m_is_struct)
	{
		gv_object* p1 = (gv_object*)psrc;
		gv_object* p2 = (gv_object*)ptgt;
		GV_ASSERT(p1->is_a(this));
		GV_ASSERT(p2->is_a(this));
	}
	if (get_super())
	{
		get_super()->copy_to(psrc, ptgt);
	}
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->m_is_no_clone || pv->m_is_transient)
			continue;
		pv->get_type()->copy_to(psrc + pv->get_offset(), ptgt + pv->get_offset());
	}
	return;
};

//=============================================================================================

void gv_class_info::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
								  gv_int indent)
{
	if (!this->m_is_struct)
	{
		gv_object* p = (gv_object*)pdata;
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	gv_string_tmp s;

	gvt_xml_write_element_open(text, *this->m_name.get_id().string(), false);
	gvt_xml_write_attribute(text, "number_property", this->m_nb_var_to_serialize);
	gvt_xml_write_element_open(text);

	if (this->get_super())
	{
		this->get_super()->export_to_xml(pdata, text, indent + 1);
	}
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->m_is_transient)
			continue;
		gvt_xml_write_element_open(text, *pv->get_name().get_id(), false);
		gvt_xml_write_attribute(text, "type", pv->get_type()->get_type_string());
		if (!pv->get_type()->is_complex_type())
		{
			gvt_xml_write_attribute_name(text, "value");
			pv->get_type()->export_to_xml(pdata + pv->get_offset(), text, indent + 1);
			text << "\t";
			gvt_xml_write_element_close(text);
		}
		else
		{
			gvt_xml_write_element_open(text);
			pv->get_type()->export_to_xml(pdata + pv->get_offset(), text, indent + 1);
			gvt_xml_write_element_close(text, *pv->get_name().get_id());
		}
	}
	gvt_xml_write_element_close(text, *this->m_name.get_id().string());
};

void gv_class_info::export_attribute(gv_byte* pdata, gv_string_tmp& text,
									 gv_int indent)
{
	if (!indent)
		gvt_xml_write_element_open(text, *this->m_name.get_id().string(), false);
	if (this->get_super())
	{
		this->get_super()->export_attribute(pdata, text, indent + 1);
	}
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		GV_ASSERT(!pv->get_type()->is_complex_type()); // to do!;
		if (pv->is_transient())
			continue;
		text << "\t";
		gvt_xml_write_attribute_name(text, *pv->get_name().get_id());
		pv->get_type()->export_to_xml(pdata + pv->get_offset(), text, indent);
	}
	gvt_xml_write_element_close(text);
};
//=============================================================================================
bool gv_class_info::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
									bool with_header, gv_sandbox* sandbox,
									gv_module* module)
{
	if (!this->m_is_struct)
	{
		gv_object* p = (gv_object*)pdata;
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	gv_string_tmp name;
	gv_xml_parser& text = *ps;
	gv_int number_property;
	if (with_header)
	{
		if (!text.read_element_open_get_name(name, false))
		{
			return false;
		}
		GV_ASSERT(name ==
				  this->m_name.get_id()
					  .string()); // this will happen if change the derivation
	}
	text.read_attribute("number_property", number_property);
	text.read_element_open();

	if (this->get_super())
	{
		this->get_super()->import_from_xml(pdata, ps, true, sandbox, module);
	}

	for (gv_int i = 0; i < number_property; i++)
	{

		text.read_element_open_get_name(name, false);
		gv_id id(*name);
		gv_var_info* pv = this->get_var(id);
		if (!pv)
		{
			GVM_WARNING(text.get_file_info() << "not found var " << name
											 << " in class " << this->m_name
											 << gv_endl);
			ps->step_out_element(*name);
			continue;
		};

		if (pv->m_is_transient)
		{
			GVM_WARNING(text.get_file_info() << "not found var " << name
											 << " in class " << this->m_name
											 << gv_endl);
			ps->step_out_element(*name);
			continue;
		};
		text.read_attribute("type", name);
		gv_xml_restore_gt_lt(name);
		gvt_ref_ptr< gvi_type > ptype;
		ptype = get_sandbox()->create_type(*name);
		if (!ptype->is_the_same_type(pv->m_type))
		{
			GVM_WARNING(text.get_file_info() << "var " << pv->m_name << " in class "
											 << this->m_name << "type mismatched!");
			// try to convert !
			text >> "value";
			text >> "=";
			if (ptype->get_type() == gve_data_type_string &&
				pv->get_type()->get_type() == gve_data_type_text)
			{
				gv_string stemp;
				gv_type_string type_string;
				type_string.import_from_xml(gvt_byte_ptr(stemp), ps, sandbox, module);
				gv_text* pt = (gv_text*)(pdata + pv->get_offset());
				*pt = *stemp;
				text.read_element_close();
				GVM_WARNING("converted string to Text!!!");
			}
			else
			{
				GVM_WARNING("failed to convert!!!");
				ps->step_out_element(*id.string());
			}
			continue;
		};

		if (!pv->get_type()->is_complex_type())
		{
			text >> "value";
			text >> "=";
			pv->get_type()->import_from_xml(pdata + pv->get_offset(), ps, sandbox,
											module);
			text.read_element_close();
		}
		else
		{
			text >> ">";
			pv->get_type()->import_from_xml(pdata + pv->get_offset(), ps, sandbox,
											module);
			text.read_element_close(*pv->get_name().get_id());
		}
	}
	text.read_element_close(*this->m_name.get_id().string());
	return 1;
};

bool gv_class_info::import_attribute(gv_byte* pdata, gv_xml_parser* ps,
									 gv_int indent)
{
	gv_string_tmp name, value;
	gv_xml_parser& text = *ps;
	if (this->get_super())
	{
		this->get_super()->import_attribute(pdata, ps, indent);
	}
	while (1)
	{
		gv_int c = text.look_ahead();
		if (!c)
			break;
		if (c == '/')
		{
			text.read_element_close();
			return true;
		}
		if (c == '>')
		{
			text.read_element_open();
			return false;
		}
		name = text.read_attribute_name();
		text >> "=";
		gv_id id(*name);
		gv_var_info* pv = this->get_var(id);
		if (!pv)
		{
			GVM_WARNING(text.get_file_info() << "not found var " << name
											 << " in class " << this->m_name
											 << gv_endl);
			text.read_attribute_value(value);
			continue;
		};

		if (pv->is_transient())
		{
			GVM_WARNING(text.get_file_info() << "not found var " << name
											 << " in class " << this->m_name
											 << gv_endl);
			text.read_attribute_value(value);
			continue;
		};
		pv->get_type()->import_from_xml(pdata + pv->get_offset(), ps);
	}
	return false;
};
//=============================================================================================
void gv_class_info::export_to_txt(gv_byte* pdata, gv_string_tmp& text)
{
	if (!this->m_is_struct)
	{
		gv_object* p = (gv_object*)pdata;
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	gv_string_tmp s;
	if (this->get_super())
	{
		this->get_super()->export_to_txt(pdata, text);
	}
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->m_is_transient)
			continue;

		if (pv->get_type()->get_type() == gve_data_type_object)
		{
			((gv_type_object*)(pv->get_type()))
				->get_class()
				->export_to_txt(pdata + pv->get_offset(), text);
		}
		else if (pv->get_type()->get_type() == gve_data_type_string)
		{
			// to avoid empty string
			pv->get_type()->export_to_xml(pdata + pv->get_offset(), text);
		}
		else
		{
			pv->get_type()->export_to_txt(pdata + pv->get_offset(), text);
			text << "\t";
		}
	}
};
//=============================================================================================
bool gv_class_info::import_from_txt(gv_byte* pdata, gv_xml_parser* ps,
									gv_sandbox* sandbox, gv_module* module)
{
	if (!this->m_is_struct)
	{
		gv_object* p = (gv_object*)pdata;
		GV_ASSERT(p->get_class()->is_derive_from(this) || p->get_class() == this);
	}
	gv_string_tmp name;
	gv_xml_parser& text = *ps;
	gv_int number_property = this->m_p_var_tbl.size();

	if (this->get_super())
	{
		get_super()->import_from_txt(pdata, ps, sandbox, module);
	}

	for (gv_int i = 0; i < number_property; i++)
	{
		gv_var_info* pv = this->m_p_var_tbl[i];
		if (pv->m_is_transient)
			continue;
		if (pv->get_type()->get_type() == gve_data_type_object)
		{
			((gv_type_object*)(pv->get_type()))
				->get_class()
				->import_from_txt(pdata + pv->get_offset(), ps, sandbox, module);
		}
		else if (pv->get_type()->get_type() == gve_data_type_string)
		{
			// to avoid empty string
			pv->get_type()->import_from_xml(pdata + pv->get_offset(), ps);
		}
		else
		{
			pv->get_type()->import_from_txt(pdata + pv->get_offset(), ps);
		}
	}
	return 1;
};
//=============================================================================================
gv_string_tmp gv_class_info::get_type_string()
{
	return gv_string_tmp(*this->m_name.m_id.string());
};

//=============================================================================================

void gv_class_info::set_super(gv_class_info* pcls)
{
	this->m_super = pcls;
	return;
}

//=============================================================================================
gv_func_info* gv_class_info::get_func_compiler(const gv_id& func_name)
{
	gv_func_info* ret = NULL;
	ret = this->get_func(func_name);
	if (ret)
		return ret;
	gv_class_info* psuper = this->m_super;
	while (psuper)
	{
		ret = psuper->get_func(func_name);
		if (ret)
			return ret;
		psuper = psuper->get_super();
	}
	return ret;
}

gv_class_info* gv_class_info::fetch_completed_class()
{
	if (this->m_is_not_completed)
	{
		gv_class_info* p = get_sandbox()->find_class(m_name.get_id());
		GV_ASSERT(p);
		GV_ASSERT(!p->m_is_not_completed);
		return p;
	}
	else
		return this;
}

void gv_class_info::relink_super()
{
	if (!this->m_super)
		return;
	if (this->m_is_native)
		return;
	if (this->m_super && this->m_super->m_is_not_completed)
	{
		this->m_super = this->m_super->fetch_completed_class();
	}
	this->m_super->relink_super();
	/*if (!this->m_IsNative && old_size!=this->m_Super->m_Size)*/ {
		gvt_array< gvt_ref_ptr< gv_var_info > > vars = this->m_p_var_tbl;
		this->m_p_var_tbl.clear();
		this->m_size = this->m_super->m_size;
		for (gv_int i = 0; i < vars.size(); i++)
		{
			vars[i]->m_offset = -1;
			this->add_var(vars[i]);
		}
	}
}

bool gv_class_info::query_property(gv_object* pobj, gv_lexer& lexer,
								   gv_byte*& pdata, gvi_type*& type,
								   gv_string_tmp& tail, gv_id& enum_id)
{
	using namespace gv_lang_cpp;
	gv_lexer* ps = &lexer;
	gv_type_object start_type;
	start_type.set_class(this);
	type = &start_type;
	pdata = (gv_byte*)pobj;
	bool ret = false;
	tail = "";
	while (1)
	{
		gv_int token;
		token = lexer.lex();
		switch (token)
		{
		case 0:
			if (type != &start_type)
				return true;
			return false;
			break;

		case TOKEN_ID:
		{
			switch (type->get_type())
			{
			case gve_data_type_array:
			{
				// must be m_size..
				tail = lexer.get_id();
				if (lexer.get_id() != "size")
					return false;
				// if (!lexer.is_empty()) return false;
				break;
			};

			case gve_data_type_color:
			{
				tail = lexer.get_id();
				// if (!lexer.is_empty())	return false;
				if (lexer.get_id() == "rgb")
					return true;
				if (lexer.get_id() == "a")
					return true;
				return false;
				break;
			}
			break;

			case gve_data_type_object:
			{
				gv_var_info* pinfo =
					((gv_type_object*)type)->get_class()->get_var(*ps->get_id());
				if (!pinfo)
					return false;
				enum_id = pinfo->get_enum_id();
				pdata = pdata + pinfo->get_offset();
				type = pinfo->get_type();
				break;
			}

			default:
				return false;
			}
		}
		break;

		case TOKEN_DOT:
		{
			if (ps->is_empty())
				return false;
		}
		break;

		case TOKEN_LB:
		{
			if (type->get_type() != gve_data_type_array &&
				type->get_type() != gve_data_type_static_array)
			{
				return false;
			}
			token = lexer.lex();
			if (token != TOKEN_ICON)
				return false;
			gv_int index = ps->get_int();
			if (type->get_type() == gve_data_type_array)
			{
				gv_type_array* t = (gv_type_array*)type;
				type = t->get_element_type();
				pdata = t->get_array_data(pdata) +
						index * t->get_element_type()->get_size();
			}
			if (type->get_type() == gve_data_type_static_array)
			{
				gv_type_static_array* t = (gv_type_static_array*)type;
				type = t->get_element_type();
				pdata = pdata += index * t->get_element_type()->get_size();
			}
			token = lexer.lex();
			if (token != TOKEN_RB)
				return false;
		}
		break;

		default:
			return false;
		}
	} // while 1
	return false;
};

gv_class_info* gv_class_info::get_persistent_class()
{
	if (!this->m_replaced_class)
		return this;
	gv_class_info* pcls = this->m_replaced_class;
	while (pcls->m_replaced_class)
	{
		pcls = pcls->m_replaced_class;
	}
	return pcls;
}

gv_int gv_class_info::get_hierarchy_distance_to(gv_class_info* pclass)
{
	gv_int dist = 0;
	if (this->is_derive_from(pclass))
	{
		gv_class_info* pc = this;
		while (pclass != pc)
		{
			dist++;
			pc = pc->get_super();
		}
		return dist;
	}
	else if (pclass->is_derive_from(this))
	{
		while (pclass != this)
		{
			dist++;
			pclass = pclass->get_super();
		}
		return -dist;
	}
	else
		return 10000;
}

void gv_class_info::set_function_list(struct gv_func_init_helper* first)
{
	if (!first)
		return;
	if (first->m_next)
		set_function_list(first->m_next);
	new gv_func_info(get_sandbox(), this, first);
};

void gv_class_info::visit_ptr(gv_byte* pdata, gvi_object_ptr_visitor& v)
{
	if (this->m_super)
		m_super->visit_ptr(pdata, v);
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pvar = this->m_p_var_tbl[i];
		if (!pvar->is_no_auto_clear())
			pvar->get_type()->visit_ptr(pdata + this->m_p_var_tbl[i]->get_offset(),
										v);
	}
};

bool gv_class_info::load_default(gv_byte* pstruct, gv_int size_of_struct)
{
	if (this->m_super)
		m_super->load_default(pstruct, size_of_struct);
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		gv_var_info* pvar = this->m_p_var_tbl[i];
		if (pvar->m_offset + pvar->get_size() > size_of_struct)
		{
			GV_ERROR_OUT("out of range !!");
		}
		pvar->set_var_to_default(pstruct);
	}
	return true;
}
bool gv_class_info::load_default(gv_object* pobj)
{
	GV_ASSERT(pobj->is_a(this));
	this->load_default((gv_byte*)pobj, this->get_size());
	return true;
};

bool gv_class_info::clamp_var(gv_object* pobj)
{
	bool ret = false;
	if (this->m_super)
		ret |= m_super->clamp_var(pobj);
	for (gv_int i = 0; i < this->m_p_var_tbl.size(); i++)
	{
		ret |= this->m_p_var_tbl[i]->clamp(pobj);
	}
	return ret;
}; // clamp var if out of range!!

bool gv_import_from_xml(gv_class_info* pcls, gv_byte* pb,
						const gv_string_tmp& file_name)
{
	gv_string_tmp text;
	bool ret = gv_load_file_to_string(*file_name, text);
	if (ret)
	{
		return gv_import_from_xml_in_memory(pcls, pb, text);
	}
	return ret;
}
bool gv_import_from_xml_in_memory(class gv_class_info* pcls, gv_byte* pb,
								  const gv_string_tmp& s)
{
	if (!s.strlen())
		return false;
	gv_xml_parser parser;
	parser.load_string(*s);
	gv_object_name bak = ((gv_object*)(pb))->get_name();
	pcls->import_from_xml(pb, &parser);
	((gv_object*)(pb))->rename(bak, false);
	return true;
};

bool gv_export_to_xml(gv_class_info* pcls, gv_byte* pb,
					  const gv_string_tmp& file_name)
{
	gv_string_tmp text;
	pcls->export_to_xml(pb, text);
	return gv_save_string_to_file(*file_name, text);
}
}