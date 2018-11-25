#pragma once


// CNewObjectDialog dialog

class CNewObjectDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewObjectDialog)

public:
	CNewObjectDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewObjectDialog();

// Dialog Data
	enum { IDD = IDD_NEW_OBJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_object_name;
};
