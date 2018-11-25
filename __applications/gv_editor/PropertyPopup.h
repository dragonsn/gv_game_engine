#pragma once


#include "GV_PropertyGridCtrl.h"
#include "GV_FunctionGridCtrl.h"

// PropertyPopup dialog

class PropertyPopup : public CDialog
{
	DECLARE_DYNAMIC(PropertyPopup)

public:
	PropertyPopup(CWnd* pParent = NULL);   // standard constructor
	virtual ~PropertyPopup();
	void AdjustLayout();
// Dialog Data
	enum { IDD = IDD_PROPERTY_POPUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool m_Save;
	virtual BOOL OnInitDialog();
	gv::gv_object * m_target;
	bool m_show_object;
	GV_PropertyGridCtrl m_wndPropList;
};
