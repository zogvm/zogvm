// zogvm.h : main header file for the ZOGVM application
//

#if !defined(AFX_ZOGVM_H__8410F995_2E74_40CC_98B5_3CE28E293F61__INCLUDED_)
#define AFX_ZOGVM_H__8410F995_2E74_40CC_98B5_3CE28E293F61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZogvmApp:
// See zogvm.cpp for the implementation of this class
//

class CZogvmApp : public CWinApp
{
public:
	CZogvmApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZogvmApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZogvmApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOGVM_H__8410F995_2E74_40CC_98B5_3CE28E293F61__INCLUDED_)
