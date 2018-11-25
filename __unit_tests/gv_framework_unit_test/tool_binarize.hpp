namespace tool_binarize 
{

	void main( gvt_array<gv_string > &args )
	{
		gv_global::sandbox_mama.get();
		gv_global::sandbox_mama->init();
		gv_sandbox * m_sandbox=NULL;
		bool do_verify=args.find("verify");
		gv_string_tmp file_name="ani_girl";
		gv_int index;
		if (args.find("file" ,index))
		{
			file_name=args[index+1];
		}
		else 
		{
			gv_global::sandbox_mama.destroy();
			gv_id::static_purge();
			return ; 
		}

		{
			m_sandbox=gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox() );
			m_sandbox->set_resource_root_path("../../../gv_resource");
			gv_module * pmod=m_sandbox->import_module(gv_id(*file_name)); 
			m_sandbox->save_module(gv_id(*file_name));
			gv_global::sandbox_mama->delete_sandbox(m_sandbox);
			m_sandbox=NULL;
			gv_id::static_purge();
		}

		if (do_verify)
		{
			m_sandbox=gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox() );
			m_sandbox->set_resource_root_path("../../../gv_resource");
			gv_module * pmod=m_sandbox->load_module(gv_id(*file_name)); 
			pmod->rename("test_mod");
			m_sandbox->export_module(pmod->get_name_id());
			gv_string file_name1;
			file_name1<<file_name<<".xml";
			gv_string file_name2="test_mod.xml";
			gv_string text1; 
			gv_string text2;
			gv_load_file_to_string(*file_name1,text1);
			gv_load_file_to_string(*file_name2,text2);
			text1.replace_all(*file_name, "test_mod");
			GV_ASSERT(text1==text2);
			gv_global::sandbox_mama->delete_sandbox(m_sandbox);
			m_sandbox=NULL;
			gv_id::static_purge();
		}
		gv_global::sandbox_mama.destroy();
		gv_id::static_purge();
	}
}


