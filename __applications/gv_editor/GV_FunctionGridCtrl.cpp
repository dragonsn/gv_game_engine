#include "StdAfx.h"
#include "GV_FunctionGridCtrl.h"

GV_FunctionGridCtrl::GV_FunctionGridCtrl(void)
{
}

GV_FunctionGridCtrl::~GV_FunctionGridCtrl(void)
{
}
BEGIN_MESSAGE_MAP(GV_FunctionGridCtrl, CMFCPropertyGridCtrl)
ON_WM_NCLBUTTONDBLCLK()
ON_WM_LBUTTONDBLCLK()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void GV_FunctionGridCtrl::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCPropertyGridCtrl::OnNcLButtonDblClk(nHitTest, point);
}
using namespace gv;
gv_string_tmp get_location(CMFCPropertyGridProperty* p);
void set_location(CMFCPropertyGridProperty* p, const gv_string_tmp& location);
void FillOneProperty(CMFCPropertyGridProperty* pfather, gvi_type* ptype,
					 gv_byte* pdata);

void GV_FunctionGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMFCPropertyGridProperty* prop = this->HitTest(point);
	if (prop)
	{
		gv_string_tmp address = get_location(prop);
		if (address.size() && address[0] == '(')
		{
			GVM_LOG(main, address << " selected!" << gv_endl)
			gv_string_tmp s = "call ";
			s << address << "?";
			gv_string_tmp param;
			{

				param = CStringA(prop->GetName());
				param += "_param";
				gv_class_info* pclass =
					get_editor()->get_sandbox()->find_class(gv_id(*param));
				if (pclass)
				{
					gv_any any(pclass);
					gv_type_object type;
					type.set_class(pclass);
					FillOneProperty(prop, &type, any.get_data());
					param = "";
					type.export_to_xml(any.get_data(), param);
				}
			}
			s << "  " << param;
			if (::MessageBoxA(NULL, *s, "call!", MB_OKCANCEL) == IDOK)
			{
				gv_global::fm->reset_work_path();
				gv_int ret =
					get_editor()->get_sandbox()->exec_global_call(address, param);
				s = "return ";
				s << ret;
				if (!ret)
				{
					s << ", failed!!! " << param;
				}
				::MessageBoxA(NULL, *s, "return!", MB_OK);
			}
		}
	}
	return;
	// CMFCPropertyGridCtrl::OnLButtonDblClk(nFlags, point);
}

int GV_FunctionGridCtrl::DropObject(UINT nFlags, CPoint point,
									gv::gv_object* drop_object)
{
	if (!drop_object)
		return NULL;
	CMFCPropertyGridProperty* prop = this->GetCurSel();
	if (prop)
	{
		gv_string_tmp address = get_location(prop);
		// gvi_type * type;
		// gv_byte * data;
		// gv_object * object;
		gv_string_tmp tail;
		GVM_LOG(main, " try to drop to " << get_location(prop) << gv_endl);
		gv_string_tmp s;
		s << drop_object->get_location();
		CString mfc = GV_TO_UNICODE(*s);
		prop->SetValue(mfc);
		this->UpdateData(FALSE);
		return 1;
	}
	::MessageBoxA(NULL,
				  "failed to drop the object reference!!!,maybe type mismatch",
				  "error", MB_OK);
	return 0;
};
void GV_FunctionGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCPropertyGridCtrl::OnMouseMove(nFlags, point);
}
