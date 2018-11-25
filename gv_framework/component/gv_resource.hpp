namespace gv
{

gv_resource::gv_resource()
{
	link_class(gv_resource::static_class());
};
gv_resource::~gv_resource(){

};
gv_ulong gv_resource::last_visit_time_in_ms()
{
	return 0;
}
void gv_resource::update_visit_time(){

};
void gv_resource::load_data(){

};
void gv_resource::unload_data(){

};
void gv_resource::start_async_loading(){

};
void gv_resource::is_loading_complete(){

};
void gv_resource::wait_loading_complete(){

};
void gv_resource::set_loading_complete_callback(gv_load_complete_callback,
												gv_object*){

};
void gv_resource::get_file_name(gv_string_tmp& s)
{
	s = *m_file_name;
};
void gv_resource::set_file_name(const gv_string_tmp& s)
{
	m_file_name = *s;
};
const gv_text& gv_resource::get_file_name() const
{
	return m_file_name;
};

void gv_resource::query_reference(gvt_array< gv_string > result){

};
void gv_resource::set_loading_complete(){

};

GVM_IMP_CLASS(gv_resource, gv_object)
GVM_VAR_ATTRIB_SET(file_name)
GVM_VAR(gv_text, m_file_name)
GVM_END_CLASS

GVM_IMP_CLASS(gv_resource_cache, gv_object)
GVM_END_CLASS

GVM_BGN_FUNC(gv_resource, scp_recache)
uncache();
return 1;
GVM_END_FUNC

GVM_BGN_FUNC(gv_resource, scp_reimport)
get_sandbox()->force_reimport(get_location());
return 1;
GVM_END_FUNC
}