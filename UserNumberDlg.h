#pragma once

#include ".\CustomUI\Title.h"
#include "util.h"

// CUserNumberDlg dialog

class CUserNumberDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserNumberDlg)

public:
	CUserNumberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserNumberDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_NUMBER };

protected:

	CTitle m_Title;
	CShadeButtonST			m_btnOK;
	CShadeButtonST			m_btnCancel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:

	CString m_strUserNumber;
};
