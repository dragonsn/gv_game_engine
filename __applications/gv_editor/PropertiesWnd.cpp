
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "gv_rts_editor.h"
#include <boost/boost/pool/singleton_pool.hpp>

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar
using namespace gv;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
gv_string_tmp get_location (CMFCPropertyGridProperty * p);
void		set_location (CMFCPropertyGridProperty * p ,const gv_string_tmp & location);
void AddOneProperty(CMFCPropertyGridCtrl &  list, CMFCPropertyGridProperty* pfather, gvi_type * ptype, gv_byte * pdata, const gv_string_tmp & name = "", gv_string_tmp  location = "", BOOL allow_edited = FALSE, BOOL is_super = FALSE, BOOL is_file_name = FALSE, gv_id enum_id = gv_id(), gv::gv_class_info * stop_class = NULL,gv::gv_string_tmp tip="");

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	int funct_list_height=(int)(rectClient.Height()*0.44444f);
	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb)-funct_list_height-2, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFuncList.SetWindowPos(NULL, rectClient.left, rectClient.bottom-funct_list_height , rectClient.Width(), funct_list_height  , SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Application"));
	m_wndObjectCombo.AddString(_T("Properties Window"));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	
	if (!m_wndFuncList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Function Grid \n");
		return -1;      // fail to create
	}
	InitPropList(NULL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
	CMainFrame::static_get()->SelectObject(get_editor());
	
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
	CMainFrame::static_get()->SelectObject(get_editor());
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
	
}



void FillOneProperty (CMFCPropertyGridProperty* pfather,	gvi_type * ptype,	gv_byte * pdata)
{
	switch(ptype->get_type())
	{
	case (gve_data_type_object):
		{
			gv_type_object *ptc=(gv_type_object*)ptype; 
			gv_class_info *pcls=ptc->get_class();
			int count =pfather->GetSubItemsCount(); 
			for ( int i=0;i <count;i++)
			{
				CMFCPropertyGridProperty * p=pfather->GetSubItem(i);
				gv_var_info * pvar=pcls->get_var(gv_id(CStringA( p->GetName())));
				FillOneProperty(p,pvar->get_type(),pdata+pvar->get_offset());
			}

		}
		break;
		//================================================================================
	case (gve_data_type_color):
		{
				gv_color c; 
				gv_string_tmp value;
				value=(CStringA)pfather->GetSubItem(0)->GetValue(); 
				value>>c.fixed32;
				value=(CStringA)pfather->GetSubItem(1)->GetValue();
				value>>c.a;
				ptype->copy_to(gvt_byte_ptr(c),pdata);
		}
		break;

		//================================================================================
	case (gve_data_type_text):
		{
			gv_text * ptext=(gv_text*)pdata;
			gv_string_tmp s;
			s=(CStringA)pfather->GetValue(); 
			(*ptext)=*s;
		}
		break;
		//================================================================================
	case (gve_data_type_array):
		{
			gv_type_array * pta =(	gv_type_array*)ptype;
			gvi_type	*   pte=pta->get_element_type();
			CMFCPropertyGridProperty *p=pfather->GetSubItem(0);
			gv_string_tmp s=(CStringA)p->GetValue(); ;
			int count ;
			s>>count;
			pta->resize_array(pdata,count);
			gv_byte * phead	   =pta->get_array_data(pdata);
			for ( int i=1; i <pfather->GetSubItemsCount();i++) 
			{
				if (i>=pta->get_array_size(pdata)) break;
				FillOneProperty(pfather->GetSubItem(i),pte,phead); 
				phead+=pte->get_size();
			}
		}
		break; 
		//================================================================================
	case (gve_data_type_static_array):
		{
			gv_type_static_array * pta =(	gv_type_static_array*)ptype;
			gvi_type	*   pte=pta->get_element_type();
			gv_string_tmp s;
			s=(CStringA)pfather->GetValue(); ;
			gv_byte * phead	   =pdata;
			for ( int i=0; i <pfather->GetSubItemsCount();i++) 
			{
				if (i>=pta->get_array_size()) break;
				FillOneProperty(pfather->GetSubItem(i),pte,phead); 
				phead+=pte->get_size();
			}
		}
		break;
		//================================================================================
	default:
		{
			gv_string_tmp s;
			s<<"\""<<(CStringA)pfather->GetValue()<<"\""; 
			gv_xml_parser parser;
			parser.load_string(*s);
			ptype->import_from_xml(pdata,&parser,get_editor()->get_sandbox(),NULL);
		}
		break;

	}
	return;
}


//=====================================================================================
void CPropertiesWnd::InitPropList (gv::gv_object *pobj)
{
	SetPropListFont();
	if (!pobj) return;
	this->m_object_location=pobj->get_location();
	m_selected_object=pobj;

	m_wndPropList.FillProperties(pobj);
	InitFunctionList(pobj);
}


void CPropertiesWnd::InitFunctionList (gv::gv_object *pobj)
{
	m_wndFuncList.EnableHeaderCtrl(FALSE);
	m_wndFuncList.EnableDescriptionArea();
	m_wndFuncList.SetVSDotNetLook();
	m_wndFuncList.MarkModifiedProperties();
	m_wndFuncList.RemoveAll();
	if(!pobj) return; 

	gv_class_info * pclass=pobj->get_class();
	gv_module *pmod=pobj->get_module();
	m_wndFuncList.ShowWindow(FALSE);
	for ( int i=0; i<(int)pclass->get_nb_func(); i++)
	{
		gv_func_info * pfunc=pclass->get_func( i); 
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(GV_TO_UNICODE(*pfunc->get_name_id()));
		gv_string_tmp loc;
		loc<<pobj->get_location(); 
		loc<<"."<<pfunc->get_name_id(); 
		set_location(pGroup1,loc);
		if (pfunc->get_param_struct())
		{
			gv_type_object type; 
			type.set_class(pfunc->get_param_struct()); 
			gv_byte * data=pfunc->get_param_struct()->create_instance(); 
			AddOneProperty(m_wndFuncList,pGroup1,&type , (BYTE *) data,loc,"",TRUE); 
			pfunc->get_param_struct()->delete_instance(data);
		}
		m_wndFuncList.AddProperty(pGroup1);
		pGroup1->Expand(FALSE);
	}
	m_wndFuncList.ShowWindow(TRUE);
}

/*
void CPropertiesWnd::InitPropList()
{
	
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static TCHAR BASED_CODE szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
}*/



void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndFuncList.SetFont(&m_fntPropList);
}
