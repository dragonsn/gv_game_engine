// NewObjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gv_rts_editor.h"
#include "NewObjectDialog.h"


// CNewObjectDialog dialog

IMPLEMENT_DYNAMIC(CNewObjectDialog, CDialog)

CNewObjectDialog::CNewObjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewObjectDialog::IDD, pParent)
	, m_object_name(_T(""))
{

}

CNewObjectDialog::~CNewObjectDialog()
{
}

void CNewObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_object_name);
}


BEGIN_MESSAGE_MAP(CNewObjectDialog, CDialog)
END_MESSAGE_MAP()


// CNewObjectDialog message handlers
