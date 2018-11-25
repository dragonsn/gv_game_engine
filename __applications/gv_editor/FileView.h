
#pragma once

#include "ViewTree.h"
#include "FileViewTree.h"
#include "ObjectViewTree.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
	friend class CMainFrame;
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
public:

	CFileViewTree m_wndFileView;
	CObjectViewTree m_wndObjectView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

protected:
	void FillFileView();
	void DoImport (int type);

// Implementation
public:
	virtual ~CFileView();
	void	FlushModule();
	void	DropObjectToFile  (gv::gv_object * object);
	void	DropObjectToObject(gv::gv_object * object);
	INT     MessageBoxA(const char *, const char *, UINT type);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadModule();
	afx_msg void OnSaveModule();
	afx_msg void OnImport0();
	afx_msg void OnImport1();
	afx_msg void OnImport2();
	afx_msg void OnImport3();
	afx_msg void OnImport4();
	afx_msg void OnImport5();
	afx_msg void OnImport6();
	afx_msg void OnImport7();
	afx_msg void OnImport8();
	afx_msg void OnImport9();
	afx_msg void OnImporta();
	afx_msg void OnImportb();
	afx_msg void OnImportc();
	afx_msg void OnExplorerBinarizemodule();
	afx_msg void OnExplorerNewmodule();
	afx_msg void OnExplorerUnloadmodule();
	afx_msg void OnExplorerXmlizemodule();
	afx_msg void OnExplorerNewMap();
	afx_msg void OnExplorerNewArchitype();
	afx_msg void OnExplorerNewArchitype2();
	afx_msg void OnExplorerSaveMaterial();
	afx_msg void OnExplorerCopyModuleAs();
	
	
};

