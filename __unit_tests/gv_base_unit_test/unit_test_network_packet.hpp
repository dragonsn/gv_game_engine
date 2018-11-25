namespace unit_test_network_packet
{
struct user_data
{
	int a, b, c;
	gv_long ld;
};

struct user_data2
{
	int a, b, c;
	gv_long ld, ld2, ld3;
};

class packet_factory
{
public:
	gv_packet* create()
	{
		return new gv_packet;
	}
} ff;

void main(gvt_array< gv_string >& args)
{
	gv_packet packet;
	test_log() << "size of packet is " << sizeof(packet) << gv_endl;
	int number = 1235;
	packet << number;
	gv_string s("hello");
	packet << s;
	double d = 123.456;
	packet << d;

	packet.seek_from_begin(0);
	packet >> number;
	GV_ASSERT(number == 1235);
	packet >> s;
	GV_ASSERT(s == "hello");
	packet >> d;
	GV_ASSERT(d == 123.456);

	user_data d1;
	d1.a = 1;
	d1.b = 121;
	d1.c = 200;
	packet.set_user_data(d1);

	packet.seek_from_begin(0);
	packet >> number;
	GV_ASSERT(number == 1235);
	packet >> s;
	GV_ASSERT(s == "hello");
	packet >> d;
	GV_ASSERT(d == 123.456);

	user_data2 d2;
	d2.a = 2;
	d2.b = 22;
	d2.c = 222;
	packet.set_user_data(d2);
	packet.seek_from_begin(0);
	packet >> number;
	GV_ASSERT(number == 1235);
	packet >> s;
	GV_ASSERT(s == "hello");
	packet >> d;
	GV_ASSERT(d == 123.456);

	user_data2* pd = NULL;
	packet.get_user_data(pd);
	GV_ASSERT(pd->c == 222);

	packet.push_protocol_header(d1);
	packet.push_protocol_header(d2);

	packet.seek_from_begin(0);
	packet >> number;
	GV_ASSERT(number == 1235);
	packet >> s;
	GV_ASSERT(s == "hello");
	packet >> d;
	GV_ASSERT(d == 123.456);
	user_data* pud;
	user_data2* pud2;
	packet.get_protocol_header(0, pud);
	packet.get_protocol_header(1, pud2);
	GV_ASSERT(pud->c == 200);
	GV_ASSERT(pud2->c == 222);

	packet.get_user_data(pd);
	GV_ASSERT(pd->c == 222);

	gv_string_tmp stm;
	for (int i = 0; i < 400; i++)
	{
		stm << i << "\r\n";
	}
	packet.reset();
	packet.write(*stm, stm.strlen());
	gvt_dlist< gvt_ref_ptr< gv_packet > > plist;
	//int nb=packet.divide(100,plist,ff);
	gvt_dlist< gvt_ref_ptr< gv_packet > >::iterator it = plist.begin();
	while (it != plist.end())
	{
		char buffer[4096];
		gv_packet* p = *it;
		p->seek_from_begin(0);
		int i = p->read(buffer, 4096);
		buffer[i] = 0;
		test_log() << buffer;
		++it;
	}
}
}
