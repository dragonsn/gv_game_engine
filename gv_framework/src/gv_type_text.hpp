void gv_type_text::read_data(gv_byte* pdata, gvi_stream* ps)
{
	(*ps) >> get(pdata);
};

void gv_type_text::write_data(gv_byte* pdata, gvi_stream* ps)
{
	(*ps) << get(pdata);
};

void gv_type_text::export_to_xml(gv_byte* pdata, gv_string_tmp& text,
								 int indent)
{
	text << "<![CDATA[";
	text << get(pdata);
	text << "]]>";
};

bool gv_type_text::import_from_xml(gv_byte* pdata, gv_xml_parser* ps,
								   gv_sandbox* sandbox, gv_module* module)
{
	int toke = ps->lex();
	if (toke == gv_lang_xml::TOKEN_XML_CDATA_BLOCK)
	{
		//"<![CDATA[ ]]>"
		gv_string_tmp s = ps->get_id();
		if (s.strlen() > 12)
		{
			gv_string_tmp t = s.middle(9, s.strlen() - 12);
			t.replace_all("\\\r\n", "");
			t.replace_all("\\"
						  "\r",
						  "");
			t.replace_all("\\"
						  "\n",
						  "");
			get(pdata) = *t;
			return true;
		}
	}
	return false;
};
