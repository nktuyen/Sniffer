
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
	,m_Sniffer()
	,NICs(/*MIB_IF_TYPE_ETHERNET*/)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSNIFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_CBO_NICs, m_cboNICs);
	DDX_Control(pDX, IDC_LVW_NIC_DETAIL, m_lvwNICDetail);
}

BEGIN_MESSAGE_MAP(CSNIFDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_CBO_NICs, &CSNIFDlg::OnCbnSelchangeCboNics)
END_MESSAGE_MAP()

BOOL CSNIFDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_lvwNICDetail.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
	m_lvwNICDetail.InsertColumn(0, _T("Property"), LVCFMT_LEFT, 100);
	m_lvwNICDetail.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 180);
	m_lvwNICDetail.DeleteAllItems();

	for(SIZE_T n=0;n<NICs.getCount();n++) {
		m_cboNICs.AddString(NICs.getInterfaceName(n));
	}


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

void CSNIFDlg::SocketData(ISocket* pSocket, const BYTE* data)
{
	if(nullptr == pSocket) {
		return;
	}


}

void CSNIFDlg::OnCbnSelchangeCboNics()
{
	m_lvwNICDetail.DeleteAllItems();
	int nSel = m_cboNICs.GetCurSel();
	m_btnStart.EnableWindow(FALSE);

	if(CB_ERR == nSel) {
		return;
	}

	CString strType = NICs.getTypeName(nSel);
	int nItem = m_lvwNICDetail.InsertItem(m_lvwNICDetail.GetItemCount(), _T("Type"));
	m_lvwNICDetail.SetItemText(nItem, 1, strType);

	CString strMAC = NICs.getMACAddress(nSel);
	nItem = m_lvwNICDetail.InsertItem(m_lvwNICDetail.GetItemCount(), _T("MAC Address"));
	m_lvwNICDetail.SetItemText(nItem, 1, strMAC);

	CString strIP = NICs.getIPAddress(nSel);
	nItem = m_lvwNICDetail.InsertItem(m_lvwNICDetail.GetItemCount(), _T("IP Address"));
	m_lvwNICDetail.SetItemText(nItem, 1, strIP);

	m_btnStart.EnableWindow(!strIP.IsEmpty());
}
