#include "gv_framework_events.h"

namespace gv
{
class gv_test_human_job_info;
class gv_test_human_record : public gv_object
{
public:
	GVM_DCL_CLASS(gv_test_human_record, gv_object);
	gv_test_human_record()
	{
		processed = false;
		history = "born!!<<< \r\n \"baby\">>>>>>>boy or girl \r\n >>>>> old man \r\n >>>> dead !!!!!!:::::last";
		pos = gv_vector2i(13, 250);
		GVM_SET_CLASS(gv_test_human_record);
	}
	~gv_test_human_record()
	{
	}

	gv_int on_event(gv_object_event* pevent)
	{
		processed = true;
		return true;
	}
	gv_string hum_name;
	gv_int age;
	gv_vector2i pos;
	;
	gvt_ref_ptr< gv_test_human_job_info > job_info;
	gvt_ref_ptr< gv_test_human_record > lover;
	gvt_array< gvt_ptr< gv_test_human_record > > friends;
	gv_box my_box;
	gvt_array< gv_sphere > my_spheres;
	bool processed;
	gv_text history;
};

class gv_test_human_job_info : public gv_object
{
public:
	GVM_DCL_CLASS(gv_test_human_job_info, gv_object);
	gv_test_human_job_info()
	{
		GVM_SET_CLASS(gv_test_human_job_info);
	};
	gv_string job_name;
	gv_int money;
};

GVM_IMP_CLASS(gv_test_human_record, gv_object)
GVM_VAR(gv_string, hum_name)
GVM_VAR(gv_int, age)
GVM_VAR_RANGE("12"
			  "64")
GVM_VAR(gv_vector2i, pos);
GVM_VAR(gvt_ref_ptr< gv_test_human_job_info >, job_info)
GVM_VAR(gvt_ref_ptr< gv_test_human_record >, lover)
GVM_VAR(gvt_array< gvt_ptr< gv_test_human_record > >, friends)
GVM_VAR(gv_box, my_box)
GVM_VAR(gvt_array< gv_sphere >, my_spheres)
GVM_VAR(gv_text, history)
GVM_END_CLASS

GVM_IMP_CLASS(gv_test_human_job_info, gv_object)
GVM_VAR(gv_string, job_name)
GVM_VAR(gv_int, money)
GVM_VAR_DEFAULT("1000")
GVM_END_CLASS
}

namespace gv
{
#define GV_DOM_NAMESPACE dom_test
#define GV_DOM_FILE "gv_dom_test.h"
#include "gv_framework/inc/gv_data_model.h"
namespace dom_test
{
test_el_2& test_el_2::operator=(const test_el_2& t)
{
	this->att_b = t.att_b;
	return *this;
}
}
}

#include <boost/boost/type_traits/is_base_of.hpp>
namespace unit_test_sandbox
{

class test_event_handler
{
public:
	test_event_handler()
	{
		processed = false;
	}
	virtual ~test_event_handler()
	{
	}
	gv_int on_event(gv_object_event* pevent)
	{
		processed = true;
		return true;
	}
	bool processed;
};
void main(gvt_array< gv_string >& args)
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();

	{
		sub_test_timer timer("test_named_event");
		gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
		test_event_handler a;
		gv_object_event_handler_c< test_event_handler >* prec = new gv_object_event_handler_c< test_event_handler >(&a, &test_event_handler::on_event);
		sandbox->register_named_event(gv_id_client, prec);
		gv_object_event event;
		event.m_id = gv_object_event_id_named;
		event.m_name = gv_id_client;
		event.m_reciever = NULL;
		sandbox->process_event(&event);
		GV_ASSERT(a.processed);
		sandbox->unregister_named_event(gv_id_client, (gv_object*)&a);
	}

	{
		sub_test_timer timer("test_normal_event");
		gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
		test_event_handler a;
		gv_object_event_handler_c< test_event_handler >* prec = new gv_object_event_handler_c< test_event_handler >(&a, &test_event_handler::on_event);
		sandbox->register_event(gv_object_event_id_first, prec);
		gv_object_event event;
		event.m_id = gv_object_event_id_first;
		event.m_name = gv_id_client;
		event.m_reciever = NULL;
		sandbox->process_event(&event);
		GV_ASSERT(a.processed);
		sandbox->unregister_event(gv_object_event_id_first, (gv_object*)&a);
	}

	{
		sub_test_timer timer("test_object_event");
		gv_sandbox* sandbox = m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_register_test_classes();
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_default);
		if (boost::is_base_of< gv_object, gv_test_human_record >::value)
		{
			int i = 1; //boost!!
		}
		gv_test_human_record* john = m_sandbox->create_object< gv_test_human_record >(gv_id("john"), NULL);
		gv_object_event_handler_c< gv_test_human_record >* prec = new gv_object_event_handler_c< gv_test_human_record >(john, &gv_test_human_record::on_event);
		sandbox->register_event(gv_object_event_id_first, prec);
		gv_object_event event;
		event.m_id = gv_object_event_id_first;
		event.m_name = gv_id_client;
		event.m_reciever = NULL;
		sandbox->process_event(&event);
		GV_ASSERT(john->processed);
		sandbox->unregister_event(gv_object_event_id_first, john);

		gv_object_event_render_init e;
		GV_ASSERT(e.m_id == gv_object_event_id_render_init);
		gv_object_event_id_render_init;

		gv_unregister_test_classes();
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		m_sandbox = NULL;
		gv_id::static_purge();
	}

	{
		sub_test_timer timer("gv_object_static_class");
		{
			gv_class_info* pcls = gv_object::static_class();
			GVM_LOG(object, "<root>");
			GVM_LOG(main, "gv_object::static_class() :" << (gv_int_ptr)pcls);
			gv_sandbox* sandbox = gv_global::sandbox_mama->get_base_sandbox();
			gv_object* obj = gvt_cast< gv_object >(sandbox->create_object(gv_object::static_class(), gv_id("test1"), NULL));
			gv_module* mod = gvt_cast< gv_module >(sandbox->create_object(gv_module::static_class(), gv_id("test_module1"), NULL));
			obj->set_owner(mod);
			gv_string_tmp s;
			s << obj->get_location();
			gv_object_handle handle = obj->get_handle();
			GV_ASSERT(sandbox->is_valid(handle));
			GV_ASSERT(obj);
			GV_ASSERT(mod);
			{
				gv_string_tmp param;
				gv_string_tmp func = s;
				func << "."
					 << "scp_log";
				scp_log_param c;
				c.s = "script log test ";
				c.condition = 1;
				scp_log_param::static_class()->export_to_xml(gvt_byte_ptr(c), param);
				sandbox->exec_global_call(func, param);
				GVM_LOG(object, param);
			};
			//obj->scp_log(gv_id_none,,1  );
			//obj->scp_log(gv_id_none,*s,1  );
			gv_object_location loc;
			gv_xml_parser ps;
			ps.goto_string(*s);
			ps >> loc;
			GV_ASSERT(loc == obj->get_location());
			s = "";
			obj->export_object_xml(s);
			GVM_LOG(object, s);
			s = "";
			gv_object::static_class()->export_object_xml(s);
			GVM_LOG(object, s);

			ps.goto_string("(3)native:0/gv_object:0/m_runtime_index:0");
			ps >> loc;
			gv_object* po = sandbox->find_object(loc);
			GV_ASSERT(po);
			gv_var_info* pvar = gvt_cast< gv_var_info >(po);
			GV_ASSERT(pvar);
			s = "";
			pvar->export_object_xml(s);
			GVM_LOG(object, s);

			s = "	";
			//for ( int i=0; i< 100; i++)
			{
				gv_object_iterator_safe it(sandbox);
				while (!it.is_empty())
				{
					s = "";
					it->export_object_xml(s);
					++it;
				};
			}

			sandbox->delete_object(obj, true);
			sandbox->delete_object(mod, true);

			GV_ASSERT(!sandbox->is_valid(handle));
			loc.clear();
			handle.invalidate();
			gv_id::static_purge();
			gv_save_string_to_file("object.xml", s);
		}
	}

	{
		sub_test_timer timer("test_module_export_reference");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_register_test_classes();

		gv_module* pmod_male = m_sandbox->create_object< gv_module >(gv_id("male"));
		gv_test_human_record* john = m_sandbox->create_object< gv_test_human_record >(gv_id("john"), pmod_male);
		gv_test_human_job_info* john_job = m_sandbox->create_object< gv_test_human_job_info >(gv_id("john_job"), pmod_male);
		john_job->load_default();
		GV_ASSERT(john_job->money == 1000);
		john->clamp_var();
		john->job_info = john_job;
		john->my_box.set(gv_vector3(-1, -2, -3), gv_vector3(4, 5, 6));

		gv_module* pmod_female = m_sandbox->create_object< gv_module >(gv_id("female"));
		gv_test_human_record* alice = m_sandbox->create_object< gv_test_human_record >(gv_id("alice"), pmod_female);
		gv_test_human_job_info* alice_job = m_sandbox->create_object< gv_test_human_job_info >(gv_id("alice_job"), pmod_female);
		alice->job_info = alice_job;
		alice->my_spheres.push_back(gv_sphere(gv_vector3(0.2f, 0.5f, 0.5f), 0.3f));
		alice->my_spheres.push_back(gv_sphere(gv_vector3(0.2f, -0.5f, 0.5f), 0.3f));

		alice->friends.push_back(john);
		john->friends.push_back(alice);
		john->lover = alice;

		gv_string_tmp text;
		m_sandbox->export_module(gv_id("male"), text);
		gv_save_string_to_file("male.xml", text);

		text = "";
		m_sandbox->export_module(gv_id("female"), text);
		gv_save_string_to_file("female.xml", text);

		gv_unregister_test_classes();
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		m_sandbox = NULL;
		gv_id::static_purge();
	}

	{
		sub_test_timer timer("test_module_load_reference");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_register_test_classes();
		gv_string_tmp s;
		gv_module* pmod_male = m_sandbox->import_module(gv_id("male"));
		if (pmod_male) //posible for the first time skip the module
		{
			gv_object_iterator_safe it(m_sandbox);
			while (!it.is_empty())
			{
				s = "";
				it->export_object_xml(s);
				GVM_LOG(main, s);
				++it;
			};
			pmod_male->rename("male2");
			m_sandbox->export_module(gv_id("male2"));

			gv_module* pmod_female = m_sandbox->import_module(gv_id("female"));
			pmod_female->rename("female2");
			m_sandbox->export_module(pmod_female->get_name_id());
		}
		gv_unregister_test_classes();
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
	}
	{
		gv_sandbox* psandbox = gv_global::sandbox_mama->get_base_sandbox();
		gv_class_info* pclass = psandbox->find_class(gv_id("gv_sphere"));
		GV_ASSERT(pclass)
		gv_string_tmp text;
		gv_sphere sp;
		sp.set_center(gv_vector3(1, 2, 3));
		sp.set_radius(4);
		pclass->export_attribute(gvt_byte_ptr(sp), text);
		gv_xml_parser lex;
		lex.load_string(*text);
		gv_byte* pbyte;
		gv_bool b;
		gv_class_info* pcls2 = psandbox->import_xml_element_simple(&lex, pbyte, b);
		GV_ASSERT(pclass == pcls2);
		gv_sphere* psp = (gv_sphere*)pbyte;
		GV_ASSERT(sp == *psp);
		pcls2->delete_instance(pbyte);
	}
	{
		using namespace dom_test;
		sub_test_timer timer("test_module_test_dom");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv::dom_test::register_classes(m_sandbox);

		test_el_1 el1;
		gv_class_info* pcls = m_sandbox->find_class(gv_id("test_el_1"));
		GV_ASSERT(pcls);
		gv_string_tmp text;
		el1.att_s = "hello world";
		el1.att_f = 1.11f;
		el1.att_b = false;
		pcls->export_attribute(gvt_byte_ptr(el1), text);

		gv::dom_test::unregister_classes(m_sandbox);
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		m_sandbox = NULL;
		gv_id::static_purge();
		test_el_2 t22, t23;
		t22 = t23;
	}
	{
		gv_ani_play_info info1, info2;
		GV_ASSERT(info1 == info2);
		info1.m_current_time = 0;
		info2.m_current_time = 1;
		GV_ASSERT(info1 < info2);
		gvt_exp_to_xml(std::cout, info1);
	}

	{
		sub_test_timer timer("test_module_load_txt");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_register_test_classes();
		gv_module* pmod_male = m_sandbox->create_object< gv_module >(gv_id("male"));
		gv_test_human_record* john = m_sandbox->create_object< gv_test_human_record >(gv_id("john"), pmod_male);
		gv_test_human_job_info* john_job = m_sandbox->create_object< gv_test_human_job_info >(gv_id("john_job"), pmod_male);
		john_job->load_default();
		GV_ASSERT(john_job->money == 1000);
		john->clamp_var();
		john->job_info = john_job;
		john->my_box.set(gv_vector3(-1, -2, -3), gv_vector3(4, 5, 6));
		gv_string_tmp s_temp;
		john->export_object_txt(s_temp);
		gv_save_string_to_file("txt_save_file.txt", s_temp);
		gv_test_human_record* john_copy = m_sandbox->create_object< gv_test_human_record >(gv_id("john_copy"), pmod_male);
		gv_xml_parser ps;
		ps.goto_string(*s_temp);
		john_copy->import_object_txt(&ps, m_sandbox, pmod_male);
		gv_unregister_test_classes();
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		m_sandbox = NULL;
		gv_id::static_purge();
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
};
}
