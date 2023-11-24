#pragma once

#include ".\CustomUI\Title.h"
#include "util.h"

// CAfeSettingDlg dialog

class CAfeSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAfeSettingDlg)

public:
	CAfeSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAfeSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_AFE_SETTING };

public:

	int m_nNCount;
	int m_nUCount;
	int m_nFrequency;

protected:

	CTitle m_Title;
	CShadeButtonST			m_btnOK;
	CShadeButtonST			m_btnCancel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

