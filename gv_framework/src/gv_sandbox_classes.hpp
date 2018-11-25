namespace gv
{
//---class   manager-------------------------------------------->
gv_class_info* gv_sandbox::find_class(const gv_id& name)
{
	gvt_ref_ptr< gv_class_info >* pp = m_pimpl->m_cls_map.find(name);
	if (!pp && this->get_outer())
	{
		return this->get_outer()->find_class(name);
	}
	if (pp)
		return *pp;
	return NULL;
};

gv_class_info* gv_sandbox::register_class(const gv_id& name)
{
	gvt_ref_ptr< gv_class_info >& pcls = m_pimpl->m_cls_map[name];
	GV_ASSERT(!pcls);
	pcls = new gv_class_info;
	pcls->m_name = name;
	this->add_object(pcls);
	GVM_DEBUG_OUT("!!!![class] [register] register a class "
				  << pcls->get_name_id() << "\t\t\t!!!in  sandbox "
				  << this->m_index);
	return pcls;
};

void gv_sandbox::unregister_class(const gv_id& name)
{
	gv_class_info* cls = m_pimpl->m_cls_map[name];
	// if (name=="gv_effect_param_info"){
	//	GV_DEBUG_BREAK;
	//}
	GV_ASSERT(cls);
	GVM_DEBUG_OUT("!!!![class] [unregister] a class " << cls->get_name_id()
													  << "\t\t\t!!!in  sandbox "
													  << this->m_index);
	m_pimpl->m_cls_map.erase(name);
	cls->clear_var_func_list();
	this->delete_object(cls);
};

void gv_sandbox::init_classes()
{
	gvt_array< gv_class_info* > all;
	gv_sandbox_data::class_map::iterator it = m_pimpl->m_cls_map.begin();
	gv_sandbox_data::class_map::iterator it_end = m_pimpl->m_cls_map.end();
	while (it != it_end)
	{
		if (!it.is_empty())
		{
			gv_class_info* pcls = *it;
			pcls->initialize();
		}
		++it;
	}
}

void gv_sandbox::query_classes_derived_from(
	const gv_id& super_name, gvt_array< gv_class_info* >& result)
{
	gvt_array< gv_class_info* > all;
	gv_sandbox_data::class_map::iterator it = m_pimpl->m_cls_map.begin();
	gv_sandbox_data::class_map::iterator it_end = m_pimpl->m_cls_map.end();
	while (it != it_end)
	{
		if (!it.is_empty())
		{
			// gv_id   id= it.get_key();
			gv_class_info* pcls = *it;
			if (pcls->is_derive_from(super_name))
				result.add(pcls);
		}
		++it;
	}
	if (this->get_outer())
		this->get_outer()->query_classes_derived_from(super_name, result);
};

void gv_sandbox::query_classes_with_prefix(const char* prefix,
										   gvt_array< gv_class_info* >& result)
{
	gvt_array< gv_class_info* > all;
	gv_sandbox_data::class_map::iterator it = m_pimpl->m_cls_map.begin();
	gv_sandbox_data::class_map::iterator it_end = m_pimpl->m_cls_map.end();
	while (it != it_end)
	{
		if (!it.is_empty())
		{
			gv_id id = it.get_key();
			gv_class_info* pcls = *it;
			gv_string_tmp s = *id.string();
			gv_string_tmp l;
			int length = gvt_strlen(prefix);
			s.left(length, l);
			if (l == prefix)
				result.add(pcls);
		}
		++it;
	}
};

//---type  -------------------------------------------->
#define GVM_DCL_PERSISTENT_TYPE_DEF_VAR
#include "gv_persistent_type.h"
gv_type_type* s_type_type;

void gv_sandbox::init_persistent_type()
{
	GV_PROFILE_EVENT(init_persistent_type, 0);
#define GVM_DCL_PERSISTENT_TYPE_INIT
#include "gv_persistent_type.h"
	s_type_type = new gv_type_type;
	m_pimpl->m_native_types.add(s_type_type);
}

gvi_type* gv_sandbox::create_type(class gv_xml_parser* ps)
{
	// already restore the lt gt operator ;
	using namespace gv_lang_xml;
	gv_string s;
	gv_int token = ps->lex();
	if (token != TOKEN_XML_NAME)
	{
		GV_ASSERT(0);
		return NULL;
	}
	else
	{
		gv_id_lexer lexer;
		lexer.goto_string(*ps->get_id());
		token = lexer.lex();
	}
	gvi_type* pret = NULL;

	switch (token)
	{
	case gv_id_space::TOKEN_array:
	{
		gv_type_array* p = new gv_type_array;
		ps->next_match(TOKEN_XML_ELE_BGN_LEFT);
		p->m_element_type = create_type(ps);
		ps->next_match(TOKEN_XML_ELE_BGN_RIGHT);
		pret = (gvi_type*)p;
	}
	break;

	case gv_id_space::TOKEN_static_array:
	{
		gv_type_static_array* p = new gv_type_static_array;
		ps->next_match(TOKEN_XML_ELE_BGN_LEFT);
		p->m_element_type = create_type(ps);
		ps->next_match(TOKEN_COMMA);
		(*ps) >> p->m_array_size;
		ps->next_match(TOKEN_XML_ELE_BGN_RIGHT);
		pret = (gvi_type*)p;
	}
	break;

	case gv_id_space::TOKEN_text:
	{
		gv_type_text* p = new gv_type_text;
		pret = (gvi_type*)p;
	}
	break;

	case gv_id_space::TOKEN_ptr:
	{
		gv_type_ptr* p = new gv_type_ptr;
		ps->next_match(TOKEN_XML_ELE_BGN_LEFT);
		p->m_pointed_type = create_type(ps);
		ps->next_match(TOKEN_XML_ELE_BGN_RIGHT);
		pret = (gvi_type*)p;
	}
	break;

	case gv_id_space::TOKEN_ref_ptr:
	{
		gv_type_ref_ptr* p = new gv_type_ref_ptr;
		ps->next_match(TOKEN_XML_ELE_BGN_LEFT);
		p->m_pointed_type = create_type(ps);
		ps->next_match(TOKEN_XML_ELE_BGN_RIGHT);
		pret = (gvi_type*)p;
	}
	break;

	case gv_id_space::TOKEN_gvi_type:
	{
		gv_type_type* p = s_type_type;
		pret = (gvi_type*)p;
	}
	break;

#define GVM_DCL_PERSISTENT_TYPE_CASE_SELECT
#include "gv_persistent_type.h"

	default:
	{
		gv_type_object* p = new gv_type_object;
		gv_class_info* pinfo;
		pinfo = this->find_class(gv_id(*ps->get_id()));
		if (!pinfo)
		{
			GV_ERROR_OUT("failed to find class " << *ps->get_id());
		}
		GV_ASSERT(pinfo);
		p->m_class = pinfo;
		pret = p;
	}
	}
	GV_ASSERT(pret);
	return pret;
};

gvi_type* gv_sandbox::create_type(const char* s)
{
	gv_xml_parser ps;
	ps.goto_string(s);
	return create_type(&ps);
}

gvi_type* gv_sandbox::create_type(gve_data_type type)
{
	switch (type)
	{
#define GVM_DCL_PERSISTENT_TYPE_CASE_CREATE
#include "gv_persistent_type.h"
	default:
		GV_ASSERT(0 && "unknow type");
	};
	return NULL;
};
void gv_sandbox::replace_class(const gv_id& name, gv_class_info* new_info)
{
	remove_class(name);
	this->m_pimpl->m_cls_map[name] = new_info;
}
void gv_sandbox::add_class(gv_class_info* pcls)
{
	this->m_pimpl->m_cls_map[pcls->m_name.get_id()] = pcls;
	GVM_DEBUG_OUT("!!!!add a class " << pcls->get_name_id() << "to sandbox "
									 << this->m_index);
};
void gv_sandbox::remove_class(const gv_id& name)
{
	this->m_pimpl->m_cls_map.erase(name);
};

gv_class_info* gv_sandbox::import_xml_element_simple(gv_xml_parser* parser,
													 gv_byte*& data,
													 gv_bool& closed)
{
	gv_string_tmp name;
	parser->read_element_open_get_name(name, false);
	gv_class_info* pcls = this->find_class(gv_id(*name));
	if (!pcls)
		return NULL;
	data = pcls->create_instance();
	closed = pcls->import_attribute(data, parser);
	return pcls;
};
bool gv_sandbox::import_xml_dom_attrib(gv_xml_parser* parser, gv_any& a,
									   gv_bool* pclosed)
{
	gv_class_info* pcls;
	a.clear();
	gv_byte*& data = a.m_data;
	gv_bool closed;
	pcls = this->import_xml_element_simple(parser, data, closed);
	if (pclosed)
		*pclosed = closed;
	if (pcls)
	{
		a.m_class = pcls;
		return true;
	}
	return false;
};

void gv_sandbox::export_xml_element_simple(gv_string_tmp& text,
										   gv_class_info* pcls, gv_byte* data)
{
	pcls->export_attribute(data, text);
};
}
