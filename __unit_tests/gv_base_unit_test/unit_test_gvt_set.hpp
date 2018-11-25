namespace unit_test_gvt_set
{
void main(gvt_array< gv_string >& args)
{
	gvt_set< int > my_set;
	my_set.insert(2);
	my_set.insert(1);
	my_set.insert(5);
	my_set.lock();
	my_set.unlock();
	//std::copy(my_set.begin(), my_set.end(), std::ostream_iterator<int>(test_log(), " "));test_log()<<gv_endl;
}
}
