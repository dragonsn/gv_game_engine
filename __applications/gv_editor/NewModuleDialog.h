#pragma once


// CNewModuleDialog dialog

class CNewModuleDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewModuleDialog)

public:
	CNewModuleDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewModuleDialog();

// Dialog Data
	enum { IDD = IDD_NEW_MODULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_use_xml;
	CString m_module_name;
};
