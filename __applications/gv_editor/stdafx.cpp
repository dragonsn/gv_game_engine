
// stdafx.cpp : source file that includes just the standard includes
// gv_rts_editor.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


//#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE MFCLibrary1DLL = { false, nullptr };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("MFCLibrary1.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(MFCLibrary1DLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(MFCLibrary1DLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("MFCLibrary1.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(MFCLibrary1DLL);
	}
	return 1;   // ok
}
