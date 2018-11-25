namespace gv
{

bool gv_sandbox::find_global_property(const gv_string_tmp& location,
									  gv_byte*& pdata, class gvi_type*& type,
									  gv_object*& outer_obj,
									  gv_string_tmp& tail_tag, gv_id& enum_id)
{
	gv_thread_lock lock(m_sandbox_mutex);
	gvt_array< gv_string > result;
	gv_object_location object_address;
	gv_lexer lexer;
	lexer.load_string(*location);
	lexer >> object_address;
	outer_obj = this->find_object(object_address);
	if (!outer_obj)
	{
		GVM_WARNING("failed to find the property " << location << gv_endl);
		return false;
	}
	bool found = outer_obj->get_class()->query_property(outer_obj, lexer, pdata,
														type, tail_tag, enum_id);
	if (!found)
	{
		GVM_WARNING("failed to find the property " << location << gv_endl);
	}
	return found;
};

bool gv_sandbox::assign_global_property_text(const gv_string_tmp& location,
											 const gv_string_tmp& new_value)
{
	bool ret = false;
	gv_byte* pval;
	gvi_type* ptype;
	gv_object* pobj;
	gv_string_tmp tail;
	gv_id enum_id;
	ret = gv_sandbox::find_global_property(location, pval, ptype, pobj, tail,
										   enum_id);
	if (!ret)
		return ret;
	gv_xml_parser paser;
	paser.load_string(*new_value);
	ret = ptype->import_from_xml(pval, &paser, this);
	return ret;
};

bool gv_sandbox::assign_global_property_binary(const gv_string_tmp& location,
											   gv_byte* new_value)
{
	bool ret = false;
	gv_byte* pval;
	gvi_type* ptype;
	gv_object* pobj;
	gv_string_tmp tail;
	gv_id enum_id;
	ret = gv_sandbox::find_global_property(location, pval, ptype, pobj, tail,
										   enum_id);
	if (!ret)
		return ret;
	ptype->copy_to(new_value, pval);
	ret = true;
	ret = true;
	return ret;
};

int gv_sandbox::compare_global_property_text(const gv_string_tmp& location,
											 const gv_string_tmp& new_value)
{
	int ret = -2;
	gv_byte* pval;
	gvi_type* ptype;
	gv_object* pobj;
	gv_string_tmp tail;
	gv_id enum_id;
	if (!gv_sandbox::find_global_property(location, pval, ptype, pobj, tail,
										  enum_id))
		return ret;
	gv_byte* pnew;
	pnew = ptype->create_instance();
	gv_xml_parser parser;
	parser.load_string(*new_value);
	if (!ptype->import_from_xml(pnew, &parser))
	{
		ptype->delete_instance(pnew);
		return ret;
	}
	ret = ptype->compare(pval, pnew);
	ptype->delete_instance(pnew);
	return ret;
};

int gv_sandbox::compare_global_property_binary(const gv_string_tmp& location,
											   gv_byte* new_value)
{
	int ret = -2;
	gv_byte* pval;
	gvi_type* ptype;
	gv_object* pobj;
	gv_string_tmp tail;
	gv_id enum_id;
	if (!gv_sandbox::find_global_property(location, pval, ptype, pobj, tail,
										  enum_id))
	{
		return -2;
	}
	ret = ptype->compare(pval, new_value);
	return ret;
};

int gv_sandbox::apply_ini_string(const gv_string& text)
{
	gvt_array< gv_string > statements;
	text.split(statements, "\n");
	for (int i = 0; i < statements.size(); i++)
	{
		gvt_array< gv_string > elements;
		statements[i].split(elements, "=");
		if (elements.size() != 2)
		{
			GVM_WARNING("skip line " << i << "in ini " << gv_endl);
			continue;
		};
		if (!gv_sandbox::assign_global_property_text(*elements[0], *elements[1]))
		{
			GVM_WARNING("error in apply ini line : " << i << ":" << elements[0] << "="
													 << elements[1] << gv_endl);
		}
	}
	return true;
};

int gv_sandbox::apply_ini_file(const char* filename)
{

	gv_string text;
	gv_load_file_to_string(filename, text);
	int ret = apply_ini_string(text);
	return ret;
};

int gv_sandbox::is_array_element(const gv_string_tmp& location,
								 gv_string& array_loc, int& array_idx,
								 int& array_size, int& array_el_size,
								 gv_byte*& parray, gvi_type*& parray_type)
{
	gv_byte *pprop, *parray_start;
	gvi_type* pptype;
	gv_object *ppobj, *parray_obj;
	gv_string_tmp tail;
	gv_id enum_id;
	if (!gv_sandbox::find_global_property(location, pprop, pptype, ppobj, tail,
										  enum_id))
	{
		return false;
	}
	char buffer[2048];
	gvt_strcpy(buffer, *location);
	char* ptr = buffer + gvt_strlen(buffer) - 1;
	while (ptr >= buffer && (*ptr) == ' ')
		ptr--;
	if (*ptr != ']' || ptr < buffer)
	{
		return false;
	}
	ptr--;
	while (ptr >= buffer && *ptr != '[')
		ptr--;
	if (ptr < buffer || *ptr != '[')
	{
		return false;
	}
	*ptr = 0;
	array_loc = buffer;
	gvt_strcat(buffer, "[%d]");
	int input_index;
	sscanf(*location, buffer, &input_index);
	/*
  gvt_array<gv_string> result;
  location.split(result,".");
  if (result.size()< 2) return false;
  array_loc=result[0];
  for ( int i=0; i< result.size()-2; i++){
  array_loc+=".";
  array_loc+=result[i+1];
  }*/

	if (!gv_sandbox::find_global_property(*array_loc, parray, parray_type,
										  parray_obj, tail, enum_id))
	{
		return false;
	}
	if (parray_type->get_type() == gve_data_type_array)
	{
		gv_type_array* ptarray = (gv_type_array*)parray_type;
		if (!ptarray->get_element_type()->is_the_same_type(pptype))
		{
			return false;
		}
		array_size = ptarray->get_array_size(parray);
		array_el_size = ptarray->get_element_type()->get_size();
		parray_start = ptarray->get_array_data(parray);
		if (pprop < parray_start &&
			pprop > parray_start + array_size * array_el_size)
		{
			return false;
		}
		array_idx = (gv_int)((pprop - parray_start) / array_el_size);
		assert(input_index == array_idx);
		return true;
	}
	return false;
}

//
gv_int gv_sandbox::exec_global_call(const gv_string_tmp& location,
									gv_string_tmp& param)
{
	using namespace gv_lang_cpp;
	gv_object* outer_obj;
	gv_thread_lock lock(m_sandbox_mutex);
	gvt_array< gv_string > result;
	gv_object_location object_address;
	gv_lexer lexer;
	lexer.load_string(*location);
	lexer >> object_address;
	outer_obj = this->find_object(object_address);
	if (!outer_obj)
		return 0;
	// lexer>>"."; //A BUG, 0. is recognized as a number but...we can just let it
	// be.
	gv_id func_id;
	lexer >> func_id;
	gv_func_info* pfunc = outer_obj->get_class()->get_func(func_id);
	if (!pfunc)
		return 0;
	gv_xml_parser parser;
	parser.goto_string(*param);
	gv_byte* param_b = 0;
	if (pfunc->get_param_struct())
	{
		param_b = pfunc->get_param_struct()->create_instance();
		if (!pfunc->get_param_struct()->import_from_xml(param_b, &parser, true,
														this))
		{
			pfunc->get_param_struct()->delete_instance(param_b);
			return false;
		};
	};
	gv_int ret = exec_global_call(outer_obj, pfunc, param_b);
	param = "";
	if (pfunc->get_param_struct())
	{
		pfunc->get_param_struct()->export_to_xml(param_b, param);
		pfunc->get_param_struct()->delete_instance(param_b);
	}
	return ret;
};

gv_int gv_sandbox::exec_global_call(gv_object* object, gv_func_info* func,
									gv_byte* param)
{
	return (object->*func->get_callback())(param);
};
}