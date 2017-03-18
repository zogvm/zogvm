// zogvm.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "zogvm.h"
#include "zogvmDlg.h"

#include "SqliteOperate.h"
CSqliteOperate SQLDB;
#include "DLGHdd.h"
#include "stdint.h"

#include "video.h"

TCHAR CurrentDir[MAX_PATH*2]="";

struct SYSTEM_SET_ST systemset={0};


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZogvmApp

BEGIN_MESSAGE_MAP(CZogvmApp, CWinApp)
	//{{AFX_MSG_MAP(CZogvmApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZogvmApp construction

CZogvmApp::CZogvmApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZogvmApp object

CZogvmApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CZogvmApp initialization

BOOL CZogvmApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	GetCurrentDirectory(MAX_PATH*2, CurrentDir);


	SQLDB.OpenDB("zogvm.db");
	SQLDB.ZiDian_CreateTable();
	SQLDB.Type_CreateTable();
	SQLDB.Hdd_CreateTable();
	SQLDB.File_CreateTable();
	SQLDB.File_CreateView();
	SQLDB.Double_CreateTable();
	SQLDB.Dir_CreateTable();
	SQLDB.SystemSet_CreateTable();
	SQLDB.SystemSet_Read(systemset);


	SQLDB.Begin();
	SQLDB.File_SetNoFlagAll();
	SQLDB.Hdd_SetNonsert();
	CDLGHdd::Add27HDDid();
	SQLDB.Commit();

	CZogvmDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	SQLDB.CloseDB();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
