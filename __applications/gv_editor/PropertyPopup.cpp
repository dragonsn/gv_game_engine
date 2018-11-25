// PropertyPopup.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyPopup.h"
#include "afxdialogex.h"


// PropertyPopup dialog

IMPLEMENT_DYNAMIC(PropertyPopup, CDialog)

PropertyPopup::PropertyPopup(CWnd* pParent /*=NULL*/)
	: CDialog(PropertyPopup::IDD, pParent)
	, m_Save(false)
{
	m_show_object = false;
}

PropertyPopup::~PropertyPopup()
{
}

void PropertyPopup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PropertyPopup, CDialog)
END_MESSAGE_MAP()


// PropertyPopup message handlers


BOOL PropertyPopup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}
	m_wndPropList.EnableDescriptionArea(TRUE);
	AdjustLayout();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PropertyPopup::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient, rectCombo;
	GetClientRect(rectClient);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + 100, rectClient.Width(), rectClient.Height() -130, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.FillProperties(m_target, gv::gv_object::static_class());
	
}
