#pragma once
#include "afxpropertygridctrl.h"

class GV_PropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
	GV_PropertyGridCtrl(void);
	~GV_PropertyGridCtrl(void);
	virtual void OnClickButton(CPoint point);
	virtual BOOL EndEditItem(BOOL bUpdateData = TRUE);
	virtual BOOL ValidateItemData(CMFCPropertyGridProperty* /*pProp*/);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

	void FillProperties(gv::gv_object* object,
						gv::gv_class_info* stop_class = NULL);
	class gv::gv_object* m_selected_object;
	int DropObject(UINT nFlags, CPoint point, gv::gv_object* object);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
