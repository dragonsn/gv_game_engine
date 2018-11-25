

namespace unit_test_ref_count
{
void main(gvt_array< gv_string >& args)
{

	{
		sub_test_timer timer("multi_thread_log");
		gvt_ref_ptr< gv_refable > p = new gv_refable;
		GV_ASSERT(p->get_ref() == 1);
		gvt_ref_ptr< gv_refable > p2 = p;
		GV_ASSERT(p->get_ref() == 2);
		{
			gvt_ref_ptr< gv_refable > p3 = p;
			GV_ASSERT(p->get_ref() == 3);
		}
		GV_ASSERT(p->get_ref() == 2);
		p = NULL;
		GV_ASSERT(p2->get_ref() == 1);
	}
}
}
