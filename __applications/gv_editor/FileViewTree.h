#pragma once
#include "viewtree.h"

class CFileViewTree :
	public CViewTree
{
public:
	CFileViewTree(void);
	~CFileViewTree(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnExplorerSavegameentityasarchitype();
};
