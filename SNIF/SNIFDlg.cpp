
// SNIFDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SNIF.h"
#include "SNIFDlg.h"
#include "afxdialogex.h"
#include "UIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSNIFDlg::CSNIFDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSNIFDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSNIFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSNIFDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CSNIFDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	return TRUE;
}

void CSNIFDlg::OnCancel()
{
	CDialogEx::OnCancel();
}

void CSNIFDlg::OnDestroy()
{
	POSITION pos = m_mapUI.GetStartPosition();
	ISocket* pSock = nullptr;
	SOCKET handle = INVALID_SOCKET;
	CDialog* pUI = nullptr;
	while(pos) {
		m_mapUI.GetNextAssoc(pos, handle, pUI);

		if(INVALID_SOCKET != handle) {
			m_mapSocket.Lookup(handle, pSock);
		}

		if(nullptr != pUI) {
			delete pUI;
			delete pSock;
		}

		if(nullptr != pSock) {
			pSock->Close();
			delete pSock;
			pSock = nullptr;
		}
	}
	m_mapUI.RemoveAll();
	m_mapSocket.RemoveAll();

	CDialogEx::OnDestroy();
}

void CSNIFDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSNIFDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool CSNIFDlg::AddSocket(ISocket* pSocket)
{
	if(nullptr == pSocket) {
		return false;
	}

	ISocket* pCurSock = nullptr;
	m_mapSocket.Lookup(pSocket->getHandle(), pCurSock);

	if(nullptr != pCurSock) { //Already map
		ASSERT(FALSE);
		return false;
	}

	CDialog* pUI = nullptr;
	m_mapUI.Lookup(pSocket->getHandle(), pUI);


	if(nullptr == pUI) {
		//Not found
		pUI = new CUIDialog();
		pUI->Create(CUIDialog::IDD);
		pUI	->ShowWindow(SW_SHOWNORMAL);

		m_mapUI.SetAt(pSocket->getHandle(), pUI);
	}

	pSocket->setManager(this);

	return true;
}

void CSNIFDlg::ReceiveData(ISocket* pSocket, const BYTE* data)
{
	if(nullptr == pSocket) {
		return;
	}


}