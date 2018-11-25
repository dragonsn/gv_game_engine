namespace unit_test_processor
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
	void print1(gv_object* p, int i)
	{
		cout << p->get_name_id() << " see " << i << endl;
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
void main(gvt_array< gv_string >& args)
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();

	{
		sub_test_timer timer("test_event_processor");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_world);
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_render);
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_physics);
		m_sandbox->register_processor(gv_event_processor::static_class(), gve_event_channel_net);

		m_sandbox->get_event_processor(gve_event_channel_world)->set_fixed_delta_time(true);

		m_sandbox->get_event_processor(gve_event_channel_render)->rename(gv_id_render);
		m_sandbox->get_event_processor(gve_event_channel_render)->set_synchronization(true);
		m_sandbox->get_event_processor(gve_event_channel_render)->set_autonomous(true);

		m_sandbox->get_event_processor(gve_event_channel_physics)->rename(gv_id_physic);
		m_sandbox->get_event_processor(gve_event_channel_physics)->set_fixed_delta_time(true);
		m_sandbox->get_event_processor(gve_event_channel_physics)->set_autonomous(true);

		int loop = 1000;
		A a;
		while (loop--)
		{
			GVM_POST_JOB(render, (&A::print, &a, loop));
			GVM_POST_JOB(render, (&A::print, gvt_ref_ptr< A >(new A), loop));
			GVM_POST_JOB_WITH_ACK(render, (&A::print, gvt_ref_ptr< A >(new A), loop), world, (&A::ack, &a));
			m_sandbox->tick();
		}
		cout << "do jobs " << s_count << endl;
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
	}

	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}
