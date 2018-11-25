#include "StdAfx.h"
#include "ClassViewTree.h"
#include "gv_rts_editor.h"
#include "MainFrm.h"

CClassViewTree::CClassViewTree(void)
{
}

CClassViewTree::~CClassViewTree(void)
{
}
BEGIN_MESSAGE_MAP(CClassViewTree, CViewTree)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CClassViewTree::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CClassViewTree::OnTvnBegindrag)
END_MESSAGE_MAP()
using namespace gv; 

void CClassViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	
	HTREEITEM hItem = GetSelectedItem();
	CStringA name;
	name=this->GetItemText(hItem);
	gv_class_info * pcls=gv_global::sandbox_mama->get_base_sandbox()->find_class(gv_id(name)); 
	if (pcls)
	{
		CMainFrame::static_get()->SelectObject(pcls);
	}
	*pResult = 0;
}

void CClassViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	HIMAGELIST himl;    // handle to image list 
	RECT rcItem;        // bounding rectangle of item 
	*pResult = 0;
	// Tell the tree-view control to create an image to use 
	// for dragging. 
	himl = TreeView_CreateDragImage(m_hWnd, pNMTreeView->itemNew.hItem); 
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CStringA name;
	name=this->GetItemText(hItem);
	gv_class_info * pcls=gv_global::sandbox_mama->get_base_sandbox()->find_class(gv_id(name)); 
	if (!pcls) return; 
	// Get the bounding rectangle of the item being dragged. 
	TreeView_GetItemRect(m_hWnd, pNMTreeView->itemNew.hItem, &rcItem, TRUE); 
	// Start the drag operation. 
	ImageList_BeginDrag(himl, 0, 0, 0);
	//ImageList_DragEnter(Cgv_rts_editorView::static_get()->m_hWnd, pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.x); 
	ImageList_DragEnter(CMainFrame::static_get()->m_hWnd, pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.x); 
	// Hide the mouse pointer, and direct mouse input to the 
	// parent window. 
	CMainFrame::static_get()->DragStart(pcls);

	
}
