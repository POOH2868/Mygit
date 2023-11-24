#pragma once
#include "resource.h"
//#include "afxwin.h"

// popup_noti 대화 상자입니다.

class popup_noti : public CDialog
{
	DECLARE_DYNAMIC(popup_noti)

public:
	popup_noti(BOOL bModelessDlg, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~popup_noti();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_POPUP_NOTI };

protected:

	BOOL 	m_bModelessDlg;
	BOOL	m_bDialogExit;

	CBrush 	mBrush;
	CFont  	mFont;
	CFont  	mbtnFont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonExit();

	DECLARE_MESSAGE_MAP()

public:

	BOOL	IsDialogExit() { return m_bDialogExit; }

};

