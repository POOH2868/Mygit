#pragma once
#include "afxwin.h"


// CInputDlg ��ȭ �����Դϴ�.

class CInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDlg)

public:
	CInputDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInputDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
