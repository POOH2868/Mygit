#pragma once


#include ".\CustomUI\RichEditEx.h"

// CTestResultDescDlg dialog

class CTestResultDescDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestResultDescDlg)

public:
	CTestResultDescDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestResultDescDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TEST_RESULT_DESC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:

	void ShowDialog(int nCmdShow);

	void AddMessage(COLORREF color, LPCTSTR pstrFormat);
	void ClearMessage();

private:

	CRichEditEx		m_RichEditResultDescription;
};
