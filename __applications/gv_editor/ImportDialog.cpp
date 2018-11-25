// ImportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gv_rts_editor.h"
#include "ImportDialog.h"


// CImportDialog dialog

IMPLEMENT_DYNAMIC(CImportDialog, CDialog)

CImportDialog::CImportDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImportDialog::IDD, pParent)
 	, m_file_name(_T(""))
{
	m_imp=0;
}

CImportDialog::~CImportDialog()
{
}

void CImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_file_name);
}


BEGIN_MESSAGE_MAP(CImportDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CImportDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CImportDialog message handlers
using namespace gv;
void CImportDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	gvt_array<gv_id> result;
	m_imp->get_import_extension(result);
	// "Chart Files (*.xlc)|*.xlc|Worksheet Files (*.xls)|*.xls|Data Files (*.xlc;*.xls)|*.xlc; *.xls|All Files (*.*)|*.*||";
	gv_string_tmp filter;
	gv_string_tmp exts;
	filter<<"import files";
	for ( int i=0; i<result.size(); i++)
	{
		exts<<"*."<<result[i];
		if (i!= result.size()-1) exts<<";";
	}
	filter<<"("<<exts<<")"<<"|"<<exts;
	CFileDialog dlg (TRUE, NULL, NULL, 0, GV_TO_UNICODE(*filter));
	if (dlg.DoModal () == IDOK)
	{
		m_file_name = dlg.GetPathName ();
		this->UpdateData(FALSE);
	} 
	gv_global::fm->reset_work_path();
}

BOOL CImportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
