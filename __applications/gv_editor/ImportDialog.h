#pragma once


// CImportDialog dialog

class CImportDialog : public CDialog
{
	DECLARE_DYNAMIC(CImportDialog)

public:
	CImportDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportDialog();

// Dialog Data
	enum { IDD = IDD_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	gv::gv_importer_exporter *  m_imp; 
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	CString m_file_name;
};
