#include "StdAfx.h"
#include "ObjectViewTree.h"
#include "mainfrm.h"
#include "NewObjectDialog.h"
using namespace gv; 

CObjectViewTree::CObjectViewTree(void)
{
	m_selected_object=NULL;
}

CObjectViewTree::~CObjectViewTree(void)
{
}

HTREEITEM	CObjectViewTree::FillOneNode (gv_object * pobj )
{
	if (!pobj) return NULL;

	HTREEITEM hClass=NULL;
	if (this->m_object_map.find(pobj,hClass))
	{
		return hClass;
	}
	HTREEITEM  father=NULL;
	if (	pobj->get_owner()
		&& 	!this->m_object_map.find(pobj->get_owner(),father)
		)
	{
		father=this->FillOneNode(pobj->get_owner());
	}
	gv_string_tmp s; 
	s<<pobj->get_name(); 
	hClass = InsertItem (GV_TO_UNICODE(*s), 6, 6,father);
	this->m_object_map.add_pair(pobj,hClass);
	return hClass;

};

void CObjectViewTree::FillObjectTree (gv::gv_module * mod)
{
	DeleteAllItems();
	m_object_map.empty();
	if (!mod) return;
	gvt_array<gv_object*> objects;
	mod->get_sandbox()->query_module_object( mod->get_name_id(),objects,false);
	for ( int j=0; j<objects.size(); j++) this->FillOneNode(objects[j]);
};

BEGIN_MESSAGE_MAP(CObjectViewTree, CViewTree)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CObjectViewTree::OnNMDblclk)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CObjectViewTree::OnTvnBegindrag)
END_MESSAGE_MAP()

void CObjectViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	using namespace gv; 
	HTREEITEM hItem = GetSelectedItem();
	gv_object * obj;
	if (m_object_map.find(hItem,obj ))
	{
		CMainFrame::static_get()->SelectObject(obj,false);
		m_selected_object=obj;
	}
	*pResult = 0;
}

void CObjectViewTree::MySelectItem(HTREEITEM item)
{
	gv_object * obj;
	this->SelectItem(item);
	if (m_object_map.find(item,obj ))
	{
		m_selected_object=obj;
	}
}


void CObjectViewTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar==VK_DELETE && m_selected_object)
	{
		gv_string_tmp s="are you sure to delete object "; 
		s<<m_selected_object->get_location()<<" and it's sub object ?"
			<<" if other object reference to it, might has problem"; 
		if (MessageBox(GV_TO_UNICODE(*s), GV_TO_UNICODE("warning!"),MB_OKCANCEL)==IDOK)
		{
			gv_string_tmp s; 
			s<<m_selected_object->get_location(); 
			get_editor()->exec("delete",s,s);
			CMainFrame::static_get()->ReflushCurrentModule(); 
			CMainFrame::static_get()->SelectObject(NULL);
			m_selected_object=NULL;
		}
		return ;
	}
	else if (nChar==VK_INSERT )
	{
		if (m_selected_object && CMainFrame::static_get()->m_selected_class)
		{
		
			gv_string_tmp s="are you sure to insert object of type  "; 
			s<<CMainFrame::static_get()->m_selected_class->get_name_id()<<" under object"
				<<m_selected_object->get_location(); 
			if (MessageBox(GV_TO_UNICODE(*s), GV_TO_UNICODE("notice"),MB_OKCANCEL)==IDOK)
			{
				CNewObjectDialog dialog; 
				dialog.DoModal(); 
				gv_string_tmp s;
				s= CStringA(dialog.m_object_name);
				gv_string_tmp o;
				gv_string_tmp c;
				c<<CMainFrame::static_get()->m_selected_class->get_name_id();
				o<<m_selected_object->get_location();
				get_editor()->exec("new",c,s,o);
				CMainFrame::static_get()->ReflushCurrentModule(); 
			}
		}
	}
	else if (nChar=='R'&& m_selected_object )
	{
		gv_string_tmp s="are you sure to rename object    "; 
		s<<m_selected_object->get_location() <<" , any reference to the object will be NULL!!"; 
		if (MessageBox(GV_TO_UNICODE(*s), GV_TO_UNICODE("notice"),MB_OKCANCEL)==IDOK)
		{
			CNewObjectDialog dialog; 
			dialog.DoModal(); 
			gv_string_tmp s;
			s= CStringA (dialog.m_object_name);
			gv_string_tmp c;
			c<<m_selected_object->get_location();
			get_editor()->exec("rename",c,s);
			CMainFrame::static_get()->ReflushCurrentModule(); 
		}
	}
	else if (nChar=='C' && m_selected_object)
	{
		gv_string_tmp s="are you sure to clone object    "; 
		s<<m_selected_object->get_location() <<" "; 
		if (::MessageBoxA(NULL, *s,"notice",MB_OKCANCEL)==IDOK)
		{
			CNewObjectDialog dialog; 
			dialog.DoModal(); 
			gv_string_tmp s;
			s= CStringA(dialog.m_object_name);
			gv_string_tmp c;
			c<<m_selected_object->get_location();
			get_editor()->exec("clone",c,s);
			CMainFrame::static_get()->ReflushCurrentModule(); 
		}
	}
	CViewTree::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CObjectViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HIMAGELIST himl;    // handle to image list 
	RECT rcItem;        // bounding rectangle of item 
	*pResult = 0;
	// Tell the tree-view control to create an image to use 
	// for dragging. 
	himl = TreeView_CreateDragImage(m_hWnd, pNMTreeView->itemNew.hItem); 
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	gv_object * obj;
	if (m_object_map.find(hItem,obj ))
	{
		//CMainFrame::static_get()->SelectObject(obj);
		//m_selected_object=obj;
	}
	else return;
	// Get the bounding rectangle of the item being dragged. 
	TreeView_GetItemRect(m_hWnd, pNMTreeView->itemNew.hItem, &rcItem, TRUE); 
	// Start the drag operation. 
	ImageList_BeginDrag(himl, 0, 0, 0);
	//ImageList_DragEnter(Cgv_rts_editorView::static_get()->m_hWnd, pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.x); 
	ImageList_DragEnter(CMainFrame::static_get()->m_hWnd, pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.x); 
	// Hide the mouse pointer, and direct mouse input to the 
	// parent window. 
	CMainFrame::static_get()->DragStart(obj);
}
