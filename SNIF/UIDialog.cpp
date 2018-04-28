// UIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SNIF.h"
#include "UIDialog.h"
#include "afxdialogex.h"


// CUIDialog dialog

IMPLEMENT_DYNAMIC(CUIDialog, CDialog)

CUIDialog::CUIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUIDialog::IDD, pParent)
{

}

CUIDialog::~CUIDialog()
{
}

void CUIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CUIDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CUIDialog::OnDestroy()
{
	CDialog::OnDestroy();
}

BEGIN_MESSAGE_MAP(CUIDialog, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()
