
// gv_rts_editorView.h : interface of the Cgv_rts_editorView class
//

#pragma once

class Cgv_rts_editorView : public CView
{
protected: // create from serialization only
	Cgv_rts_editorView();
	DECLARE_DYNCREATE(Cgv_rts_editorView)

	// Attributes
public:
	Cgv_rts_editorDoc* GetDocument() const;

	// Operations
public:
	// Overrides
public:
	virtual void OnDraw(CDC* pDC); // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	// Implementation
public:
	virtual ~Cgv_rts_editorView();
	static Cgv_rts_editorView* static_get();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

public:
	gv::gv_game_engine* m_engine;
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
	void DropObject(gv::gv_object* object);
	void LockSelection();
	void UnlockSelection();
	void SetClientSize(int x, int y);

public:
	bool m_start_drag_move_entity;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG // debug version in gv_rts_editorView.cpp
inline Cgv_rts_editorDoc* Cgv_rts_editorView::GetDocument() const
{
	return reinterpret_cast< Cgv_rts_editorDoc* >(m_pDocument);
}
#endif
