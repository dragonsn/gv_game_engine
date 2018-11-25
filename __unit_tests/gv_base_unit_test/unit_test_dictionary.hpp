namespace unit_test_dictionary
{
using namespace std;
void main(gvt_array< gv_string >& args)
{
	gvt_dictionary< int, gv_string > dict;
	dict.add_pair(23, "hello");
	GV_ASSERT(dict[23] == "hello");
	GV_ASSERT(dict["hello"] == 23);
	;
	gv_string c;
	int i;
	GV_ASSERT(!dict.find(22, c));
	GV_ASSERT(dict.find(23, c));
	GV_ASSERT(dict.find(c, i));
	GV_ASSERT(dict.find(c, i));
	GV_ASSERT(dict.erase_pair(i, c));
	dict.add_pair(13, "hello");
	dict.add_pair(23, "goodbye");
	dict.add_pair(26, "let's go");
	gvt_dictionary< int, gv_string >::iterator it = dict.begin();
	while (it != dict.end())
	{
		if (!it.is_empty())
		{
			test_log() << it.get_key() << " is " << *it << gv_endl;
		}
		it++;
	}
}
}