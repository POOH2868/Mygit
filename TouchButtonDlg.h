#pragma once

#include ".\CustomUI\XColorStatic.h"
#include "global.h"

// CTouchButtonDlg dialog

class CTouchButtonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTouchButtonDlg)

public:
	CTouchButtonDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTouchButtonDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOUCH_BUTTON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	LRESULT OnShowDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateButtonStatus(WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
	LRESULT OnUpdateButtonValue(WPARAM wParam, LPARAM lParam);
#endif

	DECLARE_MESSAGE_MAP()

	void ShowDialog(int nShow);
	void SetButtonStatus(int nIndex, int nStatus, BOOL bInit);

protected:

	CWnd *			m_pParent;

	CXColorStatic	m_ButtonStatus[MAX_DEVICE_COUNT];

	CBrush			mBrush;
	CFont			mFont;
	CFont			mbtnFont;
};
