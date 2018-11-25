
// gv_rts_editorView.cpp : implementation of the Cgv_rts_editorView class
//

#include "stdafx.h"
#include "gv_rts_editor.h"
#include "MainFrm.h"
#include "gv_rts_editorDoc.h"
#include "gv_rts_editorView.h"
#include "NewObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cgv_rts_editorView

IMPLEMENT_DYNCREATE(Cgv_rts_editorView, CView)

BEGIN_MESSAGE_MAP(Cgv_rts_editorView, CView)
ON_WM_CLOSE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONDOWN()
ON_WM_KEYDOWN()
ON_WM_SIZE()
END_MESSAGE_MAP()

// Cgv_rts_editorView construction/destruction
gv::gv_game_engine* g_engine = NULL;
Cgv_rts_editorView* g_view = NULL;
Cgv_rts_editorView::Cgv_rts_editorView()
{
	// TODO: add construction code here
	m_engine = NULL;
	g_view = this;
}

Cgv_rts_editorView* Cgv_rts_editorView::static_get()
{
	return g_view;
};

Cgv_rts_editorView::~Cgv_rts_editorView()
{
	if (m_engine)
	{
		delete m_engine;
		g_engine = NULL;
		gv::gv_rts_editor_engine::static_destroy();
	}
	g_view = NULL;
}

BOOL Cgv_rts_editorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgv_rts_editorView drawing

void Cgv_rts_editorView::OnDraw(CDC* /*pDC*/)
{
	Cgv_rts_editorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void Cgv_rts_editorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cgv_rts_editorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x,
												  point.y, this, TRUE);
}

// Cgv_rts_editorView diagnostics

#ifdef _DEBUG
void Cgv_rts_editorView::AssertValid() const
{
	CView::AssertValid();
}

void Cgv_rts_editorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cgv_rts_editorDoc*
Cgv_rts_editorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgv_rts_editorDoc)));
	return (Cgv_rts_editorDoc*)m_pDocument;
}
#endif //_DEBUG

// Cgv_rts_editorView message handlers
using namespace gv;
void Cgv_rts_editorView::OnInitialUpdate()
{
	using namespace gv;
	CView::OnInitialUpdate();
	if (get_editor())
		return;
	gv_framework_config& fc = gv_global::framework_config;
	fc.window_handle = (gv_int_ptr) this->m_hWnd;
	CRect rect(0, 0, 1024, 1024);
	{
		SetWindowPos(NULL, 0, 0, 1024, 1024,
					 SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	};

	this->GetClientRect(&rect);
	gv_global::framework_config.window_size.set(rect.Width(), rect.Height());
	gv_rts_editor_engine::static_init(0, 0, "no_log " /*no_debug_draw"*/);
	CMainFrame::static_get()->m_wndOutput.HookGvLog();
	m_engine = new gv_rts_editor_engine;
	m_engine->init();
	g_engine = m_engine;
	CMainFrame::static_get()->m_wndFileView.FlushModule();
	CMainFrame::static_get()->m_wndClassView.FillClassView();
	CMainFrame::static_get()->m_wndProperties.InitPropList(m_engine);
	SetTimer(1, 10, 0);
	// TODO: Add your specialized code here and/or call the base class
}

BOOL Cgv_rts_editorView::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CView::DestroyWindow();
}

void Cgv_rts_editorView::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CView::OnClose();
}
void Cgv_rts_editorView::LockSelection()
{
	get_editor()->set_selection_locked(true);
};
void Cgv_rts_editorView::UnlockSelection()
{
	get_editor()->set_selection_locked(false);
};

extern LRESULT WindowProc_Hook(HWND wnd, UINT message, WPARAM wParam,
							   LPARAM lParam);
LRESULT Cgv_rts_editorView::WindowProc(UINT message, WPARAM wParam,
									   LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	WindowProc_Hook(m_hWnd, message, wParam, lParam);
	return CView::WindowProc(message, wParam, lParam);
}

void Cgv_rts_editorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	bool use_shift = false;
	bool use_alt = false;
	bool use_ctrl = false;
	bool use_right = false;
	if (nFlags & MK_SHIFT)
	{
		use_shift = true;
	}
	if (nFlags & MK_CONTROL)
	{
		use_ctrl = true;
	}
	use_alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	if (nFlags & MK_RBUTTON)
	{
		use_right = true;
	}

	if (CMainFrame::static_get()->m_drag_start)
	{
		// Drag the item to the current position of the mouse pointer.
		// First convert the dialog coordinates to control coordinates.
		// ClientToScreen(hwndParent, &point);
		// ScreenToClient(hwndTV, &point);
		ImageList_DragMove(point.x, point.y);
		// Turn off the dragged image so the background can be refreshed.
		ImageList_DragShowNolock(FALSE);

		// Find out if the pointer is on the item. If it is,
		// highlight the item as a drop target.
		// tvht.pt.x = point.x;
		// tvht.pt.y = point.y;
		// if ((htiTarget = TreeView_HitTest(hwndTV, &tvht)) != NULL)
		//{
		//	TreeView_SelectDropTarget(hwndTV, htiTarget);
		//}
		ImageList_DragShowNolock(TRUE);
	}
	else if (get_editor() &&
			 get_editor()->get_editor_mode() == e_editor_mode_map &&
			 get_editor()->is_drag_move_started())
	{
		get_editor()->drag_object_with_mouse(use_shift, use_ctrl, use_alt,
											 use_right);
	}
	else if (get_editor() &&
			 get_editor()->get_editor_mode() == e_editor_mode_terrain &&
			 get_editor()->is_paint_terrain_start())
	{
		get_editor()->paint_object_with_mouse(use_shift, use_ctrl, use_alt,
											  use_right);
	}
	CView::OnMouseMove(nFlags, point);
}

void Cgv_rts_editorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (get_editor()->get_editor_mode() == e_editor_mode_map)
	{
		m_start_drag_move_entity = false;
		get_editor()->end_drag_move();
	}
	if (get_editor()->get_editor_mode() == e_editor_mode_terrain)
	{
		get_editor()->end_paint_terrain();
	}
	CView::OnLButtonUp(nFlags, point);
}

void Cgv_rts_editorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonDown(nFlags, point);
}

void Cgv_rts_editorView::DropObject(gv::gv_object* object)
{
	if (get_editor()->get_editor_mode() == e_editor_mode_map)
	{
		get_editor()->drop_object_to_world_use_mouse_position(object);
	}
	else if (get_editor()->get_editor_mode() == e_editor_mode_actor)
	{
		get_editor()->drop_object_to_actor(object);
	}
};

gv_string_tmp get_new_object_name_dialog()
{
	CNewObjectDialog dialog;
	dialog.DoModal();
	gv_string_tmp s;
	s = CStringA(dialog.m_object_name);
	return s;
}

void Cgv_rts_editorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (get_editor()->is_selection_locked() &&
		!get_editor()->get_selected_entity())
	{
		gv_vector3 pos;
		gv_entity* entity;
		get_editor()->test_mouse_point(pos, entity);
		//----------------------------------------------------------------
		// begin test code::terrain editor
		/*if (entity)
    {
            gv_com_terrain_roam *
    pterrain=entity->get_component<gv_com_terrain_roam>();
            if (pterrain)
            {
                    gv_float _pattern[]=
                    {
                            0,0,1,0,0,
                            0,1,2,1,0,
                            1,4,5,4,1,
                            0,1,2,1,0,
                            0,0,1,0,0,
                    };
                    gv_vector3 v=pterrain->world_to_terrain_local(pos);
                    gv_float ix=gvt_floor(v.x);
                    gv_float iy=gvt_floor(v.y);
                    gv_rect r(ix-2.f, iy-2.f, ix+3.f, iy+3.f);
                    gvt_array<gv_float> pattern;
                    pattern.init(_pattern,gvt_array_length(_pattern));
                    pterrain->update_heightmap(r,pattern,0);

            }
    }*/
		// end test code;
		//----------------------------------------------------------------
		CView::OnLButtonDown(nFlags, point);
		return;
	}
	if (get_editor()->get_editor_mode() == e_editor_mode_map)
	{
		gv_vector3 pos;
		gv_entity* entity;
		if (get_editor()->test_mouse_point(pos, entity))
		{
			bool is_terrain = false;
			if (entity)
			{
				is_terrain = entity->get_component< gv_com_terrain_roam >() != NULL;
			}
			// you can't drag terrain....
			if (get_editor()->is_selection_locked())
			{
				m_start_drag_move_entity = true;
				get_editor()->start_drag_move();
				return CView::OnLButtonDown(nFlags, point);
			}
			else if (!is_terrain && entity)
			{
				if (!get_editor()->is_selection_locked())
					CMainFrame::static_get()->SelectObject(entity);
				m_start_drag_move_entity = true;
				get_editor()->start_drag_move();
				return CView::OnLButtonDown(nFlags, point);
			}
		}
		CMainFrame::static_get()->SelectObject(NULL);
		get_editor()->end_drag_move();
		m_start_drag_move_entity = false;
	}
	else if (get_editor()->get_editor_mode() == e_editor_mode_terrain)
	{
		gv_vector3 pos;
		gv_entity* entity;
		if (get_editor()->test_mouse_point(pos, entity))
		{
			bool is_terrain = false;
			if (entity)
			{
				is_terrain = entity->get_component< gv_com_terrain_roam >() != NULL;
			}
			if (is_terrain)
			{
				CMainFrame::static_get()->SelectObject(entity);
				get_editor()->start_paint_terrain();
				return CView::OnLButtonDown(nFlags, point);
			}
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void Cgv_rts_editorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (get_editor()->get_editor_mode() == e_editor_mode_map)
	{
		if (nChar == VK_DELETE && get_editor()->get_selected_entity())
		{
			gv_string_tmp s = "are you sure to delete object ";
			s << get_editor()->get_selected_entity()->get_location()
			  << " and it's sub object ?"
			  << " if other object reference to it, might has problem";
			if (MessageBox(GV_TO_UNICODE(*s), GV_TO_UNICODE("warning!"),
						   MB_OKCANCEL) == IDOK)
			{
				gv_string_tmp s;
				s << get_editor()->get_selected_entity()->get_location();
				get_editor()->exec("delete", s, s);
				CMainFrame::static_get()->ReflushCurrentModule();
				CMainFrame::static_get()->SelectObject(NULL);
				this->UnlockSelection();
			}
			return;
		}
		if (nChar == VK_SPACE)
		{
			get_editor()->set_selection_locked(!get_editor()->is_selection_locked());
		}
		else if (nChar == VK_ESCAPE)
		{
			get_editor()->set_selection_locked(true);
			CMainFrame::static_get()->SelectObject(NULL);
		}
	} // map edit mode
	else if (get_editor()->get_editor_mode() == e_editor_mode_terrain)
	{
		if (nChar == VK_SPACE)
		{
			get_editor()->set_selection_locked(!get_editor()->is_selection_locked());
		}
		else if (nChar == VK_ESCAPE)
		{
			// m_selection_locked=true
			get_editor()->set_selection_locked(false);
			CMainFrame::static_get()->SelectObject(NULL);
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Cgv_rts_editorView::OnSize(UINT nType, int cx, int cy)
{
	if (!g_engine)
		CView::OnSize(nType, cx, cy);
	else
	{
		CView::OnSize(nType, cx, cy);
		// GVM_POST_EVENT_TO_SANDBOX( get_editor()->get_sandbox(),
		// render_resize_window, render, pe->sx = cx; pe->sy = cy;);
		SetWindowPos(NULL, 0, 0, gv_global::framework_config.window_size.get_x(),
					 gv_global::framework_config.window_size.get_y(),
					 SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	// TODO: Add your message handler code here
}

void Cgv_rts_editorView::SetClientSize(int x, int y)
{
	CRect myRect(0, 0, x, y);
	CRect r1, r2;
	GetWindowRect(&r1);
	GetClientRect(&r2);
	int offset_x = r1.Width() - r2.Width();
	int offset_y = r1.Height() - r2.Height();
	CalcWindowRect(&myRect, CWnd::adjustOutside);
	// MoveWindow(0, 0, myRect.Width(), myRect.Height());
	// SetWindowPos
	SetWindowPos(NULL, 0, 0, x + offset_x, y + offset_y,
				 SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	GetClientRect(&myRect);
};
