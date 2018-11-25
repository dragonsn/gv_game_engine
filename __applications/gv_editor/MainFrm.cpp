
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "gv_rts_editor.h"

#include "MainFrm.h"
#include "gv_rts_editorDoc.h"
#include "gv_rts_editorView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace gv;	
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_VIEW_GAME, &CMainFrame::OnViewGame)
	ON_COMMAND(ID_VIEW_MAP, &CMainFrame::OnViewMap)
	ON_COMMAND(ID_VIEW_ACTOR, &CMainFrame::OnViewActor)
	ON_COMMAND(ID_VIEW_TERRAIN, &CMainFrame::OnViewTerrain)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_EXPLORER_NEW_MAP, &CMainFrame::OnExplorerNewMap)
	ON_COMMAND(ID_EXPLORER_SAVEGAMEENTITYASARCHITYPE, &CMainFrame::OnExplorerSavegameentityasarchitype)
	ON_COMMAND(ID_FILE_OPENPROFILE, &CMainFrame::OnFileOpenprofile)
	ON_COMMAND(ID_SELECT_NEXT, &CMainFrame::OnSelectNext)
	ON_COMMAND(ID_SELECT_PREV, &CMainFrame::OnSelectPrev)
	ON_COMMAND(ID_VIEW_HIDE_DEBUG_DRAW, &CMainFrame::OnViewHideDebugDraw)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
 
// CMainFrame construction/destruction
namespace
{
	CMainFrame	* s_main_frame=NULL;
	const static int c_select_history_size=16;
	gvt_ring_buf< gv_object_handle > s_select_history(c_select_history_size);
	int	s_last_select_index=0;
}

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	ASSERT(!s_main_frame);
	s_main_frame=this;
	m_selected_object=NULL;
	m_selected_module=NULL;
	m_selected_class=NULL;
	m_drag_start=FALSE;
	m_drag_object=NULL; 
}

CMainFrame::~CMainFrame()
{
	s_main_frame=NULL;
}

CMainFrame * CMainFrame::static_get()
{
	return s_main_frame;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_VIEW_MODE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolSelectHistoryBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolSelectHistoryBar.LoadToolBar(IDR_BROWSER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolSelectHistoryBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	DockPane(&m_wndToolSelectHistoryBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Create and setup "Outlook" navigation bar:
	/*we don't need the calendar
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	{
		TRACE0("Failed to create navigation pane\n");
		return -1;      // fail to create
	}*/

	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}

	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);


	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
{
	CWindowDC dc(NULL);

	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	DWORD dwPaneStyle = AFX_DEFAULT_TOOLBAR_STYLE | CBRS_FLOAT_MULTI;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);

	calendar.Create(rectDummy, &bar, 1201);
	bNameValid = strTemp.LoadString(IDS_CALENDAR);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void	CMainFrame::SelectObject(gv::gv_object * obj, bool lock)
{
	using namespace gv;	
	get_editor()->double_click_obj(obj,lock);
	if (obj)
	{
		if (!s_select_history.size() ||	
			(s_select_history.size()>(unsigned)s_last_select_index && s_select_history[s_last_select_index]!= obj->get_handle())	
			){
			s_last_select_index=(int)s_select_history.size();
			s_select_history.push_back(obj->get_handle()); 
		}
		GVM_LOG(main, "select object "<<obj->get_location());
		gv_string_tmp s; 
		s<<obj->get_location(); 
		m_wndProperties.SetWindowText(GV_TO_UNICODE(*s));
		m_selected_object=obj; 
		if (obj->is_a(gv_class_info::static_class()))
		{
			m_selected_class=gvt_cast<gv_class_info > (obj); 
		}
		update_caption();
	}
	m_wndProperties.InitPropList(obj);
};


void	CMainFrame::SelectModule(gv::gv_module * mod)
{
	using namespace gv;	
	if (mod)
	{
		GVM_LOG(main,"select module "<<mod->get_name());
	}
	m_wndFileView.m_wndObjectView.FillObjectTree(mod);
	m_selected_module=mod;
};

void	CMainFrame::ReflushCurrentModule()
{
	this->SelectModule(m_selected_module);
};

BOOL	CMainFrame::UpdateObject( gv::gv_string_tmp & location ,  gv::gv_string_tmp & new_value)
{
	gv_bool b=get_editor()->exec("update", location, new_value);
	//m_wndProperties.InitPropList(m_selected_object);
	if (!b) return FALSE;
	return TRUE;      
};


void CMainFrame::OnViewGame()
{
	// TODO: Add your command handler code here
	//get_editor()->set_editor_mode(e_editor_mode_game);
	get_editor()->create_map_preview();
}

void CMainFrame::OnViewMap()
{
	// TODO: Add your command handler code here
	get_editor()->set_editor_mode(e_editor_mode_map);
}

void CMainFrame::OnViewActor()
{
	// TODO: Add your command handler code here
	get_editor()->set_editor_mode(e_editor_mode_actor);
}

void CMainFrame::OnViewTerrain()
{
	get_editor()->set_editor_mode(e_editor_mode_terrain);
};


void CMainFrame::DragStart(gv::gv_object * pobject)
{
	m_drag_object=pobject;
	m_drag_start=TRUE;
	ShowCursor(FALSE); 
	::SetCapture(::GetParent(Cgv_rts_editorView::static_get()->m_hWnd)); 
	get_editor()->start_drag(m_selected_object);
}

void CMainFrame::DragEnd()
{
	m_drag_start=FALSE;
	get_editor()->end_drag();
}
void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWndEx::OnMouseMove(nFlags, point);
	//HTREEITEM htiTarget;  // Handle to target item. 
	//TVHITTESTINFO tvht;   // Hit test information. 

	if (m_drag_start) 
	{ 
		// Drag the item to the current position of the mouse pointer. 
		// First convert the dialog coordinates to control coordinates. 
		//ClientToScreen(hwndParent, &point);
		//ScreenToClient(hwndTV, &point);
		ImageList_DragMove(point.x, point.y);
		// Turn off the dragged image so the background can be refreshed.
		ImageList_DragShowNolock(FALSE); 

		// Find out if the pointer is on the item. If it is, 
		// highlight the item as a drop target. 
		//tvht.pt.x = point.x; 
		//tvht.pt.y = point.y; 
		//if ((htiTarget = TreeView_HitTest(hwndTV, &tvht)) != NULL) 
		//{ 
		//	TreeView_SelectDropTarget(hwndTV, htiTarget); 
		//} 
		CPoint sp=point; 
		ClientToScreen(&sp);
		CWnd * pwin2=WindowFromPoint(sp);
		if (pwin2==&m_wndProperties.m_wndFuncList)
		{
			CPoint a=sp; 
			m_wndProperties.m_wndFuncList.ScreenToClient(&a); 
			CMFCPropertyGridProperty * prop=m_wndProperties.m_wndFuncList.HitTest(a); 
			m_wndProperties.m_wndFuncList.SetCurSel(prop);	
		}
		else if (pwin2==&m_wndProperties.m_wndPropList)
		{
			CPoint a=sp; 
			m_wndProperties.m_wndPropList.ScreenToClient(&a); 
			CMFCPropertyGridProperty * prop=m_wndProperties.m_wndPropList.HitTest(a); 
			m_wndProperties.m_wndPropList.SetCurSel(prop);			
		}
		else if (pwin2==&m_wndFileView.m_wndFileView)
		{
			CPoint a=sp; 
			m_wndFileView.m_wndFileView.ScreenToClient(&a); 
			HTREEITEM item =m_wndFileView.m_wndFileView.HitTest(a);
			if (item)
			{
				m_wndFileView.m_wndFileView.SelectItem(item);
				gv_string_tmp s( "drop to module ");
				s<<m_wndFileView.m_wndFileView.GetItemText(item);
				m_wndCaptionBar.SetText(GV_TO_UNICODE(*s),CMFCCaptionBar::ALIGN_LEFT);
			}
		}
		else if (pwin2==&m_wndFileView.m_wndObjectView)
		{
			CPoint a=sp; 
			m_wndFileView.m_wndObjectView.ScreenToClient(&a); 
			HTREEITEM item =m_wndFileView.m_wndObjectView.HitTest(a);
			if (item)
			{
				m_wndFileView.m_wndObjectView.MySelectItem(item);
				gv_string_tmp s( "drop to object::===> ");
				s<<m_wndFileView.m_wndObjectView.GetItemText(item);
				m_wndCaptionBar.SetText(GV_TO_UNICODE(*s),CMFCCaptionBar::ALIGN_LEFT);
			}
		}

		ImageList_DragShowNolock(TRUE);
		CPoint p=point;
		this->ClientToScreen(&p);
		Cgv_rts_editorView::static_get()->ScreenToClient(&p);
		gv_global::input->set_mouse_pos(gv_vector2i(p.x,p.y));
	} 
	return;
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (IsDragStart()) 
	{ 
		// Get destination item.
		//HTREEITEM htiDest = TreeView_GetDropHilight(hwndTV);
		//if (htiDest != NULL)
		//{
		// To do: handle the actual moving of the dragged node.
		//}
		
		ImageList_EndDrag(); 
		//TreeView_SelectDropTarget(m_hWnd, NULL);
		ReleaseCapture(); 
		ShowCursor(TRUE); 
		CWnd * pwindow=ChildWindowFromPoint(point); 
		CPoint sp=point; 
		ClientToScreen(&sp);
		CWnd * pwin2=WindowFromPoint(sp);
		if (pwindow==Cgv_rts_editorView::static_get())
		{
			int i=0; 
			Cgv_rts_editorView::static_get()->DropObject(m_drag_object);
		}
		//else if (pwin2==&m_wndProperties || pwindow==&m_wndProperties )
		//{
		//	int i=0;
		//}
		else if (pwin2==&m_wndProperties.m_wndFuncList)
		{
			m_wndProperties.m_wndFuncList.DropObject(nFlags,sp,m_drag_object);
		}
		else if (pwin2==&m_wndProperties.m_wndPropList)
		{
			m_wndProperties.m_wndPropList.DropObject(nFlags,sp,m_drag_object);
		}
		else if (pwin2==&m_wndFileView.m_wndFileView)
		{
			m_wndFileView.DropObjectToFile(m_drag_object);
			ReflushCurrentModule();
		}
		else if (pwin2==&m_wndFileView.m_wndObjectView)
		{
			m_wndFileView.DropObjectToObject(m_drag_object);
			ReflushCurrentModule();
		}
		DragEnd();
	} 
	CFrameWndEx::OnLButtonUp(nFlags, point);
}

void CMainFrame::OnExplorerNewMap()
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnExplorerSavegameentityasarchitype()
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnFileOpenprofile()
{
	// TODO: Add your command handler code here
	gv_string_tmp m_file_name;
	gvt_array<gv_id> result;
	result.add(gv_id_xml); 
	gv_string_tmp filter;
	gv_string_tmp exts;
	filter<<"open profile files";
	for ( int i=0; i<result.size(); i++)
	{
		exts<<"*."<<result[i];
		if (i!= result.size()-1) exts<<";";
	}
	filter<<"("<<exts<<")"<<"|"<<exts;
	CFileDialog dlg (TRUE, NULL, NULL, 0,GV_TO_UNICODE( *filter));
	if (dlg.DoModal () == IDOK)
	{
		m_file_name =CStringA( dlg.GetPathName ());
		m_file_name.replace_all("\\", "/");
		gv_string_tmp cmd="gv_framework_unit_test.exe tool_perf_viewer_2d tool no_log -\"";
		cmd<<m_file_name<<"\" >>a.txt";
		gv_global::fm->reset_work_path();
		gv_system_call(*cmd);
		GVM_LOG(main,"call system :"<<cmd);
		this->UpdateData(FALSE);
	} 
	gv_global::fm->reset_work_path();
}


void	CMainFrame::update_caption(const char * text)
{
	if (!text)
	{
		gv_string_tmp s;
		if (this->m_selected_object) 
		{
			s<<"[OBJ]:"<<this->m_selected_object->get_location_string();
		}
		if (this->m_selected_class) 
		{
			s<<" |||  [CLS]:"<<this->m_selected_class->get_location_string();
		}
		if (this->m_selected_module)
		{
			s<<" |||  [MODULE]:"<<this->m_selected_module->get_location_string();
		}
		m_wndCaptionBar.SetText(GV_TO_UNICODE(*s), CMFCCaptionBar::ALIGN_LEFT);
	}
	else
	{
		m_wndCaptionBar.SetText(GV_TO_UNICODE(text), CMFCCaptionBar::ALIGN_LEFT);
	}
};
void CMainFrame::OnSelectNext()
{
	// TODO: Add your command handler code here
	if (s_select_history.size())
	{
		int idx=(s_last_select_index+1)%(s_select_history.size());
		gv_object_handle hd=s_select_history[idx];
		if (hd.is_valid())	{
			s_last_select_index=idx;
			SelectObject(hd.get_object());
		}
	}
	
}

void CMainFrame::OnSelectPrev()
{
	// TODO: Add your command handler code here
	if (s_select_history.size())
	{
		int idx=(s_last_select_index-1)%(s_select_history.size());
		gv_object_handle hd=s_select_history[idx];
		if (hd.is_valid())	{
			s_last_select_index=idx;
			SelectObject(hd.get_object());
		}
	}
	
}

void CMainFrame::OnViewHideDebugDraw()
{
	// TODO: Add your command handler code here
	gv_global::rnd_opt.m_no_debug_draw=!gv_global::rnd_opt.m_no_debug_draw;
}
