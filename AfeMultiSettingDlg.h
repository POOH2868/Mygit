#pragma once

#include ".\CustomUI\Title.h"
#include "util.h"

// CAfeMultiSettingDlg dialog

class CAfeMultiSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAfeMultiSettingDlg)

public:
	CAfeMultiSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAfeMultiSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_AFE_SETTING_MULTI };

public:

	int m_nNCount_A;
	int m_nUCount_A;
	int m_nFrequency_A;

	int m_nNCount_B;
	int m_nUCount_B;
	int m_nFrequency_B;

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
