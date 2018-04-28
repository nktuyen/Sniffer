#pragma once

#include "TCPSocket.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "NetworkInterfaces.h"

class CSNIFDlg : public CDialogEx, public ISocketManager
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_SNIF_DIALOG };
protected:
	HICON m_hIcon;
	CMap<SOCKET,SOCKET,ISocket*,ISocket*> m_mapSocket;
	CMap<SOCKET,SOCKET,CDialog*,CDialog*> m_mapUI;
	TCPSocket m_Sniffer;
	CButton m_btnStart;
	CComboBox m_cboNICs;
	CNetworkInterfaces NICs;
	CListCtrl m_lvwNICDetail;
public:
	CSNIFDlg(CWnd* pParent = NULL);
	virtual ~CSNIFDlg() {; }
protected:
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);
	void OnCancel();
	bool AddSocket(ISocket* pSocket);
	void SocketData(ISocket* pSocket, const BYTE* data);
protected:	
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCboNics();
};
