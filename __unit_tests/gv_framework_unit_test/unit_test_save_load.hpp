namespace unit_test_save_load
{

void main(gvt_array< gv_string >& args)
{
	//SN_TEMP
	return; 
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{

		{
			sub_test_timer timer("test_module_save");
			m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
			gv_register_test_classes();
			gv_module* pmod_male = m_sandbox->create_object< gv_module >(gv_id("male3"));
			gv_test_human_record* john = m_sandbox->create_object< gv_test_human_record >(gv_id("john"), pmod_male);
			gv_test_human_job_info* john_job = m_sandbox->create_object< gv_test_human_job_info >(gv_id("john_job"), pmod_male);
			john_job->load_default();
			GV_ASSERT(john_job->money == 1000);
			john->clamp_var();
			john->job_info = john_job;
			john->my_box.set(gv_vector3(-1, -2, -3), gv_vector3(4, 5, 6));

			gv_module* pmod_female = m_sandbox->create_object< gv_module >(gv_id("female3"));
			gv_test_human_record* alice = m_sandbox->create_object< gv_test_human_record >(gv_id("alice"), pmod_female);
			gv_test_human_job_info* alice_job = m_sandbox->create_object< gv_test_human_job_info >(gv_id("alice_job"), pmod_female);
			alice->job_info = alice_job;
			alice->my_spheres.push_back(gv_sphere(gv_vector3(0.2f, 0.5f, 0.5f), 0.3f));
			alice->my_spheres.push_back(gv_sphere(gv_vector3(0.2f, -0.5f, 0.5f), 0.3f));

			//alice->friends.push_back(john);
			john->friends.push_back(alice);
			john->lover = alice;

			m_sandbox->save_module(gv_id("male3"));
			m_sandbox->save_module(gv_id("female3"));
			m_sandbox->export_module(gv_id("male3"));
			m_sandbox->export_module(gv_id("female3"));
			gv_unregister_test_classes();
			gv_global::sandbox_mama->delete_sandbox(m_sandbox);
			m_sandbox = NULL;
			gv_id::static_purge();
		}

		{
			sub_test_timer timer("test_module_load");
			m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
			gv_register_test_classes();
			gv_string_tmp s;
			gv_module* pmod_male = m_sandbox->load_module(gv_id("male3"));
			gv_object_iterator_safe it(m_sandbox);
			while (!it.is_empty())
			{
				s = "";
				it->export_object_xml(s);
				GVM_LOG(main, s);
				++it;
			};
			pmod_male->rename("male4");
			m_sandbox->export_module(gv_id("male4"));

			gv_module* pmod_female = m_sandbox->load_module(gv_id("female3"));
			pmod_female->rename("female4");
			m_sandbox->export_module(pmod_female->get_name_id());

			gv_unregister_test_classes();
			gv_global::sandbox_mama->delete_sandbox(m_sandbox);
			gv_id::static_purge();
			m_sandbox = NULL;
		}
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}
