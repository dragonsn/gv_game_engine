
// gv_rts_editorDoc.h : interface of the Cgv_rts_editorDoc class
//

#pragma once

class Cgv_rts_editorDoc : public CDocument
{
protected: // create from serialization only
	Cgv_rts_editorDoc();
	DECLARE_DYNCREATE(Cgv_rts_editorDoc)

	// Attributes
public:
	// Operations
public:
	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Implementation
public:
	virtual ~Cgv_rts_editorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
