
// SNIF.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CSNIFApp : public CWinApp
{
public:
	CSNIFApp();
	virtual ~CSNIFApp();
private:
	BOOL InitInstance();
	int ExitInstance();
};

extern CSNIFApp theApp;