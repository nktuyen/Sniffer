#pragma once


class CUIDialog : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CUIDialog)
	enum { IDD = IDD_CONNECTION_UI };
public:
	CUIDialog(CWnd* pParent = NULL);
	virtual ~CUIDialog();
protected:
	void DoDataExchange(CDataExchange* pDX); 
	BOOL OnInitDialog();
protected:
	afx_msg void OnDestroy();
};
