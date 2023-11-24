#pragma once

#include "TestModeView.h"
#include "TestModeView_Test.h"
#include ".\CustomUI\Title.h"

// CCurrentIdleOptionDlg dialog

class CCurrentIdleOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCurrentIdleOptionDlg)

public:
	CCurrentIdleOptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCurrentIdleOptionDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CURRENT_IDLE_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedOk();
	afx_msg void OnCheckBnClicked();

	DECLARE_MESSAGE_MAP()

	void InitUiContorl();
	void EnableDialogControls(int nUpdateID);

public:

	BOOL m_bUseIFE;
	TEST_CURRENT_CONSUMPTION_ITEM	m_Idle;
	TEST_CURRENT_CONSUMPTION_ITEM	m_Sleep;
	TEST_CURRENT_CONSUMPTION_ITEM	m_DeepSleep;

protected:

	CTitle m_Title;
	CShadeButtonST			m_btnOK;
	CShadeButtonST			m_btnCancel;
};
