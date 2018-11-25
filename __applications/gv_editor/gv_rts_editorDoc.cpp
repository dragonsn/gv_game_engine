
// gv_rts_editorDoc.cpp : implementation of the Cgv_rts_editorDoc class
//

#include "stdafx.h"
#include "gv_rts_editor.h"

#include "gv_rts_editorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cgv_rts_editorDoc

IMPLEMENT_DYNCREATE(Cgv_rts_editorDoc, CDocument)

BEGIN_MESSAGE_MAP(Cgv_rts_editorDoc, CDocument)
END_MESSAGE_MAP()

// Cgv_rts_editorDoc construction/destruction

Cgv_rts_editorDoc::Cgv_rts_editorDoc()
{
	// TODO: add one-time construction code here
}

Cgv_rts_editorDoc::~Cgv_rts_editorDoc()
{
}

BOOL Cgv_rts_editorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// Cgv_rts_editorDoc serialization

void Cgv_rts_editorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

// Cgv_rts_editorDoc diagnostics

#ifdef _DEBUG
void Cgv_rts_editorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Cgv_rts_editorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// Cgv_rts_editorDoc commands
