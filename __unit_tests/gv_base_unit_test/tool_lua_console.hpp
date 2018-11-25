

namespace  tool_lua_console
{


	void main(gvt_array<gv_string > &args)
	{
		if (args.size() == 0)  return; 
		gv_command_console c;
		c.add(new gv_commandlet_lua);
		c.start();
		gv_thread::sleep(1000000);
		c.stop();
	}
}
