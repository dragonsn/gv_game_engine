#include "StdAfx.h"
#include "FileViewTree.h"
#include "MainFrm.h"

CFileViewTree::CFileViewTree(void)
{
}

CFileViewTree::~CFileViewTree(void)
{
}
BEGIN_MESSAGE_MAP(CFileViewTree, CViewTree)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CFileViewTree::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CFileViewTree::OnTvnBegindrag)
	ON_COMMAND(ID_EXPLORER_SAVEGAMEENTITYASARCHITYPE, &CFileViewTree::OnExplorerSavegameentityasarchitype)
END_MESSAGE_MAP()

void CFileViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	using namespace gv; 
	HTREEITEM hItem = GetSelectedItem();
	CString name=this->GetItemText(hItem);
	gv_string_tmp s= CStringA(name.GetString());
	//if (name=="native0")
	if (s.has_prefix("(engine)"))
	{
		s.replace_all("(engine)","");
		CMainFrame::static_get()->SelectModule(gv_global::sandbox_mama->get_base_sandbox()->find_module(gv_id(*s)));
		*pResult = 0;
		return;
	}
	CMainFrame::static_get()->SelectModule(get_editor()->get_sandbox()->find_module(gv_id(CStringA(name))));
	*pResult = 0;
}

void CFileViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CFileViewTree::OnExplorerSavegameentityasarchitype()
{
	// TODO: Add your command handler code here
}
