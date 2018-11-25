
namespace unit_test_stats
{
using namespace std;
void main(gvt_array< gv_string >& args)
{
	gv_stats s;
	s.init();
	float f = 13;
	int a = 0;
	float ff = 250;
	s.register_category(gv_id_render, gv_color::GREEN_B(), true);
	s.register_stats(100, "fps", gv_id_render);
	s.register_stats(101, "drawcall", gv_id_render);
	s.register_stats(102, "memory(MB) ", gv_id_render, false);
	s.register_stats(103, "not_used ", gv_id_render);
	s.register_stats(104, "f ", gv_id_render, f);
	s.register_stats(105, "a ", gv_id_render, a, false);
	s.register_stats("ff", gv_id_render, ff);
	s.set_stats< gv_int >(100, 33);
	s.add_stats< gv_int >(101, 1);
	s.add_stats(101, 1);
	s.add_stats(101, 2);
	s.set_stats(102, 3.5f);
	s.add_stats(102, 0.5f);
	GVM_CONSOLE_OUT(s.to_string());
	s.reset_frame();
	a++;
	ff = 1213.f;
	GVM_CONSOLE_OUT(s.to_string());
}
}