#pragma once
#include "afxwin.h"


// SelectConfigDialog dialog

class SelectConfigDialog : public CDialog
{
	DECLARE_DYNAMIC(SelectConfigDialog)

public:
	SelectConfigDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectConfigDialog();

// Dialog Data
	enum { IDD = IDD_SELECT_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_config_list;
	CString	  m_file_name;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
