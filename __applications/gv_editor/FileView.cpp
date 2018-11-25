
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "gv_rts_editor.h"
#include "ImportDialog.h"
#include "NewObjectDialog.h" 
#include "NewModuleDialog.h"
#include "PropertyPopup.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOAD_MODULE, &CFileView::OnLoadModule)
	ON_COMMAND(ID_SAVE_MODULE, &CFileView::OnSaveModule)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER0, &CFileView::OnImport0)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER1, &CFileView::OnImport1)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER2, &CFileView::OnImport2)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER3, &CFileView::OnImport3)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER4, &CFileView::OnImport4)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER5, &CFileView::OnImport5)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER6, &CFileView::OnImport6)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER7, &CFileView::OnImport7)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER8, &CFileView::OnImport8)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER9, &CFileView::OnImport9)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER10, &CFileView::OnImporta)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER11, &CFileView::OnImportb)
	ON_COMMAND(ID_MANUPLATEOBJECTS_IMPORTPLACEHOLDER12, &CFileView::OnImportc)
	ON_COMMAND(ID_EXPLORER_BINARIZEMODULE, &CFileView::OnExplorerBinarizemodule)
	ON_COMMAND(ID_EXPLORER_NEWMODULE, &CFileView::OnExplorerNewmodule)
	ON_COMMAND(ID_EXPLORER_UNLOADMODULE, &CFileView::OnExplorerUnloadmodule)
	ON_COMMAND(ID_EXPLORER_XMLIZEMODULE, &CFileView::OnExplorerXmlizemodule)
	ON_COMMAND(ID_EXPLORER_NEW_MAP, &CFileView::OnExplorerNewMap)
	ON_COMMAND(ID_EXPLORER_SAVEGAMEENTITYASARCHITYPE, &CFileView::OnExplorerNewArchitype)
	ON_COMMAND(ID_EXPLORER_SAVECURRENTSELECTEDENTITYASARCHITYPE, &CFileView::OnExplorerNewArchitype2)
	ON_COMMAND(ID_EXPLORER_SAVECURRENTMATERIAL,&CFileView::OnExplorerSaveMaterial)
	ON_COMMAND(ID_EXPLORER_COPY_MODULE_AS	  ,&CFileView::OnExplorerCopyModuleAs)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	if (!m_wndObjectView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
	m_wndObjectView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}
using namespace gv;
void CFileView::FillFileView()
{
	/*
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp files"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp Source Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp Header Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp Resource Files"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);*/
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd == &m_wndObjectView)
	{
		m_wndObjectView.SetFocus();
		
		CMenu menu;
		//menu.Attach(theApp.GetContextMenuManager()->GetMenuById(IDR_POPUP_OBJECT));
		
		VERIFY(menu.LoadMenu(IDR_POPUP_OBJECT));
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		int i; 
		for ( i=0; i< get_editor()->get_nb_imp_exp(); i++)
		{
			gv_importer_exporter * pimp=get_editor()->get_imp_exp(i);
			gv_string_tmp s("import  ");
			UINT id = pPopup->GetMenuItemID(i);
			gvt_array<gv_id> result ; 
			pimp->get_import_extension(result);
			GV_ASSERT(result.size());
			s<<result[0]<<" file  as "<< pimp->get_import_target_class()->get_name_id();
			pPopup->ModifyMenu(i, MF_BYPOSITION, id, GV_TO_UNICODE(*s));
		}
		int count=pPopup->GetMenuItemCount();
		for (int j=i ; j<count ; j++)
		{
			pPopup->RemoveMenu(i,MF_BYPOSITION);
		}
		CWnd* pWndPopupOwner = this;
		
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
		//theApp.GetContextMenuManager()->ShowPopupMenu((HMENU)(*pPopup), point.x, point.y, this, TRUE);
		
		//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_OBJECT, point.x, point.y, this, TRUE);
		return;
	}

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}
	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height()/2 - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndObjectView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + rectClient.Height()/2, rectClient.Width() - 2, rectClient.Height()/2 - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("Properties...."));

}

void CFileView::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CFileView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CFileView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CFileView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));

	m_wndObjectView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
	m_wndObjectView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap( IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |=  ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}


void CFileView::FlushModule()
{
	using namespace gv;
	this->m_wndFileView.DeleteAllItems();
	if (gv::gv_global::sandbox_mama.try_get() )
	{
		//
		HTREEITEM hRoot = m_wndFileView.InsertItem(_T("registered modules"), 0, 0);
		m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		gvt_array < gv_id> module_id;
		gvt_array<gv_string > module_path;
		gv_sandbox_manager  * pc=gv::gv_global::sandbox_mama.get(); 
		int count=pc->query_registered_modules(module_id,module_path,true);
		module_id.sort();
		if (count)
		{
			HTREEITEM hSrc = m_wndFileView.InsertItem(_T("binary_modules"), 0, 0, hRoot);
			for ( int i=0 ;i< count; i++)
			{
				int image=2; 
				if (!get_editor()->get_sandbox()->find_module(module_id[i])) image=5; 
				m_wndFileView.InsertItem(GV_TO_UNICODE(*module_id[i].string()), image, image, hSrc);
			}
			m_wndFileView.Expand(hSrc, TVE_EXPAND);
		}
		//==========================================================================
		module_path.clear(); 
		module_id.clear();
		count=pc->query_registered_modules(module_id,module_path,false);
		module_id.sort();
		if (count)
		{
			HTREEITEM hSrc = m_wndFileView.InsertItem(_T("xml_modules"), 0, 0, hRoot);
			for ( int i=0 ;i< count; i++)
			{
				int image=1; 
				if (!get_editor()->get_sandbox()->find_module(module_id[i])) image=4; 
				m_wndFileView.InsertItem(GV_TO_UNICODE(*module_id[i].string()), image, image, hSrc);
			}
			m_wndFileView.Expand(hSrc, TVE_EXPAND);
		}
		
		//==========================================================================
		{
			HTREEITEM hSrc = m_wndFileView.InsertItem(_T("native_modules(runtime ,not saved)"), 0, 0, hRoot);
			int count=gv_global::sandbox_mama->get_base_sandbox()->get_nb_native_modules();
			for (int i=0; i<count; i++)
			{
				gv_string_tmp s; 
				gv_module * mod= gv_global::sandbox_mama->get_base_sandbox()->get_nth_native_module(i);
				s<<"(engine)";
				s<<mod->get_name_id();
				m_wndFileView.InsertItem(GV_TO_UNICODE(*s), 2, 2, hSrc);
			}
	
			m_wndFileView.InsertItem(GV_TO_UNICODE("native"), 2, 2, hSrc);
			m_wndFileView.Expand(hSrc, TVE_EXPAND);
		}
		m_wndFileView.Expand(hRoot, TVE_EXPAND);
	}
}
void CFileView::OnLoadModule()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CStringA name;
	name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(name)); 
	if (pmod) return; 
	int img, img2;
	m_wndFileView.GetItemImage(hItem,img2,img);
	gv_global::fm.get()->reset_work_path();
	if (img==4)
	{
		get_editor()->get_sandbox()->import_module(gv_id(name)); 
	}
	if (img==5)
	{
		get_editor()->get_sandbox()->load_module(gv_id(name)); 
	}
	this->FlushModule();
}

void CFileView::OnSaveModule()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CStringA name;
	name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(name)); 
	if (!pmod) return; 
	if (pmod->get_name_id()==gv_id_native) return ; 
	int img, img2;
	m_wndFileView.GetItemImage(hItem,img2,img);
	gv_string_tmp s ;
	if (img==1)
	{
		s<< "sure to save module "<<name <<"in xml format ?";
		if (MessageBoxA( *s,"warning!",MB_OKCANCEL)==IDOK)
		{
			get_editor()->get_sandbox()->export_module(gv_id(name)); 
		}
	}
	else if (img==2)
	{
		s<< "sure to save module "<<name <<"in binary format ?";
		if (MessageBoxA(*s,"warning!",MB_OKCANCEL)==IDOK)
		{
			get_editor()->get_sandbox()->save_module(gv_id(name)); 
		}
	}
	this->FlushModule();
}

void CFileView::DoImport(int type)
{
  gv_importer_exporter * pimp=get_editor()->get_imp_exp(type);
  if (!pimp) return; 
  if (!this->m_wndObjectView.m_selected_object) return; 
  CImportDialog dialog; 
  dialog.m_imp=pimp;
  gv_int_ptr ret = dialog.DoModal();
  gv_string_tmp config;
  if (ret == IDOK && pimp->get_import_config_class() )
  {
	  gv_object * pobj =pimp->get_sandbox()->create_object( pimp->get_import_config_class());
	  gv_string_tmp cfg_file_name =CStringA( dialog.m_file_name); 
	  cfg_file_name += ".imp"; 
	  gv_string_tmp txt; 
	  if (gv_load_file_to_string(cfg_file_name, txt))
	  {
		  pobj->import_object_xml_in_memory(txt);
	  }
	  
	  PropertyPopup diapop;
	  diapop.m_target = pobj;
	  diapop.DoModal();
	  pobj->export_object_xml(config);
	  gv_save_string_to_file(cfg_file_name, config);
  }
  if (ret==IDOK)
  {
	gv_class_info * pcls=pimp->get_import_target_class();
	pimp->set_import_config(config);
	CNewObjectDialog new_d; 
	new_d.DoModal(); 
	gv_string_tmp obj_name;
	obj_name=CStringA( new_d.m_object_name);

	gv_object* pobj=get_editor()->get_sandbox()->create_object(pcls, gv_id(*obj_name) ,	this->m_wndObjectView.m_selected_object);
	gv_string_tmp s; 
	s=CStringA( dialog.m_file_name);
	s.to_upper();
	gv_string_tmp resource_root; 
	resource_root =gv_global::fm->get_work_path();
	resource_root+=get_editor()->get_sandbox()->get_resource_root_path();
	resource_root=gv_get_full_path_name(resource_root);
	resource_root.to_upper(); 
	s.replace_all(resource_root,GV_RESOURCE_ROOT);
	if(gvt_cast<gv_resource> (pobj))
	{
		gvt_cast<gv_resource> (pobj)->set_file_name(*s);
	}
	s=get_editor()->get_sandbox()->get_physical_resource_path(s);
	pimp->do_import(s,pobj);
	m_wndObjectView.FillObjectTree( this->m_wndObjectView.m_selected_object->get_module());
  }
}

void CFileView::OnImport0()
{
	DoImport(0);
};
void CFileView::OnImport1()
{
	DoImport(1);
};

void CFileView::OnImport2()
{
	DoImport(2);
};
void CFileView::OnImport3()
{
	DoImport(3);
};
void CFileView::OnImport4()
{
	DoImport(4);
};
void CFileView::OnImport5()
{
	DoImport(5);
};
void CFileView::OnImport6()
{
	DoImport(6);
};
void CFileView::OnImport7()
{
	DoImport(7);
};
void CFileView::OnImport8()
{
	DoImport(8);
};
void CFileView::OnImport9()
{
	DoImport(9);
};
void CFileView::OnImporta()
{
	DoImport(10);
};

void CFileView::OnImportb()
{
	DoImport(11);
};
void CFileView::OnImportc()
{
	DoImport(12);
};
void CFileView::OnExplorerBinarizemodule()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod) 
	{
		MessageBoxA("please load the module first","problem!", MB_OK);
		return;
	}; 
	if (pmod->get_name_id()==gv_id_native) return ; 
	int img, img2;
	m_wndFileView.GetItemImage(hItem,img2,img);
	if (img==1)
	{
		gv_string_tmp path=gv_framework_config::get_binary_module_path();
		path<<"/"<<pmod->get_name_id()<<"."<<gvc_gv_bin_ext;
		gv_global::sandbox_mama->register_new_bin_module_path(gv_id(CStringA(name)),path);
		get_editor()->get_sandbox()->save_module (gv_id(CStringA(name)));
		
	}
	else
	{
		MessageBoxA("please load the module first, or the the module is already binarized","problem!",MB_OK);
		return;
	}

	this->FlushModule();
}

void CFileView::OnExplorerNewmodule()
{
	// TODO: Add your command handler code here
	CNewModuleDialog dialog; 
	if (dialog.DoModal()==IDOK)
	{
		gv_id name=gv_id(CStringA(dialog.m_module_name));
		bool is_xml=dialog.m_use_xml!=0; 
		if (!get_editor()->get_sandbox()->find_module(name))
		{
			if (is_xml)
			{
				gv_global::sandbox_mama->add_new_xml_module(name,get_editor()->get_sandbox());
			}
			else
			{
				gv_global::sandbox_mama->add_new_bin_module(name,get_editor()->get_sandbox());
			}
		}
		FlushModule();
	};
}

void CFileView::OnExplorerCopyModuleAs()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod) 
	{
		MessageBoxA("please load the module first","problem!", MB_OK);
		return;
	}; 

	CNewModuleDialog dialog; 
	if (dialog.DoModal()==IDOK)
	{
		gv_id name=gv_id(CStringA(dialog.m_module_name));
		bool is_xml=dialog.m_use_xml!=0; 
		if (!get_editor()->get_sandbox()->find_module(name))
		{
			if (is_xml)
			{
				gv_global::sandbox_mama->add_new_xml_module(name,get_editor()->get_sandbox(),false);
				pmod->rename(name);
				get_editor()->get_sandbox()->export_module(gv_id(name)); 
				
			}
			else
			{
				gv_global::sandbox_mama->add_new_bin_module(name,get_editor()->get_sandbox(),false);
				pmod->rename(name);
				get_editor()->get_sandbox()->save_module (gv_id(name)); 
			}
		}
		FlushModule();
	};
}


void CFileView::OnExplorerUnloadmodule()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod) return; 
	get_editor()->get_sandbox()->unload_module(pmod->get_name_id());
	this->FlushModule();
}

void CFileView::OnExplorerXmlizemodule()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod) 
	{
		MessageBoxA("please load the module first","problem!", MB_OK);
		return;
	}; 
	if (pmod->get_name_id()==gv_id_native) return ; 
	int img, img2;
	m_wndFileView.GetItemImage(hItem,img2,img);
	if (img==2)
	{
		gv_string_tmp path=gv_framework_config::get_xml_module_path();
		path<<"/"<<name<<"."<<gvc_gv_xml_ext;
		gv_global::sandbox_mama->register_new_xml_module_path(gv_id(CStringA(name)),path);
		get_editor()->get_sandbox()->export_module (gv_id(CStringA(name)));
	}
	else
	{
		MessageBoxA(" the the module is already xml","problem!",MB_OK);
		return;
	}

	this->FlushModule();
}


void CFileView::OnExplorerNewMap()
{

};


void CFileView::OnExplorerNewArchitype()
{
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod || !get_editor()->get_game_entity()) 
	{
		gv_string_tmp s; 
		s<<"please load the module :" << name;
		MessageBoxA(*s,"problem!", MB_OK);
		return;
	}; 
	if (get_editor()->get_game_entity())
	{
		CNewObjectDialog new_d; 
		new_d.DoModal(); 
		gv_string_tmp obj_name;
		obj_name=CStringA( new_d.m_object_name);
		gv_entity *pentity=get_editor()->get_game_entity()->clone(); 
		pentity->set_owner(pmod); 
		pentity->rename(*obj_name);
		pentity->detach_component(gv_id("gv_com_game") );
		pentity->detach_component(gv_id("gv_com_observer_camera_controller") );
		pentity->detach_component(gv_id("gv_com_cam_fps_fly") );
		pentity->detach_component(gv_id("gv_com_camera") );
		pentity->set_world(NULL);
		pentity->set_position(gv_vector3::get_zero_vector());
		pentity->set_rotation(gv_euler::get_zero_rotation());
		pentity->set_scale(gv_vector3(1,1,1));
		pentity->update_matrix(); 
	}
};

 void CFileView::OnExplorerNewArchitype2()
{
	HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	CString name=m_wndFileView.GetItemText(hItem);
	gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	if (!pmod) 
	{
		gv_string_tmp s; 
		s<<"please load the module :" << name;
		MessageBoxA(*s,"problem!", MB_OK);
		return;
	}; 
	if (get_editor()->get_selected_entity())
	{
		CNewObjectDialog new_d; 
		new_d.DoModal(); 
		gv_string_tmp obj_name;
		obj_name=CStringA( new_d.m_object_name);
		gv_entity *pentity=get_editor()->get_selected_entity()->clone(); 
		pentity->set_owner(pmod); 
		pentity->rename(*obj_name);
		pentity->set_world(NULL);
		pentity->set_position(gv_vector3::get_zero_vector());
		//pentity->set_rotation(gv_euler::get_zero_rotation());
		//pentity->set_scale(gv_vector3(1,1,1));
		pentity->update_matrix(); 
	}
	
};


 void CFileView::OnExplorerSaveMaterial()
 {
	 HTREEITEM hItem =m_wndFileView.GetSelectedItem();
	 CString name=m_wndFileView.GetItemText(hItem);
	 gv_module * pmod=get_editor()->get_sandbox()->find_module(gv_id(CStringA(name)));
	 if (!pmod || !get_editor()->get_game_entity()) 
	 {
		 gv_string_tmp s; 
		 s<<"please load the module :" << name;
		 MessageBoxA(*s,"problem!", MB_OK);
		 return;
	 }; 
	 if (get_editor()->get_game_entity())
	 {
		 CNewObjectDialog new_d; 
		 new_d.DoModal(); 
		 gv_string_tmp obj_name;
		 obj_name= CStringA(new_d.m_object_name);
		 gv_material *old; 
		 gv_com_static_mesh * pcom=get_editor()->get_game_entity()->get_component<gv_com_static_mesh>(); 
		 if (pcom) old=pcom->get_material(); 
		 gv_com_skeletal_mesh * pcoms=get_editor()->get_game_entity()->get_component<gv_com_skeletal_mesh>(); 
		 if (pcoms) old=pcom->get_material(); 
		 if (!old) return;
		 gv_material * new_material=get_editor()->get_sandbox()->clone_object(old); 
		 new_material->set_owner(pmod); 
		 new_material->rename(*obj_name);
	 }
 };

 void	CFileView::DropObjectToFile  (gv::gv_object * object)
 {
	 HTREEITEM item =m_wndFileView.GetSelectedItem();
	 if (!item) return; 
	 if (!object) return ; 
	 gv_string_tmp s;
	 s<<m_wndFileView.GetItemText(item);
	 gv_string_tmp msg="move object and subobjects from  "; 
	 msg<< object->get_name()<< " to "<<s ; 
	 gv_string_tmp obj;
	 obj<<object->get_location();
	 if (MessageBoxA(*msg,"warning!",MB_OKCANCEL)==IDOK)
	 {
		 get_editor()->exec("move",obj ,s); 
	 }
 };

 void	CFileView::DropObjectToObject(gv::gv_object * object)
 {

	 HTREEITEM item =m_wndObjectView.GetSelectedItem();
	 if (!item) return; 
	 if (!object) return ; 
	 if (!m_wndObjectView.m_selected_object ) return; 
	 gv_string_tmp s;
	 if (object->is_a(gv_class_info::static_class()))
	 {
		gv_class_info * pcls= (gv_class_info * )object; 
		 gv_string_tmp s="are you sure to insert object of type  "; 
		 s<<pcls->get_name_id()<<" under object"
			 <<m_wndObjectView.m_selected_object->get_location(); 
		 if (MessageBoxA(*s,"notice",MB_OKCANCEL)==IDOK)
			{
				CNewObjectDialog dialog; 
				dialog.DoModal(); 
				gv_string_tmp s;
				s= CStringA(dialog.m_object_name);
				gv_string_tmp o;
				gv_string_tmp c;
				c<<pcls->get_name_id();
				o<<m_wndObjectView.m_selected_object->get_location();
				get_editor()->exec("new",c,s,o);
				CMainFrame::static_get()->ReflushCurrentModule(); 
			}
	 }
	 /*
	 s<<m_wndFileView.GetItemText(item);
	 gv_string_tmp msg="move object and subobjects from  "; 
	 msg<< object->get_name()<< " to "<<s ; 
	 gv_string_tmp obj;
	 obj<<object->get_location();
	 if (MessageBoxA(*msg,"warning!",MB_OKCANCEL)==IDOK)
	 {
		 get_editor()->exec("move",obj ,s); 
	 }*/
 };


 
 INT     CFileView::MessageBoxA(const char * t1, const char * t2, UINT type)
 {
	 return ::MessageBox(NULL,GV_TO_UNICODE(t1), GV_TO_UNICODE(t2), type);
 };