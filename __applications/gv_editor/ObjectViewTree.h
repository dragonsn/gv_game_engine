#pragma once
#include "viewtree.h"

class CObjectViewTree :
	public CViewTree
{
public:
	CObjectViewTree(void);
	~CObjectViewTree(void);
	void		FillObjectTree (gv::gv_module * mod);
	gv::gv_object * m_selected_object;
	void		MySelectItem		(HTREEITEM item);
protected:
	HTREEITEM	FillOneNode	   (gv::gv_object * pobj);
	gv::gvt_dictionary<gv::gv_object*,HTREEITEM > m_object_map;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
};
