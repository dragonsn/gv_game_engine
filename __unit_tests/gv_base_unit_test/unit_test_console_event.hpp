namespace unit_test_console_event
{
using namespace std;
void main(gvt_array< gv_string >& args)
{
	gv_send_console_key_event('A');
	gv_send_console_key_event('B');
	gv_send_console_key_event('C');
	//gv_send_console_key_event('\r');
	//gv_send_console_key_event('\n');
	gv_command_console c;
	c.start();
	int i = 100;
	if (args.size())
	{
		args[0] >> i;
	}
	while (i-- > 0)
	{
		gvt_array< gv_string > ret;
		c.get_command_array(ret);
		for (int j = 0; j < ret.size(); j++)
		{
			test_log() << ret[j] << gv_endl;
		}
		gv_thread::sleep(1);
	}
	gv_thread::sleep(1);
	c.stop();
}
}
