#pragma once
#include "afxwin.h"


// CInputDlg 대화 상자입니다.

class CInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDlg)

public:
	CInputDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editInput;
	CString m_strTitle;
	CString m_strInput;
	BOOL m_bPassword;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
