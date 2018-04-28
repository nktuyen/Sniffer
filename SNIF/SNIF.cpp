
// SNIF.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SNIF.h"
#include "SNIFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSNIFApp::CSNIFApp()
{
	
}

CSNIFApp::~CSNIFApp()
{

}

CSNIFApp theApp;

BOOL CSNIFApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	WSADATA data={0};
	int nStartedup = WSAStartup(MAKEWORD(2, 2), &data);

	SetRegistryKey(_T("NKTUYEN"));

	CSNIFDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	
	return FALSE;
}

int CSNIFApp::ExitInstance()
{
	WSACleanup();

	return CWinApp::ExitInstance();
}