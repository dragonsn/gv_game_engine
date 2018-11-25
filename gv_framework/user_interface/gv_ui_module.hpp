namespace gv
{
gv_ui_module::gv_ui_module()
{
	GVM_SET_CLASS(gv_ui_module);
	m_is_active = false;
};
gv_ui_module::~gv_ui_module(){

};
void gv_ui_module::load(){
	// the place to load layout and register handler
};
void gv_ui_module::unload(){

};
void gv_ui_module::active()
{
	// should show the ui
	m_is_active = true;
};
void gv_ui_module::deactive()
{
	m_is_active = false;
};
bool gv_ui_module::is_active()
{
	return m_is_active;
};
bool gv_ui_module::set_text(const char*)
{
	return true;
};
GVM_IMP_CLASS(gv_ui_module, gv_com_tasklet)
GVM_END_CLASS;
}