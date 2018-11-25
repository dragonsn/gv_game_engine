
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar
namespace gv
{
	class gv_log_editor:public gv_log 
	{
	public:
		gv_log_editor(COutputList * plist):m_list(plist)
		{
			
		}
		gvi_stream &	gv_log_editor::operator <<(const gv_string_tmp & s)
		{
			m_list->AddString(GV_TO_UNICODE(*s));
			m_list->SetCaretIndex(m_list->GetCount());
			return *this;
		}; 
		COutputList * m_list;
	};
}
using namespace gv; 
COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL|LBS_MULTIPLESEL ;

	if (!m_wndOutputMain.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputEvent.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputDatabase.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	m_wndOutputMain.SetFont(&m_Font);
	m_wndOutputEvent.SetFont(&m_Font);
	m_wndOutputDatabase.SetFont(&m_Font);

	CString strTabName;
	BOOL bNameValid;
	

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_MAIN_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputMain, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_EVENT_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputEvent, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_DATABASE_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDatabase, strTabName, (UINT)2);

	// Fill output tabs with some dummy text (nothing magic here)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();
	GVM_INFO_LOG("C to copy , X to clear, A to select All ");
	return 0;
}

void COutputWnd::HookGvLog()
{
	gv::gv_global::log->set_log ( new gv::gv_log_editor(&m_wndOutputMain));
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_Font);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = gv::gvt_max((int)cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndOutputMain.AddString(GV_TO_UNICODE("C to copy , X to clear, A to select All "));
	m_wndOutputMain.SetCaretIndex(m_wndOutputMain.GetCount());
}

void COutputWnd::FillDebugWindow()
{
}

void COutputWnd::FillFindWindow()
{
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
//	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_COPY, &COutputList::OnEditCopy)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

//void COutputList::OnEditCopy()
//{
//	MessageBox(_T("Copy output"));
//}

void COutputList::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


using namespace gv;
void COutputList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar=='X')	{
		this->ResetContent();
	}
    if (nChar=='A')	{
		SelItemRange(1,0,this->GetCount()-1);
	}
	else if (nChar=='C'){
		// Get the indexes of all the selected items.
		int nCount = GetSelCount();
		CArray<int,int> aryListBoxSel;
		aryListBoxSel.SetSize(nCount);
		GetSelItems(nCount, aryListBoxSel.GetData()); 
		gv_string_tmp final;
		for (int i=0;i < aryListBoxSel.GetCount();i++)
		{
			CString s;
			GetText( aryListBoxSel[i], s);
			final<<s;
		}
		if (!OpenClipboard()) 	return ; 
		EmptyClipboard(); 
		int cch=final.strlen();
		LPTSTR  lptstrCopy; 
		HGLOBAL hglbCopy; 
		// Open the clipboard, and empty it. 
		hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch+1)* sizeof(TCHAR)); 
		if (hglbCopy == NULL) { 	CloseClipboard(); 	return ; } 
		// Lock the handle and copy the text to the buffer. 
		lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
		memcpy(lptstrCopy, (*final), 
			cch * sizeof(TCHAR)); 
		lptstrCopy[cch] = (TCHAR) 0;    // null character 
		GlobalUnlock(hglbCopy); 
		// Place the handle on the clipboard. 

		SetClipboardData(CF_TEXT, hglbCopy); 
		CloseClipboard(); 

	}
	
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COutputList::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void COutputList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->SelItemRange(FALSE,0,this->GetCount()-1);
	CListBox::OnLButtonDblClk(nFlags, point);
}
