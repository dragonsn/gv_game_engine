// SelectConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectConfigDialog.h"
#include "gv_rts_editor.h"


// SelectConfigDialog dialog

IMPLEMENT_DYNAMIC(SelectConfigDialog, CDialog)

SelectConfigDialog::SelectConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SelectConfigDialog::IDD, pParent)
{

}

SelectConfigDialog::~SelectConfigDialog()
{
}

void SelectConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_config_list);
}


BEGIN_MESSAGE_MAP(SelectConfigDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &SelectConfigDialog::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// SelectConfigDialog message handlers
using namespace gv;
BOOL SelectConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	gvt_array<gv_string > names; 
	gv_string fn; 
	fn<<"*."<<"config";
	gv_global::fm->find_file_with_pattern(*fn,names);
	for ( int i=0; i<names.size(); i++){
		m_config_list.AddString(GV_TO_UNICODE(*names[i]));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SelectConfigDialog::OnCbnSelchangeCombo1()
{
	short cIndex;
	cIndex=m_config_list.GetCurSel();
	m_config_list.GetLBText(cIndex,m_file_name);
	// TODO: Add your control notification handler code here
}
