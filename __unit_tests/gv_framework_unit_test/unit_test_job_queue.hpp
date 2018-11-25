

namespace unit_test_job_queue
{
using namespace std;
static int s_count = 0;
class A : public gv_refable
{
public:
	A(){};
	~A()
	{
		int a = 0;
	}
	void print(int i)
	{
		cout << i << endl;
		s_count++;
	}

	void print3(gv_object* p, gv_string t, int i)
	{
		gv_string_tmp s;
		s << p->get_name_id() << " see " << t << i << gv_endl;
		cout << s;
		s_count++;
	}

	void print2(gv_object* p, int i)
	{
		gv_string_tmp s;
		s << p->get_name_id() << " see " << i << gv_endl;
		cout << s;
		s_count++;
	}
	void print1(gv_object* p, int i)
	{
		gv_string_tmp s;
		s << p->get_name_id() << " see " << i << gv_endl;
		cout << s;
		s_count++;
	}
	void ack()
	{
		int a = 0;
	}
};
gv_sandbox* m_sandbox;
gv_sandbox* get_sandbox()
{
	return m_sandbox;
}

enum
{
	gve_event_channel_u0=1000,
	gve_event_channel_u1=1001,
	gve_event_channel_u2=1002,
	gve_event_channel_u3=1003,
	gve_event_channel_u4=1004,
};

void test_1()
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{
		sub_test_timer timer("test_event_processor");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_render);

		m_sandbox->register_processor(gv_event_processor::static_class(), 1000);
		m_sandbox->register_processor(gv_event_processor::static_class(), 1001);
		m_sandbox->register_processor(gv_event_processor::static_class(), 1002);
		m_sandbox->register_processor(gv_event_processor::static_class(), 1003);

		m_sandbox->get_event_processor(gve_event_channel_render)->rename(gv_id_render);
		m_sandbox->get_event_processor(gve_event_channel_render)->set_synchronization(true);
		m_sandbox->get_event_processor(gve_event_channel_render)->set_autonomous(true);

		m_sandbox->get_event_processor(1000)->set_base_channel(gve_event_channel_render);
		m_sandbox->get_event_processor(1001)->set_base_channel(gve_event_channel_render);
		m_sandbox->get_event_processor(1002)->set_base_channel(gve_event_channel_render);
		m_sandbox->get_event_processor(1003)->set_base_channel(gve_event_channel_render);

		m_sandbox->get_event_processor(1000)->rename(gv_id("p1000"));
		m_sandbox->get_event_processor(1000)->set_synchronization(true);
		m_sandbox->get_event_processor(1000)->set_autonomous(true);
		m_sandbox->get_event_processor(1000)->set_fixed_delta_time(true,0.01f);
		
		m_sandbox->get_event_processor(1001)->rename(gv_id("p1001"));
		m_sandbox->get_event_processor(1001)->set_synchronization(true);
		m_sandbox->get_event_processor(1001)->set_autonomous(true);

		m_sandbox->get_event_processor(1002)->rename(gv_id("p1002"));
		m_sandbox->get_event_processor(1002)->set_synchronization(true);
		m_sandbox->get_event_processor(1002)->set_autonomous(true);

		m_sandbox->get_event_processor(1003)->rename(gv_id("p1003"));
		m_sandbox->get_event_processor(1003)->set_synchronization(true);
		m_sandbox->get_event_processor(1003)->set_autonomous(true);

		int loop = 1000;
		A a;
		while (loop--)
		{
			GVM_POST_JOB1(render, (&A::print1, &a, _1, loop));
#pragma GV_REMINDER("[PITFALL] don't use smart pointer in the post job like the commented code, it will have memory leak")
			//A * c=new A;
			//GVM_POST_JOB1(render,(&A::print2, gvt_ref_ptr<A>(c),_1, loop));
			GVM_POST_JOB1(render, (&A::print2, &a, _1, loop));
			GVM_POST_JOB1(render, (&A::print2, &a, _1, loop));
			GVM_POST_JOB1(render, (&A::print2, &a, _1, loop));
			GVM_POST_JOB1(render, (&A::print2, &a, _1, loop));
			GVM_POST_JOB1(render, (&A::print2, &a, _1, loop));
			m_sandbox->tick();
		}
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
		cout << "do jobs " << s_count << endl;
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}

void test_2()
{
	GV_PROFILE_EVENT(test_3, 1)
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	{
		sub_test_timer timer("test_parallel_for");
		
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_task_manager * mgr=gv_task_manager::static_register(m_sandbox); 
		//lambda
		mgr->parallel_for( 
		[](gv_int index ) { 
			GV_PROFILE_EVENT(do_print,index)
			std::cout << "do_job: " << index << ": "  << std::endl; 
		}, 1000
		);
		gv_sleep(100); 
		m_sandbox->tick();
		gv_sleep(100); 
		m_sandbox->tick();
		gv_sleep(100); 
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}

class test_data :public gv_refable
{
public:
	test_data() 
	{ 
		static int s_num = 0; 
		number = s_num++;
	};
	gv_int number; 
}; 

void test_3()
{
	GV_PROFILE_EVENT(test_3, 0)
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	gvt_array< test_data*> object_stream;

	
	{
		sub_test_timer timer("test_parallel_object_stream");

		object_stream.resize(1000);
		//lambda
		object_stream.for_each([](test_data* & ptr) {
			ptr = new test_data;
		}
		);

		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		gv_task_manager * mgr = gv_task_manager::static_register(m_sandbox);
		//lambda
		mgr->parallel_process_stream(
			[](test_data * data) {
			if (!data) return;
			GV_PROFILE_EVENT(do_process, data->number)
				std::cout << "process: " << data->number << ": " << std::endl;
		}
		, object_stream.begin(),object_stream.size()
		);
		gv_sleep(1000);
		m_sandbox->tick();
		gv_sleep(1000);
		m_sandbox->tick();
		gv_sleep(1000);
	}
	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}


void main(gvt_array< gv_string >& args)
{
	test_1();
	test_2();
	test_3();
}

}
