#pragma once
#include "afxpropertygridctrl.h"

class GV_FunctionGridCtrl : public CMFCPropertyGridCtrl
{
public:
	GV_FunctionGridCtrl(void);
	~GV_FunctionGridCtrl(void);
	int DropObject(UINT nFlags, CPoint point, gv::gv_object* object);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
