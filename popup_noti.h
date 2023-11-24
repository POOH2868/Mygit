#pragma once
#include "resource.h"
//#include "afxwin.h"

// popup_noti ��ȭ �����Դϴ�.

class popup_noti : public CDialog
{
	DECLARE_DYNAMIC(popup_noti)

public:
	popup_noti(BOOL bModelessDlg, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~popup_noti();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_POPUP_NOTI };

protected:

	BOOL 	m_bModelessDlg;
	BOOL	m_bDialogExit;

	CBrush 	mBrush;
	CFont  	mFont;
	CFont  	mbtnFont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonExit();

	DECLARE_MESSAGE_MAP()

public:

	BOOL	IsDialogExit() { return m_bDialogExit; }

};

