#pragma once

#include ".\CustomUI\XColorStatic.h"
#include "Global.h"

// CGpioButtonDlg ��ȭ �����Դϴ�.

class CGpioButtonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGpioButtonDlg)

public:
	CGpioButtonDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGpioButtonDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_GPIO_BUTTON };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnShowDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateButtonStatus(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void ShowDialog(int nShow);
	void SetButtonStatus(int nIndex, int nStatus, BOOL bInit);

protected:

	CWnd *			m_pParent;

	CXColorStatic	m_ButtonStatus[MAX_DEVICE_COUNT];

	CXColorStatic	m_StRemainTime;

	int				m_nRemainTime;

	CBrush			mBrush;
	CFont			mFont;
	CFont			mbtnFont;

	BUTTON_STATUS	m_Status;
};
