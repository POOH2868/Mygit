#pragma once

#include ".\CustomUI\Title.h"
#include "util.h"

// CAfeNoiseSettingDlg dialog

class CAfeNoiseSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAfeNoiseSettingDlg)

public:
	CAfeNoiseSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAfeNoiseSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_AFE_SETTING_NOISE };

public:

	int m_nNCount;
	int m_nUCount;
	int m_nFrequency;

	// NOISE
	int m_nFrequency1;
	int m_nFrequency2;
	int m_nFrequency3;
	int m_nFrequency4;

	BOOL bCheckFrequency1;
	BOOL bCheckFrequency2;
	BOOL bCheckFrequency3;
	BOOL bCheckFrequency4;

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

