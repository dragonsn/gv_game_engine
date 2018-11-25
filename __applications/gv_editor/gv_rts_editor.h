
// gv_rts_editor.h : main header file for the gv_rts_editor application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols

// Cgv_rts_editorApp:
// See gv_rts_editor.cpp for the implementation of this class
//
#ifdef GV_EDITOR_INTERNAL
#      define GV_EDITOR_API   //__declspec(dllexport)
#    else
#      define GV_EDITOR_API   //__declspec(dllimport)
# endif

class GV_EDITOR_API Cgv_rts_editorApp : public CWinAppEx
{
public:
	Cgv_rts_editorApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation
	UINT m_nAppLook;
	BOOL m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};
//AS A DLL , need to call from launcher
extern GV_EDITOR_API Cgv_rts_editorApp  theApp;
