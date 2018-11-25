namespace unit_test_gv_xml_parser
{
void main(gvt_array< gv_string >& args)
{
	const char* xml_test_comments =
		"<!-- Our to do list data -->"
		"<!-- Our to do -list data -->"
		"<!-- Our to do -list- data -->"
		"<!-- -->"
		"<!-- \n"
		"Copyright 2008 Sony Computer Entertainment Inc.\n"
		"Licensed under the Creative Commons Attribution Noncommercial Share Alike license.\n"
		"See license file or www.creativecommons.org for details.\n"
		"-->";

	const char* xml_test_normal =
		"<?xml version=\"1.0\"  standalone='no' ?>\n"
		"<Attendee name=\"Voel\" position=\"counselor\" />"
		"<ToDo>\n"
		"<!-- Do I need a secure PDA? -->\n"
		"<Item priority=\"1\" distance='close'> Go to the <bold>Toy store!</bold></Item>"
		"<Item priority=\"2\" distance='none'> Do bills   </Item>"
		"<Item priority=\"2\" distance='far &amp; back'> Look for Evil Dinosaurs! </Item>"
		"</ToDo>";

	const char* xml_test_cdata =
		"<![CDATA["
		"I am > the rules!\n"
		"...since I make symbolic puns"
		"]]>"
		"<![CDATA[ ]]>"
		"<![CDATA[ ] ] ] ]]>";

	const char* xml_test_process =
		"<?xml version=\"1.0\"  standalone='no' ?>\n"
		"<?xml version=\"1.0\"  \n  standalone='no' ?>\n"
		"<?xml version=\"1.0\"  \n>  standalone='no' ?>\n";

	const char* xml_test_simple =
		"<ToDo>\n"
		"</ToDo>";

	{
		sub_test_timer timer("parser simple_xml_comment");
		using namespace gv::gv_lang_xml;
		gv_xml_parser parser;
		//gvt_lexer<gv_scanner_string>  parser;
		parser.load_string(xml_test_comments);
		test_log() << "------------------------" << gv_endl << xml_test_comments << gv_endl;
		int token = parser.lex();
		while (token)
		{
			GV_ASSERT(token == TOKEN_XML_COMMENT);
			test_log() << token_to_string(token) << "\t";
			token = parser.lex();
		}
	}

	{
		sub_test_timer timer("parser cdata_xml_");
		using namespace gv::gv_lang_xml;
		gv_xml_parser parser;
		parser.load_string(xml_test_cdata);
		test_log() << "------------------------" << gv_endl << xml_test_cdata << gv_endl;
		int token = parser.lex();
		while (token)
		{
			GV_ASSERT(token == TOKEN_XML_CDATA_BLOCK);
			test_log() << token_to_string(token) << "\t";
			token = parser.lex();
		}
	}

	{
		sub_test_timer timer("parser process_block");
		using namespace gv::gv_lang_xml;
		gv_xml_parser parser;
		parser.load_string(xml_test_process);
		test_log() << "------------------------" << gv_endl << xml_test_process << gv_endl;
		int token = parser.lex();
		while (token)
		{
			GV_ASSERT(token == TOKEN_XML_PROCESS_BLOCK);
			test_log() << token_to_string(token) << "\t";
			token = parser.lex();
		}
	}

	{
		sub_test_timer timer("parser normal");
		using namespace gv::gv_lang_xml;
		gv_xml_parser parser;
		parser.load_string(xml_test_normal);
		test_log() << "------------------------" << gv_endl << xml_test_normal << gv_endl;
		int token = parser.lex();
		while (token)
		{
			test_log() << token_to_string(token) << "\t";
			token = parser.lex();
		}
	}

	{
		sub_test_timer timer("parser xml doc0");
		using namespace gv::gv_lang_xml;
		document doc;
		gv_xml_parser parser;
		parser.load_string(xml_test_simple);
		parser.do_docment(doc);
		doc.dump_tree(test_log());
	}

	{
		sub_test_timer timer("parser xml doc");
		using namespace gv::gv_lang_xml;
		document doc;
		gv_xml_parser parser;
		parser.load_string(xml_test_normal);
		parser.do_docment(doc);
		doc.dump_tree(test_log());
	}

	{
		sub_test_timer timer("parser xml file(test.xml)");
		using namespace gv::gv_lang_xml;
		document doc;
		gv_xml_parser parser;
		parser.load_file("../test.xml");
		parser.do_docment(doc);
		doc.dump_tree(test_log());
	}

	{
		sub_test_timer timer("parser xml file(test.rfx)");
		using namespace gv::gv_lang_xml;
		document doc;
		gv_xml_parser parser;
		parser.load_file("../test.rfx");
		parser.do_docment(doc);
		doc.dump_tree(test_log());
	}

	{
		gv_string s = "just<test>";
		gv_xml_backup_gt_lt(s);
		GV_ASSERT(s == "just{test}");
		gv_xml_restore_gt_lt(s);
		GV_ASSERT(s == "just<test>");
	}
	{
		sub_test_timer timer("parser stream op");
		gv_xml_parser ps;
		gv_string s("hello");
		gv_uint nb = 1000000;
		ps << s << ":" << nb;
		ps.load_string(*ps.get_string());
		gv_string s2;
		gv_uint nb2;
		ps >> s2 >> ":" >> nb2;
		GV_ASSERT(s == s2);
		GV_ASSERT(nb == nb2);
		ps.load_string("");
		gvt_xml_write_attribute(ps, "nb", nb);
		gv_vector3 v(1, 3, 4), v2;
		gvt_xml_write_attribute(ps, "v", v);
		gv_string sss = *ps.get_string();

		ps.load_string(*ps.get_string());
		ps.read_attribute("nb", nb2);
		ps.read_attribute("v", v2);
		GV_ASSERT(nb2 == nb);
		GV_ASSERT(v2 == v);

		ps.load_string("");
		ps.goto_string(*sss);
		ps.read_attribute("nb", nb2);
		ps.read_attribute("v", v2);
		GV_ASSERT(nb2 == nb);
		GV_ASSERT(v2 == v);
	}
	{
		sub_test_timer timer("parser unified stream op");
		for (int i = 0; i < 100; i++)
		{
			gvt_vector_generic< gv_float, 4 > v;
			v.randomize(-100000, 100000);
			gv_string_tmp s;
			s << v;
			test_log() << s << gv_endl;
			gv_xml_parser parser;
			parser.load_string(*s);
			gvt_vector_generic< gv_float, 4 > v2;
			parser >> v2;
			GV_ASSERT(v == v2);

			gvt_matrix_generic< gv_float, 2, 3 > m, m2;
			m.randomize();
			s = "";
			s << m;
			test_log() << s << gv_endl;
			parser.load_string(*s);
			parser >> m2;
			GV_ASSERT(m == m2);
		};
	}
}
}
