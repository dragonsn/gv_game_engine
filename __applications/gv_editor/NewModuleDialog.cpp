// NewModuleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gv_rts_editor.h"
#include "NewModuleDialog.h"


// CNewModuleDialog dialog

IMPLEMENT_DYNAMIC(CNewModuleDialog, CDialog)

CNewModuleDialog::CNewModuleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewModuleDialog::IDD, pParent)
	, m_use_xml(FALSE)
	, m_module_name(_T(""))
{

}

CNewModuleDialog::~CNewModuleDialog()
{
}

void CNewModuleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_use_xml);
	DDX_Text(pDX, IDC_EDIT1, m_module_name);
}


BEGIN_MESSAGE_MAP(CNewModuleDialog, CDialog)
END_MESSAGE_MAP()


// CNewModuleDialog message handlers
