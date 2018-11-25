

namespace unit_test_entity
{
void main(gvt_array< gv_string >& args)
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{
		sub_test_timer timer("test_simple_entity_get_com");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("entity_test"));
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), my_mod);
		gv_register_test_classes();
		my_entity->set_position(gv_vector3(12, 13, 14));
		my_entity->set_rotation(gv_euler(gv_float_trait::pi(), 0, 0));
		my_entity->update_matrix();

		gv_com_static_mesh* pmesh0 = m_sandbox->create_object< gv_com_static_mesh >(gv_id("static_mesh0"), my_entity);
		my_entity->add_component(pmesh0);
		gv_com_static_mesh* pmesh1 = m_sandbox->create_object< gv_com_static_mesh >(gv_id("static_mesh1"), my_entity);
		my_entity->add_component(pmesh1);
		gv_com_static_mesh* pmesh2 = m_sandbox->create_object< gv_com_static_mesh >(gv_id("static_mesh2"), my_entity);
		my_entity->add_component(pmesh2);

		my_entity->detach_component(pmesh1);

		gv_com_skeletal_mesh* pskeletal_mesh0 = m_sandbox->create_object< gv_com_skeletal_mesh >(gv_id("skeletal_mesh0"), my_entity);
		my_entity->add_component(pskeletal_mesh0);
		gv_com_skeletal_mesh* pskeletal_mesh1 = m_sandbox->create_object< gv_com_skeletal_mesh >(gv_id("skeletal_mesh1"), my_entity);
		my_entity->add_component(pskeletal_mesh1);
		gv_com_skeletal_mesh* pskeletal_mesh2 = m_sandbox->create_object< gv_com_skeletal_mesh >(gv_id("skeletal_mesh2"), my_entity);
		my_entity->add_component(pskeletal_mesh2);

		my_entity->detach_component(pskeletal_mesh0);

		gv_com_graphic* pgraphic_0 = m_sandbox->create_object< gv_com_graphic >(gv_id("grphic0"), my_entity);
		my_entity->add_component(pgraphic_0);
		gv_com_graphic* pgraphic_1 = m_sandbox->create_object< gv_com_graphic >(gv_id("grphic1"), my_entity);
		my_entity->add_component(pgraphic_1);

		gv_component* pcom_0 = m_sandbox->create_object< gv_component >(gv_id("com0"), my_entity);
		my_entity->add_component(pcom_0);
		gv_component* pcom_1 = m_sandbox->create_object< gv_component >(gv_id("com1"), my_entity);
		my_entity->add_component(pcom_1);

		GV_ASSERT(my_entity->get_component< gv_com_skeletal_mesh >() == pskeletal_mesh2);
		GV_ASSERT(my_entity->get_component< gv_com_static_mesh >() == pmesh0);
		gv_entity::com_query_result result;
		my_entity->query_components(result);
		GV_ASSERT(result.size() == 8);
		my_entity->add_component<gv_com_test_ai>();
		for (int i = 0; i < 100; i++)
		{
			my_entity->tick(0.1f);
		}

		my_entity->detach_component(gv_com_graphic::static_class());
		result.clear();
		my_entity->query_components(result);
		GV_ASSERT(result.size() == 7);
		gv_unregister_test_classes();
		m_sandbox->export_module(my_mod->get_name_id());
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
	}

	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
};
}
