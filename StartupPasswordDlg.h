#pragma once

// CStartupPasswordDlg dialog

class CStartupPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStartupPasswordDlg)

public:
	CStartupPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStartupPasswordDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STARTUP_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:

	CString m_strPassword;
};
