#pragma once
#include "viewtree.h"

class CClassViewTree :
	public CViewTree
{
public:
	CClassViewTree(void);
	~CClassViewTree(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
};
