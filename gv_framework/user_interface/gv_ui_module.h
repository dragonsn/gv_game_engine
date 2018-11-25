#pragma once
namespace gv
{
class gv_ui_module : public gv_com_tasklet
{
public:
	GVM_DCL_CLASS(gv_ui_module, gv_com_tasklet);
	gv_ui_module();
	~gv_ui_module();
	virtual void load();
	virtual void unload();
	virtual void active();
	virtual void deactive();
	virtual bool is_active();
	virtual bool set_text(const char*);

protected:
	gv_bool m_is_active;
};
};
